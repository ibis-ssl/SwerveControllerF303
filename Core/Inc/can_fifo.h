#ifndef CAN_FIFO_H
#define CAN_FIFO_H

#include <stdint.h>

/* Initialize CAN TX FIFO helper.
 * - Starts CAN if not already started
 * - Enables TX mailbox empty interrupts for continuous dequeue
 */
void can_fifo_init(void);

/* Enqueue a standard ID data frame (RTR unused) for transmission.
 * std_id: 0..0x7FF, dlc: 0..8, data: pointer to payload (dlc bytes used)
 * Returns 0 on success/queued, <0 on drop (queue full).
 */
int can_fifo_send(uint16_t std_id, const uint8_t *data, uint8_t dlc);

#endif /* CAN_FIFO_H */
