#ifndef SETTINGSSCREENVIEW_HPP
#define SETTINGSSCREENVIEW_HPP

#include <gui_generated/settingsscreen_screen/SettingsScreenViewBase.hpp>
#include <gui/settingsscreen_screen/SettingsScreenPresenter.hpp>
extern "C"
{
#include "app_backend.h"
}

class SettingsScreenView : public SettingsScreenViewBase
{
public:
    SettingsScreenView();
    virtual ~SettingsScreenView() {}
    virtual void setupScreen() override;
    virtual void tearDownScreen() override;
    void updateData(
        const AppSnapshot_t& snapshot);

    virtual void btn0Clicked() ;
    virtual void btn1Clicked() ;
    virtual void btn2Clicked() ;
    virtual void btn3Clicked() ;
    virtual void btn4Clicked() ;
    virtual void btn5Clicked() ;
    virtual void btn6Clicked() ;
    virtual void btn7Clicked() ;
    virtual void btn8Clicked() ;
    virtual void btn9Clicked() ;

    virtual void btnClearClicked() ;
    virtual void btnBackspaceClicked() ;

    virtual void btnSelectT1Clicked() ;
    virtual void btnSelectT2Clicked() ;

    virtual void btnSaveClicked() ;
    virtual void btnCancelClicked() ;

protected:
private:
    uint16_t editThreshold1;
    uint16_t editThreshold2;

    bool editingThreshold1;
    bool rfidAuthorized;
    bool snapshotInitialized;
    bool messageIsError;

    void appendDigit(uint8_t digit);

    void refreshThresholdDisplay();
    void refreshSelectionDisplay();

    void setKeypadEnabled(bool enabled);

    void showStatus(const char* text);

    static void formatFourDigits(
        touchgfx::Unicode::UnicodeChar* buffer,
        uint16_t value);
    static const uint16_t TXTTHRESHOLD1_SIZE = 8U;
    static const uint16_t TXTTHRESHOLD2_SIZE = 8U;
    static const uint16_t TXTRFIDSTATUS_SIZE = 48U;

    touchgfx::Unicode::UnicodeChar
        txtThreshold1Buffer[TXTTHRESHOLD1_SIZE];

    touchgfx::Unicode::UnicodeChar
        txtThreshold2Buffer[TXTTHRESHOLD2_SIZE];

    touchgfx::Unicode::UnicodeChar
        txtRfidStatusBuffer[TXTRFIDSTATUS_SIZE];
};

#endif // SETTINGSSCREENVIEW_HPP
