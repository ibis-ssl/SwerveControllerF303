#include "error_monitor.h"

#include "stm32f3xx_hal.h"

#ifndef STEER_GEAR_RATIO
#define STEER_GEAR_RATIO (2.25f)
#endif

#ifndef ANGLE_LIMIT_TURNS
#define ANGLE_LIMIT_TURNS (3.0f) /* +-3回転 */
#endif

#ifndef OVERCURR_T1_A
#define OVERCURR_T1_A (1.0f)
#endif
#ifndef OVERCURR_T1_MS
#define OVERCURR_T1_MS (3000u)
#endif
#ifndef OVERCURR_T2_A
#define OVERCURR_T2_A (2.0f)
#endif
#ifndef OVERCURR_T2_MS
#define OVERCURR_T2_MS (1000u)
#endif

static volatile uint32_t g_reason = 0;
static uint32_t t1_start = 0;
static uint32_t t2_start = 0;

void error_monitor_init(void)
{
  g_reason = 0;
  t1_start = 0;
  t2_start = 0;
}

static void check_overcurrent(float current_a)
{
  uint32_t now = HAL_GetTick();

  /* 2A for >=1s */
  if (current_a >= OVERCURR_T2_A) {
    if (t2_start == 0) t2_start = now;
    if (!g_reason && (now - t2_start >= OVERCURR_T2_MS)) {
      g_reason |= ERR_OVERCURRENT_2A_1S;
    }
  } else {
    t2_start = 0;
  }

  /* 1A for >=3s */
  if (current_a >= OVERCURR_T1_A) {
    if (t1_start == 0) t1_start = now;
    if (!g_reason && (now - t1_start >= OVERCURR_T1_MS)) {
      g_reason |= ERR_OVERCURRENT_1A_3S;
    }
  } else {
    t1_start = 0;
  }
}

static void check_angle(float steering_rad)
{
  float limit = (float)(ANGLE_LIMIT_TURNS * 2.0f * 3.14159265358979323846f);
  float a = (steering_rad >= 0.0f) ? steering_rad : -steering_rad;
  if (a > limit) {
    g_reason |= ERR_ANGLE_LIMIT;
  }
}

void error_monitor_update(float current_a, float steering_rad)
{
  if (g_reason) return; /* ラッチ後は維持 */
  check_overcurrent(current_a);
  check_angle(steering_rad);
}

bool error_monitor_is_latched(void) { return g_reason != 0; }

uint32_t error_monitor_reason(void) { return g_reason; }
