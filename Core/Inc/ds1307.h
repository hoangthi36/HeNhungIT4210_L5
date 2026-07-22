#ifndef DS1307_H
#define DS1307_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

extern I2C_HandleTypeDef hi2c3;

typedef struct
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;

    uint8_t day_of_week;
    uint8_t date;
    uint8_t month;
    uint8_t year;
} DS1307_Time_t;

bool DS1307_IsReady(void);
bool DS1307_ReadTime(DS1307_Time_t *time);
bool DS1307_SetTime(const DS1307_Time_t *time);

#endif
