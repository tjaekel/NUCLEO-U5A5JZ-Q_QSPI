/**
  ******************************************************************************
  * @file         stm32u5xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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
#include "SYS_config.h"

extern DMA_HandleTypeDef handle_GPDMA1_Channel0;
extern DMA_HandleTypeDef handle_GPDMA1_Channel12;		//QSPI DMA

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableVddA();
}

/**
* @brief ADC MSP Initialization
* This function configures the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
#ifdef NUCLEO_BOARD_YYYY
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(hadc->Instance==ADC1)
  {
  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADCDAC;
    PeriphClkInit.AdcDacClockSelection = RCC_ADCDACCLKSOURCE_HSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_ADC12_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PC1     ------> ADC1_IN2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }
}
#endif

/**
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_ADC12_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PC1     ------> ADC1_IN2
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);
  }
}

/**
* @brief OSPI MSP Initialization
* This function configures the hardware resources used in this example
* @param hospi: OSPI handle pointer
* @retval None
*/
void HAL_OSPI_MspInit(OSPI_HandleTypeDef* hospi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(hospi->Instance==OCTOSPI1)
  {
  /** Initializes the peripherals clock
  */

#if 0
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
    PeriphClkInit.OspiClockSelection = RCC_OSPICLKSOURCE_SYSCLK;	//RCC_OSPICLKSOURCE_PLL1;	//RCC_OSPICLKSOURCE_SYSCLK; 160 MHz, could be max. 200 MHz
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }
#else
#define OK
#ifdef OK
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_OSPI|RCC_PERIPHCLK_SAI1;
    PeriphClkInit.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL2;
    PeriphClkInit.OspiClockSelection = RCC_OSPICLKSOURCE_PLL2;
    PeriphClkInit.PLL2.PLL2Source = RCC_PLLSOURCE_HSE;
    PeriphClkInit.PLL2.PLL2M = 1;		//was 1
    PeriphClkInit.PLL2.PLL2N = 36;		//40 = 160 MHz, 50 = 200 MHz - the max. for QSPI
    PeriphClkInit.PLL2.PLL2P = 2;		//SAI1, 50 MHz
    PeriphClkInit.PLL2.PLL2Q = 1;		//QSPI: 200 MHz
    PeriphClkInit.PLL2.PLL2R = 2;		//not used
    PeriphClkInit.PLL2.PLL2RGE = RCC_PLLVCIRANGE_1;
    PeriphClkInit.PLL2.PLL2FRACN = 7080;	//7080; 7667;	//max. 8191 - trim for SAI_FS 48 KHz
    ////PeriphClkInit.PLL2.PLL2FRACN = 0;
    PeriphClkInit.PLL2.PLL2ClockOut = RCC_PLL2_DIVP|RCC_PLL2_DIVQ;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
       Error_Handler();
    }

    memset(&PeriphClkInit, 0, sizeof(PeriphClkInit));
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
    PeriphClkInit.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLL3;
    PeriphClkInit.PLL3.PLL3Source = RCC_PLLSOURCE_HSE;
    PeriphClkInit.PLL3.PLL3M = 1;

    /* 48 KHz SPDIF            with scope/debug 	CubeMX cfg      correct */
    PeriphClkInit.PLL3.PLL3N = 36;					//36;			36;
    PeriphClkInit.PLL3.PLL3P = 24;					//96; 			24;		//SAI2
    PeriphClkInit.PLL3.PLL3Q = 24;											//ADF1
    PeriphClkInit.PLL3.PLL3R = 2;
    PeriphClkInit.PLL3.PLL3RGE = RCC_PLLVCIRANGE_1;
    PeriphClkInit.PLL3.PLL3FRACN = 7080;			//7080			7078; -->
    // it results in 12,288.004 KHz in FW - MCLKDIV should be 2 at the end - but it fails!
    /* but: above 12,288 KHz results in MCLKDIV +1, too large! - so, we have to get a bit below "nominal", resulting in a larger clock offset error:
     * -36Hz, compared to value 7078 with just 4 Hz error!
     * why I cannot use 7078 with 12,288.004 KHz which would be better? The MCLKDIV is wrong by +1!
     */
    PeriphClkInit.PLL3.PLL3ClockOut = RCC_PLL3_DIVP;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
          Error_Handler();
    }
#else
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
    PeriphClkInit.OspiClockSelection = RCC_OSPICLKSOURCE_PLL2;
    PeriphClkInit.PLL2.PLL2Source = RCC_PLLSOURCE_HSE;		//THIS FAILS WITH TIMEOUT!!!!
    PeriphClkInit.PLL2.PLL2M = 2;
    PeriphClkInit.PLL2.PLL2N = 50;
    PeriphClkInit.PLL2.PLL2P = 2;
    PeriphClkInit.PLL2.PLL2Q = 3;
    PeriphClkInit.PLL2.PLL2R = 2;
    PeriphClkInit.PLL2.PLL2RGE = RCC_PLLVCIRANGE_1;
    PeriphClkInit.PLL2.PLL2FRACN = 0;
    PeriphClkInit.PLL2.PLL2ClockOut = RCC_PLL2_DIVQ;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }
#endif
#endif

    /* Peripheral clock enable */
    __HAL_RCC_OSPIM_CLK_ENABLE();
    __HAL_RCC_OSPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
#ifdef NUCLEO_BOARD
    /**OCTOSPI1 GPIO Configuration
    PA2      ------> OCTOSPIM_P1_NCS
    PB0      ------> OCTOSPIM_P1_IO1
    PE12     ------> OCTOSPIM_P1_IO0
    PE14     ------> OCTOSPIM_P1_IO2
    PE15     ------> OCTOSPIM_P1_IO3
    PB10     ------> OCTOSPIM_P1_CLK
    PA1      ------> OCTOSPIM_P1_DQS - not working in SDR mode, used as NCS1
    */

#if 1
    /* we use PA2 as NCCS1, PA1 as NCCS2 - in SW GPIO mode */

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);

    /*Configure GPIO pin : OCTOSPI NCS signals */
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = gCFGparams.QSPIspeed;		//GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#else
    /* HW NCS signal */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = gCFGparams.QSPIspeed;		//GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* DQS signal */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = gCFGparams.QSPIspeed;		//GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = gCFGparams.QSPIspeed;		//GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = gCFGparams.QSPIspeed;		//GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
#else
    /**OCTOSPI1 GPIO Configuration - my PCB board
    PA4      ------> OCTOSPIM_P1_NCS
    PB0      ------> OCTOSPIM_P1_IO1
    PB1      ------> OCTOSPIM_P1_IO0
    PA7      ------> OCTOSPIM_P1_IO2
    PA6      ------> OCTOSPIM_P1_IO3
    PA3      ------> OCTOSPIM_P1_CLK
    PA1      ------> OCTOSPIM_P1_DQS - not working in SDR mode, used as NCS1
    */

#if 1
#ifndef PDM_MCU
    /* we use PA4 as NCCS1, PA1 as NCCS2 - in SW GPIO mode */

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);

    /*Configure GPIO pin : OCTOSPI NCS signals */
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = gCFGparams.QSPIspeed;		//GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
#else
    /* HW NCS signal */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = gCFGparams.QSPIspeed;		//GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;		//it should be ALT3! GPIO_AF3_OCTOSPI1
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* DQS signal */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = gCFGparams.QSPIspeed;		//GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = gCFGparams.QSPIspeed;		//GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = gCFGparams.QSPIspeed;		//GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

#ifdef PDM_MCU
    GPIO_InitStruct.Pin = GPIO_PIN_4;					//HW NCS
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = gCFGparams.QSPIspeed;		//GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_OCTOSPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
#endif

#ifdef QSPI_DMA
    /* QSPI DMA */
    handle_GPDMA1_Channel12.Instance = GPDMA1_Channel12;
    handle_GPDMA1_Channel12.Init.Request = GPDMA1_REQUEST_OCTOSPI1;
    handle_GPDMA1_Channel12.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_GPDMA1_Channel12.Init.Direction = DMA_MEMORY_TO_PERIPH;			//DMA_PERIPH_TO_MEMORY;
    handle_GPDMA1_Channel12.Init.SrcInc = DMA_DINC_INCREMENTED;				//DMA_SINC_FIXED;
    handle_GPDMA1_Channel12.Init.DestInc = DMA_SINC_FIXED;					//DMA_DINC_INCREMENTED;
    handle_GPDMA1_Channel12.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;		//DMA_SRC_DATAWIDTH_WORD;
    handle_GPDMA1_Channel12.Init.DestDataWidth = DMA_SRC_DATAWIDTH_BYTE;	//DMA_DEST_DATAWIDTH_WORD;
    handle_GPDMA1_Channel12.Init.Priority = DMA_HIGH_PRIORITY;
    handle_GPDMA1_Channel12.Init.SrcBurstLength = 4;
    handle_GPDMA1_Channel12.Init.DestBurstLength = 4;
    handle_GPDMA1_Channel12.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT1;
    handle_GPDMA1_Channel12.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_GPDMA1_Channel12.Init.Mode = DMA_NORMAL;
    if (HAL_DMA_Init(&handle_GPDMA1_Channel12) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hospi, hdma, handle_GPDMA1_Channel12);

    if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel12, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      Error_Handler();
    }

    /* OCTOSPI2 interrupt Init */
    HAL_NVIC_SetPriority(OCTOSPI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(OCTOSPI1_IRQn);
#endif
  }
}

/**
* @brief OSPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hospi: OSPI handle pointer
* @retval None
*/
void HAL_OSPI_MspDeInit(OSPI_HandleTypeDef* hospi)
{
  if(hospi->Instance==OCTOSPI1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_OSPIM_CLK_DISABLE();
    __HAL_RCC_OSPI1_CLK_DISABLE();

    /**OCTOSPI1 GPIO Configuration
    PA2     ------> OCTOSPIM_P1_NCS
    PB0     ------> OCTOSPIM_P1_IO1
    PE12     ------> OCTOSPIM_P1_IO0
    PE14     ------> OCTOSPIM_P1_IO2
    PE15     ------> OCTOSPIM_P1_IO3
    PB10     ------> OCTOSPIM_P1_CLK
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_10);
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_15);

    HAL_DMA_DeInit(hospi->hdma);
    HAL_NVIC_DisableIRQ(OCTOSPI1_IRQn);
  }
}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(huart->Instance==USART1)
  {
  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10    ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;			//necessary: otherwise all hangs if UART1 Rx is floating!
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    //XXXX
    GPIO_InitStruct.Pin = GPIO_PIN_9;
#ifdef NUCLEO_BOARD
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;		//Tx has a level shifter
#else
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;		//for CP2102N with pull-up to VDD 3V3 on it
#endif
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* GPDMA1_REQUEST_USART1_TX Init */
    handle_GPDMA1_Channel0.Instance = GPDMA1_Channel0;
    handle_GPDMA1_Channel0.Init.Request = GPDMA1_REQUEST_USART1_TX;
    handle_GPDMA1_Channel0.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_GPDMA1_Channel0.Init.Direction = DMA_MEMORY_TO_PERIPH;
    handle_GPDMA1_Channel0.Init.SrcInc = DMA_SINC_INCREMENTED;
    handle_GPDMA1_Channel0.Init.DestInc = DMA_DINC_FIXED;
    handle_GPDMA1_Channel0.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel0.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel0.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
    handle_GPDMA1_Channel0.Init.SrcBurstLength = 1;
    handle_GPDMA1_Channel0.Init.DestBurstLength = 1;
    handle_GPDMA1_Channel0.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT0;
    handle_GPDMA1_Channel0.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_GPDMA1_Channel0.Init.Mode = DMA_NORMAL;
    if (HAL_DMA_Init(&handle_GPDMA1_Channel0) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(huart, hdmatx, handle_GPDMA1_Channel0);

    if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel0, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    HAL_DMA_DeInit(huart->hdmatx);

    HAL_NVIC_DisableIRQ(USART1_IRQn);
  }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	if (hspi->Instance == SPI3)
	{
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SPI3;
		PeriphClkInit.Spi1ClockSelection = RCC_SPI3CLKSOURCE_PCLK3;	//??RCC_SPI1CLKSOURCE_PCLK2;	//RCC_SPI1CLKSOURCE_SYSCLK;	//RCC_SPI1CLKSOURCE_PCLK2;		//RCC_SPI1CLKSOURCE_SYSCLK;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			Error_Handler();
		}

	    /* Peripheral clock enable */
	    __HAL_RCC_SPI3_CLK_ENABLE();

	    __HAL_RCC_GPIOC_CLK_ENABLE();

	    /**SPI3 GPIO Configuration
	    PC10    ------> SPI3_SCK
	    PA15    ------> SPI3_NSS - not used, in SW mode
	    PC11    ------> SPI3_MISO ==> ATT: flip on slave the signals!
	    PC12    ------> SPI3_MOSI - not used)
	    */
	    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
	    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

#if 0
	    //is this needed if we run SPI3 in DMA Rx mode?
	    HAL_NVIC_SetPriority(SPI3_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(SPI3_IRQn);
#endif
	}

	if (hspi->Instance == SPI1)
	{
	  /** Initializes the peripherals clock
	  */
	    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
	    PeriphClkInit.Spi1ClockSelection = RCC_SPI1CLKSOURCE_SYSCLK;
	    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	    {
	      Error_Handler();
	    }

	    /* Peripheral clock enable */
	    __HAL_RCC_SPI1_CLK_ENABLE();

	    __HAL_RCC_GPIOA_CLK_ENABLE();
	    __HAL_RCC_GPIOB_CLK_ENABLE();
	    /**SPI1 GPIO Configuration
	    PA15 (JTDI)     ------> SPI1_NSS
	    PB3 (JTDO/TRACESWO)     ------> SPI1_SCK
	    PB4 (NJTRST)     ------> SPI1_MISO
	    PB5     ------> SPI1_MOSI
	    */
	    GPIO_InitStruct.Pin = GPIO_PIN_15 /*| GPIO_PIN_7*/;			//plus PA7 as SPI1_MOSI works - PA15 just after USB working!
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;	//GPIO_PULLUP;			//GPIO_NOPULL;	//WHY low if not used - even with pull-up enabled?
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;				//GPIO_SPEED_FREQ_LOW;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;		//PB5 as SPI1_MOSI fails!
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;				//GPIO_SPEED_FREQ_LOW;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
	if (hspi->Instance == SPI3)
	{
	    /* Peripheral clock disable */
	    __HAL_RCC_SPI3_CLK_DISABLE();

	    /**SPI3 GPIO Configuration
		PC10    ------> SPI3_SCK
	    PA15    ------> SPI3_NSS - not used, in SW mode
	    PC11    ------> SPI3_MISO ==> ATT: flip on slave the signals!
	    PC12    ------> SPI3_MOSI - not used)
	    */
	    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11);

	    HAL_NVIC_DisableIRQ(SPI3_IRQn);
	}

	if (hspi->Instance == SPI1)
	{
	    /* Peripheral clock disable */
	    __HAL_RCC_SPI1_CLK_DISABLE();

	    /**SPI1 GPIO Configuration
	    PA15 (JTDI)     ------> SPI1_NSS
	    PB3 (JTDO/TRACESWO)     ------> SPI1_SCK
	    PB4 (NJTRST)     ------> SPI1_MISO
	    PB5     ------> SPI1_MOSI
	    */
	    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);

	    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
	}
}

/**
* @brief PCD MSP Initialization
* This function configures the hardware resources used in this example
* @param hpcd: PCD handle pointer
* @retval None
*/
#ifndef STM32U5A5xx
void HAL_PCD_MspInit(PCD_HandleTypeDef* hpcd)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(hpcd->Instance==USB_OTG_FS)
  {
  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
    PeriphClkInit.IclkClockSelection = RCC_CLK48CLKSOURCE_HSI48;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USB_OTG_FS GPIO Configuration
    PA12     ------> USB_OTG_FS_DP
    PA11     ------> USB_OTG_FS_DM
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_USB;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_USB_CLK_ENABLE();

    /* Enable VDDUSB */
    if(__HAL_RCC_PWR_IS_CLK_DISABLED())
    {
      __HAL_RCC_PWR_CLK_ENABLE();
      HAL_PWREx_EnableVddUSB();
      __HAL_RCC_PWR_CLK_DISABLE();
    }
    else
    {
      HAL_PWREx_EnableVddUSB();
    }
    /* USB_OTG_FS interrupt Init */
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
  }
}
#else
void HAL_PCD_MspInit(PCD_HandleTypeDef* hpcd)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(hpcd->Instance==USB_OTG_HS)
  {
    __HAL_RCC_SYSCFG_CLK_ENABLE();

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USBPHY;
#ifdef ORI_16MHz_OSC
    PeriphClkInit.UsbPhyClockSelection = RCC_USBPHYCLKSOURCE_HSE;
#else
    PeriphClkInit.UsbPhyClockSelection = RCC_USBPHYCLKSOURCE_PLL1_DIV2;	//RCC_USBPHYCLKSOURCE_PLL1;	//RCC_USBPHYCLKSOURCE_PLL1_DIV2;
#endif
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

  /** Set the OTG PHY reference clock selection
  */
    HAL_SYSCFG_SetOTGPHYReferenceClockSelection(SYSCFG_OTG_HS_PHY_CLK_SELECT_1);

    /* Peripheral clock enable */
    __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
    __HAL_RCC_USBPHYC_CLK_ENABLE();

    /* Enable VDDUSB */
    if(__HAL_RCC_PWR_IS_CLK_DISABLED())
    {
      __HAL_RCC_PWR_CLK_ENABLE();
      HAL_PWREx_EnableVddUSB();

      /*configure VOSR register of USB*/
      HAL_PWREx_EnableUSBHSTranceiverSupply();
      ////__HAL_RCC_PWR_CLK_DISABLE();
    }
    else
    {
      HAL_PWREx_EnableVddUSB();

      /*configure VOSR register of USB*/
      HAL_PWREx_EnableUSBHSTranceiverSupply();
    }

    /*Configuring the SYSCFG registers OTG_HS PHY*/
    /*OTG_HS PHY enable*/
    HAL_SYSCFG_EnableOTGPHY(SYSCFG_OTG_HS_PHY_ENABLE);
    /* USB_OTG_HS interrupt Init */
    HAL_NVIC_SetPriority(OTG_HS_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
  }
}
#endif

/**
* @brief PCD MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hpcd: PCD handle pointer
* @retval None
*/
#ifndef STM32U5A5xx
void HAL_PCD_MspDeInit(PCD_HandleTypeDef* hpcd)
{
  if(hpcd->Instance==USB_OTG_FS)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USB_CLK_DISABLE();

    /**USB_OTG_FS GPIO Configuration
    PA12     ------> USB_OTG_FS_DP
    PA11     ------> USB_OTG_FS_DM
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_12|GPIO_PIN_11);

    /* USB_OTG_FS interrupt DeInit */
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
  }
}
#else
void HAL_PCD_MspDeInit(PCD_HandleTypeDef* hpcd)
{
  if(hpcd->Instance==USB_OTG_HS)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
    __HAL_RCC_USBPHYC_CLK_DISABLE();

    /* USB_OTG_HS interrupt DeInit */
    HAL_NVIC_DisableIRQ(OTG_HS_IRQn);
  }
}
#endif

#ifndef NUCLEO_BOARD
/**
* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(hi2c->Instance==I2C3)
  {
  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C3;
    PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK3;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**I2C3 GPIO Configuration
    PC0     ------> I2C3_SCL
    PC1     ------> I2C3_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C3_CLK_ENABLE();
  }
  if(hi2c->Instance==I2C1)
    {
    /** Initializes the peripherals clock
    */
      PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
      PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
      if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
      {
        Error_Handler();
      }

      __HAL_RCC_GPIOB_CLK_ENABLE();
      /**I2C1 GPIO Configuration
      PB6     ------> I2C1_SCL
      PB7     ------> I2C1_SDA
      */
      GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      /* Peripheral clock enable */
      __HAL_RCC_I2C1_CLK_ENABLE();
    }
}

/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  if(hi2c->Instance==I2C3)
  {
    /* Peripheral clock disable */
    __HAL_RCC_I2C3_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PC0     ------> I2C3_SCL
    PC1     ------> I2C3_SDA
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);
  }
  if(hi2c->Instance==I2C1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_I2C3_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
  }
}
#endif

extern DMA_NodeTypeDef Node_GPDMA1_Channel4;
extern DMA_QListTypeDef List_GPDMA1_Channel4;
extern DMA_HandleTypeDef handle_GPDMA1_Channel4;

extern DMA_NodeTypeDef Node_GPDMA1_Channel5;
extern DMA_QListTypeDef List_GPDMA1_Channel5;
extern DMA_HandleTypeDef handle_GPDMA1_Channel5;

static uint32_t SAI1_client =0;
static uint32_t SAI2_client =0;

void HAL_SAI_MspInit(SAI_HandleTypeDef* hsai)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  DMA_NodeConfTypeDef NodeConfig;

#ifndef SPDIF_TEST
  /* SAI1 */
  if(hsai->Instance==SAI1_Block_A)
  {
    /* Peripheral clock enable */
    if (SAI1_client == 0)
    {
       __HAL_RCC_SAI1_CLK_ENABLE();

       /* Peripheral interrupt init*/
       HAL_NVIC_SetPriority(SAI1_IRQn, 0, 0);
       HAL_NVIC_EnableIRQ(SAI1_IRQn);
    }
    SAI1_client++;

    /**SAI1_A_Block_A GPIO Configuration
    PA8     ------> SAI1_SCK_A
    PA9     ------> SAI1_FS_A
    PA10    ------> SAI1_SD_A
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_SAI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      /* Peripheral DMA init*/

    NodeConfig.NodeType = DMA_GPDMA_LINEAR_NODE;
    NodeConfig.Init.Request = GPDMA1_REQUEST_SAI1_A;
    NodeConfig.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    NodeConfig.Init.Direction = DMA_PERIPH_TO_MEMORY;
    NodeConfig.Init.SrcInc = DMA_SINC_FIXED;
    NodeConfig.Init.DestInc = DMA_DINC_INCREMENTED;
    NodeConfig.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
    NodeConfig.Init.DestDataWidth = DMA_SRC_DATAWIDTH_WORD;
    NodeConfig.Init.SrcBurstLength = 1;
    NodeConfig.Init.DestBurstLength = 1;
    NodeConfig.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT0;
    NodeConfig.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    NodeConfig.Init.Mode = DMA_NORMAL;
    NodeConfig.TriggerConfig.TriggerPolarity = DMA_TRIG_POLARITY_MASKED;
    NodeConfig.DataHandlingConfig.DataExchange = DMA_EXCHANGE_NONE;
    NodeConfig.DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;
    if (HAL_DMAEx_List_BuildNode(&NodeConfig, &Node_GPDMA1_Channel4) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DMAEx_List_InsertNode(&List_GPDMA1_Channel4, NULL, &Node_GPDMA1_Channel4) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DMAEx_List_SetCircularMode(&List_GPDMA1_Channel4) != HAL_OK)
    {
      Error_Handler();
    }

    handle_GPDMA1_Channel4.Instance = GPDMA1_Channel4;
    handle_GPDMA1_Channel4.InitLinkedList.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
    handle_GPDMA1_Channel4.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
    handle_GPDMA1_Channel4.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT0;
    handle_GPDMA1_Channel4.InitLinkedList.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_GPDMA1_Channel4.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_CIRCULAR;
    if (HAL_DMAEx_List_Init(&handle_GPDMA1_Channel4) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DMAEx_List_LinkQ(&handle_GPDMA1_Channel4, &List_GPDMA1_Channel4) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hsai, hdmarx, handle_GPDMA1_Channel4);

    if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel4, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      Error_Handler();
    }
  }
#endif
#ifdef SPDIF_TEST_A
  if(hsai->Instance==SAI1_Block_A)
#else
  if(hsai->Instance==SAI2_Block_B)
#endif
  {
        /* Peripheral clock enable */
        if (SAI2_client == 0)
        {
#ifdef SPDIF_TEST_A
        	 __HAL_RCC_SAI1_CLK_ENABLE();

        	 /* Peripheral interrupt init*/
        	 HAL_NVIC_SetPriority(SAI1_IRQn, 0, 0);
        	 HAL_NVIC_EnableIRQ(SAI1_IRQn);
#else
        	 __HAL_RCC_SAI2_CLK_ENABLE();

        	 /* Peripheral interrupt init*/
        	 HAL_NVIC_SetPriority(SAI2_IRQn, 0, 0);
        	 HAL_NVIC_EnableIRQ(SAI2_IRQn);
#endif
        }
        SAI2_client++;

        /**SAI1_B_Block_B GPIO Configuration
        PB5     ------> SAI1_SD_B
        */
#ifdef SPDIF_TEST_A
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF13_SAI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#else
        GPIO_InitStruct.Pin = GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF13_SAI2;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif
        /* Peripheral DMA init*/
        NodeConfig.NodeType = DMA_GPDMA_LINEAR_NODE;
#ifdef SPDIF_TEST_A
        NodeConfig.Init.Request = GPDMA1_REQUEST_SAI1_A;
#else
        NodeConfig.Init.Request = GPDMA1_REQUEST_SAI2_B;
#endif
        NodeConfig.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
        NodeConfig.Init.Direction = DMA_MEMORY_TO_PERIPH;
        NodeConfig.Init.SrcInc = DMA_SINC_INCREMENTED;
        NodeConfig.Init.DestInc = DMA_DINC_FIXED;
        NodeConfig.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
        NodeConfig.Init.DestDataWidth = DMA_SRC_DATAWIDTH_WORD;
        NodeConfig.Init.SrcBurstLength = 1;
        NodeConfig.Init.DestBurstLength = 1;
        NodeConfig.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT0;
        NodeConfig.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
        NodeConfig.Init.Mode = DMA_NORMAL;
        NodeConfig.TriggerConfig.TriggerPolarity = DMA_TRIG_POLARITY_MASKED;
        NodeConfig.DataHandlingConfig.DataExchange = DMA_EXCHANGE_NONE;
        NodeConfig.DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;
        if (HAL_DMAEx_List_BuildNode(&NodeConfig, &Node_GPDMA1_Channel5) != HAL_OK)
        {
          Error_Handler();
        }

        if (HAL_DMAEx_List_InsertNode(&List_GPDMA1_Channel5, NULL, &Node_GPDMA1_Channel5) != HAL_OK)
        {
          Error_Handler();
        }

        if (HAL_DMAEx_List_SetCircularMode(&List_GPDMA1_Channel5) != HAL_OK)
        {
          Error_Handler();
        }

        handle_GPDMA1_Channel5.Instance = GPDMA1_Channel5;
        handle_GPDMA1_Channel5.InitLinkedList.Priority = DMA_LOW_PRIORITY_MID_WEIGHT;
        handle_GPDMA1_Channel5.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
        handle_GPDMA1_Channel5.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT0;
        handle_GPDMA1_Channel5.InitLinkedList.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
        handle_GPDMA1_Channel5.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_CIRCULAR;
        if (HAL_DMAEx_List_Init(&handle_GPDMA1_Channel5) != HAL_OK)
        {
          Error_Handler();
        }

        if (HAL_DMAEx_List_LinkQ(&handle_GPDMA1_Channel5, &List_GPDMA1_Channel5) != HAL_OK)
        {
          Error_Handler();
        }

        __HAL_LINKDMA(hsai, hdmatx, handle_GPDMA1_Channel5);

        if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel5, DMA_CHANNEL_NPRIV) != HAL_OK)
        {
          Error_Handler();
        }
    }
}

void HAL_SAI_MspDeInit(SAI_HandleTypeDef* hsai)
{
/* SAI1 */
    if(hsai->Instance==SAI1_Block_A)
    {
    SAI1_client --;
    if (SAI1_client == 0)
      {
      /* Peripheral clock disable */
       __HAL_RCC_SAI1_CLK_DISABLE();
      /* SAI1 interrupt DeInit */
      HAL_NVIC_DisableIRQ(SAI1_IRQn);
      }

    /**SAI1_A_Block_A GPIO Configuration
    PA8     ------> SAI1_SCK_A
    PA9     ------> SAI1_FS_A
    PA10     ------> SAI1_SD_A
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10);

    /* SAI1 DMA Deinit */
    HAL_DMA_DeInit(hsai->hdmarx);
    }
}
