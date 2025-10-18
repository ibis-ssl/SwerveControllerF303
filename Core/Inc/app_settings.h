#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <stdint.h>

#include "stm32f3xx_hal.h"

typedef struct
{
  uint32_t magic;    /* 'CFG1' */
  uint16_t version;  /* 構造体バージョン */
  uint16_t reserved; /* アライメント用 */
  uint32_t board_id; /* 0..3 */
  uint32_t reserved2[4];
} app_settings_t;

#define APP_SETTINGS_MAGIC (0x31464743u) /* 'CFG1' little-endian */
#define APP_SETTINGS_VER (0x0001u)

/* 読み込み（無効時は既定値を返す） */
void app_settings_load(app_settings_t * out);

/* 書き込み（全領域消去→先頭へ書込） */
HAL_StatusTypeDef app_settings_save(const app_settings_t * in);

#endif /* APP_SETTINGS_H */
