/*
 * ADF_PDM.c
 *
 *  Created on: May 14, 2024
 *      Author: tj925438
 */

#include "main.h"
#include "linked_list.h"
#include "ADF_PDM.h"

MDF_HandleTypeDef AdfHandle0;
MDF_FilterConfigTypeDef AdfFilterConfig0;

extern DMA_HandleTypeDef handle_GPDMA1_Channel11;

MDF_DmaConfigTypeDef         DMAConfig;

/*Buffer location and size should aligned to cache line size (32 bytes) */
int16_t        RecBuff[REC_BUFF_SIZE * 2U];		//2 * 1024 * 2 = 4096 bytes
__IO uint32_t  DmaRecHalfBuffCplt  = 0;
__IO uint32_t  DmaRecBuffCplt      = 0;
uint32_t       PlaybackStarted     = 0;

extern DMA_QListTypeDef MDFQueue;

/**
* @brief MDF MSP Initialization
* This function configures the hardware resources used in this example
* @param hmdf: MDF handle pointer
* @retval None
*/
void HAL_MDF_MspInit(MDF_HandleTypeDef* hmdf)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(IS_ADF_INSTANCE(hmdf->Instance))
  {
  /* USER CODE BEGIN ADF1_MspInit 0 */

  /* USER CODE END ADF1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADF1;
#if 1
    PeriphClkInit.Adf1ClockSelection = RCC_ADF1CLKSOURCE_PLL3;	//RCC_ADF1CLKSOURCE_MSIK;

    /* the same as in HAL_OSPI_MspInit(), actually done already */
    PeriphClkInit.PLL3.PLL3Source = RCC_PLLSOURCE_HSE;
    PeriphClkInit.PLL3.PLL3M = 1;

    /* 48 KHz SPDIF            with scope/debug 	CubeMX cfg      correct */
    PeriphClkInit.PLL3.PLL3N = 36;					//36;			36;
    PeriphClkInit.PLL3.PLL3P = 24;					//96; 			24;		//SAI2
    PeriphClkInit.PLL3.PLL3Q = 24;											//ADF1 - see: OutputClock.Divider
    PeriphClkInit.PLL3.PLL3R = 2;
    PeriphClkInit.PLL3.PLL3RGE = RCC_PLLVCIRANGE_1;
    PeriphClkInit.PLL3.PLL3FRACN = 7080;			//7080			7078; -->
    // it results in 12,288.004 KHz in FW - MCLKDIV should be 2 at the end - but it fails!
    /* but: above 12,288 KHz results in MCLKDIV +1, too large! - so, we have to get a bit below "nominal", resulting in a larger clock offset error:
     * -36Hz, compared to value 7078 with just 4 Hz error!
     * why I cannot use 7078 with 12,288.004 KHz which would be better? The MCLKDIV is wrong by +1!
     */
    PeriphClkInit.PLL3.PLL3ClockOut = RCC_PLL3_DIVQ;
#else
    PeriphClkInit.Adf1ClockSelection = RCC_ADF1CLKSOURCE_MSIK;
#endif
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_ADF1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**ADF1 GPIO Configuration
    PB4 (NJTRST)     ------> ADF1_SDI0
    PB3 (JTDO/TRACESWO)     ------> ADF1_CCK0
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_ADF1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN ADF1_MspInit 1 */

  /* USER CODE END ADF1_MspInit 1 */
  }

}

/**
* @brief MDF MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hmdf: MDF handle pointer
* @retval None
*/
void HAL_MDF_MspDeInit(MDF_HandleTypeDef* hmdf)
{
  if(IS_ADF_INSTANCE(hmdf->Instance))
  {
  /* USER CODE BEGIN ADF1_MspDeInit 0 */

  /* USER CODE END ADF1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADF1_CLK_DISABLE();

    /**ADF1 GPIO Configuration
    PB4 (NJTRST)     ------> ADF1_SDI0
    PB3 (JTDO/TRACESWO)     ------> ADF1_CCK0
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4|GPIO_PIN_3);

  /* USER CODE BEGIN ADF1_MspDeInit 1 */

  /* USER CODE END ADF1_MspDeInit 1 */
  }

}

/**
  * @brief ADF1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADF1_Init(void)
{

  /* USER CODE BEGIN ADF1_Init 0 */

  /* USER CODE END ADF1_Init 0 */

  /* USER CODE BEGIN ADF1_Init 1 */

  /* USER CODE END ADF1_Init 1 */

  /**
    AdfHandle0 structure initialization and HAL_MDF_Init function call
  */
  AdfHandle0.Instance = ADF1_Filter0;
  AdfHandle0.Init.CommonParam.ProcClockDivider = 1;
  AdfHandle0.Init.CommonParam.OutputClock.Activation = ENABLE;
  AdfHandle0.Init.CommonParam.OutputClock.Pins = MDF_OUTPUT_CLOCK_0;
  AdfHandle0.Init.CommonParam.OutputClock.Divider = 4;			//see: PLL3.PLL3Q
  AdfHandle0.Init.CommonParam.OutputClock.Trigger.Activation = DISABLE;
  AdfHandle0.Init.SerialInterface.Activation = ENABLE;
  AdfHandle0.Init.SerialInterface.Mode = MDF_SITF_NORMAL_SPI_MODE;
  AdfHandle0.Init.SerialInterface.ClockSource = MDF_SITF_CCK0_SOURCE;
  AdfHandle0.Init.SerialInterface.Threshold = 31;
  AdfHandle0.Init.FilterBistream = MDF_BITSTREAM0_RISING;	//MDF_BITSTREAM0_FALLING; //select L/R
  if (HAL_MDF_Init(&AdfHandle0) != HAL_OK)
  {
    Error_Handler();
  }

  /**
    AdfFilterConfig0 structure initialization

    WARNING : only structure is filled, no specific init function call for filter
  */
  AdfFilterConfig0.DataSource = MDF_DATA_SOURCE_BSMX;
  AdfFilterConfig0.Delay = 0;
  AdfFilterConfig0.CicMode = MDF_ONE_FILTER_SINC4;
  AdfFilterConfig0.DecimationRatio = 64;
  AdfFilterConfig0.Gain = 1;				//-16..+24, but 10 is already saturated!
  AdfFilterConfig0.ReshapeFilter.Activation = DISABLE;
  AdfFilterConfig0.HighPassFilter.Activation = DISABLE;
  AdfFilterConfig0.SoundActivity.Activation = DISABLE;
  AdfFilterConfig0.AcquisitionMode = MDF_MODE_ASYNC_CONT;
  AdfFilterConfig0.FifoThreshold = MDF_FIFO_THRESHOLD_NOT_EMPTY;
  AdfFilterConfig0.DiscardSamples = 0;
  /* USER CODE BEGIN ADF1_Init 2 */
  /* USER CODE END ADF1_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  MDF DMA configuration
  * @param  None
  * @retval None
  */
static void MDF_DMAConfig(void)
{
  /* Initialize DMA configuration parameters */
  DMAConfig.Address                              = (uint32_t)&RecBuff[0];
  DMAConfig.DataLength                           = (REC_BUFF_SIZE * 4U);		//4096 bytes
  DMAConfig.MsbOnly                              = ENABLE;
}

/**
  * @brief This function handles GPDMA1 Channel 11 global interrupt.
  */
void GPDMA1_Channel11_IRQHandler(void)
{
  /* USER CODE BEGIN GPDMA1_Channel11_IRQn 0 */

  /* USER CODE END GPDMA1_Channel11_IRQn 0 */
  HAL_DMA_IRQHandler(&handle_GPDMA1_Channel11);
  /* USER CODE BEGIN GPDMA1_Channel11_IRQn 1 */

  /* USER CODE END GPDMA1_Channel11_IRQn 1 */
}

/**
  * @brief  Half regular conversion complete callback.
  * @param  hmdf : MDF andle.
  * @retval None
  */
void HAL_MDF_AcqHalfCpltCallback(MDF_HandleTypeDef *hmdf)
{
	(void)hmdf;
    DmaRecHalfBuffCplt = 1;
}

/**
  * @brief  Regular conversion complete callback.
  * @note   In interrupt mode, user has to read conversion value in this function
            using HAL_MDF_GetAcqValue.
  * @param  hmdf : MDF handle.
  * @retval None
  */
void HAL_MDF_AcqCpltCallback(MDF_HandleTypeDef *hmdf)
{
	(void)hmdf;
    DmaRecBuffCplt = 1;
}

/* ------------------------------------------------------- */

void ADF_PDM_Init(void)
{
	MX_ADF1_Init();

	MX_MDFQueue_Config();

	HAL_DMAEx_List_LinkQ(&handle_GPDMA1_Channel11, &MDFQueue);

	MDF_DMAConfig();
	if (HAL_MDF_AcqStart_DMA(&AdfHandle0, &AdfFilterConfig0, &DMAConfig) != HAL_OK)
	{
	    Error_Handler();
	}
}

void ADF_PDM_DeInit(void)
{
	HAL_MDF_MspDeInit(&AdfHandle0);
}
