#ifndef DHT11_H
#define DHT11_H

#include "stm32l4xx_hal.h"

typedef struct {
    uint8_t temperature;
    uint8_t humidity;
} DHT11_Data;

void DHT11_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t DHT11_Read(DHT11_Data *data);

#endif
