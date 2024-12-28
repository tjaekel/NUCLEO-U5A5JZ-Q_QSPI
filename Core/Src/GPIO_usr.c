/*
 * GPIO_usr.c
 *
 *  Created on: Dec 19, 2024
 *      Author: tj
 */

#include "GPIO_usr.h"
#include "SYS_config.h"

/* table with all the user GPIO ports and pins */
const TGPIOUsr GPIOUsr[GPIO_NUM_BITS] = {
		/* GPIO_INT and GPIO_RES means: do not touch or configure, use as configured somewhere else */
#ifdef LEVEL_SHIFTER
		{INT0_PORT, INT0_PIN, GPIO_INT | GPIO_INPUT, "INT0"},					//b0 - is INT0
		{INT1_PORT, INT1_PIN, GPIO_INT | GPIO_INPUT, "INT1"},					//b1 - is INT1
		{INT2_PORT, INT2_PIN, GPIO_INT | GPIO_INPUT, "INT2"},					//b2 - is INT2
#else
		{INT0_PORT, INT0_PIN, GPIO_INT | GPIO_INPUT | GPIO_PU, "INT0"},			//b0 - is INT0
		{INT1_PORT, INT1_PIN, GPIO_INT | GPIO_INPUT | GPIO_PU, "INT1"},			//b1 - is INT1
		{INT2_PORT, INT2_PIN, GPIO_INT | GPIO_INPUT | GPIO_PU, "INT2"},			//b2 - is INT2
#endif
		{RES_PORT,  RES_PIN,  GPIO_RES | GPIO_OUTPUT | GPIO_OD, "RES "},		//b3 - is RES
		/* configurable pins */
		{GPIO2_PORT,  GPIO2_PIN,  GPIO_INPUT | GPIO_OUTPUT, "IO2 "},			//b4 - is GPIO2 - INT3
		{GPIO3_PORT,  GPIO3_PIN,  GPIO_INPUT | GPIO_OUTPUT, "IO3 "},			//b5 - is GPIO3 - INT4
		{GPIO4_PORT,  GPIO4_PIN,  GPIO_INPUT | GPIO_OUTPUT, "IO4 "},			//b6 - is GPIO4	- INT5
		{GPIO5_PORT,  GPIO5_PIN,  GPIO_INPUT | GPIO_OUTPUT, "IO5 "},			//b7 - is GPIO5
		//{GPIO6_PORT,  GPIO6_PIN,  GPIO_INPUT | GPIO_OUTPUT, "IO6 "},			//b8 - is GPIO6
		{GPIO6_PORT,  GPIO6_PIN,  GPIO_NONE,                "n.c."},			//b8 - is GPIO6
		{GPIO7_PORT,  GPIO7_PIN,  GPIO_INPUT | GPIO_OUTPUT, "IO7 "},			//b9 - is GPIO7
		{GPIO8_PORT,  GPIO8_PIN,  GPIO_INPUT | GPIO_OUTPUT, "IO8 "},			//b10 - is GPIO8
		{GPIO9_PORT,  GPIO9_PIN,  GPIO_INPUT | GPIO_OUTPUT, "IO9 "},			//b11 - is GPIO9
		{GPIO10_PORT, GPIO10_PIN, GPIO_INPUT | GPIO_OUTPUT, "IO10"},			//b12 - is GPIO10
};

int GPIO_Config(unsigned long mask)
{
	/* mask is 0 for Output, 1 for Input - not configurable pins excluded */
	GPIO_InitTypeDef GPIO_InitStruct;
	size_t i;
	unsigned long shiftMask = 0x1;

	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 		//GPIO_SPEED_FREQ_LOW;

	for (i = 0; i < (sizeof(GPIOUsr) / sizeof(TGPIOUsr)); i++)
	{
		GPIO_InitStruct.Pull  = GPIO_NOPULL;			//GPIO_PULLUP;
		GPIO_InitStruct.Pin = GPIOUsr[i].GPIO_Pin;
		if (GPIOUsr[i].GPIO_Mode & GPIO_NONE)
		{
			shiftMask <<= 1;
			continue;
		}
		if ((GPIOUsr[i].GPIO_Mode & GPIO_INT) || (GPIOUsr[i].GPIO_Mode & GPIO_RES))
		{
			/* fix config */
			if (GPIOUsr[i].GPIO_Mode & GPIO_INT)
			{
				GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
				if (GPIOUsr[i].GPIO_Mode & GPIO_PU)
					GPIO_InitStruct.Pull = GPIO_PULLUP;
			}
			else
			{
				/* reset is output */
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
				if (GPIOUsr[i].GPIO_Mode & GPIO_OD)
					GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
				HAL_GPIO_WritePin(GPIOUsr[i].GPIOx, GPIOUsr[i].GPIO_Pin, GPIO_PIN_SET);	/* no Reset */
			}
			HAL_GPIO_Init(GPIOUsr[i].GPIOx, &GPIO_InitStruct);
		}
		else
		{
			if (shiftMask & mask)	/* 1 is input */
				GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			else
			{
				if (gCFGparams.GPIOod & shiftMask)
					GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
				else
					GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
				/* set the default output value */
				if (shiftMask & gCFGparams.GPIOout)
					HAL_GPIO_WritePin(GPIOUsr[i].GPIOx, GPIOUsr[i].GPIO_Pin, GPIO_PIN_SET);
				else
					HAL_GPIO_WritePin(GPIOUsr[i].GPIOx, GPIOUsr[i].GPIO_Pin, GPIO_PIN_RESET);
			}
			HAL_GPIO_Init(GPIOUsr[i].GPIOx, &GPIO_InitStruct);
		}
		shiftMask <<= 1;
	}

	gCFGparams.GPIOdir = mask;			/* save the config */
	return 0;		//OK, no error
}

void GPIO_Set(unsigned long bits, unsigned long mask)
{
	size_t i;
	unsigned long shiftMask = 0x1;

	if (mask == 0)
		mask = 0xFFFFFFFF;				//default: all bits affected

	for (i = 0; i < (sizeof(GPIOUsr) / sizeof(TGPIOUsr)); i++)
	{
		if ((GPIOUsr[i].GPIO_Mode & GPIO_OUTPUT) && (mask & shiftMask))
		{
			/* just if allowed to be used as output */
			if (bits & shiftMask)
				HAL_GPIO_WritePin(GPIOUsr[i].GPIOx, GPIOUsr[i].GPIO_Pin, GPIO_PIN_SET);
			else
				HAL_GPIO_WritePin(GPIOUsr[i].GPIOx, GPIOUsr[i].GPIO_Pin, GPIO_PIN_RESET);
		}
		shiftMask <<= 1;
	}
}

unsigned long GPIO_Get(void)
{
	size_t i;
	unsigned long shiftMask = 0x1;
	unsigned long pinStates = 0L;

	for (i = 0; i < (sizeof(GPIOUsr) / sizeof(TGPIOUsr)); i++)
	{
		//if (GPIOUsr[i].GPIO_Mode & GPIO_INPUT)	/* read back all */
		{
			/* just if possible as an input */
			if (HAL_GPIO_ReadPin(GPIOUsr[i].GPIOx, GPIOUsr[i].GPIO_Pin) == GPIO_PIN_SET)
				pinStates |= shiftMask;
		}
		 shiftMask <<= 1;
	}

	return pinStates;
}

void GPIO_SetReset(unsigned long val)
{
	unsigned int i;

	/* assuming, we have just one GPIO_RES definition */
	for (i = 0; i < (sizeof(GPIOUsr) / sizeof(TGPIOUsr)); i++)
	{
		if (GPIOUsr[i].GPIO_Mode & GPIO_RES)
		{
			if (val)
				HAL_GPIO_WritePin(GPIOUsr[i].GPIOx, GPIOUsr[i].GPIO_Pin, GPIO_PIN_SET);
			else
				HAL_GPIO_WritePin(GPIOUsr[i].GPIOx, GPIOUsr[i].GPIO_Pin, GPIO_PIN_RESET);
		}
	}
}
