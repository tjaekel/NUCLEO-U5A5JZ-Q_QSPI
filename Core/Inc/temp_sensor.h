/*
 * temp_sensor.h
 *
 *  Created on: Jan 17, 2024
 *      Author: tj925438
 */

#ifndef INC_TEMP_SENSOR_H_
#define INC_TEMP_SENSOR_H_

#include "main.h"
#include "stm32u5xx_ll_adc.h"
#include "cmd_dec.h"

void ADC_Init(void);
int ADC_RunOnce(EResultOut out, int flag);

#endif /* INC_TEMP_SENSOR_H_ */
