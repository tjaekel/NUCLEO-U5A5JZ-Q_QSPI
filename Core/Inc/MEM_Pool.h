/*
 * MEM_Pool.h
 *
 *  Created on: Feb 12, 2017
 *  Author: Torsten Jaekel
 */

#ifndef MEM_POOL_H_
#define MEM_POOL_H_

#include <stdint.h>

#define MEM_POOL_SEG_SIZE		(4096)		//byte chunk size greater 4K*16bit! - aligned for Cache Line Size!

#if 0
/* comes from linker script */
//extern int __mempool_start__;		/* = 0x38000000 - start D3 */
//extern int __mempool_end__;		/* = 0x38010000 - end D3 */

#define	SRAM_FREE_START			0x38000000	//start D3
#define SRAM_FREE_END			0x38010000	//end D3

#define MEM_POOL_SEGMENTS		((SRAM_FREE_END - SRAM_FREE_START) / MEM_POOL_SEG_SIZE)
#else
/*
 * ATT: for SDMMC1 - we can only use D1 (not access-able D2 and D3!)
 * So, we use also D1 for the mempool !!!!
 */
#ifdef CHIP4
#define MEM_POOL_SEGMENTS		64u		//55u - 32+3 is minimum with Network enabled!
//the max is 55 with all not needed commands and buffers disabled

/** ATT: make sure we have enough SEGMENTS and SEG_SIZE:
 * ATT: for binary commands we need TWO * SPI_FRAME_SIZE from MEMPool! 2*64K
 * larger as 64K SPI we cannot do right now - all limited by a 16bit length
 */
#else
#ifdef WITH_HTTPD
//36 without Web Server
#define MEM_POOL_SEGMENTS		30		/* depends on other buffer sizes defined: 32 is max. on 8K segments with minimal stuff enabled */
#else
#define MEM_POOL_SEGMENTS		6
#endif
#endif
extern uint8_t sMemPoolRAM[MEM_POOL_SEG_SIZE * MEM_POOL_SEGMENTS];
#define SRAM_FREE_START			((unsigned long)&sMemPoolRAM[0])
#endif

#define ALLOC_USED		1		//start of allocated entries
#define ALLOC_SUBSEQ	2		//it belongs to the start with ALLOC_USED
#define ALLOC_FREE		0		//free entry

typedef struct MEM_POOL {
	unsigned long	startAddr;
	int				alloc;
} TMem_Pool;

extern void  MEM_PoolInit(void);
extern void *MEM_PoolAlloc(unsigned int n);
extern void  MEM_PoolFree(void *ptr);
unsigned int MEM_PoolWatermark(void);
unsigned int MEM_PoolAvailable(void);
unsigned int MEM_PoolMax(void);

#endif /* MEM_POOL_H_ */
