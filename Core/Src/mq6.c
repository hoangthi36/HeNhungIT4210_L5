#include "mq6.h"

#include <math.h>
#include <stddef.h>

/*
 * Mạch chia áp:
 * AO -- 15k -- ADC -- 27k -- GND
 */
#define MQ6_R_TOP_OHM       15000.0f
#define MQ6_R_BOTTOM_OHM    27000.0f

#define MQ6_ADC_VREF        3.3f
#define MQ6_SENSOR_VCC      5.0f

/*
 * Giá trị RL phải đo trên module thực tế.
 */
#define MQ6_RL_OHM          10000.0f

/*
 * R0 và hệ số đường cong phải được hiệu chuẩn.
 * Các giá trị dưới đây chỉ là nơi chứa cấu hình,
 * không được xem là thông số đo đã kiểm định.
 */
#define MQ6_R0_OHM          10000.0f
#define MQ6_CURVE_M         (-0.45f)
#define MQ6_CURVE_B         (1.30f)

static float MQ6_Clamp(float value, float minimum, float maximum)
{
    if (value < minimum) {
        return minimum;
    }

    if (value > maximum) {
        return maximum;
    }

    return value;
}

void MQ6_Process(uint16_t adc_raw, MQ6_Result_t *result)
{
    if (result == NULL) {
        return;
    }

    result->valid = false;
    result->adc_raw = adc_raw;

    const float divider =
        MQ6_R_BOTTOM_OHM /
        (MQ6_R_TOP_OHM + MQ6_R_BOTTOM_OHM);

    const float adc_voltage =
        ((float)adc_raw * MQ6_ADC_VREF) / 4095.0f;

    const float sensor_voltage =
        adc_voltage / divider;

    result->adc_voltage_mv =
        (uint16_t)(adc_voltage * 1000.0f);

    result->sensor_voltage_mv =
        (uint16_t)(sensor_voltage * 1000.0f);

    if (sensor_voltage < 0.02f ||
        sensor_voltage > (MQ6_SENSOR_VCC - 0.02f)) {
        return;
    }

    result->rs_ohm =
        MQ6_RL_OHM *
        (MQ6_SENSOR_VCC - sensor_voltage) /
        sensor_voltage;

    result->ratio = result->rs_ohm / MQ6_R0_OHM;

    if (result->ratio <= 0.0f) {
        return;
    }

    result->ppm = powf(
        10.0f,
        (log10f(result->ratio) - MQ6_CURVE_B) /
        MQ6_CURVE_M);

    result->ppm =
        MQ6_Clamp(result->ppm, 0.0f, 9999.0f);

    result->valid = true;
}
