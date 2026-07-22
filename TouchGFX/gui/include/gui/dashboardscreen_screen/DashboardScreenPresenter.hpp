#ifndef DASHBOARDSCREENPRESENTER_HPP
#define DASHBOARDSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class DashboardScreenView;

class DashboardScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    DashboardScreenPresenter(DashboardScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~DashboardScreenPresenter() {}

    virtual void onAppDataUpdated(
        const AppSnapshot_t& snapshot) override;

    virtual void onHardwareButton(
        AppUiEvent_t event) override;

private:
    DashboardScreenPresenter();

    DashboardScreenView& view;

};

#endif // DASHBOARDSCREENPRESENTER_HPP
