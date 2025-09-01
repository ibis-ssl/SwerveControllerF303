/*
 * as5047p.h
 *
 *  Created on: Sep 1, 2025
 *      Author: hiroyuki
 */

#ifndef INC_AS5047P_H_
#define INC_AS5047P_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
  float radian;
  int enc_raw;
  int pre_enc_raw;
} as5047p_t;

void as5047p_update(as5047p_t * enc);

#endif /* INC_AS5047P_H_ */
