/*
 * GPIO_usr.h
 *
 *  Created on: Dec 19, 2024
 *      Author: tj
 */

#ifndef INC_GPIO_USR_H_
#define INC_GPIO_USR_H_

#include "main.h"
#include "stm32u5xx_hal.h"

#ifdef LEVEL_SHIFT
#define	INT0_PIN		GPIO_PIN_9
#define INT0_PORT		GPIOB
#define INT1_PIN		GPIO_PIN_11
#define INT1_PORT		GPIOD
#define INT2_PIN		GPIO_PIN_0
#define	INT2_PORT		GPIOE

#define RES_PIN			GPIO_PIN_8
#define RES_PORT		GPIOB

#define	GPIO2_PIN		GPIO_PIN_4			//INT3
#define GPIO2_PORT		GPIOB
#define	GPIO3_PIN		GPIO_PIN_7			//INT4
#define GPIO3_PORT		GPIOA
#define	GPIO4_PIN		GPIO_PIN_3			//INT5 - it is SWO!!! - constant driven high!
#define GPIO4_PORT		GPIOB				//remove SB44 on NUCLEO!
#define	GPIO5_PIN		GPIO_PIN_4
#define GPIO5_PORT		GPIOA
#define	GPIO6_PIN		GPIO_PIN_9			//n.c. on hdr but drive as output
#define GPIO6_PORT		GPIOE
#define	GPIO7_PIN		GPIO_PIN_7
#define GPIO7_PORT		GPIOD
#define	GPIO8_PIN		GPIO_PIN_2
#define GPIO8_PORT		GPIOC
#define	GPIO9_PIN		GPIO_PIN_1
#define GPIO9_PORT		GPIOA
//#define	GPIO10_PIN		GPIO_PIN_10		//already as QCLK!
//#define GPIO10_PORT		GPIOB
#else
#define	INT0_PIN		GPIO_PIN_7
#define INT0_PORT		GPIOB
#define INT1_PIN		GPIO_PIN_2
#define INT1_PORT		GPIOA
#define INT2_PIN		GPIO_PIN_0
#define	INT2_PORT		GPIOA

#define RES_PIN			GPIO_PIN_6
#define RES_PORT		GPIOB

#define	GPIO2_PIN		GPIO_PIN_9
#define GPIO2_PORT		GPIOA
#define	GPIO3_PIN		GPIO_PIN_10
#define GPIO3_PORT		GPIOA
#define	GPIO4_PIN		GPIO_PIN_15
#define GPIO4_PORT		GPIOB
#define	GPIO5_PIN		GPIO_PIN_9
#define GPIO5_PORT		GPIOC
#define	GPIO6_PIN		GPIO_PIN_2			//n.c.
#define GPIO6_PORT		GPIOB
#define	GPIO7_PIN		GPIO_PIN_8
#define GPIO7_PORT		GPIOC
#define	GPIO8_PIN		GPIO_PIN_2
#define GPIO8_PORT		GPIOD
#define	GPIO9_PIN		GPIO_PIN_12
#define GPIO9_PORT		GPIOC
#define	GPIO10_PIN		GPIO_PIN_8
#define GPIO10_PORT		GPIOB
#endif

#define GPIO_NUM_BITS	12	//13
//PB10 is 2.54V - WHY??? --> PB10 is 2x on HDR! (as QCLK)

/*
 * User GPIOs - have a table for the ports, pins
 */
#define GPIO_OUTPUT				0x0001
#define GPIO_INPUT				0x0002
#define	GPIO_INT				0x0004					/* do not touch */
#define GPIO_RELAY				0x0009				 	/* do not touch */
#define GPIO_OD					0x0010
#define GPIO_RES				0x0020					/* do not touch */
#define	GPIO_PU					0x0040					/* MCU pull-up enabled */
#define GPIO_NONE				0x8000

typedef struct {
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	uint16_t GPIO_Mode;					//bit 0: OUTPUT, bit 1: INPUT, bit 2: INT - don't touch as GPIO config
	const char *Descr;					//string pointer, max. 4 characters, as name of GPIO, for ggpio -d
} TGPIOUsr;

extern const TGPIOUsr GPIOUsr[GPIO_NUM_BITS];

int GPIO_Config(unsigned long mask);
void GPIO_Set(unsigned long bits, unsigned long mask);
unsigned long GPIO_Get(void);
void GPIO_SetReset(unsigned long val);

#endif /* INC_GPIO_USR_H_ */
