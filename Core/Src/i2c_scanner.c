#include "i2c_scanner.h"

#include <stdio.h>
#include <string.h>

static void Scanner_Print(UART_HandleTypeDef *huart,
                          const char *text)
{
    if ((huart == NULL) || (text == NULL))
    {
        return;
    }

    (void)HAL_UART_Transmit(
        huart,
        (uint8_t *)text,
        (uint16_t)strlen(text),
        200U);
}

void I2C_Scanner_Run(I2C_HandleTypeDef *hi2c,
                     UART_HandleTypeDef *huart)
{
    char message[80];
    uint16_t found_count = 0U;

    if ((hi2c == NULL) || (huart == NULL))
    {
        return;
    }

    Scanner_Print(huart, "\r\nI2C SCAN START\r\n");

    for (uint16_t address = 1U; address < 127U; address++)
    {
        const HAL_StatusTypeDef status =
            HAL_I2C_IsDeviceReady(
                hi2c,
                (uint16_t)(address << 1U),
                2U,
                20U);

        if (status == HAL_OK)
        {
            found_count++;

            const int length = snprintf(
                message,
                sizeof(message),
                "FOUND: 7BIT=0x%02X HAL_ADDR=0x%02X\r\n",
                (unsigned int)address,
                (unsigned int)(address << 1U));

            if (length > 0)
            {
                (void)HAL_UART_Transmit(
                    huart,
                    (uint8_t *)message,
                    (uint16_t)length,
                    200U);
            }
        }
    }

    (void)snprintf(
        message,
        sizeof(message),
        "I2C SCAN END: FOUND=%u\r\n\r\n",
        (unsigned int)found_count);

    Scanner_Print(huart, message);
}
