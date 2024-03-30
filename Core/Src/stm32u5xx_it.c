/**
  ******************************************************************************
  * @file    stm32u5xx_it.c
  * @brief   Interrupt Service Routines.
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

#include "main.h"
#include "stm32u5xx_it.h"
#include "usbpd.h"

#include "cmd_dec.h"

extern DMA_HandleTypeDef handle_GPDMA1_Channel0;
#if 0
extern DMA_HandleTypeDef handle_GPDMA1_Channel7;	//SPI3 DMA Rx
#endif
extern DMA_HandleTypeDef handle_GPDMA1_Channel6;	//SPI3 DMA Rx
#ifdef QSPI_DMA
extern DMA_HandleTypeDef handle_GPDMA1_Channel12;	//QSPI DMA
#endif
extern OSPI_HandleTypeDef hospi1;
#ifndef CODEC_SAI
extern UART_HandleTypeDef huart1;
#endif
extern SPI_HandleTypeDef  hspi3;
#ifndef STM32U5A5xx
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
#else
extern PCD_HandleTypeDef hpcd_USB_OTG_HS;
#endif
extern TIM_HandleTypeDef htim6;

extern DMA_NodeTypeDef Node_GPDMA1_Channel4;
extern DMA_QListTypeDef List_GPDMA1_Channel4;
extern DMA_HandleTypeDef handle_GPDMA1_Channel4;	//SAI CODEC DMA
extern SAI_HandleTypeDef hsai_BlockA1;

extern DMA_NodeTypeDef Node_GPDMA1_Channel5;
extern DMA_QListTypeDef List_GPDMA1_Channel5;
extern DMA_HandleTypeDef handle_GPDMA1_Channel5;
extern SAI_HandleTypeDef hsai_BlockB1;

#ifndef NUCLEO_BOARD
void LED_Toggle(int dly);
extern int GBothLEDs;
#endif

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
}

/******************************************************************************/
/* STM32U5xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32u5xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles GPDMA1 Channel 0 global interrupt.
  */
void GPDMA1_Channel0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&handle_GPDMA1_Channel0);
}

#ifdef UCPD_DMA_USED
/**
  * @brief This function handles GPDMA1 Channel 3 global interrupt.
  */
void GPDMA1_Channel3_IRQHandler(void)
{
}

/**
  * @brief This function handles GPDMA1 Channel 5 global interrupt.
  */
void GPDMA1_Channel5_IRQHandler(void)
{
}
#endif

#ifdef SPI3_DMA
void GPDMA1_Channel6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&handle_GPDMA1_Channel6);
}

#if 0
/**
  * @brief This function handles GPDMA1 Channel 7 global interrupt - SPI3 DMA Tx
  */
void GPDMA1_Channel7_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&handle_GPDMA1_Channel7);
}
#endif

void GPDMA1_Channel4_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&handle_GPDMA1_Channel4);
}

void GPDMA1_Channel5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&handle_GPDMA1_Channel5);
}

#ifdef QSPI_DMA
/**
  * @brief This function handles GPDMA1 Channel 12 global interrupt - QSPI1 DMA
  */
void GPDMA1_Channel12_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&handle_GPDMA1_Channel12);
}
#endif

/**
  * @brief This function handles SPI1 global interrupt - SPI3 DMA Rx
  */
void SPI3_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&hspi3);
}
#endif

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim6);
#ifndef NUCLEO_BOARD
  if (GBothLEDs)
	  LED_Toggle(0);
#endif
}

#ifndef CODEC_SAI
/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
}
#endif

/**
  * @brief This function handles USB OTG HS global interrupt.
  */
#ifndef STM32U5A5xx
void OTG_FS_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}
#else
void OTG_HS_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_HS);
}
#endif

/* it is enabled and needed by the USB stack! */
/**
  * @brief This function handles UCPD1 global interrupt.
  */
void UCPD1_IRQHandler(void)
{
  USBPD_PORT0_IRQHandler();
}

#ifdef QSPI_DMA
void OCTOSPI1_IRQHandler(void)
{
  HAL_OSPI_IRQHandler(&hospi1);
}
#endif

/**
  * @brief This function handles Serial Audio Interface 1 global interrupt.
  */
void SAI1_IRQHandler(void)
{
#ifdef SPDIF_TEST
  HAL_SAI_IRQHandler(&hsai_BlockB1);
#else
  HAL_SAI_IRQHandler(&hsai_BlockA1);
#endif
}

void SAI2_IRQHandler(void)
{
  ////HAL_SAI_IRQHandler(&hsai_BlockA1);
  HAL_SAI_IRQHandler(&hsai_BlockB1);
}

