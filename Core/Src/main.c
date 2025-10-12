/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "adc.h"
#include "can.h"
#include "dma.h"
#include "gpio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>

#include "as5047p.h"
#include "can_fifo.h"
#include "debug_print.h"
#include "motor_drive.h"
#include "photo_control.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
struct
{
  float kd, kp, ki;
  float ki_limit;
} pid = {.kp = 5.0, .ki = 1.0, .kd = -0.03};
struct
{
  float intg, duty;
} out_limit = {.intg = 0.1, .duty = 0.0};
struct
{
  float cur_rad, pre_rad_raw;
  float rad_per_sec;
} motor = {0};
struct
{
  float rad_per_sec;
  float rad;
} target = {0};
struct
{
  float pos, div, intg;
  float intg_limit;
} diff = {0};
float out_duty = 0;

as5047p_t enc;
static const int CYCLE_PER_SEC = 1000;
static void uart_adjust_pid_from_rx(void)
{
  /* q/a: Kp +/- , w/s: Ki +/- , e/d: Kd +/- */
  const float step_kp = 0.1f;
  const float step_ki = 0.01f;
  const float step_kd = 0.01f;
  uint8_t ch;
  bool changed = false;
  while (uart_rx_get_byte(&ch)) {
    switch (ch) {
      case 'q':
        pid.kp += step_kp;
        changed = true;
        break;
      case 'a':
        pid.kp -= step_kp;
        changed = true;
        break;
      case 'w':
        pid.ki += step_ki;
        changed = true;
        break;
      case 's':
        pid.ki -= step_ki;
        changed = true;
        break;
      case 'e':
        pid.kd += step_kd;
        changed = true;
        break;
      case 'd':
        pid.kd -= step_kd;
        changed = true;
        break;
      case 'r':
        target.rad += M_PI * 2 * 2.25;
        changed = true;
        break;
      case 'f':
        target.rad -= M_PI * 2 * 2.25;
        changed = true;
        break;
      default:
        break;
    }
  }
  if (changed) {
    p("PID kp=%.4f ki=%.4f kd=%.4f\n", pid.kp, pid.ki, pid.kd);
  }
}
static void motor_control_cycle()
{
  as5047p_update(&enc);
  float rad_per_tick = 0;
  rad_per_tick = enc.radian - motor.pre_rad_raw;
  // 0またいだ場合の処理
  if (rad_per_tick > M_PI) {
    rad_per_tick -= 2 * M_PI;
  } else if (rad_per_tick < -M_PI) {
    rad_per_tick += 2 * M_PI;
  }

  // 現在値の更新
  motor.cur_rad += rad_per_tick;
  target.rad += target.rad_per_sec / CYCLE_PER_SEC;

  // diff errorの更新
  diff.pos = target.rad - motor.cur_rad;
  diff.div = rad_per_tick * CYCLE_PER_SEC;
  diff.intg += diff.pos;

  // pos
  float DEAD_ZONE = 0.02;
  if (diff.pos > DEAD_ZONE) {
    diff.pos -= DEAD_ZONE;
  } else if (diff.pos < -DEAD_ZONE) {
    diff.pos += DEAD_ZONE;
  } else if (fabs(diff.pos) < DEAD_ZONE) {
    diff.pos = 0;
  }

  // div
  /*   float IGNORE_SPEED = 1.0;
  if (diff.div > IGNORE_SPEED) {
    diff.div -= IGNORE_SPEED;
  } else if (diff.div < -IGNORE_SPEED) {
    diff.div += IGNORE_SPEED;
  } else if (fabs(diff.div) < IGNORE_SPEED) {
    diff.div = 0;
  } */

  // intg制限
  if (diff.intg > out_limit.intg) {
    diff.intg = out_limit.intg;
  } else if (diff.intg < -out_limit.intg) {
    diff.intg = -out_limit.intg;
  }

  // 出力と計算
  out_duty = diff.pos * pid.kp + diff.div * pid.kd + diff.intg * pid.ki;
  if (out_duty > out_limit.duty) {
    out_duty = out_limit.duty;
  } else if (out_duty < -out_limit.duty) {
    out_duty = -out_limit.duty;
  }

  motor_drive_set(out_duty);

  motor.pre_rad_raw = enc.radian;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim) { motor_control_cycle(); }
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_CAN_Init();
  MX_USART1_UART_Init();
  MX_TIM8_Init();
  MX_SPI1_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  __HAL_SPI_ENABLE(&hspi1);

  HAL_TIM_Base_Start_IT(&htim7);

  debug_print_init();
  motor_drive_init();
  can_fifo_init();

  p("\n\nibis SwerveDrive Controller\n\n");

  uint16_t adc_raw[3] = {0};
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_raw, 3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  out_limit.duty = 1.0;
  out_limit.intg = 0.1;
  target.rad_per_sec = 0;
  while (1) {
    uart_adjust_pid_from_rx();
    HAL_Delay(100);
    p("Out %+4.2f Tar %+4.2f Mtr %+4.2f cnt %3d\n", out_duty, target.rad, motor.cur_rad);
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_TIM8 | RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  PeriphClkInit.Tim8ClockSelection = RCC_TIM8CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
    p("");
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t * file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
