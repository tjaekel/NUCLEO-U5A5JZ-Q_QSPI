/*
 * I2C3_flash.h
 *
 *  Created on: Feb 14, 2024
 *      Author: tj
 */

#ifndef INC_I2C3_FLASH_H_
#define INC_I2C3_FLASH_H_

#ifndef NUCLEO_BOARD

#include "main.h"

extern I2C_HandleTypeDef hi2c3;

int FLASH_Read(unsigned long addr, unsigned char *data, size_t bytes);
int FLASH_Write(unsigned long addr, unsigned char *data, size_t bytes);

#endif

#endif /* INC_I2C3_FLASH_H_ */
