/*
 * photo_control.c
 *
 *  Created on: Aug 31, 2025
 *      Author: hiroyuki
 */

#include "photo_control.h"

#include "adc.h"
#include "gpio.h"

static int drive_state = 0;
static int adc_raw[8] = {0};

static void set_gpio(uint8_t pin);

void photo_controller_init(void) {}

void photo_controller_cycle(void)
{
  adc_raw[drive_state] = HAL_ADC_GetValue(&hadc1);

  drive_state++;
  if (drive_state >= 8) {
    drive_state = 0;
  }
  set_gpio(drive_state);
}

static void set_gpio(uint8_t pin)
{
  switch (pin) {
    case 0:
      HAL_GPIO_WritePin(PH_DRV_0_GPIO_Port, PH_DRV_0_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(PH_DRV_1_GPIO_Port, PH_DRV_1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_2_GPIO_Port, PH_DRV_2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_3_GPIO_Port, PH_DRV_3_Pin, GPIO_PIN_SET);

      HAL_GPIO_WritePin(PH_DRV_4_GPIO_Port, PH_DRV_4_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_5_GPIO_Port, PH_DRV_5_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_6_GPIO_Port, PH_DRV_6_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_7_GPIO_Port, PH_DRV_7_Pin, GPIO_PIN_SET);
      break;
    case 1:
      HAL_GPIO_WritePin(PH_DRV_0_GPIO_Port, PH_DRV_0_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_1_GPIO_Port, PH_DRV_1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(PH_DRV_2_GPIO_Port, PH_DRV_2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_3_GPIO_Port, PH_DRV_3_Pin, GPIO_PIN_SET);

      HAL_GPIO_WritePin(PH_DRV_4_GPIO_Port, PH_DRV_4_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_5_GPIO_Port, PH_DRV_5_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_6_GPIO_Port, PH_DRV_6_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_7_GPIO_Port, PH_DRV_7_Pin, GPIO_PIN_SET);
      break;
    case 2:
      HAL_GPIO_WritePin(PH_DRV_0_GPIO_Port, PH_DRV_0_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_1_GPIO_Port, PH_DRV_1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_2_GPIO_Port, PH_DRV_2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(PH_DRV_3_GPIO_Port, PH_DRV_3_Pin, GPIO_PIN_SET);

      HAL_GPIO_WritePin(PH_DRV_4_GPIO_Port, PH_DRV_4_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_5_GPIO_Port, PH_DRV_5_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_6_GPIO_Port, PH_DRV_6_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_7_GPIO_Port, PH_DRV_7_Pin, GPIO_PIN_SET);
      break;
    case 3:
      HAL_GPIO_WritePin(PH_DRV_0_GPIO_Port, PH_DRV_0_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_1_GPIO_Port, PH_DRV_1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_2_GPIO_Port, PH_DRV_2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_3_GPIO_Port, PH_DRV_3_Pin, GPIO_PIN_RESET);

      HAL_GPIO_WritePin(PH_DRV_4_GPIO_Port, PH_DRV_4_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_5_GPIO_Port, PH_DRV_5_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_6_GPIO_Port, PH_DRV_6_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_7_GPIO_Port, PH_DRV_7_Pin, GPIO_PIN_SET);
      break;
    case 4:
      HAL_GPIO_WritePin(PH_DRV_0_GPIO_Port, PH_DRV_0_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_1_GPIO_Port, PH_DRV_1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_2_GPIO_Port, PH_DRV_2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_3_GPIO_Port, PH_DRV_3_Pin, GPIO_PIN_SET);

      HAL_GPIO_WritePin(PH_DRV_4_GPIO_Port, PH_DRV_4_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(PH_DRV_5_GPIO_Port, PH_DRV_5_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_6_GPIO_Port, PH_DRV_6_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_7_GPIO_Port, PH_DRV_7_Pin, GPIO_PIN_SET);
      break;
    case 5:
      HAL_GPIO_WritePin(PH_DRV_0_GPIO_Port, PH_DRV_0_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_1_GPIO_Port, PH_DRV_1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_2_GPIO_Port, PH_DRV_2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_3_GPIO_Port, PH_DRV_3_Pin, GPIO_PIN_SET);

      HAL_GPIO_WritePin(PH_DRV_4_GPIO_Port, PH_DRV_4_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_5_GPIO_Port, PH_DRV_5_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(PH_DRV_6_GPIO_Port, PH_DRV_6_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_7_GPIO_Port, PH_DRV_7_Pin, GPIO_PIN_SET);
      break;
    case 6:
      HAL_GPIO_WritePin(PH_DRV_0_GPIO_Port, PH_DRV_0_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_1_GPIO_Port, PH_DRV_1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_2_GPIO_Port, PH_DRV_2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_3_GPIO_Port, PH_DRV_3_Pin, GPIO_PIN_SET);

      HAL_GPIO_WritePin(PH_DRV_4_GPIO_Port, PH_DRV_4_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_5_GPIO_Port, PH_DRV_5_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_6_GPIO_Port, PH_DRV_6_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(PH_DRV_7_GPIO_Port, PH_DRV_7_Pin, GPIO_PIN_SET);
      break;
    case 7:
      HAL_GPIO_WritePin(PH_DRV_0_GPIO_Port, PH_DRV_0_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_1_GPIO_Port, PH_DRV_1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_2_GPIO_Port, PH_DRV_2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_3_GPIO_Port, PH_DRV_3_Pin, GPIO_PIN_SET);

      HAL_GPIO_WritePin(PH_DRV_4_GPIO_Port, PH_DRV_4_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_5_GPIO_Port, PH_DRV_5_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_6_GPIO_Port, PH_DRV_6_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_7_GPIO_Port, PH_DRV_7_Pin, GPIO_PIN_RESET);
      break;
    default:
      HAL_GPIO_WritePin(PH_DRV_0_GPIO_Port, PH_DRV_0_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_1_GPIO_Port, PH_DRV_1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_2_GPIO_Port, PH_DRV_2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_3_GPIO_Port, PH_DRV_3_Pin, GPIO_PIN_SET);

      HAL_GPIO_WritePin(PH_DRV_4_GPIO_Port, PH_DRV_4_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_5_GPIO_Port, PH_DRV_5_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_6_GPIO_Port, PH_DRV_6_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PH_DRV_7_GPIO_Port, PH_DRV_7_Pin, GPIO_PIN_SET);
      break;
  }
}