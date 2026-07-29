#include "lcd_ili9341.h"
#include "main.h"

#define LCD_CS_PORT          GPIOC
#define LCD_CS_PIN           GPIO_PIN_2
#define LCD_DC_PORT          GPIOD
#define LCD_DC_PIN           GPIO_PIN_13
#define LCD_RDX_PORT         GPIOD
#define LCD_RDX_PIN          GPIO_PIN_12
#define GYRO_CS_PORT         GPIOC
#define GYRO_CS_PIN          GPIO_PIN_1
#define LCD_SPI_TIMEOUT_MS   100U

static SPI_HandleTypeDef lcd_spi5;

static bool LCD_SPI_WriteByte(uint8_t value)
{
    return HAL_SPI_Transmit(&lcd_spi5, &value, 1U,
                            LCD_SPI_TIMEOUT_MS) == HAL_OK;
}

static bool LCD_WriteCommand(uint8_t command)
{
    bool ok;

    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
    ok = LCD_SPI_WriteByte(command);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);

    return ok;
}

static bool LCD_WriteData(uint8_t data)
{
    bool ok;

    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
    ok = LCD_SPI_WriteByte(data);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);

    return ok;
}

static bool LCD_WriteRegister(uint8_t command,
                              const uint8_t *data,
                              uint32_t length)
{
    uint32_t index;

    if (!LCD_WriteCommand(command)) {
        return false;
    }

    for (index = 0U; index < length; index++) {
        if (!LCD_WriteData(data[index])) {
            return false;
        }
    }

    return true;
}

static bool LCD_SPI5_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_SPI5_CLK_ENABLE();

    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    gpio.Pin = LCD_CS_PIN | GYRO_CS_PIN;
    HAL_GPIO_Init(GPIOC, &gpio);

    gpio.Pin = LCD_DC_PIN | LCD_RDX_PIN;
    HAL_GPIO_Init(GPIOD, &gpio);

    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GYRO_CS_PORT, GYRO_CS_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_RDX_PORT, LCD_RDX_PIN, GPIO_PIN_SET);

    gpio.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_PULLDOWN;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = GPIO_AF5_SPI5;
    HAL_GPIO_Init(GPIOF, &gpio);

    lcd_spi5.Instance = SPI5;
    lcd_spi5.Init.Mode = SPI_MODE_MASTER;
    lcd_spi5.Init.Direction = SPI_DIRECTION_2LINES;
    lcd_spi5.Init.DataSize = SPI_DATASIZE_8BIT;
    lcd_spi5.Init.CLKPolarity = SPI_POLARITY_LOW;
    lcd_spi5.Init.CLKPhase = SPI_PHASE_1EDGE;
    lcd_spi5.Init.NSS = SPI_NSS_SOFT;
    lcd_spi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    lcd_spi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
    lcd_spi5.Init.TIMode = SPI_TIMODE_DISABLE;
    lcd_spi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    lcd_spi5.Init.CRCPolynomial = 7U;

    return HAL_SPI_Init(&lcd_spi5) == HAL_OK;
}

bool LCD_ILI9341_Init(void)
{
    static const uint8_t command_ca[] = {0xC3, 0x08, 0x50};
    static const uint8_t power_b[] = {0x00, 0xC1, 0x30};
    static const uint8_t power_sequence[] = {0x64, 0x03, 0x12, 0x81};
    static const uint8_t timing_a[] = {0x85, 0x00, 0x78};
    static const uint8_t power_a[] = {0x39, 0x2C, 0x00, 0x34, 0x02};
    static const uint8_t timing_b[] = {0x00, 0x00};
    static const uint8_t frame_rate[] = {0x00, 0x1B};
    static const uint8_t display_control_a[] = {0x0A, 0xA2};
    static const uint8_t vcom_a[] = {0x45, 0x15};
    static const uint8_t display_control_b[] = {0x0A, 0xA7, 0x27, 0x04};
    static const uint8_t column_address[] = {0x00, 0x00, 0x00, 0xEF};
    static const uint8_t page_address[] = {0x00, 0x00, 0x01, 0x3F};
    static const uint8_t interface_control[] = {0x01, 0x00, 0x06};
    static const uint8_t positive_gamma[] = {
        0x0F, 0x29, 0x24, 0x0C, 0x0E, 0x09, 0x4E, 0x78,
        0x3C, 0x09, 0x13, 0x05, 0x17, 0x11, 0x00};
    static const uint8_t negative_gamma[] = {
        0x00, 0x16, 0x1B, 0x04, 0x11, 0x07, 0x31, 0x33,
        0x42, 0x05, 0x0C, 0x0A, 0x28, 0x2F, 0x0F};
    static const uint8_t pump_ratio[] = {0x20};
    static const uint8_t power_1[] = {0x10};
    static const uint8_t power_2[] = {0x10};
    static const uint8_t vcom_2[] = {0x90};
    static const uint8_t memory_access[] = {0xC8};
    static const uint8_t gamma_3_enable[] = {0x00};
    static const uint8_t rgb_interface[] = {0xC2};
    static const uint8_t gamma_curve[] = {0x01};

    if (!LCD_SPI5_Init()) return false;
    if (!LCD_WriteRegister(0xCA, command_ca, sizeof(command_ca))) return false;
    if (!LCD_WriteRegister(0xCF, power_b, sizeof(power_b))) return false;
    if (!LCD_WriteRegister(0xED, power_sequence, sizeof(power_sequence))) return false;
    if (!LCD_WriteRegister(0xE8, timing_a, sizeof(timing_a))) return false;
    if (!LCD_WriteRegister(0xCB, power_a, sizeof(power_a))) return false;
    if (!LCD_WriteRegister(0xF7, pump_ratio, sizeof(pump_ratio))) return false;
    if (!LCD_WriteRegister(0xEA, timing_b, sizeof(timing_b))) return false;
    if (!LCD_WriteRegister(0xB1, frame_rate, sizeof(frame_rate))) return false;
    if (!LCD_WriteRegister(0xB6, display_control_a, sizeof(display_control_a))) return false;
    if (!LCD_WriteRegister(0xC0, power_1, sizeof(power_1))) return false;
    if (!LCD_WriteRegister(0xC1, power_2, sizeof(power_2))) return false;
    if (!LCD_WriteRegister(0xC5, vcom_a, sizeof(vcom_a))) return false;
    if (!LCD_WriteRegister(0xC7, vcom_2, sizeof(vcom_2))) return false;
    if (!LCD_WriteRegister(0x36, memory_access, sizeof(memory_access))) return false;
    if (!LCD_WriteRegister(0xF2, gamma_3_enable, sizeof(gamma_3_enable))) return false;
    if (!LCD_WriteRegister(0xB0, rgb_interface, sizeof(rgb_interface))) return false;
    if (!LCD_WriteRegister(0xB6, display_control_b, sizeof(display_control_b))) return false;
    if (!LCD_WriteRegister(0x2A, column_address, sizeof(column_address))) return false;
    if (!LCD_WriteRegister(0x2B, page_address, sizeof(page_address))) return false;
    if (!LCD_WriteRegister(0xF6, interface_control, sizeof(interface_control))) return false;
    if (!LCD_WriteCommand(0x2C)) return false;
    HAL_Delay(200U);
    if (!LCD_WriteRegister(0x26, gamma_curve, sizeof(gamma_curve))) return false;
    if (!LCD_WriteRegister(0xE0, positive_gamma, sizeof(positive_gamma))) return false;
    if (!LCD_WriteRegister(0xE1, negative_gamma, sizeof(negative_gamma))) return false;
    if (!LCD_WriteCommand(0x11)) return false;
    HAL_Delay(200U);
    if (!LCD_WriteCommand(0x29)) return false;
    return LCD_WriteCommand(0x2C);
}
