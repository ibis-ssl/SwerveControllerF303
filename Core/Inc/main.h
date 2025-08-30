/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_0_Pin GPIO_PIN_13
#define LED_0_GPIO_Port GPIOC
#define LED_1_Pin GPIO_PIN_14
#define LED_1_GPIO_Port GPIOC
#define SW_0_Pin GPIO_PIN_15
#define SW_0_GPIO_Port GPIOC
#define BATT_V_Pin GPIO_PIN_0
#define BATT_V_GPIO_Port GPIOA
#define MOTOR_A_Current_Pin GPIO_PIN_1
#define MOTOR_A_Current_GPIO_Port GPIOA
#define CurrentLimit_Pin GPIO_PIN_2
#define CurrentLimit_GPIO_Port GPIOA
#define PH_SENS_Pin GPIO_PIN_3
#define PH_SENS_GPIO_Port GPIOA
#define PH_DRV_0_Pin GPIO_PIN_5
#define PH_DRV_0_GPIO_Port GPIOA
#define PH_DRV_1_Pin GPIO_PIN_6
#define PH_DRV_1_GPIO_Port GPIOA
#define PH_DRV_2_Pin GPIO_PIN_7
#define PH_DRV_2_GPIO_Port GPIOA
#define PH_DRV_3_Pin GPIO_PIN_0
#define PH_DRV_3_GPIO_Port GPIOB
#define PH_DRV_4_Pin GPIO_PIN_1
#define PH_DRV_4_GPIO_Port GPIOB
#define PH_DRV_5_Pin GPIO_PIN_2
#define PH_DRV_5_GPIO_Port GPIOB
#define PH_DRV_6_Pin GPIO_PIN_10
#define PH_DRV_6_GPIO_Port GPIOB
#define PH_DRV_7_Pin GPIO_PIN_11
#define PH_DRV_7_GPIO_Port GPIOB
#define ENC_0_Pin GPIO_PIN_6
#define ENC_0_GPIO_Port GPIOB
#define MOTOT_PWM_1_Pin GPIO_PIN_8
#define MOTOT_PWM_1_GPIO_Port GPIOB
#define MOTOT_PWM_2_Pin GPIO_PIN_9
#define MOTOT_PWM_2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
