#include <gui/dashboardscreen_screen/DashboardScreenView.hpp>

#include <touchgfx/Color.hpp>
#include <touchgfx/Unicode.hpp>

DashboardScreenView::DashboardScreenView()
{
}

void DashboardScreenView::setupScreen()
{
    DashboardScreenViewBase::setupScreen();

    /*
     * =====================================================
     * Liên kết các TextArea với wildcard buffer
     * =====================================================
     *
     * Các TextArea trong TouchGFX Designer phải sử dụng
     * Text Resource có một wildcard:
     *
     * <*>
     */

    txtPpm.setWildcard1(
        txtPpmBuffer);

    txtAdcVoltage.setWildcard1(
        txtAdcVoltageBuffer);

    txtMq6Voltage.setWildcard1(
        txtMq6VoltageBuffer);

    txtTime.setWildcard1(
        txtTimeBuffer);

    txtDate.setWildcard1(
        txtDateBuffer);

    txtStatus.setWildcard1(
        txtStatusBuffer);

    /*
     * =====================================================
     * Giá trị mặc định trước snapshot đầu tiên
     * =====================================================
     */

    touchgfx::Unicode::snprintf(
        txtPpmBuffer,
        TXTPPM_SIZE,
        "%u ppm",
        0U);

    touchgfx::Unicode::snprintf(
        txtAdcVoltageBuffer,
        TXTADCVOLTAGE_SIZE,
        "%u.%03u V",
        0U,
        0U);

    touchgfx::Unicode::snprintf(
        txtMq6VoltageBuffer,
        TXTMQ6VOLTAGE_SIZE,
        "%u.%03u V",
        0U,
        0U);

    touchgfx::Unicode::snprintf(
        txtTimeBuffer,
        TXTTIME_SIZE,
        "--:--:--");

    touchgfx::Unicode::snprintf(
        txtDateBuffer,
        TXTDATE_SIZE,
        "--/--/----");

    touchgfx::Unicode::snprintf(
        txtStatusBuffer,
        TXTSTATUS_SIZE,
        "DANG KHOI DONG");

    /*
     * Màu mặc định khi chưa có dữ liệu hợp lệ.
     */
    txtStatus.setColor(
        touchgfx::Color::getColorFromRGB(
            120U,
            120U,
            120U));

    /*
     * Đưa indicator về đầu thanh.
     *
     * Hàm này sẽ tự cập nhật lại khi snapshot đầu tiên tới.
     */
    updatePpmIndicator(
        0U,
        1000U,
        2000U);

    /*
     * Yêu cầu TouchGFX vẽ lại.
     */
    txtPpm.invalidate();
    txtAdcVoltage.invalidate();
    txtMq6Voltage.invalidate();
    txtTime.invalidate();
    txtDate.invalidate();
    txtStatus.invalidate();
}

void DashboardScreenView::tearDownScreen()
{
    DashboardScreenViewBase::tearDownScreen();
}

void DashboardScreenView::updateData(
    const AppSnapshot_t& snapshot)
{
    /*
     * =====================================================
     * Cập nhật nồng độ ppm
     * =====================================================
     */

    touchgfx::Unicode::snprintf(
        txtPpmBuffer,
        TXTPPM_SIZE,
        "%u ppm",
        snapshot.ppm);

    txtPpm.invalidate();

    /*
     * =====================================================
     * Điện áp tại chân ADC sau mạch chia áp
     * =====================================================
     */

    const uint16_t adcVoltInteger =
        snapshot.adc_voltage_mv / 1000U;

    const uint16_t adcVoltFraction =
        snapshot.adc_voltage_mv % 1000U;

    touchgfx::Unicode::snprintf(
        txtAdcVoltageBuffer,
        TXTADCVOLTAGE_SIZE,
        "%u.%03u V",
        adcVoltInteger,
        adcVoltFraction);

    txtAdcVoltage.invalidate();

    /*
     * =====================================================
     * Điện áp ước tính tại chân AO của MQ6
     * trước mạch chia áp
     * =====================================================
     */

    const uint16_t mq6VoltInteger =
        snapshot.mq6_voltage_mv / 1000U;

    const uint16_t mq6VoltFraction =
        snapshot.mq6_voltage_mv % 1000U;

    touchgfx::Unicode::snprintf(
        txtMq6VoltageBuffer,
        TXTMQ6VOLTAGE_SIZE,
        "%u.%03u V",
        mq6VoltInteger,
        mq6VoltFraction);

    txtMq6Voltage.invalidate();

    /*
     * =====================================================
     * Cập nhật thời gian DS1307
     * =====================================================
     */

    if (snapshot.rtc_ok)
    {
        touchgfx::Unicode::snprintf(
            txtTimeBuffer,
            TXTTIME_SIZE,
            "%02u:%02u:%02u",
            snapshot.hour,
            snapshot.minute,
            snapshot.second);

        /*
         * snapshot.year được giả định là năm dạng 2 chữ số.
         *
         * Ví dụ:
         * snapshot.year = 26
         * hiển thị thành 2026.
         */
        touchgfx::Unicode::snprintf(
            txtDateBuffer,
            TXTDATE_SIZE,
            "%02u/%02u/20%02u",
            snapshot.date,
            snapshot.month,
            snapshot.year);
    }
    else
    {
        touchgfx::Unicode::snprintf(
            txtTimeBuffer,
            TXTTIME_SIZE,
            "--:--:--");

        touchgfx::Unicode::snprintf(
            txtDateBuffer,
            TXTDATE_SIZE,
            "--/--/----");
    }

    txtTime.invalidate();
    txtDate.invalidate();

    /*
     * =====================================================
     * Trạng thái cảnh báo khí gas
     * =====================================================
     */

    updateGasStatus(
        snapshot.gas_level);

    /*
     * =====================================================
     * Vạch chỉ thị nồng độ ppm
     * =====================================================
     */

    updatePpmIndicator(
        snapshot.ppm,
        snapshot.threshold_1,
        snapshot.threshold_2);
}

void DashboardScreenView::updateGasStatus(
    GasLevel_t level)
{
    switch (level)
    {
        case GAS_LEVEL_SAFE:
        {
            touchgfx::Unicode::snprintf(
                txtStatusBuffer,
                TXTSTATUS_SIZE,
                "KHONG KHI AN TOAN");

            txtStatus.setColor(
                touchgfx::Color::getColorFromRGB(
                    0U,
                    180U,
                    0U));

            break;
        }

        case GAS_LEVEL_WARNING:
        {
            touchgfx::Unicode::snprintf(
                txtStatusBuffer,
                TXTSTATUS_SIZE,
                "RO RI NHE");

            txtStatus.setColor(
                touchgfx::Color::getColorFromRGB(
                    255U,
                    165U,
                    0U));

            break;
        }

        case GAS_LEVEL_DANGER:
        {
            touchgfx::Unicode::snprintf(
                txtStatusBuffer,
                TXTSTATUS_SIZE,
                "NGUY HIEM");

            txtStatus.setColor(
                touchgfx::Color::getColorFromRGB(
                    220U,
                    0U,
                    0U));

            break;
        }

        case GAS_LEVEL_SENSOR_ERROR:
        default:
        {
            touchgfx::Unicode::snprintf(
                txtStatusBuffer,
                TXTSTATUS_SIZE,
                "LOI CAM BIEN MQ6");

            txtStatus.setColor(
                touchgfx::Color::getColorFromRGB(
                    120U,
                    120U,
                    120U));

            break;
        }
    }

    txtStatus.invalidate();
}

void DashboardScreenView::updatePpmIndicator(
    uint16_t ppm,
    uint16_t threshold1,
    uint16_t threshold2)
{
    /*
     * =====================================================
     * Cấu hình thanh ppm
     * =====================================================
     *
     * Các giá trị này phải khớp với vị trí thật trên
     * TouchGFX Designer.
     */

    const int16_t barStartX = 20;
    const int16_t barWidth = 200;

    int32_t relativeX = 0;

    /*
     * Bảo vệ trong trường hợp ngưỡng backend không hợp lệ.
     */
    if (threshold1 >= threshold2)
    {
        threshold1 = 1000U;
        threshold2 = 2000U;
    }

    /*
     * =====================================================
     * Vùng an toàn: 0 đến T1
     * Chiếm 1/3 chiều rộng thanh
     * =====================================================
     */

    if (ppm < threshold1)
    {
        if (threshold1 == 0U)
        {
            relativeX = 0;
        }
        else
        {
            relativeX =
                static_cast<int32_t>(ppm) *
                (barWidth / 3) /
                threshold1;
        }
    }

    /*
     * =====================================================
     * Vùng cảnh báo: T1 đến T2
     * Chiếm 1/3 chiều rộng thanh
     * =====================================================
     */

    else if (ppm < threshold2)
    {
        const uint16_t range =
            threshold2 - threshold1;

        const uint16_t value =
            ppm - threshold1;

        if (range == 0U)
        {
            relativeX = barWidth / 3;
        }
        else
        {
            relativeX =
                (barWidth / 3) +
                static_cast<int32_t>(value) *
                (barWidth / 3) /
                range;
        }
    }

    /*
     * =====================================================
     * Vùng nguy hiểm: T2 đến 9999
     * Chiếm 1/3 chiều rộng thanh
     * =====================================================
     */

    else
    {
        const uint16_t maximumPpm = 9999U;

        if (ppm > maximumPpm)
        {
            ppm = maximumPpm;
        }

        const uint16_t range =
            maximumPpm - threshold2;

        const uint16_t value =
            ppm - threshold2;

        if (range == 0U)
        {
            relativeX = barWidth;
        }
        else
        {
            relativeX =
                (2 * barWidth / 3) +
                static_cast<int32_t>(value) *
                (barWidth / 3) /
                range;
        }
    }

    /*
     * Giới hạn vị trí tương đối trong thanh.
     */
    if (relativeX < 0)
    {
        relativeX = 0;
    }

    if (relativeX > barWidth)
    {
        relativeX = barWidth;
    }

    /*
     * Căn tâm indicator theo vị trí ppm.
     */
    int16_t indicatorX =
        static_cast<int16_t>(
            barStartX +
            relativeX -
            indicatorCurrent.getWidth() / 2);

    const int16_t minimumX =
        static_cast<int16_t>(
            barStartX -
            indicatorCurrent.getWidth() / 2);

    const int16_t maximumX =
        static_cast<int16_t>(
            barStartX +
            barWidth -
            indicatorCurrent.getWidth() / 2);

    if (indicatorX < minimumX)
    {
        indicatorX = minimumX;
    }

    if (indicatorX > maximumX)
    {
        indicatorX = maximumX;
    }

    indicatorCurrent.setX(
        indicatorX);

    indicatorCurrent.invalidate();
}
