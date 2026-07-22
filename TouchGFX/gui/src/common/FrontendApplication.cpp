#include <gui/common/FrontendApplication.hpp>
#include <gui/common/FrontendHeap.hpp>

#include <gui/dashboardscreen_screen/DashboardScreenView.hpp>
#include <gui/dashboardscreen_screen/DashboardScreenPresenter.hpp>

#include <gui/graphscreen_screen/GraphScreenView.hpp>
#include <gui/graphscreen_screen/GraphScreenPresenter.hpp>

#include <gui/settingsscreen_screen/SettingsScreenView.hpp>
#include <gui/settingsscreen_screen/SettingsScreenPresenter.hpp>

#include <touchgfx/transitions/NoTransition.hpp>

FrontendApplication::FrontendApplication(
    Model& m,
    FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
{
}

void FrontendApplication::
gotoDashboardScreenNoTransition()
{
    /*
     * Hàm này đã được TouchGFX sinh trong Base.
     */
    FrontendApplicationBase::
        gotoDashboardScreenScreenNoTransition();
}

void FrontendApplication::
gotoGraphScreenNoTransition()
{
    touchgfx::makeTransition<
        GraphScreenView,
        GraphScreenPresenter,
        touchgfx::NoTransition,
        Model>(
            &currentScreen,
            &currentPresenter,
            frontendHeap,
            &currentTransition,
            &model);
}

void FrontendApplication::
gotoSettingsScreenNoTransition()
{
    touchgfx::makeTransition<
        SettingsScreenView,
        SettingsScreenPresenter,
        touchgfx::NoTransition,
        Model>(
            &currentScreen,
            &currentPresenter,
            frontendHeap,
            &currentTransition,
            &model);
}
