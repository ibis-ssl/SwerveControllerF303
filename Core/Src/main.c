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
#include "math.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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
  /* USER CODE BEGIN 2 */
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  __HAL_SPI_ENABLE(&hspi1);

  debug_print_init();
  motor_drive_init();
  can_fifo_init();

  p("\n\nibis SwerveDrive Controller\n\n");

  uint16_t adc_raw[3] = {0};
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_raw, 3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  as5047p_t enc;
  uint8_t tx_data[8] = {0};

  uint16_t sen_buf[8];
  int pre_max_idx = 0;

  float pre_motor_radian = 0, motor_radian_speed = 0;
  float motor_target_radian = 0;
  float motor_current_radian = 0;
  float motor_target_rad_per_tick = 0.001;

  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    //can_fifo_send(0x100, tx_data, 8);
    //can_fifo_send(0x101, tx_data, 8);
    //p("%5dmV %5dmA %5d ENC %+6d\n", (int)(adc_raw[0] / 1.14), adc_raw[1], adc_raw[2], enc.enc_raw);
    //HAL_Delay(100);
    int max_idx = 0, max_value = 0;

    // photo abs encoder update
    /*     for (uint32_t i = 0; i < 8; i++) {
      photo_controller_cycle();
      HAL_Delay(1);
      sen_buf[i] = adc_raw[2];
      if (max_value < sen_buf[i]) {
        max_value = sen_buf[i];
        max_idx = i;
      }
    } */

    // motor encoder update
    as5047p_update(&enc);

    motor_radian_speed = enc.radian - pre_motor_radian;
    if (motor_radian_speed > M_PI) {
      motor_radian_speed -= 2 * M_PI;
    } else if (motor_radian_speed < -M_PI) {
      motor_radian_speed += 2 * M_PI;
    }
    motor_current_radian += motor_radian_speed;
    motor_target_radian += motor_target_rad_per_tick;
    float diff = (motor_target_radian - motor_current_radian) / 2;

    float OUT_DUTY_LIMIT = 0.3;
    if (diff > OUT_DUTY_LIMIT) {
      diff = OUT_DUTY_LIMIT;
    } else if (diff < -OUT_DUTY_LIMIT) {
      diff = -OUT_DUTY_LIMIT;
    }
    motor_drive_set(diff);

    p("now %3d tar %3d diff %+4.3f\n", (int)(motor_current_radian * 180 / 3.14), (int)(motor_target_radian * 180 / 3.14), diff);
    if (pre_max_idx == 5 && max_idx == 4) {
      //p("%d %6ddeg\n", max_idx, (int)(enc.radian * 180 / 3.14));
    }
    //p("Sensor : %3d %3d %3d %3d %3d %3d %3d %3d , MAX : %d\n", sen_buf[0], sen_buf[1], sen_buf[2], sen_buf[3], sen_buf[4], sen_buf[5], sen_buf[6], sen_buf[7], max_idx);

    pre_motor_radian = enc.radian;
    pre_max_idx = max_idx;
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
