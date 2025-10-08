/*
 * as5047p.c
 *
 *  Created on: Sep 1, 2025
 *      Author: hiroyuki
 */

#include "as5047p.h"

#include <math.h>

#include "gpio.h"
#include "spi.h"

void as5047p_update(as5047p_t * enc)
{
  HAL_GPIO_WritePin(ENC_0_GPIO_Port, ENC_0_Pin, GPIO_PIN_RESET);

  enc->pre_enc_raw = enc->enc_raw;

  enc->enc_raw = hspi1.Instance->DR;
  // addr 0x3FFF & 1 << 14 (read) & parity
  // 0x3FFE : without dynamic errro compensation
  // 0x3FFF : with dynamic errro compensation
  hspi1.Instance->DR = 0xFFFF;
  while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE) == RESET) {
  }
  // 分解能は14bitだが、後段であまり算をするため、16bitに変換
  enc->enc_raw = (hspi1.Instance->DR & 0x3FFF) << 2;

  enc->radian = (float)enc->enc_raw / HARF_OF_ENC_CNT_MAX * M_PI;

  HAL_GPIO_WritePin(ENC_0_GPIO_Port, ENC_0_Pin, GPIO_PIN_SET);
}