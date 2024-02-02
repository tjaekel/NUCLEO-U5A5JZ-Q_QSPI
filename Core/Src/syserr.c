/** syserr.c
 *
 *  Created on: Oct 23, 2019
 *      Author: tj925438
 */

#include "syserr.h"
#include "stdio.h"
#include "main.h"

extern char GitmBuf[120];

unsigned long GSysErr;

void SYS_SetError(unsigned long errCode)
{
	GSysErr |= errCode;
}

inline unsigned long SYS_GetError(void)
{
	return GSysErr;
}

void SYS_ClrError(void)
{
	////BSP_LED_Off(LED_RED);		//red LED Off
	GSysErr = 0;
}
