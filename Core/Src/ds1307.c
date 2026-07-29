#include "ds1307.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart1;
#define DS1307_ADDRESS        (0x68U << 1)
#define DS1307_TIME_REGISTER  0x00U
static uint8_t BCD_ToDecimal(uint8_t value)
{
    return (uint8_t)(
        ((value >> 4U) * 10U) +
        (value & 0x0FU));
}
static uint8_t Decimal_ToBCD(uint8_t value)
{
    return (uint8_t)(
        ((value / 10U) << 4U) |
        (value % 10U));
}

static bool BCD_IsValid(uint8_t value)
{
    return ((value & 0x0FU) <= 9U) && (((value >> 4U) & 0x0FU) <= 9U);
}

static bool Time_IsValid(const DS1307_Time_t *time)
{
    return (time != NULL) &&
           (time->second <= 59U) &&
           (time->minute <= 59U) &&
           (time->hour <= 23U) &&
           (time->day_of_week >= 1U) && (time->day_of_week <= 7U) &&
           (time->date >= 1U) && (time->date <= 31U) &&
           (time->month >= 1U) && (time->month <= 12U) &&
           (time->year <= 99U);
}

static bool RawTime_IsValid(const uint8_t data[7])
{
    DS1307_Time_t time;

    if ((data[0] & 0x80U) != 0U) {
        return false;
    }

    if (!BCD_IsValid(data[0] & 0x7FU) ||
        !BCD_IsValid(data[1] & 0x7FU) ||
        !BCD_IsValid(data[2] & 0x3FU) ||
        !BCD_IsValid(data[3] & 0x07U) ||
        !BCD_IsValid(data[4] & 0x3FU) ||
        !BCD_IsValid(data[5] & 0x1FU) ||
        !BCD_IsValid(data[6])) {
        return false;
    }

    time.second = BCD_ToDecimal(data[0] & 0x7FU);
    time.minute = BCD_ToDecimal(data[1] & 0x7FU);
    time.hour = BCD_ToDecimal(data[2] & 0x3FU);
    time.day_of_week = BCD_ToDecimal(data[3] & 0x07U);
    time.date = BCD_ToDecimal(data[4] & 0x3FU);
    time.month = BCD_ToDecimal(data[5] & 0x1FU);
    time.year = BCD_ToDecimal(data[6]);

    return Time_IsValid(&time);
}
bool DS1307_IsReady(void)
{
    return HAL_I2C_IsDeviceReady(
               &hi2c3,
               DS1307_ADDRESS,
               3U,
               100U) == HAL_OK;
}
bool DS1307_ReadTime(DS1307_Time_t *time)
{
	HAL_UART_Transmit(&huart1,
	                  (uint8_t*)"ENTER READ\r\n",
	                  12,
	                  100);
    uint8_t data[7];
    HAL_StatusTypeDef status;
    char msg[128];
    if (time == NULL) {
        return false;
    }
    status = HAL_I2C_Mem_Read(
                &hi2c3,
                DS1307_ADDRESS,
                DS1307_TIME_REGISTER,
                I2C_MEMADD_SIZE_8BIT,
                data,
                sizeof(data),
                100U);
    sprintf(msg,
            "STATUS=%d ERR=%lu\r\n",
            status,
            HAL_I2C_GetError(&hi2c3));
    HAL_UART_Transmit(
            &huart1,
            (uint8_t *)msg,
            strlen(msg),
            100);
    if(status != HAL_OK)
    {
        return false;
    }
    sprintf(msg,
            "RAW=%02X %02X %02X %02X %02X %02X %02X\r\n",
            data[0],
            data[1],
            data[2],
            data[3],
            data[4],
            data[5],
            data[6]);

    HAL_UART_Transmit(
            &huart1,
            (uint8_t *)msg,
            strlen(msg),
            100);
if (!RawTime_IsValid(data))
{
    return false;
}
time->second =
BCD_ToDecimal(data[0] & 0x7FU);
time->minute =
BCD_ToDecimal(data[1] & 0x7FU);
time->hour =
BCD_ToDecimal(data[2] & 0x3FU);
time->day_of_week =
BCD_ToDecimal(data[3] & 0x07U);
time->date =
BCD_ToDecimal(data[4] & 0x3FU);
time->month =
BCD_ToDecimal(data[5] & 0x1FU);
time->year =
BCD_ToDecimal(data[6]);
sprintf(msg,
        "TIME=%02u:%02u:%02u %02u/%02u/%02u\r\n",
        time->hour,
        time->minute,
        time->second,
        time->date,
        time->month,
        time->year);

HAL_UART_Transmit(
        &huart1,
        (uint8_t *)msg,
        strlen(msg),
        100);
return true;
}
bool DS1307_SetTime(const DS1307_Time_t *time)
{
uint8_t data[7];
if (!Time_IsValid(time)) {
return false;
}
data[0] = Decimal_ToBCD(time->second) & 0x7FU;
data[1] = Decimal_ToBCD(time->minute);
data[2] = Decimal_ToBCD(time->hour);
data[3] = Decimal_ToBCD(time->day_of_week);
data[4] = Decimal_ToBCD(time->date);
data[5] = Decimal_ToBCD(time->month);
data[6] = Decimal_ToBCD(time->year);
return HAL_I2C_Mem_Write(
       &hi2c3,
       DS1307_ADDRESS,
       DS1307_TIME_REGISTER,
       I2C_MEMADD_SIZE_8BIT,
       data,
       sizeof(data),
       100U) == HAL_OK;
}

bool DS1307_InitializeIfNeeded(const DS1307_Time_t *initial_time,
                              bool *was_initialized)
{
uint8_t data[7];

if ((initial_time == NULL) || (was_initialized == NULL)) {
return false;
}

*was_initialized = false;

if (HAL_I2C_Mem_Read(
        &hi2c3,
        DS1307_ADDRESS,
        DS1307_TIME_REGISTER,
        I2C_MEMADD_SIZE_8BIT,
        data,
        sizeof(data),
        100U) != HAL_OK) {
return false;
}

if (RawTime_IsValid(data)) {
return true;
}

if (!DS1307_SetTime(initial_time)) {
return false;
}

*was_initialized = true;
return true;
}
