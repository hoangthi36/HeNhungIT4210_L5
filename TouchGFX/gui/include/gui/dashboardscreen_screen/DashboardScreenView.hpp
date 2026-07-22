#ifndef DASHBOARDSCREENVIEW_HPP
#define DASHBOARDSCREENVIEW_HPP

#include <gui_generated/dashboardscreen_screen/DashboardScreenViewBase.hpp>
#include <gui/dashboardscreen_screen/DashboardScreenPresenter.hpp>

class DashboardScreenView : public DashboardScreenViewBase
{
public:
    DashboardScreenView();
    virtual ~DashboardScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void updateData(const AppSnapshot_t& snapshot);
protected:
private:
    /*
     * Cập nhật vị trí thanh chỉ thị ppm.
     */
    void updatePpmIndicator(
        uint16_t ppm,
        uint16_t threshold1,
        uint16_t threshold2);

    /*
     * Cập nhật nội dung và màu trạng thái.
     */
    void updateGasStatus(
        GasLevel_t level);

    static const uint16_t TXTPPM_SIZE = 16U;
    static const uint16_t TXTADCVOLTAGE_SIZE = 20U;
    static const uint16_t TXTMQ6VOLTAGE_SIZE = 20U;
    static const uint16_t TXTTIME_SIZE = 16U;
    static const uint16_t TXTDATE_SIZE = 16U;
    static const uint16_t TXTSTATUS_SIZE = 24U;

    touchgfx::Unicode::UnicodeChar
        txtPpmBuffer[TXTPPM_SIZE];

    touchgfx::Unicode::UnicodeChar
        txtAdcVoltageBuffer[TXTADCVOLTAGE_SIZE];

    touchgfx::Unicode::UnicodeChar
        txtMq6VoltageBuffer[TXTMQ6VOLTAGE_SIZE];

    touchgfx::Unicode::UnicodeChar
        txtTimeBuffer[TXTTIME_SIZE];

    touchgfx::Unicode::UnicodeChar
        txtDateBuffer[TXTDATE_SIZE];

    touchgfx::Unicode::UnicodeChar
        txtStatusBuffer[TXTSTATUS_SIZE];
};

#endif // DASHBOARDSCREENVIEW_HPP
