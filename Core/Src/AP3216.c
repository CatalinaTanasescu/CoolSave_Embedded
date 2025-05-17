#include "AP3216.h"

#define AP3216C_ADDR        (0x1E << 1)  // I2C 8-bit address
#define SYS_CFG_REG         0x00
#define ALS_CFG_REG         0x10
#define ALS_DATA_L_REG      0x0C
#define ALS_DATA_H_REG      0x0D

static I2C_HandleTypeDef *ap_i2c;

// ALS gain = 0b00 → Range 1 → 0~23360 lux → resolution = 0.36 lux/count
#define ALS_GAIN_SETTING    0x00
#define ALS_RESOLUTION_LUX  0.36f

HAL_StatusTypeDef AP3216C_Init(I2C_HandleTypeDef *hi2c) {
    uint8_t data;
    ap_i2c = hi2c;

    // Software Reset
    data = 0x04;
    if (HAL_I2C_Mem_Write(ap_i2c, AP3216C_ADDR, SYS_CFG_REG, 1, &data, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;
    HAL_Delay(50);

    // Enable only ALS (ambient light sensor)
    data = 0x01;
    if (HAL_I2C_Mem_Write(ap_i2c, AP3216C_ADDR, SYS_CFG_REG, 1, &data, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    // Set ALS gain/range and filter persist
    data = ALS_GAIN_SETTING;
    if (HAL_I2C_Mem_Write(ap_i2c, AP3216C_ADDR, ALS_CFG_REG, 1, &data, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    HAL_Delay(10);
    return HAL_OK;
}

HAL_StatusTypeDef AP3216C_ReadALS(AP3216C_Data *data) {
    uint8_t als_low, als_high;

    if (HAL_I2C_Mem_Read(ap_i2c, AP3216C_ADDR, ALS_DATA_L_REG, 1, &als_low, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    if (HAL_I2C_Mem_Read(ap_i2c, AP3216C_ADDR, ALS_DATA_H_REG, 1, &als_high, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    data->als_raw = ((uint16_t)als_high << 8) | als_low;
    data->als_lux = data->als_raw * ALS_RESOLUTION_LUX;

    return HAL_OK;
}
