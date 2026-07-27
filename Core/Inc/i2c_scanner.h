#ifndef I2C_SCANNER_H
#define I2C_SCANNER_H

#include "main.h"

void I2C_Scanner_Run(I2C_HandleTypeDef *hi2c,
                     UART_HandleTypeDef *huart);

#endif /* I2C_SCANNER_H */
