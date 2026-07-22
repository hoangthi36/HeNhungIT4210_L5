#ifndef MFRC522_H
#define MFRC522_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

#include <stdbool.h>
#include <stdint.h>

/*
 * UID phổ biến:
 * 4 byte, 7 byte hoặc 10 byte.
 *
 * Driver dưới đây đọc UID CL1 4 byte,
 * phù hợp với phần lớn thẻ/tag RC522 thông dụng.
 */
#define MFRC522_MAX_UID_SIZE 10U

typedef struct
{
    uint8_t uid[MFRC522_MAX_UID_SIZE];
    uint8_t uid_size;
    bool valid;

} MFRC522_Card_t;

/*
 * Khởi tạo MFRC522.
 *
 * SPI4 và GPIO phải được CubeMX khởi tạo trước.
 */
bool MFRC522_Init(void);

/*
 * Kiểm tra module có phản hồi hay không.
 */
bool MFRC522_CheckConnection(void);

/*
 * Kiểm tra có thẻ nằm trong vùng đọc hay không.
 */
bool MFRC522_IsCardPresent(void);

/*
 * Đọc UID thẻ.
 */
bool MFRC522_ReadCard(MFRC522_Card_t* card);

/*
 * Đưa thẻ về trạng thái HALT.
 */
void MFRC522_Halt(void);

#ifdef __cplusplus
}
#endif

#endif
