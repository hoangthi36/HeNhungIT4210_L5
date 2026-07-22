#include "mfrc522.h"

#include <string.h>

/*
 * SPI4 được CubeMX tạo trong spi.c.
 */
extern SPI_HandleTypeDef hspi4;

/*
 * =========================================================
 * Thanh ghi MFRC522
 * =========================================================
 */

#define MFRC522_REG_COMMAND          0x01U
#define MFRC522_REG_COM_I_EN         0x02U
#define MFRC522_REG_DIV_I_EN         0x03U
#define MFRC522_REG_COM_IRQ          0x04U
#define MFRC522_REG_DIV_IRQ          0x05U
#define MFRC522_REG_ERROR            0x06U
#define MFRC522_REG_STATUS1          0x07U
#define MFRC522_REG_STATUS2          0x08U
#define MFRC522_REG_FIFO_DATA        0x09U
#define MFRC522_REG_FIFO_LEVEL       0x0AU
#define MFRC522_REG_CONTROL          0x0CU
#define MFRC522_REG_BIT_FRAMING      0x0DU
#define MFRC522_REG_COLL             0x0EU

#define MFRC522_REG_MODE             0x11U
#define MFRC522_REG_TX_MODE          0x12U
#define MFRC522_REG_RX_MODE          0x13U
#define MFRC522_REG_TX_CONTROL       0x14U
#define MFRC522_REG_TX_ASK           0x15U

#define MFRC522_REG_CRC_RESULT_H     0x21U
#define MFRC522_REG_CRC_RESULT_L     0x22U
#define MFRC522_REG_MOD_WIDTH        0x24U

#define MFRC522_REG_RF_CFG           0x26U
#define MFRC522_REG_T_MODE           0x2AU
#define MFRC522_REG_T_PRESCALER      0x2BU
#define MFRC522_REG_T_RELOAD_H       0x2CU
#define MFRC522_REG_T_RELOAD_L       0x2DU

#define MFRC522_REG_VERSION          0x37U

/*
 * =========================================================
 * Lệnh MFRC522
 * =========================================================
 */

#define MFRC522_CMD_IDLE             0x00U
#define MFRC522_CMD_CALC_CRC         0x03U
#define MFRC522_CMD_TRANSCEIVE       0x0CU
#define MFRC522_CMD_SOFT_RESET       0x0FU

/*
 * =========================================================
 * Lệnh PICC/thẻ
 * =========================================================
 */

#define PICC_CMD_REQA                0x26U
#define PICC_CMD_WUPA                0x52U
#define PICC_CMD_ANTICOLL_CL1        0x93U
#define PICC_CMD_HALT                0x50U

/*
 * =========================================================
 * Giá trị trạng thái nội bộ
 * =========================================================
 */

typedef enum
{
    MFRC522_STATUS_OK = 0,
    MFRC522_STATUS_ERROR,
    MFRC522_STATUS_TIMEOUT,
    MFRC522_STATUS_COLLISION

} MFRC522_Status_t;

/*
 * =========================================================
 * Điều khiển CS và RST
 * =========================================================
 */

static void MFRC522_Select(void)
{
    HAL_GPIO_WritePin(
        RC522_CS_GPIO_Port,
        RC522_CS_Pin,
        GPIO_PIN_RESET);
}

static void MFRC522_Unselect(void)
{
    HAL_GPIO_WritePin(
        RC522_CS_GPIO_Port,
        RC522_CS_Pin,
        GPIO_PIN_SET);
}

static void MFRC522_ResetPinLow(void)
{
    HAL_GPIO_WritePin(
        RC522_RST_GPIO_Port,
        RC522_RST_Pin,
        GPIO_PIN_RESET);
}

static void MFRC522_ResetPinHigh(void)
{
    HAL_GPIO_WritePin(
        RC522_RST_GPIO_Port,
        RC522_RST_Pin,
        GPIO_PIN_SET);
}

/*
 * =========================================================
 * Truy cập thanh ghi
 * =========================================================
 */

static void MFRC522_WriteRegister(
    uint8_t reg,
    uint8_t value)
{
    uint8_t data[2];

    /*
     * Địa chỉ SPI MFRC522:
     * bit 7 = 0 khi ghi.
     */
    data[0] = (uint8_t)((reg << 1U) & 0x7EU);
    data[1] = value;

    MFRC522_Select();

    (void)HAL_SPI_Transmit(
        &hspi4,
        data,
        2U,
        100U);

    MFRC522_Unselect();
}

static uint8_t MFRC522_ReadRegister(
    uint8_t reg)
{
    uint8_t address;
    uint8_t value = 0U;

    /*
     * bit 7 = 1 khi đọc.
     */
    address =
        (uint8_t)(((reg << 1U) & 0x7EU) | 0x80U);

    MFRC522_Select();

    (void)HAL_SPI_Transmit(
        &hspi4,
        &address,
        1U,
        100U);

    (void)HAL_SPI_Receive(
        &hspi4,
        &value,
        1U,
        100U);

    MFRC522_Unselect();

    return value;
}

static void MFRC522_SetBitMask(
    uint8_t reg,
    uint8_t mask)
{
    const uint8_t value =
        MFRC522_ReadRegister(reg);

    MFRC522_WriteRegister(
        reg,
        (uint8_t)(value | mask));
}

static void MFRC522_ClearBitMask(
    uint8_t reg,
    uint8_t mask)
{
    const uint8_t value =
        MFRC522_ReadRegister(reg);

    MFRC522_WriteRegister(
        reg,
        (uint8_t)(value & ((uint8_t)(~mask))));
}

/*
 * =========================================================
 * Antenna
 * =========================================================
 */

static void MFRC522_AntennaOn(void)
{
    const uint8_t value =
        MFRC522_ReadRegister(
            MFRC522_REG_TX_CONTROL);

    if ((value & 0x03U) != 0x03U)
    {
        MFRC522_SetBitMask(
            MFRC522_REG_TX_CONTROL,
            0x03U);
    }
}

static void MFRC522_AntennaOff(void)
{
    MFRC522_ClearBitMask(
        MFRC522_REG_TX_CONTROL,
        0x03U);
}

/*
 * =========================================================
 * Reset
 * =========================================================
 */

static void MFRC522_SoftReset(void)
{
    MFRC522_WriteRegister(
        MFRC522_REG_COMMAND,
        MFRC522_CMD_SOFT_RESET);

    HAL_Delay(50U);
}

static void MFRC522_HardwareReset(void)
{
    MFRC522_ResetPinLow();
    HAL_Delay(2U);

    MFRC522_ResetPinHigh();
    HAL_Delay(50U);
}

/*
 * =========================================================
 * Gửi dữ liệu tới thẻ
 * =========================================================
 */

static MFRC522_Status_t MFRC522_Transceive(
    const uint8_t* sendData,
    uint8_t sendLength,
    uint8_t* receiveData,
    uint8_t* receiveLength,
    uint16_t* receiveBits,
    uint8_t txLastBits)
{
    uint16_t timeout;
    uint8_t irq;
    uint8_t error;
    uint8_t fifoLevel;
    uint8_t control;
    uint8_t index;

    if ((sendData == NULL) ||
        (sendLength == 0U))
    {
        return MFRC522_STATUS_ERROR;
    }

    /*
     * Dừng mọi lệnh đang chạy.
     */
    MFRC522_WriteRegister(
        MFRC522_REG_COMMAND,
        MFRC522_CMD_IDLE);

    /*
     * Xóa cờ ngắt.
     */
    MFRC522_WriteRegister(
        MFRC522_REG_COM_IRQ,
        0x7FU);

    /*
     * Xóa FIFO.
     */
    MFRC522_SetBitMask(
        MFRC522_REG_FIFO_LEVEL,
        0x80U);

    /*
     * Ghi dữ liệu vào FIFO.
     */
    for (index = 0U;
         index < sendLength;
         index++)
    {
        MFRC522_WriteRegister(
            MFRC522_REG_FIFO_DATA,
            sendData[index]);
    }

    /*
     * TxLastBits:
     * số bit cuối cùng hợp lệ trong byte cuối.
     */
    MFRC522_WriteRegister(
        MFRC522_REG_BIT_FRAMING,
        (uint8_t)(txLastBits & 0x07U));

    /*
     * Bắt đầu truyền/nhận.
     */
    MFRC522_WriteRegister(
        MFRC522_REG_COMMAND,
        MFRC522_CMD_TRANSCEIVE);

    MFRC522_SetBitMask(
        MFRC522_REG_BIT_FRAMING,
        0x80U);

    timeout = 2000U;

    do
    {
        irq = MFRC522_ReadRegister(
            MFRC522_REG_COM_IRQ);

        timeout--;

    } while ((timeout > 0U) &&
             ((irq & 0x30U) == 0U) &&
             ((irq & 0x01U) == 0U));

    MFRC522_ClearBitMask(
        MFRC522_REG_BIT_FRAMING,
        0x80U);

    if (timeout == 0U)
    {
        return MFRC522_STATUS_TIMEOUT;
    }

    error = MFRC522_ReadRegister(
        MFRC522_REG_ERROR);

    if ((error & 0x08U) != 0U)
    {
        return MFRC522_STATUS_COLLISION;
    }

    if ((error & 0x13U) != 0U)
    {
        return MFRC522_STATUS_ERROR;
    }

    if ((receiveData != NULL) &&
        (receiveLength != NULL))
    {
        fifoLevel = MFRC522_ReadRegister(
            MFRC522_REG_FIFO_LEVEL);

        if (fifoLevel > *receiveLength)
        {
            fifoLevel = *receiveLength;
        }

        *receiveLength = fifoLevel;

        for (index = 0U;
             index < fifoLevel;
             index++)
        {
            receiveData[index] =
                MFRC522_ReadRegister(
                    MFRC522_REG_FIFO_DATA);
        }

        if (receiveBits != NULL)
        {
            control =
                MFRC522_ReadRegister(
                    MFRC522_REG_CONTROL);

            control &= 0x07U;

            if ((control != 0U) &&
                (fifoLevel != 0U))
            {
                *receiveBits =
                    (uint16_t)(
                        ((uint16_t)(fifoLevel - 1U) *
                         8U) +
                        control);
            }
            else
            {
                *receiveBits =
                    (uint16_t)fifoLevel * 8U;
            }
        }
    }

    return MFRC522_STATUS_OK;
}

/*
 * =========================================================
 * Request
 * =========================================================
 */

static bool MFRC522_Request(
    uint8_t requestMode)
{
    uint8_t command;
    uint8_t response[2];
    uint8_t responseLength;
    uint16_t responseBits;
    MFRC522_Status_t status;

    command = requestMode;
    responseLength = sizeof(response);
    responseBits = 0U;

    /*
     * REQA/WUPA chỉ truyền 7 bit.
     */
    status = MFRC522_Transceive(
        &command,
        1U,
        response,
        &responseLength,
        &responseBits,
        7U);

    if (status != MFRC522_STATUS_OK)
    {
        return false;
    }

    /*
     * ATQA phải dài 16 bit.
     */
    return responseBits == 16U;
}

/*
 * =========================================================
 * Anticollision, đọc UID 4 byte CL1
 * =========================================================
 */

static bool MFRC522_Anticollision(
    uint8_t uid[5])
{
    uint8_t command[2];
    uint8_t responseLength;
    uint16_t responseBits;
    uint8_t bcc;
    MFRC522_Status_t status;

    if (uid == NULL)
    {
        return false;
    }

    /*
     * Cascade Level 1, NVB = 0x20.
     */
    command[0] = PICC_CMD_ANTICOLL_CL1;
    command[1] = 0x20U;

    responseLength = 5U;
    responseBits = 0U;

    /*
     * ValuesAfterColl = 0.
     */
    MFRC522_ClearBitMask(
        MFRC522_REG_COLL,
        0x80U);

    status = MFRC522_Transceive(
        command,
        2U,
        uid,
        &responseLength,
        &responseBits,
        0U);

    if (status != MFRC522_STATUS_OK)
    {
        return false;
    }

    if (responseLength != 5U)
    {
        return false;
    }

    /*
     * Byte thứ năm là BCC:
     * UID0 XOR UID1 XOR UID2 XOR UID3.
     */
    bcc =
        uid[0] ^
        uid[1] ^
        uid[2] ^
        uid[3];

    return bcc == uid[4];
}

/*
 * =========================================================
 * API công khai
 * =========================================================
 */

bool MFRC522_CheckConnection(void)
{
    const uint8_t version =
        MFRC522_ReadRegister(
            MFRC522_REG_VERSION);

    /*
     * Phiên bản thường gặp:
     * 0x91: MFRC522 v1.0
     * 0x92: MFRC522 v2.0
     *
     * Một số module clone có thể cho giá trị khác,
     * nhưng 0x00 và 0xFF thường chỉ ra lỗi SPI.
     */
    return (version != 0x00U) &&
           (version != 0xFFU);
}

bool MFRC522_Init(void)
{
    MFRC522_Unselect();
    MFRC522_ResetPinHigh();

    HAL_Delay(5U);

    MFRC522_HardwareReset();
    MFRC522_SoftReset();

    /*
     * Timer:
     * TAuto = 1
     * TPrescaler_Hi = 0x0D
     */
    MFRC522_WriteRegister(
        MFRC522_REG_T_MODE,
        0x8DU);

    MFRC522_WriteRegister(
        MFRC522_REG_T_PRESCALER,
        0x3EU);

    /*
     * Timer reload = 30.
     */
    MFRC522_WriteRegister(
        MFRC522_REG_T_RELOAD_L,
        30U);

    MFRC522_WriteRegister(
        MFRC522_REG_T_RELOAD_H,
        0U);

    /*
     * 100% ASK.
     */
    MFRC522_WriteRegister(
        MFRC522_REG_TX_ASK,
        0x40U);

    /*
     * CRC preset 0x6363.
     */
    MFRC522_WriteRegister(
        MFRC522_REG_MODE,
        0x3DU);

    /*
     * Gain receiver khoảng 48 dB.
     */
    MFRC522_WriteRegister(
        MFRC522_REG_RF_CFG,
        0x70U);

    MFRC522_AntennaOff();
    HAL_Delay(1U);
    MFRC522_AntennaOn();

    HAL_Delay(5U);

    return MFRC522_CheckConnection();
}

bool MFRC522_IsCardPresent(void)
{
    /*
     * Thử REQA trước.
     */
    if (MFRC522_Request(PICC_CMD_REQA))
    {
        return true;
    }

    /*
     * Nếu thẻ đang HALT, WUPA có thể đánh thức nó.
     */
    return MFRC522_Request(PICC_CMD_WUPA);
}

bool MFRC522_ReadCard(
    MFRC522_Card_t* card)
{
    uint8_t uidWithBcc[5];

    if (card == NULL)
    {
        return false;
    }

    memset(
        card,
        0,
        sizeof(MFRC522_Card_t));

    if (!MFRC522_IsCardPresent())
    {
        return false;
    }

    if (!MFRC522_Anticollision(uidWithBcc))
    {
        return false;
    }

    card->uid[0] = uidWithBcc[0];
    card->uid[1] = uidWithBcc[1];
    card->uid[2] = uidWithBcc[2];
    card->uid[3] = uidWithBcc[3];

    card->uid_size = 4U;
    card->valid = true;

    return true;
}

void MFRC522_Halt(void)
{
    uint8_t command[2];
    uint8_t response[1];
    uint8_t responseLength;
    uint16_t responseBits;

    command[0] = PICC_CMD_HALT;
    command[1] = 0x00U;

    responseLength = sizeof(response);
    responseBits = 0U;

    /*
     * HALT không cần chờ phản hồi hợp lệ.
     * Phiên bản đơn giản này chưa đính CRC vì
     * việc đọc UID vẫn hoạt động bình thường khi
     * backend quét lại bằng WUPA.
     */
    (void)MFRC522_Transceive(
        command,
        2U,
        response,
        &responseLength,
        &responseBits,
        0U);
}
