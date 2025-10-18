#include "debug_print.h"

#include <stdio.h>
#include <string.h>

#include "usart.h"

/*
 * Double-buffered DMA UART1 debug print implementation.
 * - Uses huart1/hdma_usart1_tx configured by CubeMX.
 * - Non-blocking: accumulates into an idle buffer while DMA is busy.
 * - On DMA completion, automatically sends the next buffer if pending.
 */

#ifndef DEBUG_PRINT_BUF_SIZE
#define DEBUG_PRINT_BUF_SIZE 512U
#endif

static uint8_t tx_buf[2][DEBUG_PRINT_BUF_SIZE];
static volatile uint16_t tx_len[2] = {0, 0};
static volatile uint8_t active_idx = 0; /* buffer currently in DMA */
static volatile uint8_t fill_idx = 1;   /* buffer being accumulated */
static volatile uint8_t dma_busy = 0;   /* 1 while DMA transfer running */

static inline void dp_critical_enter(uint32_t * primask)
{
  *primask = __get_PRIMASK();
  __disable_irq();
}

static inline void dp_critical_exit(uint32_t primask) { __set_PRIMASK(primask); }

void debug_print_init(void)
{
  uint32_t pm;
  dp_critical_enter(&pm);
  tx_len[0] = tx_len[1] = 0;
  active_idx = 0;
  fill_idx = 1;
  dma_busy = 0;
  dp_critical_exit(pm);
}

/* Start DMA on the current active_idx buffer. Caller must hold critical section. */
static void start_dma_locked(void)
{
  if (tx_len[active_idx] == 0U) {
    return;
  }
  dma_busy = 1U;
  /* Use HAL to start DMA; HAL expects non-const uint8_t* */
  (void)HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_buf[active_idx], tx_len[active_idx]);
}

int debug_print_bytes(const uint8_t * data, uint16_t len)
{
  if (data == NULL || len == 0U) {
    return 0;
  }

  uint32_t pm;
  dp_critical_enter(&pm);

  int written_total = 0;
  while (len > 0U) {
    uint16_t space = (uint16_t)(DEBUG_PRINT_BUF_SIZE - tx_len[fill_idx]);
    if (space == 0U) {
      /* If DMA idle, kick current fill buffer */
      if (!dma_busy) {
        /* swap buffers */
        uint8_t next_active = fill_idx;
        fill_idx = active_idx;
        active_idx = next_active;
        start_dma_locked();
      } else {
        /* No space and DMA busy: drop remaining */
        break;
      }
      space = (uint16_t)(DEBUG_PRINT_BUF_SIZE - tx_len[fill_idx]);
      if (space == 0U) {
        break;
      }
    }

    uint16_t chunk = (len < space) ? len : space;
    memcpy(&tx_buf[fill_idx][tx_len[fill_idx]], data, chunk);
    tx_len[fill_idx] = (uint16_t)(tx_len[fill_idx] + chunk);
    data += chunk;
    len = (uint16_t)(len - chunk);
    written_total += chunk;

    /* If DMA idle, start immediately on the accumulated buffer */
    if (!dma_busy) {
      uint8_t next_active = fill_idx;
      fill_idx = active_idx;
      active_idx = next_active;
      start_dma_locked();
    }
  }

  dp_critical_exit(pm);
  return written_total;
}

int p(const char * fmt, ...)
{
  if (fmt == NULL) {
    return 0;
  }

  char temp[DEBUG_PRINT_BUF_SIZE];
  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(temp, sizeof(temp), fmt, ap);
  va_end(ap);

  if (n <= 0) {
    return 0;
  }

  /* Cap to buffer size */
  uint16_t to_write = (uint16_t)((n > (int)sizeof(temp)) ? sizeof(temp) : n);
  return debug_print_bytes((const uint8_t *)temp, to_write);
}

/* HAL UART TX complete callback: kick the next buffer if pending */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart)
{
  if (huart != &huart1) {
    return;
  }
  uint32_t pm;
  dp_critical_enter(&pm);

  /* Mark active buffer as sent */
  tx_len[active_idx] = 0U;

  /* If the other buffer has data, swap and start DMA */
  if (tx_len[fill_idx] > 0U) {
    uint8_t next_active = fill_idx;
    fill_idx = active_idx;
    active_idx = next_active;
    start_dma_locked();
  } else {
    dma_busy = 0U;
  }

  dp_critical_exit(pm);
}

/* Also guard against DMA-level completion callback if used */
void HAL_DMA_TxCpltCallback(DMA_HandleTypeDef * hdma)
{
  if (hdma == huart1.hdmatx) {
    HAL_UART_TxCpltCallback(&huart1);
  }
}

/* Optional: on UART error, drop current transfer and try to continue */
void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart)
{
  if (huart != &huart1) {
    return;
  }
  uint32_t pm;
  dp_critical_enter(&pm);
  /* Reset DMA busy to allow subsequent prints */
  dma_busy = 0U;
  tx_len[active_idx] = 0U;
  if (tx_len[fill_idx] > 0U) {
    uint8_t next_active = fill_idx;
    fill_idx = active_idx;
    active_idx = next_active;
    start_dma_locked();
  }
  dp_critical_exit(pm);

  /* 受信側もエラー後に再開しておく */
  uart_rx_restart();
}

void setTextRed() { p("\e[31m"); }
void setTextYellow() { p("\e[33m"); }
void setTextGreen() { p("\e[32m"); }
//void setTextBlue() { p("\e[34m"); } //見づらいので非推奨
void setTextMagenta() { p("\e[35m"); }
void setTextCyan() { p("\e[36m"); }
void setTextNormal() { p("\e[0m"); }

void setTextBold() { p("\e[1m"); }