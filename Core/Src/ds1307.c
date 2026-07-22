#include "ds1307.h"
#include <stddef.h>
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
    uint8_t data[7];
    if (time == NULL) {
        return false;
    }
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
return true;
}
bool DS1307_SetTime(const DS1307_Time_t *time)
{
uint8_t data[7];
if (time == NULL) {
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
