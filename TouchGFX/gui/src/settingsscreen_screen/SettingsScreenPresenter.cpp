#include <gui/settingsscreen_screen/SettingsScreenView.hpp>
#include <gui/settingsscreen_screen/SettingsScreenPresenter.hpp>
#include <gui/common/FrontendApplication.hpp>

extern "C"
{
#include "app_backend.h"
}

SettingsScreenPresenter::SettingsScreenPresenter(
    SettingsScreenView& v)
    : view(v)
{
}

void SettingsScreenPresenter::activate()
{
}

void SettingsScreenPresenter::deactivate()
{
}

void SettingsScreenPresenter::onAppDataUpdated(
    const AppSnapshot_t& snapshot)
{
    view.updateData(snapshot);
}

void SettingsScreenPresenter::onHardwareButton(
    AppUiEvent_t event)
{
    if (event == APP_UI_EVENT_SHORT_PRESS)
    {
        closeSettings();
    }
}

bool SettingsScreenPresenter::saveThreshold(
    uint16_t threshold1,
    uint16_t threshold2)
{
    return AppBackend_SetThresholds(
        threshold1,
        threshold2);
}

void SettingsScreenPresenter::setSettingsVisible(
    bool visible)
{
    AppBackend_SetSettingsVisible(visible);
}

void SettingsScreenPresenter::closeSettings()
{
    AppBackend_SetSettingsVisible(false);

    FrontendApplication* app =
        static_cast<FrontendApplication*>(
            touchgfx::Application::getInstance());

    app->gotoDashboardScreenNoTransition();
}
