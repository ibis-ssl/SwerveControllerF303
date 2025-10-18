#include "config_mode.h"

#include "app_settings.h"
#include "debug_print.h"
#include "usart.h"
#include "stm32f3xx_hal.h"

static app_settings_t g_settings;

void config_mode_init(void)
{
  app_settings_load(&g_settings);
  p("[CFG] Loaded board_id=%lu\n", (unsigned long)g_settings.board_id);
}

uint32_t config_mode_get_board_id(void)
{
  return g_settings.board_id;
}

void config_mode_run_window(void)
{
  const uint32_t window_ms = 2000U;
  const uint32_t max_interval_ms = 2000U;
  const int required_newlines = 5;
  uint32_t start = HAL_GetTick();
  int nl_count = 0;
  uint32_t last_nl_time = start;

  p("[CFG] Press ENTER %d times within %lu ms to enter config.\n",
    required_newlines, (unsigned long)window_ms);

  while ((HAL_GetTick() - start) < window_ms) {
    uint8_t ch;
    if (!uart_rx_get_byte(&ch)) {
      continue;
    }
    if (ch == '\r' || ch == '\n') {
      uint32_t now = HAL_GetTick();
      if ((now - last_nl_time) > max_interval_ms) {
        nl_count = 0;
      }
      last_nl_time = now;
      nl_count++;
      if (nl_count >= required_newlines) {
        /* 設定モード */
        p("\n[CFG] Enter CONFIG MODE. Current board_id=%lu\n",
          (unsigned long)g_settings.board_id);
        p("[CFG] Input board_id (0-3) then ENTER: ");

        /* 入力待ちループ（空Enterは無視） */
        uint32_t wait_start = HAL_GetTick();
        char line[16];
        int len = 0;
        while (1) {
          uint8_t c2;
          if (uart_rx_get_byte(&c2)) {
            if (c2 == '\r' || c2 == '\n') {
              line[len] = '\0';
              /* 空行（空白のみ含む）なら無視して再度プロンプト */
              int has_digit = 0;
              for (int i = 0; i < len; i++) {
                if (line[i] >= '0' && line[i] <= '9') { has_digit = 1; break; }
                if (line[i] != ' ' && line[i] != '\t') { has_digit = -1; break; }
              }
              if (has_digit == 0) {
                p("\r[CFG] Input board_id (0-3) then ENTER: ");
                len = 0;
                continue;
              }

              /* 数値化 */
              int bid = -1;
              for (int i = 0; i < len; i++) {
                if (line[i] >= '0' && line[i] <= '9') {
                  if (bid < 0) bid = 0;
                  bid = bid * 10 + (line[i] - '0');
                } else if (line[i] == ' ' || line[i] == '\t') {
                  continue;
                } else {
                  bid = -1; break;
                }
              }

              if (bid < 0 || bid > 3) {
                p("\r[CFG] Invalid. Input 0-3 then ENTER: ");
                len = 0;
                continue;
              }

              /* 保存 */
              g_settings.board_id = (uint32_t)bid;
              HAL_StatusTypeDef st = app_settings_save(&g_settings);
              if (st != HAL_OK) {
                uint32_t err = HAL_FLASH_GetError();
                p("\n[CFG] Save failed. st=%ld err=0x%08lX\n",
                  (long)st, (unsigned long)err);
                return;
              }
              p("\n[CFG] Saved board_id=%ld. Rebooting...\n", (long)bid);
              HAL_Delay(100);
              NVIC_SystemReset();
            } else {
              if (len < (int)(sizeof(line) - 1)) {
                line[len++] = (char)c2;
              }
            }
          }
          if ((HAL_GetTick() - wait_start) > 15000U) {
            p("\n[CFG] Timeout. Abort.\n");
            return;
          }
        }
      }
    } else {
      /* 他のキーで連続改行カウントはリセット */
      nl_count = 0;
    }
  }
}
