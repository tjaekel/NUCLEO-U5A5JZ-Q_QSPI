/*
 * I2C3_flash.c
 *
 *  Created on: Feb 14, 2024
 *      Author: tj
 */

#if !defined(NUCLEO_BOARD) || defined(LEVEL_SHIFT)

#include "I2C3_flash.h"

int FLASH_Read(unsigned long addr, unsigned char *data, size_t bytes)
{
#ifdef LEVEL_SHIFT
	unsigned short devAddr = 0xA8;
#else
	unsigned short devAddr = 0xA0;
#endif
	devAddr |= (uint16_t)((addr >> 15) & 0x6);
#ifdef LEVEL_SHIFT
	if (HAL_I2C_Mem_Read(&hi2c2, devAddr, (uint16_t)addr, 2, data, bytes, 5000) == HAL_OK)
#else
	if (HAL_I2C_Mem_Read(&hi2c3, devAddr, (uint16_t)addr, 2, data, bytes, 5000) == HAL_OK)
#endif
	{
		return 0;
	}

	return 1;	//ERROR
}

int FLASH_Write(unsigned long addr, unsigned char *data, size_t bytes)
{
#ifdef LEVEL_SHIFT
	unsigned short devAddr = 0xA8;
#else
	unsigned short devAddr = 0xA0;
#endif
	devAddr |= (uint16_t)((addr >> 15) & 0x6);
#ifdef LEVEL_SHIFT
	if (HAL_I2C_Mem_Write(&hi2c2, devAddr, (uint16_t)addr, 2, data, bytes, 5000) == HAL_OK)
#else
	if (HAL_I2C_Mem_Write(&hi2c3, devAddr, (uint16_t)addr, 2, data, bytes, 5000) == HAL_OK)
#endif
	{
		return 0;
	}

	return 1;	//ERROR
}

#endif
