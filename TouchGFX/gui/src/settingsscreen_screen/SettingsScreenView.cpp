#include <gui/settingsscreen_screen/SettingsScreenView.hpp>

#include <touchgfx/Color.hpp>
#include <touchgfx/Unicode.hpp>

SettingsScreenView::SettingsScreenView()
    : editThreshold1(1000U),
      editThreshold2(2000U),
      editingThreshold1(true),
      rfidAuthorized(false),
      snapshotInitialized(false),
      messageIsError(false)
{
}

void SettingsScreenView::setupScreen()
{
    SettingsScreenViewBase::setupScreen();

    /*
     * =====================================================
     * Liên kết TextArea với wildcard buffer
     * =====================================================
     *
     * Ba TextArea trong Designer phải sử dụng
     * Text Resource có wildcard:
     *
     * <*>
     */

    txtThreshold1.setWildcard1(
        txtThreshold1Buffer);

    txtThreshold2.setWildcard1(
        txtThreshold2Buffer);

    txtRfidStatus.setWildcard1(
        txtRfidStatusBuffer);

    /*
     * =====================================================
     * Khởi tạo trạng thái giao diện
     * =====================================================
     *
     * Đây chỉ là giá trị tạm. updateData() sẽ đọc ngưỡng
     * thật từ backend đúng một lần.
     */

    editThreshold1 = 1000U;
    editThreshold2 = 2000U;

    editingThreshold1 = true;

    rfidAuthorized = false;
    snapshotInitialized = false;
    messageIsError = false;

    /*
     * Hiển thị giá trị mặc định.
     */
    refreshThresholdDisplay();

    /*
     * T1 đang được chọn mặc định, nhưng vì RFID chưa hợp lệ
     * nên refreshSelectionDisplay() sẽ hiển thị yêu cầu
     * quét thẻ.
     */
    refreshSelectionDisplay();

    /*
     * Khóa toàn bộ bàn phím và Save.
     */
    setKeypadEnabled(false);

    /*
     * Thông báo ban đầu.
     */
    showStatus(
        "VUI LONG QUET THE");

    /*
     * Báo backend rằng SettingsScreen đang hiển thị.
     *
     * Backend chỉ bắt đầu quét RC522 sau lệnh này.
     */
    presenter->setSettingsVisible(
        true);
}

void SettingsScreenView::tearDownScreen()
{
    /*
     * Báo backend rời khỏi SettingsScreen.
     *
     * Backend sẽ:
     * - ngừng quét RC522;
     * - xóa trạng thái xác thực RFID;
     * - yêu cầu quét lại ở lần vào sau.
     */
    presenter->setSettingsVisible(
        false);

    SettingsScreenViewBase::tearDownScreen();
}

void SettingsScreenView::updateData(
    const AppSnapshot_t& snapshot)
{
    /*
     * =====================================================
     * Nhận ngưỡng backend đúng một lần
     * =====================================================
     *
     * Không lấy lại mỗi frame vì sẽ ghi đè số người dùng
     * đang nhập.
     */

    if (!snapshotInitialized)
    {
        editThreshold1 =
            snapshot.threshold_1;

        editThreshold2 =
            snapshot.threshold_2;

        snapshotInitialized = true;

        refreshThresholdDisplay();
    }

    /*
     * =====================================================
     * Theo dõi trạng thái xác thực RFID
     * =====================================================
     */

    if (rfidAuthorized !=
        snapshot.rfid_authorized)
    {
        rfidAuthorized =
            snapshot.rfid_authorized;

        messageIsError = false;

        setKeypadEnabled(
            rfidAuthorized);

        if (rfidAuthorized)
        {
            refreshSelectionDisplay();
        }
        else
        {
            showStatus(
                "VUI LONG QUET THE");
        }
    }
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

    buffer[0] =
        static_cast<
            touchgfx::Unicode::UnicodeChar>(
                '0' +
                ((value / 1000U) % 10U));

    buffer[1] =
        static_cast<
            touchgfx::Unicode::UnicodeChar>(
                '0' +
                ((value / 100U) % 10U));

    buffer[2] =
        static_cast<
            touchgfx::Unicode::UnicodeChar>(
                '0' +
                ((value / 10U) % 10U));

    buffer[3] =
        static_cast<
            touchgfx::Unicode::UnicodeChar>(
                '0' +
                (value % 10U));

    buffer[4] = 0;
}

void SettingsScreenView::
refreshThresholdDisplay()
{
    formatFourDigits(
        txtThreshold1Buffer,
        editThreshold1);

    formatFourDigits(
        txtThreshold2Buffer,
        editThreshold2);

    txtThreshold1.invalidate();
    txtThreshold2.invalidate();
}

void SettingsScreenView::showStatus(
    const char* text)
{
    if (text == nullptr)
    {
        return;
    }

    touchgfx::Unicode::fromUTF8(
        reinterpret_cast<const uint8_t*>(
            text),
        txtRfidStatusBuffer,
        TXTRFIDSTATUS_SIZE);

    txtRfidStatus.invalidate();
}

void SettingsScreenView::
refreshSelectionDisplay()
{
    if (!rfidAuthorized)
    {
        /*
         * Khi chưa được xác thực, đưa màu hai giá trị về
         * trạng thái bình thường.
         */
        txtThreshold1.setColor(
            touchgfx::Color::getColorFromRGB(
                255U,
                255U,
                255U));

        txtThreshold2.setColor(
            touchgfx::Color::getColorFromRGB(
                255U,
                255U,
                255U));

        txtThreshold1.invalidate();
        txtThreshold2.invalidate();

        showStatus(
            "VUI LONG QUET THE");

        return;
    }

    messageIsError = false;

    if (editingThreshold1)
    {
        showStatus(
            "THE HOP LE - DANG NHAP T1");

        txtThreshold1.setColor(
            touchgfx::Color::getColorFromRGB(
                0U,
                120U,
                255U));

        txtThreshold2.setColor(
            touchgfx::Color::getColorFromRGB(
                255U,
                255U,
                255U));
    }
    else
    {
        showStatus(
            "THE HOP LE - DANG NHAP T2");

        txtThreshold1.setColor(
            touchgfx::Color::getColorFromRGB(
                255U,
                255U,
                255U));

        txtThreshold2.setColor(
            touchgfx::Color::getColorFromRGB(
                0U,
                120U,
                255U));
    }

    txtThreshold1.invalidate();
    txtThreshold2.invalidate();
}

void SettingsScreenView::setKeypadEnabled(
    bool enabled)
{
    /*
     * Phím số.
     */
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

    /*
     * Phím chỉnh sửa.
     */
    btnClear.setTouchable(enabled);
    btnBackspace.setTouchable(enabled);

    /*
     * Chọn ngưỡng.
     */
    btnSelectT1.setTouchable(enabled);
    btnSelectT2.setTouchable(enabled);

    /*
     * Save chỉ được sử dụng khi RFID hợp lệ.
     */
    btnSave.setTouchable(enabled);

    /*
     * Cancel luôn hoạt động để người dùng có thể rời màn hình
     * ngay cả khi chưa quét thẻ.
     */
    btnCancel.setTouchable(true);

    /*
     * Vẽ lại các nút.
     */
    btn0.invalidate();
    btn1.invalidate();
    btn2.invalidate();
    btn3.invalidate();
    btn4.invalidate();
    btn5.invalidate();
    btn6.invalidate();
    btn7.invalidate();
    btn8.invalidate();
    btn9.invalidate();

    btnClear.invalidate();
    btnBackspace.invalidate();

    btnSelectT1.invalidate();
    btnSelectT2.invalidate();

    btnSave.invalidate();
    btnCancel.invalidate();
}

void SettingsScreenView::appendDigit(
    uint8_t digit)
{
    if (!rfidAuthorized)
    {
        showStatus(
            "VUI LONG QUET THE");

        return;
    }

    if (digit > 9U)
    {
        return;
    }

    uint16_t* currentValue = nullptr;

    if (editingThreshold1)
    {
        currentValue =
            &editThreshold1;
    }
    else
    {
        currentValue =
            &editThreshold2;
    }

    const uint32_t newValue =
        static_cast<uint32_t>(
            *currentValue) *
        10U +
        digit;

    if (newValue > 9999U)
    {
        messageIsError = true;

        showStatus(
            "GIA TRI TOI DA LA 9999");

        return;
    }

    *currentValue =
        static_cast<uint16_t>(
            newValue);

    messageIsError = false;

    refreshThresholdDisplay();
    refreshSelectionDisplay();
}

/*
 * =========================================================
 * Phím số
 * =========================================================
 */

void SettingsScreenView::btn0Clicked()
{
    appendDigit(0U);
}

void SettingsScreenView::btn1Clicked()
{
    appendDigit(1U);
}

void SettingsScreenView::btn2Clicked()
{
    appendDigit(2U);
}

void SettingsScreenView::btn3Clicked()
{
    appendDigit(3U);
}

void SettingsScreenView::btn4Clicked()
{
    appendDigit(4U);
}

void SettingsScreenView::btn5Clicked()
{
    appendDigit(5U);
}

void SettingsScreenView::btn6Clicked()
{
    appendDigit(6U);
}

void SettingsScreenView::btn7Clicked()
{
    appendDigit(7U);
}

void SettingsScreenView::btn8Clicked()
{
    appendDigit(8U);
}

void SettingsScreenView::btn9Clicked()
{
    appendDigit(9U);
}

/*
 * =========================================================
 * Clear và Backspace
 * =========================================================
 */

void SettingsScreenView::
btnClearClicked()
{
    if (!rfidAuthorized)
    {
        showStatus(
            "VUI LONG QUET THE");

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

    messageIsError = false;

    refreshThresholdDisplay();
    refreshSelectionDisplay();
}

void SettingsScreenView::
btnBackspaceClicked()
{
    if (!rfidAuthorized)
    {
        showStatus(
            "VUI LONG QUET THE");

        return;
    }

    if (editingThreshold1)
    {
        editThreshold1 =
            static_cast<uint16_t>(
                editThreshold1 / 10U);
    }
    else
    {
        editThreshold2 =
            static_cast<uint16_t>(
                editThreshold2 / 10U);
    }

    messageIsError = false;

    refreshThresholdDisplay();
    refreshSelectionDisplay();
}

/*
 * =========================================================
 * Chọn T1 hoặc T2
 * =========================================================
 */

void SettingsScreenView::
btnSelectT1Clicked()
{
    if (!rfidAuthorized)
    {
        showStatus(
            "VUI LONG QUET THE");

        return;
    }

    editingThreshold1 = true;

    refreshSelectionDisplay();
}

void SettingsScreenView::
btnSelectT2Clicked()
{
    if (!rfidAuthorized)
    {
        showStatus(
            "VUI LONG QUET THE");

        return;
    }

    editingThreshold1 = false;

    refreshSelectionDisplay();
}

/*
 * =========================================================
 * Save và Cancel
 * =========================================================
 */

void SettingsScreenView::
btnSaveClicked()
{
    if (!rfidAuthorized)
    {
        showStatus(
            "VUI LONG QUET THE");

        return;
    }

    /*
     * Điều kiện:
     *
     * 0 <= T1 < T2 <= 9999
     *
     * Hai biến là uint16_t nên không thể nhỏ hơn 0.
     */
    if (editThreshold1 >=
        editThreshold2)
    {
        messageIsError = true;

        showStatus(
            "LOI: T1 PHAI NHO HON T2");

        return;
    }

    const bool result =
        presenter->saveThreshold(
            editThreshold1,
            editThreshold2);

    if (!result)
    {
        messageIsError = true;

        showStatus(
            "LOI KHI LUU NGUONG");

        return;
    }

    messageIsError = false;

    /*
     * Backend đã lưu thành công.
     * Trở về Dashboard.
     */
    presenter->closeSettings();
}

void SettingsScreenView::
btnCancelClicked()
{
    /*
     * Không gửi giá trị đang chỉnh sửa xuống backend.
     */
    presenter->closeSettings();
}
