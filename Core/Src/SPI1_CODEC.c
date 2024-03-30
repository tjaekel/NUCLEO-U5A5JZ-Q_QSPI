/*
 * SPI1_CODEC.c
 *
 *  Created on: Mar 15, 2024
 *      Author: tj925438
 */

#include "SPI1_CODEC.h"

#define SPI_MAX_LEN	64

static uint8_t sSPI_TX[SPI_MAX_LEN + 1];
static uint8_t sSPI_RX[SPI_MAX_LEN + 1];

int CODEC_ReadRegisters(unsigned long addr, unsigned long *buf, unsigned long bytes)
{
	unsigned long i;
	sSPI_TX[0] = (uint8_t)((addr << 1) | 0x1);			//7bit addr plus bit0 = 1 for Read
	if (bytes > SPI_MAX_LEN)
		bytes = SPI_MAX_LEN;
	for (i = 0; i < bytes; i++)
		sSPI_TX[1 + i] = 0;

	HAL_SPI_TransmitReceive(&hspi1, sSPI_TX, sSPI_RX, (uint16_t)(bytes + 1), 3000);

	for (i = 0; i < bytes; i++)
		*buf++ = (unsigned long)(sSPI_RX[1 + i]);

	return 1;			//actually, check for timeout above
}

int CODEC_WriteRegisters(unsigned long addr, unsigned long *buf, unsigned long bytes)
{
	unsigned long i;
	sSPI_TX[0] = (uint8_t)((addr << 1) & 0xFE);		//7bit addr plus bit0 = 0 for write
	if (bytes > SPI_MAX_LEN)
		bytes = SPI_MAX_LEN;
	for (i = 0; i < bytes; i++)
		sSPI_TX[1 + i] = (uint8_t)*buf++;

	HAL_SPI_TransmitReceive(&hspi1, sSPI_TX, sSPI_RX, (uint16_t)(bytes + 1), 3000);

	return 1;
}
