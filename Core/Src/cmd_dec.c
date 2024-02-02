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

#include "app_azure_rtos.h"		/* for delay */

volatile int rxFlag[2] = {0,0};

extern void PutCmdLine(int idx, uint8_t *str);

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

volatile int Generic_GetRxFlag(EResultOut out)
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

ECMD_DEC_Status CMD_qspi(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_qspideinit(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_qspiclk(TCMD_DEC_Results *res, EResultOut out);
ECMD_DEC_Status CMD_cid(TCMD_DEC_Results *res, EResultOut out);

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

		/* chip specific */
		{
				.cmd = (const char *)"qspi",
				.help = (const char *)"qspi [-numRdWords] <cmd> <addr> <alt> [wrWord ...]",
				.func = CMD_qspi
		},
		{
				.cmd = (const char *)"qspideinit",
				.help = (const char *)"uninitialize QSPI (release GPIO pins)",
				.func = CMD_qspideinit
		},
		{
				.cmd = (const char *)"qspiclk",
				.help = (const char *)"get or set QSDPI clk divider [0|1..255]",
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

ECMD_DEC_Status CMD_qspi(TCMD_DEC_Results *res, EResultOut out)
{
	int numRead = 0;			//default is 0
	int x;

	/* check if we have enough parameters */
	x = 1 + ((gCFGparams.QSPIaddr + 3) / 4) + ((gCFGparams.QSPIalt + 3) / 4);
	if (res->num < x)
		return CMD_DEC_INVPARAM;

	if (res->opt)
	{
		if (*res->opt == '-')
		{
			sscanf((res->opt + 1), (const char *)"%i", &numRead);
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
	unsigned long cid;

	cid = QSPI_ReadChipID(out);
	print_log(out, "0x%08lx\r\n", (unsigned long int)cid);

	return CMD_DEC_OK;
}

ECMD_DEC_Status CMD_test(TCMD_DEC_Results *res, EResultOut out)
{
	int i;

	OSPI_SPITransaction(res->val, res->num);
	for (i = 0; i < res->num; i++)
	{
		print_log(out, "%08lx ", res->val[i]);
	}
	print_log(out, "\r\n");

	return CMD_DEC_OK;
}
