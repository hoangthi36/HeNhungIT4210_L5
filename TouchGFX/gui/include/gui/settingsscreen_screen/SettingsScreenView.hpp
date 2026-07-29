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

    void handleHardwareButton(
        AppUiEvent_t event);

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
    enum UiMode
    {
        MODE_MAIN_MENU,
        MODE_WAITING_RFID,
        MODE_KEYPAD,
        MODE_SAVE_SUCCESS
    };

    enum MainItem
    {
        MAIN_SELECT_T1 = 0,
        MAIN_SELECT_T2,
        MAIN_SAVE,
        MAIN_CANCEL,
        MAIN_ITEM_COUNT
    };

    enum KeypadItem
    {
        KEY_1 = 0,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,
        KEY_0,
        KEY_CLEAR,
        KEY_BACKSPACE,
        KEY_SAVE,
        KEY_CANCEL,
        KEY_ITEM_COUNT
    };

    uint16_t editThreshold1;
    uint16_t editThreshold2;

    UiMode uiMode;
    uint8_t mainCursor;
    uint8_t keypadCursor;
    uint32_t saveResultTick;

    bool editingThreshold1;
    bool rfidAuthorized;
    bool snapshotInitialized;
    bool messageIsError;
    bool replaceOnNextDigit;

    void appendDigit(uint8_t digit);

    void refreshThresholdDisplay();
    void refreshSelectionDisplay();
    void refreshCursorDisplay();

    void setKeypadEnabled(bool enabled);
    void setKeypadVisible(bool visible);

    void selectThreshold(bool selectThreshold1);
    void enterKeypad();
    void activateMainItem();
    void activateKeypadItem();

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
