#include "motor_drive.h"

#include <math.h>

#include "tim.h"

/* Use TIM8 CH2 (PB8) and CH3 (PB9) as PWM outputs */

static inline uint32_t get_period(void)
{
  /* Read current ARR to follow runtime changes, if any */
  return __HAL_TIM_GET_AUTORELOAD(&htim8);
}

void motor_drive_init(void)
{
  /* Start PWM on both channels */
  (void)HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
  (void)HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
  HAL_GPIO_WritePin(CurrentLimit_GPIO_Port, CurrentLimit_Pin, GPIO_PIN_SET);

  /* Default to stop (both High) */
  motor_drive_set(0.0f);
}

void motor_drive_set(float value)
{
  /* Clamp to [-1, 1] */
  if (value > 1.0f) value = 1.0f;
  if (value < -1.0f) value = -1.0f;

  uint32_t period = get_period();

  if (value == 0.0f) {
    /* Both channels High */
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, period);
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, period);
    return;
  }

  float mag = value > 0.0f ? value : -value;
  /* Map magnitude to [0..period], rounding to nearest */
  uint32_t d = (uint32_t)(mag * (float)period + 0.5f);
  if (d > period) d = period; /* guard */

  uint32_t d_comp = period - d; /* complementary */

  if (value > 0.0f) {
    /* Forward: CH2=D, CH3=complement */
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, d);
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, d_comp);
  } else {
    /* Reverse: CH2=complement, CH3=D */
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, d_comp);
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, d);
  }
}
