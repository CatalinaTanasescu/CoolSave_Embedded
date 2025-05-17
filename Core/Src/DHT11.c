#include "DHT11.h"

extern void delay_us(uint16_t us);  // trebuie definit în proiect

static GPIO_TypeDef *DHT_PORT;
static uint16_t DHT_PIN;

void DHT11_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    DHT_PORT = GPIOx;
    DHT_PIN = GPIO_Pin;
}

static void DHT_SetPinOutput(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT_PORT, &GPIO_InitStruct);
}

static void DHT_SetPinInput(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT_PORT, &GPIO_InitStruct);
}

static uint8_t DHT_ReadByte(void) {
    uint8_t i, byte = 0;
    for (i = 0; i < 8; i++) {
        while (!HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN));     // wait for high
        delay_us(40);                                     // wait 40us

        if (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN))
            byte |= (1 << (7 - i));                       // bit = 1
        while (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN));      // wait for low
    }
    return byte;
}

uint8_t DHT11_Read(DHT11_Data *data) {
    uint8_t rh_int, rh_dec, temp_int, temp_dec, checksum;

    DHT_SetPinOutput();
    HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_RESET);
    HAL_Delay(18); // Start signal (18ms)
    HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_SET);
    delay_us(30); // Wait 30us
    DHT_SetPinInput();

    // Check response from DHT11
    if (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == GPIO_PIN_RESET) {
        delay_us(80);
        if (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == GPIO_PIN_SET) {
            delay_us(80);

            rh_int    = DHT_ReadByte();
            rh_dec    = DHT_ReadByte();
            temp_int  = DHT_ReadByte();
            temp_dec  = DHT_ReadByte();
            checksum  = DHT_ReadByte();

            if ((rh_int + rh_dec + temp_int + temp_dec) == checksum) {
                data->humidity = rh_int;
                data->temperature = temp_int;
                return 1;
            }
        }
    }

    return 0; // read failed
}
