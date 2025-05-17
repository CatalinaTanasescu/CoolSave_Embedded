#ifndef AP3216C_H
#define AP3216C_H

#include "stm32l4xx_hal.h"

typedef struct {
    uint16_t als_raw;
    float als_lux;
} AP3216C_Data;

HAL_StatusTypeDef AP3216C_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef AP3216C_ReadALS(AP3216C_Data *data);

#endif
