/*
 * cmd_dec.c
 *
 *  Created on: May 22, 2018
 *  Author: Torsten Jaekel
 */

#include <stdio.h>				/* for NULL */
#include <string.h>				/* for strlen */
#include "main.h"				/* for FW version */
#include "MEM_Pool.h"
#include "cmd_dec.h"
#include "syserr.h"
#include "SYS_config.h"
#include "QSPI.h"
#include "temp_sensor.h"
#ifndef NUCLEO_BOARD
#include "i2c3_flash.h"
#include "i2c1_IMU.h"
#endif
#include "SPI1_CODEC.h"

#include "ADF_PDM.h"

#include "app_azure_rtos.h"		/* for delay */

#include "math.h"

volatile int rxFlag[2] = {0,0};

extern void PutCmdLine(int idx, uint8_t *str);
extern void LED_Status(int val);

static char usrCmdBuf[USR_CMD_STR_SIZE]  = {'\0'};

/* ---------------------------------------------- */
void Generic_Send(const uint8_t *s, int l, EResultOut out)
{
	if ( ! l)
		return;

	if (out == UART_OUT)
	{
		UART_Send(s, l);
	}
	if (out == VCP_OUT)
	{
		VCP_UART_Send(s, l);
	}
}

void Gerneric_ClrRxFlag(EResultOut out)
{
	if (out == UART_OUT)
	{
		rxFlag[1] = 0;
	}
	if (out == VCP_OUT)
	{
		rxFlag[0] = 0;
	}
}

int Generic_GetRxFlag(EResultOut out)
{
	if (out == UART_OUT)
	{
		return rxFlag[1];
	}
	if (out == VCP_OUT)
	{
		return rxFlag[0];
	}
	return 0;
}

void Generic_SetRxFlag(EResultOut out)
{
	if (out == UART_OUT)
	{
		rxFlag[1] = 1;
		rxFlag[0] = 1;		/* USB VCP cannot be stopped - stop it from UART - works */
	}
	if (out == VCP_OUT)
	{
		rxFlag[0] = 1;
	}
}

uint8_t XPrintBufVCP[XPRINT_LEN];
uint8_t XPrintBufUART[XPRINT_LEN];

/* ---------------------------------------------- */
extern unsigned long GSysStatus;

/* user command string buffer */
////#define USR_CMD_STR_SIZE		80
////static char usrCmdBuf[USR_CMD_STR_SIZE]  = {'\0'};
////static char usrCmdBuf2[USR_CMD_STR_SIZE] = {'\0'};

static unsigned long usrVar = 0l;
////static unsigned long sysVal = 0l;
uint8_t LastCmdLine[2][CMD_LINE_LEN];

#define HLP_STR_OPT		"[-P|-A|-T|-D]"

/* prototypes */
ECMD_DEC_Status CMD_help(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_print(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_syscfg(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_setcfg(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_sysinfo(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_repeat(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_debug(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_delay(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_usr(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_led(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_dumpm(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_memw(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_memt(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_fwreset(TCMD_DEC_Results *res, EResultOut out);
#ifndef NUCLEO_BOARD
ECMD_DEC_Status CMD_flashr(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_flashw(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_imur(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_imuw(TCMD_DEC_Results *res, EResultOut out);
#endif

ECMD_DEC_Status CMD_qspi(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_rawspi(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_sqspi(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_qspideinit(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_qspiclk(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_cid(TCMD_DEC_Results *res, EResultOut out);

ECMD_DEC_Status CMD_codece(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_codecr(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_codecw(TCMD_DEC_Results *res, EResultOut out);

ECMD_DEC_Status CMD_adf(TCMD_DEC_Results *res, EResultOut out);

ECMD_DEC_Status CMD_test(TCMD_DEC_Results *res, EResultOut out);

const TCMD_DEC_Command Commands[] = {
		{
				.cmd = (const char *)"help",
				.help = (const char *)"[cmd] list of all defined commands or help for cmd",
				.func = CMD_help
		},
		{
				.cmd = (const char *)"print",
				.help = (const char *)"print [-n | -u] [rest of cmd]",
				.func = CMD_print
		},
		{
				.cmd = (const char *)"syscfg",
				.help = (const char *)"show system config [-D], [-d] set defaults",
				.func = CMD_syscfg
		},
		{
				.cmd = (const char *)"setcfg",
				.help = (const char *)"<idx> [val] set syscfg idx 1..n with 32bit val",
				.func = CMD_setcfg
		},
		{
				.cmd = (const char *)"sysinfo",
				.help = (const char *)"print system information",
				.func = CMD_sysinfo
		},
		{
				.cmd = (const char *)"debug",
				.help = (const char *)"set debug level [val]",
				.func = CMD_debug
		},
		{
				.cmd = (const char *)"repeat",
				.help = (const char *)"repeat [-0|-num] [cmd [;cmd ...]] cmds endless [-0] or [-num] times",
				.func = CMD_repeat
		},
		{
				.cmd = (const char *)"delay",
				.help = (const char *)"delay <millisec>",
				.func = CMD_delay
		},
		{
				.cmd = (const char *)"usr",
				.help = (const char *)"define [-p|-d [cmd [cmd; ...]]] print, define or invoke usr command",
				.func = CMD_usr
		},
		{
				.cmd = (const char *)"led",
				.help = (const char *)"led off, on1, on2 [0..3]",
				.func = CMD_led
		},
		{
				.cmd = (const char *)"dumpm",
				.help = (const char *)"MCU dump memory <addr> <bytes> [mode]",
				.func = CMD_dumpm
		},
		{
				.cmd = (const char *)"memw",
				.help = (const char *)"MCU memory write <addr> [val]",
				.func = CMD_memw
		},
		{
				.cmd = (const char *)"memt",
				.help = (const char *)"MCU memory test <addr> <words>",
				.func = CMD_memt
		},
		{
				.cmd = (const char *)"fwreset",
				.help = (const char *)"reset MCU",
				.func = CMD_fwreset
		},
#ifndef NUCLEO_BOARD
		{
				.cmd = (const char *)"flashr",
				.help = (const char *)"read flash <addr> <numbytes>",
				.func = CMD_flashr
		},
		{
				.cmd = (const char *)"flashw",
				.help = (const char *)"write flash <addr> <byte> ...",
				.func = CMD_flashw
		},
		{
				.cmd = (const char *)"imur",
				.help = (const char *)"read IMU <addr> <numbytes>",
				.func = CMD_imur
		},
		{
				.cmd = (const char *)"imuw",
				.help = (const char *)"write IMU <addr> <byte> ...",
				.func = CMD_imuw
		},
#endif
		{
				.cmd = (const char *)"codece",
				.help = (const char *)"enable SPI and CODEC",
				.func = CMD_codece
		},
		{
				.cmd = (const char *)"codecr",
				.help = (const char *)"read CODEC registers <addr> <num>",
				.func = CMD_codecr
		},
		{
				.cmd = (const char *)"codecw",
				.help = (const char *)"write CODEC registers <addr> <byte> ...",
				.func = CMD_codecw
		},
		{
				.cmd = (const char *)"adf",
				.help = (const char *)"stop or start ADF PDM [0|1]",
				.func = CMD_adf
		},

		/* chip specific */
		{
				.cmd = (const char *)"qspi",
				.help = (const char *)"qspi [-numRdWords] <cmd> <addr> <alt> [wrWord ...]",
				.func = CMD_qspi
		},
		{
				.cmd = (const char *)"rawspi",
				.help = (const char *)"rawspi <byte> ...",
				.func = CMD_rawspi
		},
		{
				.cmd = (const char *)"sqspi",
				.help = (const char *)"sqspi [0|1..3] get or select active QSPI NCSs",
				.func = CMD_sqspi
		},
		{
				.cmd = (const char *)"qspideinit",
				.help = (const char *)"uninitialize QSPI (release GPIO pins)",
				.func = CMD_qspideinit
		},
		{
				.cmd = (const char *)"qspiclk",
				.help = (const char *)"get or set QSPI clk divider [0|1..255]",
				.func = CMD_qspiclk
		},
		{
				.cmd = (const char *)"cid",
				.help = (const char *)"read ChipID via QSPI",
				.func = CMD_cid
		},
		{
				.cmd = (const char *)"test",
				.help = (const char *)"test command",
				.func = CMD_test
		},
};

static unsigned int CMD_DEC_findCmd(const char *cmd, unsigned int len)
{
	size_t i;
	for (i = 0; i < (sizeof(Commands)/sizeof(TCMD_DEC_Command)); i++)
	{
		if (Commands[i].func == NULL)
			continue;
		/* if length does not match - keep going */
		if (len != (unsigned int)strlen(Commands[i].cmd))
			continue;
		if (strncmp(cmd, Commands[i].cmd, (size_t)len) == 0)
			return (unsigned int)i;
	}

	return (unsigned int)-1;
}

static void CMD_DEC_DecodeError(ECMD_DEC_Status err, EResultOut out)
{
	/*
	 * TODO: decode command errors - empty for now, silently ignored on errors
	 */
	(void)err;
	(void)out;

	return;
}

static unsigned int CMD_DEC_decode(char *cmdStr, TCMD_DEC_Results *res)
{
	unsigned int i;
	int state;
	char *xCmdStr = cmdStr;

	/* set default results */
	res->cmd = NULL;
	res->cmdLen = 0;
	res->offset = 0;
	res->opt = NULL;
	res->str = NULL;
	res->ctl = 0;
	res->num = 0;
	/* set all values to 0 as default, if omitted but used */
	for (i = 0; i < CMD_DEC_NUM_VAL; i++)
		res->val[i] = 0L;

	state = 0;
	i = 0;
	while ((*cmdStr != '\0') && (*cmdStr != '\r') && (*cmdStr != '\n') && (*cmdStr != CMD_DEC_SEPARATOR) && (*cmdStr != CMD_DEC_COMMENT))
	{
		/* skip leading spaces, tabs */
		while ((*cmdStr == ' ') || (*cmdStr == '\t'))
		{
			if ((state == 1) || (state == 3))
			{
				////*cmdStr = '\0';
				state++;
			}
			if (state == 5)
				state--;

			cmdStr++;
		}

		switch (state)
		{
		case 0:	/* find command keyword	*/
			res->cmd = cmdStr;
			////res->cmdLen = 0;
			state = 1;
			break;
		case 1:	/* find end of keyword */
			if ((*cmdStr != '\0') && (*cmdStr != '\r') && (*cmdStr != '\n') && (*cmdStr != CMD_DEC_SEPARATOR) && (*cmdStr != CMD_DEC_COMMENT))
			{
				cmdStr++;
				res->cmdLen++;
			}
			break;
		case 2:	/* check if option is there */
			if (*cmdStr == CMD_DEC_OPT_SIGN)
			{
				res->opt = cmdStr;
				state = 3;
			}
			else
				state = 4;
			break;
		case 3:	/* find end of option string */
			if ((*cmdStr != '\0') && (*cmdStr != '\r') && (*cmdStr != '\n') && (*cmdStr != CMD_DEC_SEPARATOR) && (*cmdStr != CMD_DEC_COMMENT))
				cmdStr++;
			break;
		case 4: /* now we scan just values or option value */
			if (i < CMD_DEC_NUM_VAL)
			{
				if (i == 0)
					if ( ! res->str)
						res->str = cmdStr;
				/* ignore incorrect values, e.g. strings, missing 0xAA etc. */
				/*
				 * ATTENTION: this sscanf just signed values (decimal or hex), values above
				 * 0x8000000 results in a wrong value!
				 */
				////if (sscanf(cmdStr, "%li", (long int *)(&res->val[i])) == 1)
				////	i++;

				////fixed, and add '$' for user variable
				if (*cmdStr == '$')
				{
					res->val[i] = usrVar;
					i++;
				}
				else
				{
					if ((cmdStr[1] == 'x') || (cmdStr[1] == 'X'))
					{
						if (sscanf(cmdStr, "%lx", (unsigned long *)(&res->val[i])) == 1)
							i++;
					}
					else
					{
						if (sscanf(cmdStr, "%lu", (unsigned long *)(&res->val[i])) == 1)
							i++;
					}
				}

				res->num = i;
			}
			state = 5;
			break;
		case 5:	/* skip value characters */
			if ((*cmdStr != '\0') && (*cmdStr != '\r') && (*cmdStr != '\n') && (*cmdStr != CMD_DEC_SEPARATOR) && (*cmdStr != CMD_DEC_COMMENT))
				cmdStr++;
			break;
		}
	} /* end while */

	if (*cmdStr == CMD_DEC_SEPARATOR)
	{
		////*cmdStr++ = '\0';
		cmdStr++;
		res->offset = (unsigned long)(cmdStr - xCmdStr);
		return (unsigned int)(cmdStr - xCmdStr);
	}

	if (*cmdStr == CMD_DEC_COMMENT)
		*cmdStr = '\0';

	if (res->cmd)
		if (*res->cmd == '\0')
			res->cmd = NULL;

	if ((*cmdStr == '\r') || (*cmdStr == '\n'))
		*cmdStr = '\0';

	return 0;
}

ECMD_DEC_Status CMD_DEC_execute(char *cmd, EResultOut out)
{
	TCMD_DEC_Results *cmdRes;
	unsigned int res, offset, idx;
	unsigned long numCmdsDone = 0;

	cmdRes = MEM_PoolAlloc(sizeof(TCMD_DEC_Results));
	if ( ! cmdRes)
	{
		return CMD_DEC_OOMEM;
	}

	offset = 0;
	do
	{
		res = CMD_DEC_decode(cmd + offset, cmdRes);
		offset += res;

		if (cmdRes->cmd)
			if (*cmdRes->cmd != '\0')	/* ignore empty line, just spaces */
			{
				idx = CMD_DEC_findCmd(cmdRes->cmd, cmdRes->cmdLen);
				if (idx != (unsigned int)-1)
				{
					ECMD_DEC_Status err;

					err = Commands[idx].func(cmdRes, out);

					/* decode the err code */
					CMD_DEC_DecodeError(err, out);

					numCmdsDone++;
				}
				else
				{
					Generic_Send((const uint8_t *)"*E: unknown >", 13, out);
					Generic_Send((const uint8_t *)cmdRes->cmd, cmdRes->cmdLen, out);
					Generic_Send((const uint8_t *)"<\r\n", 3, out);
					SYS_SetError(SYS_ERR_INVALID_CMD);
					MEM_PoolFree(cmdRes);
					return CMD_DEC_INVALID;
				}
			}

		if (cmdRes->ctl == 1)
			break;

	} while (res);

	MEM_PoolFree(cmdRes);

	/* keep the last command - but just if not an empty line */
	if (*cmd)
	{
		if (out == VCP_OUT)
			strcpy((char *)LastCmdLine[0], cmd);
		else
			strcpy((char *)LastCmdLine[1], cmd);
	}

#if 0
	if ( ! numCmdsDone)
		SYS_SetError(SYS_ERR_EMPTYCMD);
#endif
	return CMD_DEC_OK;
}

/* ---------------------------------------------------------------------- */

/* helper function for help - keyword length */
static unsigned int CMD_keywordLen(const char *str)
{
	unsigned int l = 0;

	while (*str)
	{
		if ((*str == ';') || (*str == ' ') || (*str == '\r') || (*str == '\n'))
			break;
		str++;
		l++;
	}

	return l;
}

/* helper function for print - string length up to ';' or '#' */
static unsigned int CMD_lineLen(const char *str)
{
	unsigned int l = 0;

	while (*str)
	{
		if ((*str == ';') || (*str == '#') || (*str == '\r') || (*str == '\n'))
			break;
		str++;
		l++;
	}

	return l;
}

#if 0
/* helper function to find string <filename> after a value <IDX> */
static const char * CMD_nextStr(const char *str)
{
	const char *resStr = NULL;		/* set to invalid result */

	/* find space after <IDX> */
	while (*str)
	{
		if ((*str == ' ') || (*str == '\r') || (*str == '\n'))
			break;
		str++;
	}

	/* find start of <filename> as not a space */
	while (*str)
	{
		if (*str != ' ')
		{
			resStr = str;
			break;
		}
		str++;
	}

	return resStr;
}
#endif

/* other helper functions */

void hex_dump(uint8_t *ptr, uint16_t len, int mode, EResultOut out)
{
	int i = 0;
	int xLen = (int)len;

#if 0
	if (ptr == NULL)
		return;
#endif
	if (len == 0)
		return;

	{
		while (xLen > 0)
		{
			if ((mode == 1) || (mode == 0))
			{
				//bytes
				print_log(out, (const char *)"%02X ", (int)*ptr);
				ptr++;
				xLen--;
				i++;
			}
			else
			if (mode == 2)
			{
				//short words, little endian
				print_log(out, (const char *)"%04X ", (int)(*ptr | (*(ptr + 1) << 8)));
				ptr += 2;
				xLen -= 2;
				i += 2;
			}
			else
			if (mode == 4)
			{
				//32bit words, little endian
				print_log(out, (const char *)"%08lX ", (unsigned long)(*ptr |
																	  (*(ptr + 1) << 8) |
																	  (*(ptr + 2) << 16) |
																	  (*(ptr + 3) << 24)
																	 ));
				ptr += 4;
				xLen -= 4;
				i += 4;
			}
			else
			if (mode == 12)
			{
				i = 1;
				////print_log(out, (const char *)"len: %d\r\n", xLen);
				//CLATCH FIFO display, 12 bytes (6 words) - with endian flip and just valid entries
				if ((*(ptr + 11) & 0x80) == 0x80)
				{
					print_log(out, (const char *)"0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
						*(ptr + 11), *(ptr + 10), *(ptr + 9), *(ptr + 8), *(ptr + 7), *(ptr + 6),
						*(ptr + 5), *(ptr + 4), *(ptr + 3), *(ptr + 2), *(ptr + 1), *(ptr + 0));
					i = 16;	//force a newline
				}
				ptr += 12;
				xLen -= 12;
				if (xLen < 12)
					xLen = 0;			//break the loop
			}
			else
				break;		//just for now to make it safe

			if ((i % 16) == 0)
				Generic_Send((const uint8_t *)"\r\n", 2, out);
		}

		if ((i % 16) != 0)
			Generic_Send((const uint8_t *)"\r\n", 2, out);
	}
}

/* verify the option and get the SPI interface number - starting at 1 */
int CMD_getSPIoption(char *str)
{
	if (str)
	{
		if (strncmp(str, "-T", 2) == 0)
			return 1;
		if (strncmp(str, "-A", 2) == 0)
			return 3;
		//dummy SPI
		if (strncmp(str, "-D", 2) == 0)
			return 6;
	}
	return 2;		//-P is default
}

/* ------------ command handlers ----------------------------*/

ECMD_DEC_Status CMD_help(TCMD_DEC_Results *res, EResultOut out)
{
	unsigned int idx;

	if (res->str)
	{
		/* we have 'help <CMD>' - print just the single line help */
		idx = CMD_DEC_findCmd(res->str, CMD_keywordLen(res->str));
		if (idx != (unsigned int)-1)
			print_log(out, (const char *)"%-10s: %s\r\n", Commands[idx].cmd, Commands[idx].help);
		else
		{
			Generic_Send((const uint8_t *)"*E: unknown\r\n", 13, out);
			return CMD_DEC_UNKNOWN;
		}
	}
	else
	{
		/* just 'help' - print entire list */
		for (idx = 0; (size_t)idx < (sizeof(Commands)/sizeof(TCMD_DEC_Command)); idx++)
			print_log(out, (const char *)"%-10s: %s\r\n", Commands[idx].cmd, Commands[idx].help);
	}

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_print(TCMD_DEC_Results *res, EResultOut out)
{
	if (res->str)
		Generic_Send((const uint8_t *)res->str, CMD_lineLen(res->str), out);
	if (res->opt)
	{
		if (strncmp(res->opt, "-n", 2) == 0)
		{
			/* no newline to add */
		}
		else
			Generic_Send((const uint8_t *)"\r\n", 2, out);
	}
	else
		Generic_Send((const uint8_t *)"\r\n", 2, out);

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_syscfg(TCMD_DEC_Results *res, EResultOut out)
{
	if (res->opt)
	{
		if (strncmp(res->opt, "-d", 2) == 0)
		{
			CFG_Default();
		}
		else
		if (strncmp(res->opt, "-D", 2) == 0)
		{
				CFG_Print_hex(out);
		}
	}
	else
	{
		CFG_Print(out);
	}
	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_setcfg(TCMD_DEC_Results *res, EResultOut out)
{
	(void)out;

	if (res->num < 2)
		return CMD_DEC_INVPARAM;

	CFG_Set(res->val[0], res->val[1]);
	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_repeat(TCMD_DEC_Results *res, EResultOut out)
{
	int numRepeat = 1;			//default is 1
	int endLess = 0;
	char *keepStr;				//remember the command(s) to repeat

	if (res->opt)
	{
		if (*res->opt == '-')
		{
			sscanf((res->opt + 1), (const char *)"%i", &numRepeat);
		}
	}

	if (numRepeat == 0)
	{
		endLess = 1;
		numRepeat = 1;
	}

	keepStr = res->str;			//remember the rest of string as commands(s)

	Gerneric_ClrRxFlag(out);			//prepare to break endless loop with any UART Rx */
	/* loop numRepeat times to repeat the commands(s) */
	do
	{
		/* check UART reception - any received character will break the loop */
		if (Generic_GetRxFlag(out))
		{
			break;				//break loop with any UART Rx received */
		}

		/* call the command interpreter - one recursive step deeper */
		CMD_DEC_execute(keepStr, out);

		/* if numRepeat is 0 - we do it endless */
		if (endLess)
			continue;
		else
			numRepeat--;
	} while (numRepeat);

	res->ctl = 1;			//break the outer command interpreter, we can have ';', done here

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_debug(TCMD_DEC_Results *res, EResultOut out)
{
	(void)out;

	gCFGparams.Debug = res->val[0];

	return CMD_DEC_OK;
}


ECMD_DEC_Status CMD_sysinfo(TCMD_DEC_Results *res, EResultOut out)
{
	(void)res;

	SYSINFO_print(out);

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_delay(TCMD_DEC_Results *res, EResultOut out)
{
	(void)out;

	tx_thread_sleep(res->val[0]);

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_usr(TCMD_DEC_Results *res, EResultOut out)
{
	if (res->opt)
	{
		if (strncmp(res->opt, "-d", 2) == 0)
		{
			/* define the user command */
			if (res->str)
				strncpy(usrCmdBuf, res->str, sizeof(usrCmdBuf)-1);
			else
				usrCmdBuf[0] = '\0';

			res->ctl = 1;			//break the outer command interpreter, we can have ';', done here

			return CMD_DEC_OK;
		}

		if (strncmp(res->opt, "-p", 2) == 0)
		{
			/* print the defined the user command */
			Generic_Send((const uint8_t *)usrCmdBuf, (int)strlen(usrCmdBuf), out);
			Generic_Send((const uint8_t *)"\r\n", 2, out);

			return CMD_DEC_OK;
		}
	}

	if (usrCmdBuf[0] != '\0')
		return CMD_DEC_execute(usrCmdBuf, out);

	/* empty command, if not defined it is OK */
	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_sqspi(TCMD_DEC_Results *res, EResultOut out)
{
	(void)out;

	if (res->val[0] == 0)
	{
		print_log(out, "QSPI NCS: %ld\r\n", QSPI_GetQSPI());
		return CMD_DEC_OK;
	}

	QSPI_SetQSPI(res->val[0]);

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_qspi(TCMD_DEC_Results *res, EResultOut out)
{
	unsigned long numRead = 0;			//default is 0
	unsigned long x;

	/* check if we have enough parameters */
	x = 1 + ((gCFGparams.QSPIaddr + 3) / 4) + ((gCFGparams.QSPIalt + 3) / 4);
	if (res->num < x)
		return CMD_DEC_INVPARAM;

	if (res->opt)
	{
		if (*res->opt == '-')
		{
			sscanf((res->opt + 1), (const char *)"%i", (int *)&numRead);
		}
	}

	if (numRead)
	{
		/* we can only write 3 words as header on a read, not possible to write when doing a read */
		if (res->num > 3)
			res->num = 3;
	}

	if (gCFGparams.Debug & DBG_VERBOSE)
	{
		unsigned long i;
		print_log(out, "Write: %d\r\n", (int)res->num);
		for (i = 0; i < res->num; i++)
		{
			if (i == 0)
				print_log(out, " CMD : %02lx\r\n", res->val[i]);
			if (i == 1)
			{
				switch (gCFGparams.QSPIaddr)
				{
				case 0 : break;
				case 1 : print_log(out, " ADDR: %02lx\r\n", res->val[i]); break;
				case 2 : print_log(out, " ADDR: %04lx\r\n", res->val[i]); break;
				case 3 : print_log(out, " ADDR: %06lx\r\n", res->val[i]); break;
				default: print_log(out, " ADDR: %08lx\r\n", res->val[i]); break;
				}
			}

			if (i == 2)
			{
				switch (gCFGparams.QSPIalt)
				{
				case 0 : break;
				case 1 : print_log(out, " ALT : %02lx", res->val[i]); break;
				case 2 : print_log(out, " ALT : %04lx", res->val[i]); break;
				case 3 : print_log(out, " ALT : %06lx", res->val[i]); break;
				default: print_log(out, " ALT : %08lx", res->val[i]); break;
				}
				if (res->num > 3)
					print_log(out, "\r\n wr  : ");
			}
			if (i > 2)
				print_log(out, "%08lx ", res->val[i]);
		}
		if (numRead)
		{
			print_log(out, "\r\nRead: %d\r\n", (int)numRead);
			print_log(out, " turn: %d\r\n", (int)gCFGparams.QSPIturn);
			print_log(out, " num : %d", (int)numRead);
		}
		Generic_Send((uint8_t *)"\r\n", 2, out);
	}

	QSPI_Transaction(res->val, res->num, numRead);

	////if (gCFGparams.Debug & DBG_VERBOSE)
	{
		if (numRead)
		{
			unsigned long i;
			for (i = 0; i < numRead; i++)
			{
				print_log(out, " %08lx", res->val[i]);
			}
			Generic_Send((uint8_t *)"\r\n", 2, out);
		}
	}

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_qspideinit(TCMD_DEC_Results *res, EResultOut out)
{
	(void) res;
	(void) out;

	QSPI_DeInit();

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_qspiclk(TCMD_DEC_Results *res, EResultOut out)
{
	if ( ! res->val[0])
	{
		print_log(out, "QSPI div: %d [%d KHz]\r\n", (int)gCFGparams.QSPIdiv, (int)((SystemCoreClock / 1000) / gCFGparams.QSPIdiv));
		return CMD_DEC_OK;
	}

	QSPI_SetClock(res->val[0]);

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_cid(TCMD_DEC_Results *res, EResultOut out)
{
	(void)res;

	unsigned long cid;

	cid = QSPI_ReadChipID(out);
	print_log(out, "0x%08lx\r\n", (unsigned long int)cid);

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_test(TCMD_DEC_Results *res, EResultOut out)
{
	(void)res;
	(void)out;
#if 0
	static unsigned char spiTx[512];

	unsigned long i;

	if (res->num < 2)
		return CMD_DEC_INVPARAM;

	for (i = 0; i < res->num; i++)
		spiTx[i] = (unsigned char)res->val[i];

	OSPI_SPITransaction(spiTx, res->num);
	for (i = 0; i < res->num; i++)
	{
		print_log(out, "%02x ", spiTx[i]);
	}
	print_log(out, "\r\n");
#endif

#if 0
	/* measure VCP UART speed */
	int i;
	unsigned int startTS, endTS;
	startTS = HAL_GetTick();
	for (i = 0; i < 10000; i++)
		VCP_UART_Send((const uint8_t *)"1111111111222222222233333333334444444444555555555566666666667777", 64);
	endTS = HAL_GetTick();
	print_log(out, "\r\nstart: %u | end: %u | delta: %u | %u bytes\r\n", startTS, endTS, endTS - startTS, i * 64);
#endif

#if 0
	/* test if DCache is usable */
	unsigned long *p = (unsigned long *)0x60000000;
	unsigned long i;
	unsigned long val;
	val = res->val[1];
	for (i = 0; i < res->val[0]; i++)
		*p++ = val++;
	p = (unsigned long *)0x60000000;
	for (i = 0; i< res->val[0]; i++)
	{
		val = *p++;
		print_log(out, "%8lx ", val);
	}
	print_log(out, "\r\n");
#endif

#if 0
	/* test PB5 - it works (toggles) */
	if (res->val[0] == 0)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
#endif

#if 0
	/* test PB15 - release with high CODEC SHDNZ */
	if (res->val[0] == 0)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
#endif

#if 0
	extern void MX_SAI1_Init(void);
	extern SAI_HandleTypeDef hsai_BlockA1;
	MX_SAI1_Init();

	HAL_SAI_Receive_DMA(&hsai_BlockA1, SAIRxBuf, 192 * 16);
#endif
	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_led(TCMD_DEC_Results *res, EResultOut out)
{
	(void)out;

	LED_Status((int)res->val[0]);

	return CMD_DEC_OK;
}

#ifndef NUCLEO_BOARD
ECMD_DEC_Status CMD_flashr(TCMD_DEC_Results *res, EResultOut out)
{
	unsigned char *b;

	if (res->val[1] == 0)
		return CMD_DEC_INVPARAM;

	b = (unsigned char *)MEM_PoolAlloc(MEM_POOL_SEG_SIZE);
	if ( !b )
		return CMD_DEC_OOMEM;

	if (FLASH_Read(res->val[0], b, res->val[1]) == 0)
	{
		hex_dump(b, res->val[1], 1, out);
	}

	MEM_PoolFree(b);
	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_flashw(TCMD_DEC_Results *res, EResultOut out)
{
	(void) out;
	unsigned char *b;

	unsigned long i;

	if (res->num < 2)
		return CMD_DEC_INVPARAM;

	b = (unsigned char *)MEM_PoolAlloc(MEM_POOL_SEG_SIZE);
	if ( !b )
		return CMD_DEC_OOMEM;

	for (i = 0; i < res->num - 1; i++)
	{
		b[i] = (unsigned char)res->val[1 + i];
	}

	if (FLASH_Write(res->val[0], b, res->num - 1) != 0)
	{
		MEM_PoolFree(b);
		return CMD_DEC_ERROR;
	}

	MEM_PoolFree(b);
	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_imur(TCMD_DEC_Results *res, EResultOut out)
{
	unsigned char *b;

	if (res->val[1] == 0)
		return CMD_DEC_INVPARAM;

	b = (unsigned char *)MEM_PoolAlloc(MEM_POOL_SEG_SIZE);
	if ( !b )
		return CMD_DEC_OOMEM;

	if (res->val[1] == 0)
		res->val[1] = 1;
	if (IMU_Read(res->val[0], b, res->val[1]) == 0)
	{
		hex_dump(b, res->val[1], 1, out);
	}

	MEM_PoolFree(b);
	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_imuw(TCMD_DEC_Results *res, EResultOut out)
{
	(void) out;
	unsigned char *b;

	unsigned long i;

	if (res->num < 2)
		return CMD_DEC_INVPARAM;

	b = (unsigned char *)MEM_PoolAlloc(MEM_POOL_SEG_SIZE);
	if ( !b )
		return CMD_DEC_OOMEM;

	for (i = 0; i < res->num - 1; i++)
	{
		b[i] = (unsigned char)res->val[1 + i];
	}

	if (IMU_Write(res->val[0], b, res->num - 1) != 0)
	{
		MEM_PoolFree(b);
		return CMD_DEC_ERROR;
	}

	MEM_PoolFree(b);
	return CMD_DEC_OK;
}
#endif

ECMD_DEC_Status CMD_rawspi(TCMD_DEC_Results *res, EResultOut out)
{
	(void)res;
	(void)out;
	unsigned char *spiTx;
	unsigned long i;
	uint8_t err;

	if (res->num < 2)
		return CMD_DEC_INVPARAM;

	spiTx = (unsigned char *)MEM_PoolAlloc(MEM_POOL_SEG_SIZE);
	if ( ! spiTx )
		return CMD_DEC_OOMEM;

	for (i = 0; i < res->num; i++)
		spiTx[i] = (unsigned char)res->val[i];

	err = OSPI_SPITransaction(spiTx, res->num);
	if ( ! err)
	{
		for (i = 0; i < res->num; i++)
		{
			print_log(out, "%02x ", spiTx[i]);
		}
		print_log(out, "\r\n");
	}

	MEM_PoolFree(spiTx);
	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_dumpm(TCMD_DEC_Results *res, EResultOut out)
{
	hex_dump((uint8_t *)res->val[0], (uint16_t)res->val[1], res->val[2], out);

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_memw(TCMD_DEC_Results *res, EResultOut out)
{
	(void)out;
	unsigned long i;
	unsigned long *p = (unsigned long *)res->val[0];	//address

	for (i = 1; i < res->num; i++)
		*p++ = res->val[i];

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_fwreset(TCMD_DEC_Results *res, EResultOut out)
{
	(void)res;
	(void)out;

	NVIC_SystemReset();

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_memt(TCMD_DEC_Results *res, EResultOut out)
{
	unsigned long *p;
	unsigned long i;
	unsigned long tick1, tick2;

	if ( !res->val[1] )
		return CMD_DEC_OK;

	//write memory:
	p = (unsigned long *)res->val[0];
	tick1 = HAL_GetTick();
	for (i = 0; i < res->val[1]; i++)
	{
		*p = (unsigned long)p;
		p++;
	}
	tick1 = HAL_GetTick() - tick1;
	//check memory:
	tick2 = HAL_GetTick();
	p = (unsigned long *)res->val[0];
	for (i = 0; i < res->val[1]; i++)
	{
		if (*p != (unsigned long)p)
		{
			hex_dump((uint8_t *)p, 16, 4, out);
			break;
		}
		p++;
	}
	tick2 = HAL_GetTick() - tick2;

	print_log(out, "WR: %d [ms] | RD: %d [ms]\r\n", (int)tick1, (int)tick2);

	return CMD_DEC_OK;
}

#ifndef SPDIF_TEST
#define SPDIF_FRAMES			192
#define SAI_CHANNELS			2		/* stereo, two MIC channels */
#define SAI_BYTES_PER_SAMPLE	4		/* 24bit, for SPDIF - used as 32bit - buffer as int32_t! */
#define SAI_AUDIO_FREQ			48		/* 48 KHz */
#define SAI_BUFFER_SIZE			4		/* as N times 1ms samples - 1 second */
#define SAI_DOUBLE_BUFFER		1		/* 2 for double buffering! */
////int32_t SAIRxBuf[(SAI_CHANNELS * SAI_AUDIO_FREQ) * SAI_BUFFER_SIZE * SAI_DOUBLE_BUFFER] __aligned(4);
int32_t SAIRxBuf[SPDIF_FRAMES * 2] __aligned(4);
#endif

#ifdef SPDIF_TEST
#define SPDIF_FRAMES	192
#define SPDIF_WORDS		4				/* 24bit values, but 32bit word */
#define SPDIF_CHANNELS	2
#define	SPDIF_AUDIOFREQ	48
#define SPDIF_DOUBLEBUF	2
#define SPDIF_CS_WORD	0x02010204;		/* 48KHz, no copy..., but w/o channel */

int32_t SPDIF_out_test[SPDIF_CHANNELS * SPDIF_FRAMES] __aligned(4);
void GenerateSPDIFOut(void)
{
	int i;
	int32_t val = 0;
	double d;
	unsigned long CSword = SPDIF_CS_WORD;

	for (i = 0; i < SPDIF_FRAMES; i++)
	{
		d  = sin(((2 * M_PI) * i) / SPDIF_AUDIOFREQ);
		d *= (double)0x00700000;		//volume scaling
		val = (int32_t)d;
		val &= 0x00FFFFFF;
		if (i < 32)
		{
			val |= (CSword & 0x1) << 26;
			CSword >>= 1;
		}
		if (i == 32)
		{
			val |= (1 << 26);
		}

		if (i == 20)
		{
			SPDIF_out_test[SPDIF_CHANNELS * i + 0] = val | (1 << 26);	/* channel A */
			SPDIF_out_test[SPDIF_CHANNELS * i + 1] = val;
		}
		else if (i == 21)
		{
			SPDIF_out_test[SPDIF_CHANNELS * i + 0] = val;
			SPDIF_out_test[SPDIF_CHANNELS * i + 1] = val | (1 << 26);	/* channel B */
		}
		else
		{
			SPDIF_out_test[SPDIF_CHANNELS * i + 0] = val;
			SPDIF_out_test[SPDIF_CHANNELS * i + 1] = val;
		}
	}
}
#endif

ECMD_DEC_Status CMD_codece(TCMD_DEC_Results *res, EResultOut out)
{
	(void)res;
	(void)out;
	extern void MX_SAI_Init(void);

	extern SAI_HandleTypeDef hsai_BlockA1;
	extern SAI_HandleTypeDef hsai_BlockB1;

	extern void MX_SPI1_Init(void);
#ifndef SPDIF_TEST_A
	MX_SPI1_Init();
#endif
#ifdef SPDIF_TEST
	GenerateSPDIFOut();
	MX_SAI_Init();
#if 1
	////HAL_SAI_Transmit_DMA(&hsai_BlockB1, (uint8_t *)SPDIF_out_test, (uint16_t)(sizeof(SPDIF_out_test) / sizeof(uint32_t)));		//FAILS!!!!!!!!
	HAL_SAI_Transmit_IT(&hsai_BlockB1, (uint8_t *)SPDIF_out_test, (uint16_t)(sizeof(SPDIF_out_test) / sizeof(uint32_t)));			//WORKS!
#else
	while (1)
	{
		HAL_SAI_Transmit(&hsai_BlockB1, (uint8_t *)SPDIF_out_test, (uint16_t)(sizeof(SPDIF_out_test) / sizeof(uint32_t)), 5000);	//WORKS!
	}
#endif
#else
#if 1
	/* release CODEC from standby */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
#endif
	MX_SAI_Init();
#if 1
	////HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t *)SAIRxBuf, (uint16_t)sizeof(SAIRxBuf) / sizeof(uint32_t));
	HAL_SAI_Receive_IT(&hsai_BlockA1, (uint8_t *)SAIRxBuf, (uint16_t)sizeof(SAIRxBuf) / sizeof(uint32_t));
	HAL_SAI_Transmit_IT(&hsai_BlockB1, (uint8_t *)SAIRxBuf, (uint16_t)(sizeof(SAIRxBuf) / sizeof(uint32_t)));

	/* wait at least 2ms for CODEC out of standby */
	tx_thread_sleep(4);
#endif
	////HAL_SAI_Transmit_DMA(&hsai_BlockB1, (uint8_t *)SAIRxBuf, (uint16_t)sizeof(SAIRxBuf) / sizeof(uint32_t));
	////HAL_SAI_Transmit_IT(&hsai_BlockB1, (uint8_t *)SAIRxBuf, (uint16_t)(sizeof(SAIRxBuf) / sizeof(uint32_t)));
#if 1
	/* configure CODEC for two channels PDM MIC */
	res->val[0] = 0x02; res->val[1] = 0x01;
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);
	tx_thread_sleep(5);										/* wait until CODEC out of sleep */
	res->val[0] = 0x07; res->val[1] = 0x60;					/* 48KHz, 24bit samples */
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);
	res->val[0] = 0x0B; res->val[1] = 0x00;
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);
	res->val[0] = 0x0C; res->val[1] = 0x20;					//see reg. 0x0B = mono with 0x00 and 0x20!
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);
	res->val[0] = 0x1F; res->val[1] = 0x00;
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);
	res->val[0] = 0x22; res->val[1] = 0x41;
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);
	res->val[0] = 0x2B; res->val[1] = 0x40;
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);
	res->val[0] = 0x3C; res->val[1] = 0x40;
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);
	res->val[0] = 0x3E; res->val[1] = 0xB0;			//gain
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);
	res->val[0] = 0x41; res->val[1] = 0x40;
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);
	res->val[0] = 0x43; res->val[1] = 0xB0;			//gain
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);
	res->val[0] = 0x73; res->val[1] = 0xC0;
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);
	res->val[0] = 0x74; res->val[1] = 0xC0;
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);
	res->val[0] = 0x75; res->val[1] = 0x60;
	CODEC_WriteRegisters(res->val[0], &res->val[1], 1);

	tx_thread_sleep(10);	/* necessary to wait here! */
	res->val[0] = 0x77;
	/* cross check if all is OK */
	CODEC_ReadRegisters(res->val[0], &(res->val[0]), 1);
	if (res->val[0] == 0xE0)
		print_log(out, "OK\r\n");
	else
		print_log(out, "FAIL\r\n");
#endif
#endif
	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_codecr(TCMD_DEC_Results *res, EResultOut out)
{
	unsigned long l;
	unsigned long i;

	if (res->val[1] == 0)
		res->val[1] = 1;
	l = res->val[1];
	CODEC_ReadRegisters(res->val[0], &(res->val[0]), res->val[1]);

	for (i = 0; i < l; i++)
	{
		print_log(out, "%02x ", (unsigned int)res->val[i]);
	}
	print_log(out, "\r\n");

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_codecw(TCMD_DEC_Results *res, EResultOut out)
{
	(void)out;

	CODEC_WriteRegisters(res->val[0], &(res->val[1]), res->num - 1);

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_adf(TCMD_DEC_Results *res, EResultOut out)
{
	(void)out;

	if (res->val[0])
		ADF_PDM_Init();
	else
		ADF_PDM_DeInit();

	return CMD_DEC_OK;
}

