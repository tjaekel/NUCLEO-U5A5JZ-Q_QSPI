/*
 * I2C1_IMU.c
 *
 *  Created on: Mar 13, 2024
 *      Author: tj
 */

#ifndef NUCLEO_BOARD

#ifdef IMU_AVAIL
#include "I2C1_IMU.h"

int IMU_Read(unsigned long addr, unsigned char *data, size_t bytes)
{
	unsigned short devAddr = 0xD4;
	if (HAL_I2C_Mem_Read(&hi2c1, devAddr, (uint16_t)addr, 1, data, bytes, 5000) == HAL_OK)
	{
		return 0;
	}

	return 1;	//ERROR
}

int IMU_Write(unsigned long addr, unsigned char *data, size_t bytes)
{
	unsigned short devAddr = 0xD4;
	if (HAL_I2C_Mem_Write(&hi2c1, devAddr, (uint16_t)addr, 1, data, bytes, 5000) == HAL_OK)
	{
		return 0;
	}

	return 1;	//ERROR
}
#endif

#endif


