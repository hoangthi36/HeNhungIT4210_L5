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
    : FrontendApplicationBase(m, heap),
      customTransitionCallback()
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
    customTransitionCallback =
        touchgfx::Callback<FrontendApplication>(
            this,
            &FrontendApplication::
                gotoGraphScreenNoTransitionImpl);

    pendingScreenTransitionCallback =
        &customTransitionCallback;
}

void FrontendApplication::
gotoGraphScreenNoTransitionImpl()
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
    customTransitionCallback =
        touchgfx::Callback<FrontendApplication>(
            this,
            &FrontendApplication::
                gotoSettingsScreenNoTransitionImpl);

    pendingScreenTransitionCallback =
        &customTransitionCallback;
}

void FrontendApplication::
gotoSettingsScreenNoTransitionImpl()
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
