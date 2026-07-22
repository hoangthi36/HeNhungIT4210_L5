	#include "app_backend.h"

#include "mfrc522.h"
#include <string.h>
#include "ds1307.h"
#include "mq6.h"

#include <stdio.h>
#include <string.h>

#define ADC_SAMPLE_COUNT 32U

static uint16_t adc_dma_buffer[ADC_SAMPLE_COUNT];

static AppSnapshot_t app_snapshot;
static AppUiEvent_t pending_ui_event;

static bool settings_visible;

static uint32_t sensor_tick;
static uint32_t rtc_tick;
static uint32_t led_tick;

static char uart_buffer[160];
/*
 * =========================================================
 * RC522
 * =========================================================
 */

/*
 * UID này chỉ là UID ví dụ.
 * Sau khi đọc được UID thật, bạn thay lại bốn byte này.
 */
static const uint8_t admin_uid[] =
{
    0x12U,
    0x34U,
    0x56U,
    0x78U
};

#define ADMIN_UID_SIZE \
    ((uint8_t)sizeof(admin_uid))

/*
 * Chu kỳ quét RC522.
 */
static uint32_t rfid_tick = 0U;

/*
 * Khi quét đúng thẻ, quyền được giữ cho đến khi
 * người dùng rời khỏi SettingsScreen.
 */
static bool rfid_authorization_latched = false;

static uint16_t ADC_GetAverage(void)
{
    uint32_t sum = 0U;

    for (uint32_t i = 0U; i < ADC_SAMPLE_COUNT; i++) {
        sum += adc_dma_buffer[i];
    }

    return (uint16_t)(sum / ADC_SAMPLE_COUNT);
}

static void Button_Process(uint32_t now_ms)
{
    static bool previous_raw;
    static bool stable_state;

    static uint32_t debounce_tick;
    static uint32_t press_tick;

    static bool long_event_sent;

    const bool raw =
        HAL_GPIO_ReadPin(
            B1_USER_GPIO_Port,
            B1_USER_Pin) == GPIO_PIN_SET;

    if (raw != previous_raw) {
        previous_raw = raw;
        debounce_tick = now_ms;
    }

    if ((now_ms - debounce_tick) < 30U) {
        return;
    }

    if (stable_state != raw) {
        stable_state = raw;

        if (stable_state) {
            press_tick = now_ms;
            long_event_sent = false;
        } else {
            if (!long_event_sent) {
                pending_ui_event =
                    APP_UI_EVENT_SHORT_PRESS;
            }
        }
    }

    if (stable_state &&
        !long_event_sent &&
        (now_ms - press_tick >= 3000U)) {

        long_event_sent = true;
        pending_ui_event =
            APP_UI_EVENT_LONG_PRESS;
    }
}

static GasLevel_t GasLevel_Classify(
    uint16_t ppm,
    bool sensor_valid)
{
    if (!sensor_valid) {
        return GAS_LEVEL_SENSOR_ERROR;
    }

    if (ppm < app_snapshot.threshold_1) {
        return GAS_LEVEL_SAFE;
    }

    if (ppm < app_snapshot.threshold_2) {
        return GAS_LEVEL_WARNING;
    }

    return GAS_LEVEL_DANGER;
}

static void Sensor_Process(void)
{
    const uint16_t adc_average =
        ADC_GetAverage();

    MQ6_Result_t result;
    MQ6_Process(adc_average, &result);

    app_snapshot.adc_raw =
        result.adc_raw;

    app_snapshot.adc_voltage_mv =
        result.adc_voltage_mv;

    app_snapshot.mq6_voltage_mv =
        result.sensor_voltage_mv;

    app_snapshot.mq6_ok =
        result.valid;

    if (result.valid) {
        app_snapshot.ppm =
            (uint16_t)result.ppm;
    } else {
        app_snapshot.ppm = 0U;
    }

    app_snapshot.gas_level =
        GasLevel_Classify(
            app_snapshot.ppm,
            result.valid);
}

static void Led_Process(uint32_t now_ms)
{
    static bool blink_state;

    switch (app_snapshot.gas_level)
    {
        case GAS_LEVEL_SAFE:
            HAL_GPIO_WritePin(
                LED_GREEN_GPIO_Port,
                LED_GREEN_Pin,
                GPIO_PIN_SET);

            HAL_GPIO_WritePin(
                LED_RED_GPIO_Port,
                LED_RED_Pin,
                GPIO_PIN_RESET);
            break;

        case GAS_LEVEL_WARNING:
            if ((now_ms - led_tick) >= 500U) {
                led_tick = now_ms;
                blink_state = !blink_state;

                HAL_GPIO_WritePin(
                    GPIOG,
                    LED_GREEN_Pin | LED_RED_Pin,
                    blink_state ?
                        GPIO_PIN_SET :
                        GPIO_PIN_RESET);
            }
            break;

        case GAS_LEVEL_DANGER:
            if ((now_ms - led_tick) >= 125U) {
                led_tick = now_ms;
                blink_state = !blink_state;

                HAL_GPIO_WritePin(
                    GPIOG,
                    LED_GREEN_Pin | LED_RED_Pin,
                    blink_state ?
                        GPIO_PIN_SET :
                        GPIO_PIN_RESET);
            }
            break;

        default:
            if ((now_ms - led_tick) >= 250U) {
                led_tick = now_ms;
                blink_state = !blink_state;

                HAL_GPIO_WritePin(
                    LED_GREEN_GPIO_Port,
                    LED_GREEN_Pin,
                    blink_state ?
                        GPIO_PIN_SET :
                        GPIO_PIN_RESET);

                HAL_GPIO_WritePin(
                    LED_RED_GPIO_Port,
                    LED_RED_Pin,
                    blink_state ?
                        GPIO_PIN_RESET :
                        GPIO_PIN_SET);
            }
            break;
    }
}

static void RTC_Process(void)
{
    DS1307_Time_t time;

    app_snapshot.rtc_ok =
        DS1307_ReadTime(&time);

    if (!app_snapshot.rtc_ok) {
        return;
    }

    app_snapshot.hour = time.hour;
    app_snapshot.minute = time.minute;
    app_snapshot.second = time.second;

    app_snapshot.date = time.date;
    app_snapshot.month = time.month;
    app_snapshot.year = time.year;
}

static const char *GasLevel_ToString(GasLevel_t level)
{
    switch (level)
    {
        case GAS_LEVEL_SAFE:
            return "SAFE";

        case GAS_LEVEL_WARNING:
            return "WARNING";

        case GAS_LEVEL_DANGER:
            return "DANGER";

        default:
            return "SENSOR_ERROR";
    }
}

static void UART_Process(void)
{
    if (huart1.gState != HAL_UART_STATE_READY) {
        return;
    }

    const int length = snprintf(
        uart_buffer,
        sizeof(uart_buffer),
        "GAS,20%02u-%02u-%02u,%02u:%02u:%02u,"
        "ADC=%u,ADC_MV=%u,MQ6_MV=%u,PPM=%u,"
        "LEVEL=%s,T1=%u,T2=%u\r\n",
        app_snapshot.year,
        app_snapshot.month,
        app_snapshot.date,
        app_snapshot.hour,
        app_snapshot.minute,
        app_snapshot.second,
        app_snapshot.adc_raw,
        app_snapshot.adc_voltage_mv,
        app_snapshot.mq6_voltage_mv,
        app_snapshot.ppm,
        GasLevel_ToString(app_snapshot.gas_level),
        app_snapshot.threshold_1,
        app_snapshot.threshold_2);

    if (length <= 0) {
        return;
    }

    HAL_UART_Transmit_DMA(
        &huart1,
        (uint8_t *)uart_buffer,
        (uint16_t)length);
}

static bool Rfid_IsAdminUid(
    const MFRC522_Card_t *card)
{
    if (card == NULL) {
        return false;
    }

    if (!card->valid) {
        return false;
    }

    if (card->uid_size != ADMIN_UID_SIZE) {
        return false;
    }

    return memcmp(
        card->uid,
        admin_uid,
        ADMIN_UID_SIZE) == 0;
}

static void Rfid_Process(uint32_t now_ms)
{
    MFRC522_Card_t card;

    /*
     * Chỉ đọc thẻ khi SettingsScreen đang mở.
     */
    if (!settings_visible) {
        app_snapshot.rfid_authorized = false;
        rfid_authorization_latched = false;
        return;
    }

    /*
     * Quét mỗi 200 ms.
     */
    if ((uint32_t)(now_ms - rfid_tick) < 200U) {
        return;
    }

    rfid_tick = now_ms;

    memset(&card, 0, sizeof(card));

    /*
     * Phiên bản driver thống nhất:
     * MFRC522_ReadCard() tự kiểm tra có thẻ,
     * sau đó đọc UID.
     */
    if (!MFRC522_ReadCard(&card)) {
        /*
         * Khi đã xác thực thành công, việc lấy thẻ ra
         * không khóa lại bàn phím ngay.
         */
        app_snapshot.rfid_authorized =
            rfid_authorization_latched;

        return;
    }

    if (Rfid_IsAdminUid(&card)) {
        rfid_authorization_latched = true;
    }

    app_snapshot.rfid_authorized =
        rfid_authorization_latched;
}

void AppBackend_Init(void)
{
    /*
     * Xóa snapshot trước khi gán các giá trị mặc định.
     */
    memset(&app_snapshot, 0, sizeof(app_snapshot));

    app_snapshot.threshold_1 = 1000U;
    app_snapshot.threshold_2 = 2000U;

    /*
     * Trạng thái ban đầu của Settings và RFID.
     */
    settings_visible = false;
    rfid_tick = 0U;
    rfid_authorization_latched = false;

    app_snapshot.rfid_authorized = false;

    /*
     * Đưa chân CS về trạng thái không chọn RC522.
     */
    HAL_GPIO_WritePin(
        RC522_CS_GPIO_Port,
        RC522_CS_Pin,
        GPIO_PIN_SET);

    /*
     * Đưa RC522 ra khỏi trạng thái reset.
     */
    HAL_GPIO_WritePin(
        RC522_RST_GPIO_Port,
        RC522_RST_Pin,
        GPIO_PIN_SET);

    /*
     * Khởi tạo RC522.
     *
     * MX_SPI4_Init() phải chạy trước AppBackend_Init()
     * trong main.c.
     */
    (void)MFRC522_Init();

    /*
     * Khởi động ADC DMA cho MQ6.
     */
    HAL_ADC_Start_DMA(
        &hadc1,
        (uint32_t *)adc_dma_buffer,
        ADC_SAMPLE_COUNT);

    /*
     * Đọc RTC lần đầu.
     */
    RTC_Process();
}

void AppBackend_Tick(uint32_t now_ms)
{
    /*
     * Xử lý nút B1.
     */
    Button_Process(now_ms);

    /*
     * Đọc MQ6 và gửi UART mỗi 100 ms.
     */
    if ((uint32_t)(now_ms - sensor_tick) >= 100U) {
        sensor_tick = now_ms;

        Sensor_Process();
        UART_Process();
    }

    /*
     * Đọc DS1307 mỗi 1000 ms.
     */
    if ((uint32_t)(now_ms - rtc_tick) >= 1000U) {
        rtc_tick = now_ms;

        RTC_Process();
    }

    /*
     * Quét RC522. Bên trong Rfid_Process() sẽ tự kiểm tra
     * settings_visible và chu kỳ 200 ms.
     */
    Rfid_Process(now_ms);

    /*
     * Cập nhật LED cảnh báo.
     */
    Led_Process(now_ms);
}

void AppBackend_GetSnapshot(AppSnapshot_t *snapshot)
{
    if (snapshot != NULL) {
        *snapshot = app_snapshot;
    }
}

AppUiEvent_t AppBackend_GetAndClearUiEvent(void)
{
    const AppUiEvent_t event = pending_ui_event;
    pending_ui_event = APP_UI_EVENT_NONE;

    return event;
}

bool AppBackend_SetThresholds(
    uint16_t threshold_1,
    uint16_t threshold_2)
{
    /*
     * Chỉ cho phép lưu ngưỡng khi đã xác thực RFID.
     */
    if (!app_snapshot.rfid_authorized) {
        return false;
    }

    /*
     * Điều kiện:
     * 0 <= T1 < T2 <= 9999.
     *
     * Hai biến là uint16_t nên không cần kiểm tra âm.
     */
    if (threshold_1 >= threshold_2 ||
        threshold_2 > 9999U) {
        return false;
    }

    app_snapshot.threshold_1 = threshold_1;
    app_snapshot.threshold_2 = threshold_2;

    return true;
}

void AppBackend_SetSettingsVisible(bool visible)
{
    settings_visible = visible;

    /*
     * Khi vừa vào Settings, cho phép RC522 quét ngay
     * ở lần AppBackend_Tick tiếp theo.
     */
    rfid_tick = 0U;

    if (!visible) {
        /*
         * Rời Settings thì hủy quyền quản trị.
         * Lần vào sau phải quét thẻ lại.
         */
        rfid_authorization_latched = false;
        app_snapshot.rfid_authorized = false;
    }
}
