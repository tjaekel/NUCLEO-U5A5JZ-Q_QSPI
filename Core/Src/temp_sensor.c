/*
 * temp_sensor.c
 *
 *  Created on: Jan 17, 2024
 *      Author: tj925438
 */

#include "temp_sensor.h"

ADC_HandleTypeDef hadc;

uint16_t ADC_Val[3];
uint32_t VRefInt, VBat, TSensor;

/**
 * ATTENTION: ADC1 is used by USB PWR monitor, ADC2 here,
 * but ADC2 does not exist on U575
 * ADC1 stuff has to be disabled in stm32u5xx_nucelo_usbpd_pwr.c ! (we fake the result voltage)
 */

void ADC_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADCDAC;
  PeriphClkInit.AdcDacClockSelection = RCC_ADCDACCLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    ////Error_Handler();
  }

  /* Peripheral clock enable */
  __HAL_RCC_ADC12_CLK_ENABLE();

  /** Common config
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;		//running with PLL2
  hadc.Init.Resolution = ADC_RESOLUTION_14B;
  hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = 5;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    //Error_Handler();
  }

  /** Configure Regular Channel */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_68CYCLES;	//ADC_SAMPLETIME_1CYCLE_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    //Error_Handler();
  }

  sConfig.Channel = ADC_CHANNEL_VBAT;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    //Error_Handler();
  }

  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    //Error_Handler();
  }

  HAL_ADCEx_Calibration_Start(&hadc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
}

/* ----------------------------------------------------------------- */

int ADC_RunOnce(EResultOut out, int flag)
{
	static int initDone = 0;

	if ( ! initDone)
	{
		ADC_Init();
		initDone = 1;
	}

	HAL_ADC_Start(&hadc);
	for (uint8_t i = 0; i < (sizeof(ADC_Val) / sizeof(ADC_Val[0])); i++)
	{
		HAL_ADC_PollForConversion(&hadc, 1000);
		ADC_Val[i] = HAL_ADC_GetValue(&hadc);
	}
	HAL_ADC_Stop(&hadc);

	VRefInt 	= __HAL_ADC_CALC_VREFANALOG_VOLTAGE(hadc, ADC_Val[0], hadc.Init.Resolution);
	VBat 		= __HAL_ADC_CALC_DATA_TO_VOLTAGE(hadc, VRefInt, ADC_Val[1], hadc.Init.Resolution);
	TSensor	    = __HAL_ADC_CALC_TEMPERATURE(hadc, VRefInt, ADC_Val[2], hadc.Init.Resolution);

	//VBat value is 1/4 when read
	VBat *= 4;

	/* we have to compensate the temperature, formula:
	 * Temp (in C) = (100 - 30) / (TS_CAL2 - TS_CAL1) x (TS_DATA = TS_CAL1) + 30
	 * This is done by macro __HAL_ADC_CALC_TEMPERATURE
	 */

	if (flag)
		print_log(out, "MCU params : VRef: %ld.%ldV VBat: %ld.%ldV Temp: %ldC\r\n", VRefInt/1000, VRefInt%1000, VBat/1000, VBat%1000, TSensor);

	return 1;
}
