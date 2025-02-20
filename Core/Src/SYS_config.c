/*
 * SYS_config.c
 *
 *  Created on: Jul 2, 2018
 *      Author: tj925438
 */

#include "SYS_config.h"
#include "MEM_Pool.h"
#include "temp_sensor.h"
#include "I2C3_flash.h"
#include <string.h>

const tCFGparams defaultCFGparams = {
		.key 			= CFG_KEY_VALID,

		/* QSPI defaults */
		.QSPIdiv		= 20,			// 1: QSPI divider
		.QSPImode		= 3,			// 2: QSPI peripheral: PHA and POL, SPI mode 0..3
		.QSPIaddr		= 4,			// 3: QSPI peripheral: number ADDR bytes
		.QSPIalt		= 3,			// 4: QSPI peripheral: number ALL bytes
		.QSPIturn		= 2,			// 5: QSPI peripheral: clocks for turnaround
		.QSPIshift		= 0,			// 6: QSPI peripheral: additional 1/2 clock shift
		.QSPIqCycle 	= 0,			// 7: QSPI peripheral: quarter cycle hold shift
		.QSPIdlyb		= 0,			// 8: QSPI peripheral: DLYB used
		.QSPIspeed		= 2,			// 9: QSPI GPIO speed: 0..3
		.DLYBunit		= 0,			//10: DLYB unit: 0.128
		.DLYBphase		= 0,			//11: DLYB phase: 0..12 [3:0]
		.GPIOdir		= 0xFFFFFFDF,	//12: GPIO direction: 1 = input, 0 = output - never set output b5 = 1!
		.GPIOout		= 0x00000000,	//13: GPIO output value
		.GPIOod			= 0x00000000,	//14: GPIO OpenDrain

		/* debug and other sys config */
		.Debug			= 0,			//15: debug flags
		.CfgFlags		= 0				//16: config flags
};

tCFGparams gCFGparams;

#ifdef LEVEL_SHIFT
void CFG_Read(void)
{
	if (FLASH_Read(0, (unsigned char *)&gCFGparams, sizeof(tCFGparams)) == 0)
	{
		if (gCFGparams.key != CFG_KEY_VALID)
			memcpy(&gCFGparams, &defaultCFGparams, sizeof(tCFGparams));
	}
	else
		memcpy(&gCFGparams, &defaultCFGparams, sizeof(tCFGparams));
}

void CFG_Write(void)
{
	FLASH_Write(0, (unsigned char *)&gCFGparams, sizeof(tCFGparams));
}
#endif

void CFG_Default(void)
{
	memcpy(&gCFGparams, &defaultCFGparams, sizeof(tCFGparams));
}

/*
 * write value to a specific index in the structure
 */
void CFG_Set(unsigned long idx, unsigned long val)
{
	unsigned long *sPtr;

	/* it allows also to write 'key'! */
	if (idx >= (sizeof(tCFGparams) / 4))
		return;			/* just N unsigned long elements */

	/* handle structure like an array */
	sPtr = (unsigned long *)&gCFGparams;

	sPtr[idx] = val;
}

void CFG_Print(EResultOut out)
{
	/* print SPI config */
	Generic_Send((const uint8_t *)"Sys Config:\r\n", 13, out);
	print_log(out, (const char *)"[ 1] QSPI div          : %ld\r\n", gCFGparams.QSPIdiv);
	print_log(out, (const char *)"[ 2] QSPI mode         : %ld\r\n", gCFGparams.QSPImode);
	print_log(out, (const char *)"[ 3] QSPI addr         : %ld\r\n", gCFGparams.QSPIaddr);
	print_log(out, (const char *)"[ 4] QSPI alt          : %ld\r\n", gCFGparams.QSPIalt);
	print_log(out, (const char *)"[ 5] QSPI turn         : %ld\r\n", gCFGparams.QSPIturn);
	print_log(out, (const char *)"[ 6] QSPI shift enable : %lx\r\n", gCFGparams.QSPIshift);
	print_log(out, (const char *)"[ 7] QSPI qcycle enable: %lx\r\n", gCFGparams.QSPIqCycle);
	print_log(out, (const char *)"[ 8] QSPI DLYB enable  : %lx\r\n", gCFGparams.QSPIdlyb);
	print_log(out, (const char *)"[ 9] QSPI GPIO speed   : %ld\r\n", gCFGparams.QSPIspeed);
	print_log(out, (const char *)"[10] QSPI DLYB unit    : %ld\r\n", gCFGparams.DLYBunit);
	print_log(out, (const char *)"[11] QSPI DLYB phase   : %ld\r\n", gCFGparams.DLYBphase);
	print_log(out, (const char *)"[12] GPIOdir           : %lx\r\n", gCFGparams.GPIOdir);
	print_log(out, (const char *)"[13] GPIOout           : %lx\r\n", gCFGparams.GPIOout);
	print_log(out, (const char *)"[14] GPIOod            : %lx\r\n", gCFGparams.GPIOod);
	print_log(out, (const char *)"[15] Debug             : %lx\r\n", gCFGparams.Debug);
	print_log(out, (const char *)"[16] CfgFlags          : %lx\r\n", gCFGparams.CfgFlags);
}

void CFG_Print_hex(EResultOut out)
{
	size_t i;
	unsigned long *ulPtr = (unsigned long *)&gCFGparams;

	for (i = 0; i < (sizeof(tCFGparams) / 4); i++)
	{
		print_log(out, (const char *)"%2d : 0x%08lx\r\n", (int)i, *ulPtr++);
	}
}

void SYSINFO_print(EResultOut out)
{
	print_log(out, "%s", VERSION_STRING);
	print_log(out, (const char *)"MEMPool    : max: %d | free: %d | used: %d\r\n", (int)MEM_PoolMax(), (int)MEM_PoolAvailable(), (int)MEM_PoolWatermark());
	print_log(out, (const char *)"CPU ID     : %08lx\r\n", (long)SCB->CPUID);
	print_log(out, (const char *)"REV ID     : %08lx\r\n", *((long *)DBGMCU_BASE));
	print_log(out, (const char *)"CPU Clock  : %ld\r\n", SystemCoreClock);
	print_log(out, (const char *)"Board      : %s\r\n", BOARD_INFO);
	print_log(out, (const char *)"UART       : ");
	if (out == UART_OUT)
		print_log(out, (const char *)"Debug UART\r\n");
	else
		print_log(out, (const char *)"USB VCP\r\n");
	ADC_RunOnce(out, 1);
	////print_log(out, (const char *)"volt range : %ld\r\n", 4 - (HAL_PWREx_GetVoltageRange() >> 16));
}
