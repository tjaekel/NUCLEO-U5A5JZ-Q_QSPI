/*
 * PSRAM.h
 *
 *  Created on: Mar 3, 2024
 *      Author: tj
 */

#ifndef INC_PSRAM_H_
#define INC_PSRAM_H_

/* APS6408L APMemory memory */

/* Read Operations */
#define READ_CMD                                0xEB	//0x0000
////#define READ_LINEAR_BURST_CMD                   0x2020
////#define READ_HYBRID_BURST_CMD                   0x3F3F

/* Write Operations */
#define ENTER_QPI								0x35

#define WRITE_CMD                               0x38	//0x8080
////#define WRITE_LINEAR_BURST_CMD                  0xA0A0
////#define WRITE_HYBRID_BURST_CMD                  0xBFBF

/* Reset definition */
////#define MR0                                     0x00000000
////#define MR1                                     0x00000001
////#define MR2                                     0x00000002
////#define MR3                                     0x00000003
////#define MR4                                     0x00000004
////#define MR8                                     0x00000008

/* Reset Operations */
/////#define RESET_CMD                               0xFF

/* Register Operations */
////#define READ_REG_CMD                            0x40
////#define WRITE_REG_CMD                           0xC0

/* Default dummy clocks cycles */
#define DUMMY_CLOCK_CYCLES_READ                 6	//8
#define DUMMY_CLOCK_CYCLES_WRITE                0	//4

/* Size of buffers */
#define BUFFERSIZE                              16
#define KByte                                   1024

int PSRAM_Init(void);

#endif /* INC_PSRAM_H_ */
