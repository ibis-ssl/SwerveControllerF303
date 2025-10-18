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

/* --- internal helpers ---------------------------------------------------- */
static void print_enter_hint(uint32_t window_ms, int required_newlines)
{
  p("[CFG] Press ENTER %d times within %lu ms to enter config.\n",
    required_newlines, (unsigned long)window_ms);
}

static int wait_enter_trigger(uint32_t window_ms, uint32_t max_interval_ms, int required_newlines)
{
  const uint32_t start = HAL_GetTick();
  uint32_t last_nl = start;
  int nl_count = 0;

  while ((HAL_GetTick() - start) < window_ms) {
    uint8_t ch;
    if (!uart_rx_get_byte(&ch)) {
      continue;
    }
    if (ch == '\r' || ch == '\n') {
      uint32_t now = HAL_GetTick();
      if ((now - last_nl) > max_interval_ms) {
        nl_count = 0;
      }
      last_nl = now;
      if (++nl_count >= required_newlines) {
        return 1; /* triggered */
      }
    } else {
      nl_count = 0; /* reset on other key */
    }
  }
  return 0; /* timeout */
}

static void prompt_board_id(int carriage_return)
{
  if (carriage_return) {
    p("\r[CFG] Input board_id (0-3) then ENTER: ");
  } else {
    p("[CFG] Input board_id (0-3) then ENTER: ");
  }
}

static int read_line_blocking(char *line, int max_len, uint32_t timeout_ms)
{
  uint32_t start = HAL_GetTick();
  int len = 0;
  while ((HAL_GetTick() - start) <= timeout_ms) {
    uint8_t c;
    if (uart_rx_get_byte(&c)) {
      if (c == '\r' || c == '\n') {
        if (len < max_len) line[len] = '\0';
        return len; /* success */
      }
      if (len < max_len - 1) {
        line[len++] = (char)c;
      }
    }
  }
  return -1; /* timeout */
}

/* returns -2(blank), -1(invalid), 0..3(valid id) */
static int parse_board_id_line(const char *line, int len)
{
  int seen_digit = 0;
  int val = -1;
  for (int i = 0; i < len; i++) {
    char c = line[i];
    if (c >= '0' && c <= '9') {
      if (val < 0) val = 0;
      val = val * 10 + (c - '0');
      seen_digit = 1;
    } else if (c == ' ' || c == '\t') {
      continue;
    } else {
      return -1; /* invalid char */
    }
  }
  if (!seen_digit) return -2; /* blank */
  if (val < 0 || val > 3) return -1;
  return val;
}

static HAL_StatusTypeDef save_and_reboot(uint32_t bid)
{
  g_settings.board_id = bid;
  HAL_StatusTypeDef st = app_settings_save(&g_settings);
  if (st != HAL_OK) {
    uint32_t err = HAL_FLASH_GetError();
    p("\n[CFG] Save failed. st=%ld err=0x%08lX\n", (long)st, (unsigned long)err);
    return st;
  }
  p("\n[CFG] Saved board_id=%ld. Rebooting...\n", (long)bid);
  HAL_Delay(100);
  NVIC_SystemReset();
  return HAL_OK; /* never reached */
}

/* --- public API ----------------------------------------------------------- */
void config_mode_run_window(void)
{
  const uint32_t window_ms = 2000U;
  const uint32_t max_interval_ms = 2000U;
  const int required_newlines = 5;

  print_enter_hint(window_ms, required_newlines);
  if (!wait_enter_trigger(window_ms, max_interval_ms, required_newlines)) {
    return; /* not entered */
  }

  /* 設定モード */
  p("\n[CFG] Enter CONFIG MODE. Current board_id=%lu\n",
    (unsigned long)g_settings.board_id);

  char line[16];
  for (;;) {
    prompt_board_id(0);
    int len = read_line_blocking(line, (int)sizeof(line), 15000U);
    if (len < 0) {
      p("\n[CFG] Timeout. Abort.\n");
      return;
    }
    int bid = parse_board_id_line(line, len);
    if (bid == -2) {
      /* blank: re-prompt */
      prompt_board_id(1);
      continue;
    }
    if (bid < 0) {
      p("\r[CFG] Invalid. Input 0-3 then ENTER: ");
      continue;
    }
    (void)save_and_reboot((uint32_t)bid);
  }
}
