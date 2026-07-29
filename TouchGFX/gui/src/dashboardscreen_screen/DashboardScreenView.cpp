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
     * Designer chỉ tự tính chiều rộng theo phần chữ tĩnh
     * (Date, Time, Ppm, ...). Phần wildcard dài hơn sẽ bị
     * cắt nếu không dành trước đủ vùng vẽ.
     */
    txtPpm.setWidth(190);
    txtPpm.setHeight(26);

    txtAdcVoltage.setWidth(220);
    txtAdcVoltage.setHeight(26);

    txtMq6Voltage.setWidth(220);
    txtMq6Voltage.setHeight(26);

    txtTime.setWidth(122);
    txtTime.setHeight(26);

    txtDate.setWidth(180);
    txtDate.setHeight(26);

    txtStatus.setWidth(235);
    txtStatus.setHeight(26);

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

    /* Indicator cũ không còn dùng trong giao diện mới. */
    indicatorCurrent.setVisible(false);

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

}

void DashboardScreenView::updateGasStatus(
    GasLevel_t level)
{
    boxSafe.setVisible(level == GAS_LEVEL_SAFE);
    boxWarning.setVisible(level == GAS_LEVEL_WARNING);
    boxDanger.setVisible(level == GAS_LEVEL_DANGER);

    boxSafe.invalidate();
    boxWarning.invalidate();
    boxDanger.invalidate();

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
