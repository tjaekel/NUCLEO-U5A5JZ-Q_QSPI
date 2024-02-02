#ifndef __SYSERR_H__
#define __SYSERR_H__

/**
 * system error values and global variable:
 * we or the codes together in order to see several different errors occured
 */

#define	SYS_ERR_OUT_OF_MEM		0x00000001U
#define	SYS_ERR_UART_OVERRUN	0x00000002U
#define SYS_ERR_INVALID_CMD		0x00000004U
#define	SYS_ERR_TIR_INVALID		0x00000008U
#define SYS_ERR_DRV_LOCKED		0x00000010U
#define SYS_ERR_I2C				0x00000020U
#define	SYS_ERR_NO_FILE			0x00000040U
#define SYS_ERR_NETWORK			0x00000080U		//something wrong on network
#define SYS_ERR_SDNOTOPEN		0x00000100U
#define SYS_ERR_SDERROR			0x00000200U
#define	SYS_ERR_DHCPTO			0x00000400U
#define SYS_ERR_ETHDOWN			0x00000800U
#define SYS_ERR_ETHOVERRUN		0x00001000U		//UDP thread has an overrun - too slow
/* TEST_FIFO errors */
#define SYS_ERR_FIFOSEQ			0x00002000U		//S[] out of sequence
#define SYS_ERR_FIFOSYNC		0x00004000U		//FIFO not in sync with sync word
#define SYS_ERR_SPIERROR		0x00008000U		//SPI hangs
/* MEM_PoolFree with NULL pointer */
#define	SYS_ERR_NULL_PTR		0x00010000U		//try to release a NULL pointer
#define SYS_ERR_INV_PTR			0x00020000U		//release a mem pointer not found
#define	SYS_ERR_PYACCEL_EVNT	0x00040000U		//pyaccel event not found (not triggered)
#define	SYS_ERR_PYACCEL_CMD		0x00080000U		//pyaccel command not found

#define SYS_ERR_UDPDEST			0x00100000U		//UDP host port not set

#define	SYS_ERR_NETCLOSED		0x00200000U		//network closed
#define	SYS_ERR_EMPTYCMD		0x00400000U		//empty command seen
#define	SYS_ERR_ASSERT			0x00800000U		//a debug assert check seen

#define SYS_API_ERROR			0x01000000U		//wrong use of API call (for User Threads)

#define SYS_ERR_ZEROLEN			0x02000000U		//variable FIFO length is zero

/* FATAL Error */
#define	SYS_ERR_ERR_HANDLER		0x80000000U		//not usable - dead forever

extern unsigned long GSysErr;

extern void SYS_SetError(unsigned long errCode);
extern unsigned long SYS_GetError(void);
extern void SYS_ClrError(void);

#endif
