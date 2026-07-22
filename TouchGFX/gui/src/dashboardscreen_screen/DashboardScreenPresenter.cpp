#include <gui/dashboardscreen_screen/DashboardScreenView.hpp>
#include <gui/dashboardscreen_screen/DashboardScreenPresenter.hpp>
#include <gui/common/FrontendApplication.hpp>

#include <touchgfx/Application.hpp>

void DashboardScreenPresenter::onAppDataUpdated(
    const AppSnapshot_t& snapshot)
{
    view.updateData(snapshot);
}

void DashboardScreenPresenter::onHardwareButton(
    AppUiEvent_t event)
{
    FrontendApplication* application =
        static_cast<FrontendApplication*>(
            touchgfx::Application::getInstance());

    switch (event)
    {
        /*
         * Nhấn ngắn B1:
         * Dashboard → Graph.
         */
        case APP_UI_EVENT_SHORT_PRESS:
        {
            application->
			    gotoGraphScreenNoTransition();

            break;
        }

        /*
         * Giữ B1 khoảng 3 giây:
         * Dashboard → Settings.
         */
        case APP_UI_EVENT_LONG_PRESS:
        {
            application->
                gotoSettingsScreenNoTransition();

            break;
        }

        case APP_UI_EVENT_NONE:
        default:
        {
            /*
             * Không có sự kiện.
             */
            break;
        }
    }
}
#include <gui/dashboardscreen_screen/DashboardScreenView.hpp>
#include <gui/dashboardscreen_screen/DashboardScreenPresenter.hpp>

DashboardScreenPresenter::DashboardScreenPresenter(DashboardScreenView& v)
    : view(v)
{

}

void DashboardScreenPresenter::activate()
{

}

void DashboardScreenPresenter::deactivate()
{

}
