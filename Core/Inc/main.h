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

#include "stm32u5xx_ll_exti.h"

#include "stm32u5xx_nucleo.h"

void Error_Handler(void);
void MX_USART1_UART_Init(void);
#ifndef STM32U5A5xx
void MX_USB_OTG_FS_PCD_Init(void);
#else
void MX_USB_OTG_HS_PCD_Init(void);
#endif

#define LED_RED_Pin GPIO_PIN_2
#define LED_RED_GPIO_Port GPIOG

#define CMD_LINE_LEN		(6*1024)			/* length of UART command line (ASCII based) */

#define VERSION_INFO		"V2.1"
#define VERSION_STRING		"---- QSPI MCU - " VERSION_INFO " ----\r\n"
#ifdef NUCLEO_BOARD
#define BOARD_INFO			"NUCLEO"
#else
#define	BOARD_INFO			"QSPI_MCU"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
