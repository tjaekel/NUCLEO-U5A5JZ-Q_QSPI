/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* how to build for...?
 *
 * NUCLEO board plus level shifter board:
 * define macros in Settings as:
 * NUCLEO_BOARD
 * USE_NUCLEO_144
 * LEVEL_SHIFT
 *
 * for "my" QSPI MCU board:
 * XXX_NUCLEO_BOARD
 * XXX_USE_NUCLEO_144
 * USE_NUCLEO_64
 * XXX_LEVEL_SHIFT
 *
 * macro:
 * PDM_MCU (for PDM setup) is not verified again (don't use for now)
 *
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32u5xx_hal.h"

#include "stm32u5xx_ll_ucpd.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_cortex.h"
#include "stm32u5xx_ll_rcc.h"
#include "stm32u5xx_ll_system.h"
#include "stm32u5xx_ll_utils.h"
#include "stm32u5xx_ll_pwr.h"
#include "stm32u5xx_ll_gpio.h"
#include "stm32u5xx_ll_dma.h"
#include "stm32u5xx_ll_spi.h"

#include "stm32u5xx_ll_exti.h"

#include "stm32u5xx_nucleo.h"

void Error_Handler(void);
#ifndef CODEC_SAI
void MX_USART1_UART_Init(void);
#endif
#ifndef STM32U5A5xx
void MX_USB_OTG_FS_PCD_Init(void);
#else
void MX_USB_OTG_HS_PCD_Init(void);
#endif

#ifdef NUCLEO_BOARD
#define LED_RED_Pin GPIO_PIN_2
#define LED_RED_GPIO_Port GPIOG
#else
#define LED_RED_Pin GPIO_PIN_13
#define LED_RED_GPIO_Port GPIOC
#endif

#define CMD_LINE_LEN		(6*1024)			/* length of UART command line (ASCII based) */

#define VERSION_INFO		"V5.1"
#ifdef PDM_MCU
#define VERSION_STRING		"---- PDM MCU - " VERSION_INFO " ----\r\n"
#else
#define VERSION_STRING		"---- QSPI MCU - " VERSION_INFO " ----\r\n"
#endif
#ifdef NUCLEO_BOARD
#define BOARD_INFO			"NUCLEO"
#else
#ifdef PDM_MCU
#define	BOARD_INFO			"PDM_MCU"
#else
#define	BOARD_INFO			"QSPI_MCU"
#endif
#endif

void LED_Status(int val);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
