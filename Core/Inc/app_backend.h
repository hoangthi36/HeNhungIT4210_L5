#ifndef APP_BACKEND_H
#define APP_BACKEND_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart1;

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        GAS_LEVEL_SAFE = 0,
        GAS_LEVEL_WARNING,
        GAS_LEVEL_DANGER,
        GAS_LEVEL_SENSOR_ERROR
    } GasLevel_t;

    typedef enum
    {
        APP_UI_EVENT_NONE = 0,
        APP_UI_EVENT_SHORT_PRESS,
        APP_UI_EVENT_LONG_PRESS
    } AppUiEvent_t;

    typedef struct
    {
        uint16_t adc_raw;
        uint16_t adc_voltage_mv;
        uint16_t mq6_voltage_mv;
        uint16_t ppm;

        uint16_t threshold_1;
        uint16_t threshold_2;

        GasLevel_t gas_level;

        uint8_t hour;
        uint8_t minute;
        uint8_t second;

        uint8_t date;
        uint8_t month;
        uint8_t year;

        bool rtc_ok;
        bool mq6_ok;
        bool rc522_ok;
        bool rfid_authorized;
    } AppSnapshot_t;

    void AppBackend_Init(void);
    void AppBackend_Tick(uint32_t now_ms);

    void AppBackend_GetSnapshot(AppSnapshot_t *snapshot);
    AppUiEvent_t AppBackend_GetAndClearUiEvent(void);

    bool AppBackend_SetThresholds(uint16_t threshold_1,
                                  uint16_t threshold_2);

    void AppBackend_SetSettingsVisible(bool visible);

#ifdef __cplusplus
}
#endif

#endif
