/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : linked_list.h
  * Description        : This file provides code for the configuration
  *                      of the LinkedList.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern DMA_NodeTypeDef Node_tx;
extern DMA_QListTypeDef Queue_tx;
extern DMA_NodeTypeDef Node_rx;
extern DMA_QListTypeDef Queue_rx;
extern DMA_QListTypeDef Queue_rx_SAI;

extern DMA_NodeTypeDef NodeRx;
extern DMA_QListTypeDef MDFQueue;

HAL_StatusTypeDef MX_Queue_tx_Config(void);
HAL_StatusTypeDef MX_Queue_rx_Config(void);
HAL_StatusTypeDef MX_MDFQueue_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* LINKED_LIST_H */

