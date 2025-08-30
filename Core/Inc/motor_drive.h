#ifndef MOTOR_DRIVE_H
#define MOTOR_DRIVE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize and start TIM8 PWM channels used for motor drive. */
void motor_drive_init(void);

/*
 * Set motor output.
 * value in [-1.0f, 1.0f]
 * - value > 0: CH2 and CH3 run complementary PWM (forward)
 * - value < 0: CH2 and CH3 run complementary PWM (reverse)
 * - value == 0: both channels High (100% duty)
 */
void motor_drive_set(float value);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_DRIVE_H */
