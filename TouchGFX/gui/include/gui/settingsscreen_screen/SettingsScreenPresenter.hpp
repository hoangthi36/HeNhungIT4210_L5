#ifndef SETTINGSSCREENPRESENTER_HPP
#define SETTINGSSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SettingsScreenView;

class SettingsScreenPresenter :
    public touchgfx::Presenter,
    public ModelListener
{
public:
    SettingsScreenPresenter(SettingsScreenView& v);

    virtual void activate();
    virtual void deactivate();

    virtual void onAppDataUpdated(
        const AppSnapshot_t& snapshot);

    virtual void onHardwareButton(
        AppUiEvent_t event);

    bool saveThreshold(
        uint16_t threshold1,
        uint16_t threshold2);

    void setSettingsVisible(bool visible);

    void closeSettings();

private:
    SettingsScreenPresenter();

    SettingsScreenView& view;
};

#endif
