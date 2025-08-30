#include "can_fifo.h"
#include "can.h"
#include <string.h>

extern CAN_HandleTypeDef hcan;

#ifndef CAN_FIFO_QUEUE_LEN
#define CAN_FIFO_QUEUE_LEN 32
#endif

typedef struct {
    CAN_TxHeaderTypeDef hdr;
    uint8_t data[8];
    uint8_t used; /* 0 empty, 1 filled */
} canq_item_t;

static canq_item_t q[CAN_FIFO_QUEUE_LEN];
static volatile uint16_t q_head = 0; /* push position */
static volatile uint16_t q_tail = 0; /* pop position */

static inline void crit_enter(uint32_t *pm){ *pm = __get_PRIMASK(); __disable_irq(); }
static inline void crit_exit(uint32_t pm){ __set_PRIMASK(pm); }

static int queue_is_empty(void)
{
    return (q_head == q_tail) && (q[q_tail].used == 0);
}

static int queue_is_full(void)
{
    return (q_head == q_tail) && (q[q_tail].used == 1);
}

static int queue_push(const CAN_TxHeaderTypeDef *hdr, const uint8_t *data)
{
    if (queue_is_full()) return -1;
    canq_item_t *it = &q[q_head];
    it->hdr = *hdr;
    memcpy(it->data, data, 8);
    it->used = 1;
    q_head = (uint16_t)((q_head + 1) % CAN_FIFO_QUEUE_LEN);
    return 0;
}

static int queue_pop(CAN_TxHeaderTypeDef *hdr, uint8_t *data)
{
    if (queue_is_empty()) return -1;
    canq_item_t *it = &q[q_tail];
    *hdr = it->hdr;
    memcpy(data, it->data, 8);
    it->used = 0;
    q_tail = (uint16_t)((q_tail + 1) % CAN_FIFO_QUEUE_LEN);
    return 0;
}

static void try_kick_tx_locked(void)
{
    /* Fill all free mailboxes while queue has data */
    while (!queue_is_empty() && HAL_CAN_GetTxMailboxesFreeLevel(&hcan) > 0U) {
        CAN_TxHeaderTypeDef hdr; uint8_t data[8];
        (void)queue_pop(&hdr, data);
        uint32_t mbox;
        (void)HAL_CAN_AddTxMessage(&hcan, &hdr, data, &mbox);
    }
}

void can_fifo_init(void)
{
    uint32_t pm; crit_enter(&pm);
    memset((void*)q, 0, sizeof(q));
    q_head = q_tail = 0;
    crit_exit(pm);

    /* Start CAN (idempotent) */
    (void)HAL_CAN_Start(&hcan);
    /* Enable TX mailbox empty interrupt to keep draining queue */
    (void)HAL_CAN_ActivateNotification(&hcan, CAN_IT_TX_MAILBOX_EMPTY);
}

int can_fifo_send(uint16_t std_id, const uint8_t *data, uint8_t dlc)
{
    if (dlc > 8U) dlc = 8U;
    CAN_TxHeaderTypeDef hdr = {0};
    hdr.StdId = std_id & 0x7FFU;
    hdr.ExtId = 0U;
    hdr.IDE   = CAN_ID_STD;
    hdr.RTR   = CAN_RTR_DATA; /* RTR unused (always data frame) */
    hdr.DLC   = dlc;
    hdr.TransmitGlobalTime = DISABLE;

    uint8_t payload[8] = {0};
    if (data && dlc) memcpy(payload, data, dlc);

    uint32_t pm; crit_enter(&pm);

    int ret = 0;
    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) > 0U) {
        uint32_t mbox;
        HAL_StatusTypeDef st = HAL_CAN_AddTxMessage(&hcan, &hdr, payload, &mbox);
        if (st != HAL_OK) {
            /* enqueue on failure */
            ret = queue_push(&hdr, payload);
            try_kick_tx_locked();
        }
    } else {
        ret = queue_push(&hdr, payload);
    }

    crit_exit(pm);
    return ret;
}

/* TX complete callbacks: drain queue when a mailbox frees up */
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hc)
{
    if (hc != &hcan) return;
    uint32_t pm; crit_enter(&pm);
    try_kick_tx_locked();
    crit_exit(pm);
}
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hc)
{
    if (hc != &hcan) return;
    uint32_t pm; crit_enter(&pm);
    try_kick_tx_locked();
    crit_exit(pm);
}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hc)
{
    if (hc != &hcan) return;
    uint32_t pm; crit_enter(&pm);
    try_kick_tx_locked();
    crit_exit(pm);
}
