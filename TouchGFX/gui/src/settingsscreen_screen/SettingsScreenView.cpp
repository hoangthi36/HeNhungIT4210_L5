#include <gui/settingsscreen_screen/SettingsScreenView.hpp>

#include <touchgfx/Color.hpp>
#include <touchgfx/Unicode.hpp>

namespace
{
template <typename ButtonType>
void setButtonSelected(ButtonType& button, bool selected)
{
    const touchgfx::colortype normalBackground =
        touchgfx::Color::getColorFromRGB(0, 102, 153);
    const touchgfx::colortype pressedBackground =
        touchgfx::Color::getColorFromRGB(0, 153, 204);
    const touchgfx::colortype normalBorder = selected
        ? touchgfx::Color::getColorFromRGB(255, 215, 0)
        : touchgfx::Color::getColorFromRGB(0, 51, 102);
    const touchgfx::colortype pressedBorder = selected
        ? touchgfx::Color::getColorFromRGB(255, 255, 0)
        : touchgfx::Color::getColorFromRGB(51, 102, 153);

    button.setBoxWithBorderColors(
        normalBackground,
        pressedBackground,
        normalBorder,
        pressedBorder);
    button.invalidate();
}
}

SettingsScreenView::SettingsScreenView()
    : editThreshold1(1000U),
      editThreshold2(2000U),
      uiMode(MODE_MAIN_MENU),
      mainCursor(MAIN_SELECT_T1),
      keypadCursor(KEY_1),
      saveResultTick(0U),
      editingThreshold1(true),
      rfidAuthorized(false),
      snapshotInitialized(false),
      messageIsError(false),
      replaceOnNextDigit(true)
{
}

void SettingsScreenView::setupScreen()
{
    SettingsScreenViewBase::setupScreen();

    txtThreshold1.setWildcard1(txtThreshold1Buffer);
    txtThreshold2.setWildcard1(txtThreshold2Buffer);
    txtRfidStatus.setWildcard1(txtRfidStatusBuffer);

    txtThreshold1.setWidth(90);
    txtThreshold1.setHeight(26);
    txtThreshold2.setWidth(90);
    txtThreshold2.setHeight(26);
    txtRfidStatus.setWidth(300);
    txtRfidStatus.setHeight(28);

    txtTitle.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
    txtThreshold1.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
    txtThreshold2.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
    txtRfidStatus.setColor(touchgfx::Color::getColorFromRGB(255, 90, 90));
    txtStatus.setVisible(false);

    uiMode = MODE_MAIN_MENU;
    mainCursor = MAIN_SELECT_T1;
    keypadCursor = KEY_1;
    saveResultTick = 0U;
    editingThreshold1 = true;
    rfidAuthorized = false;
    snapshotInitialized = false;
    messageIsError = false;
    replaceOnNextDigit = true;

    refreshThresholdDisplay();
    setKeypadVisible(false);
    setKeypadEnabled(false);
    refreshCursorDisplay();

    presenter->setSettingsVisible(true);
}

void SettingsScreenView::tearDownScreen()
{
    presenter->setSettingsVisible(false);
    SettingsScreenViewBase::tearDownScreen();
}

void SettingsScreenView::updateData(const AppSnapshot_t& snapshot)
{
    if (uiMode == MODE_SAVE_SUCCESS &&
        (HAL_GetTick() - saveResultTick) >= 1500U)
    {
        presenter->closeSettings();
        return;
    }

    if (!snapshotInitialized)
    {
        editThreshold1 = snapshot.threshold_1;
        editThreshold2 = snapshot.threshold_2;
        snapshotInitialized = true;
        refreshThresholdDisplay();
    }

    if (rfidAuthorized != snapshot.rfid_authorized)
    {
        rfidAuthorized = snapshot.rfid_authorized;
        messageIsError = false;

        if (rfidAuthorized)
        {
            txtRfidStatus.setColor(
                touchgfx::Color::getColorFromRGB(40, 220, 80));

            if (uiMode == MODE_WAITING_RFID)
            {
                enterKeypad();
            }
            else
            {
                refreshCursorDisplay();
            }
        }
        else if (uiMode == MODE_WAITING_RFID)
        {
            txtRfidStatus.setColor(
                touchgfx::Color::getColorFromRGB(255, 90, 90));
            showStatus("VUI LONG QUET THE");
        }
    }
}

void SettingsScreenView::handleHardwareButton(AppUiEvent_t event)
{
    if (event == APP_UI_EVENT_SHORT_PRESS)
    {
        if (uiMode == MODE_MAIN_MENU)
        {
            mainCursor = static_cast<uint8_t>(
                (mainCursor + 1U) % MAIN_ITEM_COUNT);
            refreshCursorDisplay();
        }
        else if (uiMode == MODE_KEYPAD)
        {
            keypadCursor = static_cast<uint8_t>(
                (keypadCursor + 1U) % KEY_ITEM_COUNT);
            refreshCursorDisplay();
        }
    }
    else if (event == APP_UI_EVENT_CONFIRM)
    {
        if (uiMode == MODE_MAIN_MENU)
        {
            activateMainItem();
        }
        else if (uiMode == MODE_KEYPAD)
        {
            activateKeypadItem();
        }
    }
}

void SettingsScreenView::selectThreshold(bool selectThreshold1)
{
    editingThreshold1 = selectThreshold1;
    mainCursor = selectThreshold1 ? MAIN_SELECT_T1 : MAIN_SELECT_T2;
    refreshSelectionDisplay();

    if (rfidAuthorized)
    {
        enterKeypad();
        return;
    }

    uiMode = MODE_WAITING_RFID;
    txtRfidStatus.setColor(
        touchgfx::Color::getColorFromRGB(255, 90, 90));
    refreshCursorDisplay();
}

void SettingsScreenView::enterKeypad()
{
    uiMode = MODE_KEYPAD;
    keypadCursor = KEY_1;
    replaceOnNextDigit = true;
    setKeypadVisible(true);
    setKeypadEnabled(true);
    refreshSelectionDisplay();
    refreshCursorDisplay();
}

void SettingsScreenView::activateMainItem()
{
    switch (mainCursor)
    {
        case MAIN_SELECT_T1:
            selectThreshold(true);
            break;

        case MAIN_SELECT_T2:
            selectThreshold(false);
            break;

        case MAIN_SAVE:
            showStatus("CHON T1 HOAC T2 TRUOC");
            break;

        case MAIN_CANCEL:
            presenter->closeSettings();
            break;

        default:
            break;
    }
}

void SettingsScreenView::activateKeypadItem()
{
    switch (keypadCursor)
    {
        case KEY_1: btn1Clicked(); break;
        case KEY_2: btn2Clicked(); break;
        case KEY_3: btn3Clicked(); break;
        case KEY_4: btn4Clicked(); break;
        case KEY_5: btn5Clicked(); break;
        case KEY_6: btn6Clicked(); break;
        case KEY_7: btn7Clicked(); break;
        case KEY_8: btn8Clicked(); break;
        case KEY_9: btn9Clicked(); break;
        case KEY_0: btn0Clicked(); break;
        case KEY_CLEAR: btnClearClicked(); break;
        case KEY_BACKSPACE: btnBackspaceClicked(); break;
        case KEY_SAVE: btnSaveClicked(); break;
        case KEY_CANCEL: btnCancelClicked(); break;
        default: break;
    }
}

void SettingsScreenView::refreshCursorDisplay()
{
    setButtonSelected(btnSelectT1,
        uiMode != MODE_KEYPAD && mainCursor == MAIN_SELECT_T1);
    setButtonSelected(btnSelectT2,
        uiMode != MODE_KEYPAD && mainCursor == MAIN_SELECT_T2);
    setButtonSelected(btnSave,
        (uiMode == MODE_MAIN_MENU && mainCursor == MAIN_SAVE) ||
        (uiMode == MODE_KEYPAD && keypadCursor == KEY_SAVE));
    setButtonSelected(btnCancel,
        (uiMode == MODE_MAIN_MENU && mainCursor == MAIN_CANCEL) ||
        (uiMode == MODE_KEYPAD && keypadCursor == KEY_CANCEL));

    setButtonSelected(btn1, uiMode == MODE_KEYPAD && keypadCursor == KEY_1);
    setButtonSelected(btn2, uiMode == MODE_KEYPAD && keypadCursor == KEY_2);
    setButtonSelected(btn3, uiMode == MODE_KEYPAD && keypadCursor == KEY_3);
    setButtonSelected(btn4, uiMode == MODE_KEYPAD && keypadCursor == KEY_4);
    setButtonSelected(btn5, uiMode == MODE_KEYPAD && keypadCursor == KEY_5);
    setButtonSelected(btn6, uiMode == MODE_KEYPAD && keypadCursor == KEY_6);
    setButtonSelected(btn7, uiMode == MODE_KEYPAD && keypadCursor == KEY_7);
    setButtonSelected(btn8, uiMode == MODE_KEYPAD && keypadCursor == KEY_8);
    setButtonSelected(btn9, uiMode == MODE_KEYPAD && keypadCursor == KEY_9);
    setButtonSelected(btn0, uiMode == MODE_KEYPAD && keypadCursor == KEY_0);
    setButtonSelected(btnClear,
        uiMode == MODE_KEYPAD && keypadCursor == KEY_CLEAR);
    setButtonSelected(btnBackspace,
        uiMode == MODE_KEYPAD && keypadCursor == KEY_BACKSPACE);

    if (uiMode == MODE_WAITING_RFID)
    {
        showStatus("VUI LONG QUET THE");
        return;
    }

    if (uiMode == MODE_MAIN_MENU)
    {
        static const char* const labels[MAIN_ITEM_COUNT] =
        {
            "CHON T1 - GIU 1.5S",
            "CHON T2 - GIU 1.5S",
            "LUU - GIU 1.5S",
            "HUY - GIU 1.5S"
        };
        showStatus(labels[mainCursor]);
        return;
    }

    static const char* const keypadLabels[KEY_ITEM_COUNT] =
    {
        "PHIM 1", "PHIM 2", "PHIM 3", "PHIM 4", "PHIM 5",
        "PHIM 6", "PHIM 7", "PHIM 8", "PHIM 9", "PHIM 0",
        "XOA HET", "XOA 1 SO", "LUU", "HUY"
    };
    showStatus(keypadLabels[keypadCursor]);
}

void SettingsScreenView::formatFourDigits(
    touchgfx::Unicode::UnicodeChar* buffer,
    uint16_t value)
{
    if (buffer == nullptr)
    {
        return;
    }

    if (value > 9999U)
    {
        value = 9999U;
    }

    touchgfx::Unicode::snprintf(buffer, 8U, "%u", value);
}

void SettingsScreenView::refreshThresholdDisplay()
{
    formatFourDigits(txtThreshold1Buffer, editThreshold1);
    formatFourDigits(txtThreshold2Buffer, editThreshold2);
    txtThreshold1.invalidate();
    txtThreshold2.invalidate();
}

void SettingsScreenView::showStatus(const char* text)
{
    if (text == nullptr)
    {
        return;
    }

    touchgfx::Unicode::fromUTF8(
        reinterpret_cast<const uint8_t*>(text),
        txtRfidStatusBuffer,
        TXTRFIDSTATUS_SIZE);
    txtRfidStatus.invalidate();
}

void SettingsScreenView::refreshSelectionDisplay()
{
    txtThreshold1.setColor(touchgfx::Color::getColorFromRGB(
        editingThreshold1 ? 0U : 255U,
        editingThreshold1 ? 180U : 255U,
        255U));
    txtThreshold2.setColor(touchgfx::Color::getColorFromRGB(
        editingThreshold1 ? 255U : 0U,
        editingThreshold1 ? 255U : 180U,
        255U));
    txtThreshold1.invalidate();
    txtThreshold2.invalidate();
}

void SettingsScreenView::setKeypadEnabled(bool enabled)
{
    btn0.setTouchable(enabled);
    btn1.setTouchable(enabled);
    btn2.setTouchable(enabled);
    btn3.setTouchable(enabled);
    btn4.setTouchable(enabled);
    btn5.setTouchable(enabled);
    btn6.setTouchable(enabled);
    btn7.setTouchable(enabled);
    btn8.setTouchable(enabled);
    btn9.setTouchable(enabled);
    btnClear.setTouchable(enabled);
    btnBackspace.setTouchable(enabled);
    btnSave.setTouchable(enabled);
    btnCancel.setTouchable(true);
}

void SettingsScreenView::setKeypadVisible(bool visible)
{
    btn0.setVisible(visible);
    btn1.setVisible(visible);
    btn2.setVisible(visible);
    btn3.setVisible(visible);
    btn4.setVisible(visible);
    btn5.setVisible(visible);
    btn6.setVisible(visible);
    btn7.setVisible(visible);
    btn8.setVisible(visible);
    btn9.setVisible(visible);
    btnClear.setVisible(visible);
    btnBackspace.setVisible(visible);
}

void SettingsScreenView::appendDigit(uint8_t digit)
{
    if (!rfidAuthorized || digit > 9U)
    {
        return;
    }

    uint16_t* value = editingThreshold1
        ? &editThreshold1
        : &editThreshold2;

    uint32_t newValue = digit;
    if (!replaceOnNextDigit)
    {
        newValue = static_cast<uint32_t>(*value) * 10U + digit;
    }

    if (newValue > 9999U)
    {
        messageIsError = true;
        showStatus("GIA TRI TOI DA 9999");
        return;
    }

    *value = static_cast<uint16_t>(newValue);
    replaceOnNextDigit = false;
    messageIsError = false;
    refreshThresholdDisplay();
}

void SettingsScreenView::btn0Clicked() { appendDigit(0U); }
void SettingsScreenView::btn1Clicked() { appendDigit(1U); }
void SettingsScreenView::btn2Clicked() { appendDigit(2U); }
void SettingsScreenView::btn3Clicked() { appendDigit(3U); }
void SettingsScreenView::btn4Clicked() { appendDigit(4U); }
void SettingsScreenView::btn5Clicked() { appendDigit(5U); }
void SettingsScreenView::btn6Clicked() { appendDigit(6U); }
void SettingsScreenView::btn7Clicked() { appendDigit(7U); }
void SettingsScreenView::btn8Clicked() { appendDigit(8U); }
void SettingsScreenView::btn9Clicked() { appendDigit(9U); }

void SettingsScreenView::btnClearClicked()
{
    if (!rfidAuthorized)
    {
        return;
    }

    if (editingThreshold1)
    {
        editThreshold1 = 0U;
    }
    else
    {
        editThreshold2 = 0U;
    }
    replaceOnNextDigit = false;
    refreshThresholdDisplay();
}

void SettingsScreenView::btnBackspaceClicked()
{
    if (!rfidAuthorized)
    {
        return;
    }

    uint16_t* value = editingThreshold1
        ? &editThreshold1
        : &editThreshold2;
    *value = static_cast<uint16_t>(*value / 10U);
    replaceOnNextDigit = false;
    refreshThresholdDisplay();
}

void SettingsScreenView::btnSelectT1Clicked()
{
    selectThreshold(true);
}

void SettingsScreenView::btnSelectT2Clicked()
{
    selectThreshold(false);
}

void SettingsScreenView::btnSaveClicked()
{
    if (!rfidAuthorized)
    {
        txtRfidStatus.setColor(
            touchgfx::Color::getColorFromRGB(255, 90, 90));
        showStatus("VUI LONG QUET THE");
        return;
    }

    if (editThreshold1 >= editThreshold2)
    {
        messageIsError = true;
        txtRfidStatus.setColor(
            touchgfx::Color::getColorFromRGB(255, 90, 90));
        showStatus("LOI: T1 PHAI NHO HON T2");
        return;
    }

    if (!presenter->saveThreshold(editThreshold1, editThreshold2))
    {
        messageIsError = true;
        txtRfidStatus.setColor(
            touchgfx::Color::getColorFromRGB(255, 90, 90));
        showStatus("LOI KHI LUU NGUONG");
        return;
    }

    messageIsError = false;
    uiMode = MODE_SAVE_SUCCESS;
    saveResultTick = HAL_GetTick();
    setKeypadVisible(false);
    txtRfidStatus.setColor(
        touchgfx::Color::getColorFromRGB(40, 220, 80));
    showStatus("LUU THANH CONG");
}

void SettingsScreenView::btnCancelClicked()
{
    presenter->closeSettings();
}
