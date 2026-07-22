#include <gui/graphscreen_screen/GraphScreenView.hpp>
#include <gui/graphscreen_screen/GraphScreenPresenter.hpp>
#include <gui/common/FrontendApplication.hpp>

#include <touchgfx/Application.hpp>

GraphScreenPresenter::
GraphScreenPresenter(
    GraphScreenView& v)
    : view(v)
{
}

void GraphScreenPresenter::activate()
{
}

void GraphScreenPresenter::deactivate()
{
}

void GraphScreenPresenter::
onAppDataUpdated(
    const AppSnapshot_t& snapshot)
{
    view.updateGraph(snapshot);
}

void GraphScreenPresenter::
onHardwareButton(
    AppUiEvent_t event)
{
    FrontendApplication* app =
        static_cast<FrontendApplication*>(
            touchgfx::Application::
                getInstance());

    if (event ==
        APP_UI_EVENT_SHORT_PRESS)
    {
        app->
            gotoDashboardScreenNoTransition();
    }
    else if (event ==
             APP_UI_EVENT_LONG_PRESS)
    {
        app->
            gotoSettingsScreenNoTransition();
    }
}
