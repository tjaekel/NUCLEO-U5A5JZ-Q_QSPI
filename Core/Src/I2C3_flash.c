/*
 * I2C3_flash.c
 *
 *  Created on: Feb 14, 2024
 *      Author: tj
 */

#ifndef NUCLEO_BOARD

#include "I2C3_flash.h"

int FLASH_Read(unsigned long addr, unsigned char *data, size_t bytes)
{
	unsigned short devAddr = 0xA0;
	devAddr |= (uint16_t)((addr >> 15) & 0x6);
	if (HAL_I2C_Mem_Read(&hi2c3, devAddr, (uint16_t)addr, 2, data, bytes, 5000) == HAL_OK)
	{
		return 0;
	}

	return 1;	//ERROR
}

int FLASH_Write(unsigned long addr, unsigned char *data, size_t bytes)
{
	unsigned short devAddr = 0xA0;
	devAddr |= (uint16_t)((addr >> 15) & 0x6);
	if (HAL_I2C_Mem_Write(&hi2c3, devAddr, (uint16_t)addr, 2, data, bytes, 5000) == HAL_OK)
	{
		return 0;
	}

	return 1;	//ERROR
}

#endif
