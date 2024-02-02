/*
 * cmd_dec.h
 *
 *  Created on: May 22, 2018
 *  Author: Torsten Jaekel
 */

#ifndef CMD_DEC_H_
#define CMD_DEC_H_

#include <string.h>
#include <stdio.h>
#include "main.h"

/**
 * make sure this correlates with CHIP_NUM_FIFO_WORDS
 */
#ifdef CHIP4
#define CMD_DEC_NUM_VAL		(6 +1)
#else
#define CMD_DEC_NUM_VAL		(512)		/* maximum number of value parameters */
#endif

#define USR_CMD_STR_SIZE	1024

typedef enum {
	UART_OUT,
	VCP_OUT,
	SILENT
} EResultOut;


extern void UART_Send(const uint8_t *p, size_t l);
extern void VCP_UART_Send(const uint8_t *p, size_t l);
extern void Generic_Send(const uint8_t *s, int l, EResultOut out);
extern void Gerneric_ClrRxFlag(EResultOut out);
extern volatile int Generic_GetRxFlag(EResultOut out);
extern void Generic_SetRxFlag(EResultOut out);

#define XPRINT_LEN	1024
extern uint8_t XPrintBufVCP[XPRINT_LEN];
extern uint8_t XPrintBufUART[XPRINT_LEN];

#define print_log(out, ...)		do {\
									if (out == UART_OUT) {\
										snprintf((char *)XPrintBufUART, XPRINT_LEN, __VA_ARGS__);\
										UART_Send((const uint8_t *)XPrintBufUART, strlen((char *)XPrintBufUART));\
									}\
									if (out == VCP_OUT) {\
										snprintf((char *)XPrintBufVCP, XPRINT_LEN, __VA_ARGS__);\
										VCP_UART_Send((const uint8_t *)XPrintBufVCP, strlen((char *)XPrintBufVCP));\
									}\
								} while(0)

/**
 * ATT: we need the maximum length for RBLK on ADC memory which is 4K 16bit words!
 */

#define CMD_DEC_OPT_SIGN	'-'				/* option starts with '-' */
#define CMD_DEC_SEPARATOR	';'				/* different commands in single line */
#define CMD_DEC_COMMENT		'#'				/* comment after command - rest of line ignored */

typedef enum {
	CMD_DEC_OK = 0,							/* all OK */
	CMD_DEC_UNKNOWN,						/* command does not exist */
	CMD_DEC_INVALID,						/* wrong command syntax */
	CMD_DEC_ERROR,							/* error on command execution */
	CMD_DEC_EMPTY,							/* empty command, no keyword, e.g. just ENTER */
	CMD_DEC_OOMEM,							/* out of memory to get buffer from mempool */
	CMD_DEC_INVPARAM,						/* invalid parameter, e.g. length too large or too short */
	CMD_DEC_TIMEOUT							/* time out on command */
} ECMD_DEC_Status;

typedef struct {
	char *cmd;								/* command key word */
	char *opt;								/* if option starting with '-' - the string afterwards */
	char *str;								/* rest of line as string */
	unsigned long cmdLen;					/* character length of CMD keyword */
	unsigned long  offset;					/* end of command string, or next semicolon */
	unsigned long  num;						/* number of specified values */
	unsigned long  ctl;						/* break outer command interpreter loop, 'concur' seen */
	unsigned long val[CMD_DEC_NUM_VAL];		/* index 0 can be an optVal */
} TCMD_DEC_Results;

typedef ECMD_DEC_Status (*TCmdFunc)(TCMD_DEC_Results *res, EResultOut out);

typedef struct {
	const char *cmd;						/* command key word */
	const char *help;						/* help text */
	const TCmdFunc func;					/* the command handler function */
} TCMD_DEC_Command;

extern char gMETA[6][32];

//int CMD_DEC_decode(char *cmdStr, TCMD_DEC_Results *res);

ECMD_DEC_Status CMD_DEC_execute(char *cmd, EResultOut out);
void hex_dump(uint8_t *ptr, uint16_t len, int mode, EResultOut out);

#endif /* CMD_DEC_H_ */
