#ifndef ERROR_MONITOR_H
#define ERROR_MONITOR_H

#include <stdbool.h>
#include <stdint.h>

/* エラー理由ビット */
#define ERR_OVERCURRENT_1A_3S (1u << 0)
#define ERR_OVERCURRENT_2A_1S (1u << 1)
#define ERR_ANGLE_LIMIT (1u << 2)

void error_monitor_init(void);

/*
 * 監視更新
 * - current_a: モーター電流[A]
 * - steering_rad: ステアリング角[rad]（ゼロ中心、+/-）
 */
void error_monitor_update(float current_a, float steering_rad);

bool error_monitor_is_latched(void);
uint32_t error_monitor_reason(void);

#endif /* ERROR_MONITOR_H */
