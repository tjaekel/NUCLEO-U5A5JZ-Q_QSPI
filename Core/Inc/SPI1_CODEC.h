/*
 * SPI1_CODEC.h
 *
 *  Created on: Mar 15, 2024
 *      Author: tj925438
 */

#ifndef INC_SPI1_CODEC_H_
#define INC_SPI1_CODEC_H_

#include "main.h"

extern SPI_HandleTypeDef 		hspi1;

int CODEC_ReadRegisters(unsigned long addr, unsigned long *buf, unsigned long bytes);
int CODEC_WriteRegisters(unsigned long addr, unsigned long *buf, unsigned long bytes);

#endif /* INC_SPI1_CODEC_H_ */
