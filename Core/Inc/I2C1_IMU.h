/*
 * I2C1_IMU.h
 *
 *  Created on: Mar 13, 2024
 *      Author: tj
 */

#ifndef INC_I2C1_IMU_H_
#define INC_I2C1_IMU_H_

#ifndef NUCLEO_BOARD

#include "main.h"

extern I2C_HandleTypeDef hi2c1;

int IMU_Read(unsigned long addr, unsigned char *data, size_t bytes);
int IMU_Write(unsigned long addr, unsigned char *data, size_t bytes);

#endif

#endif /* INC_I2C1_IMU_H_ */
