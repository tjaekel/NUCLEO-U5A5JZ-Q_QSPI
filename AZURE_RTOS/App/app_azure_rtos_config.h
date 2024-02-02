
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_azure_rtos_config.h
  * @author  MCD Application Team
  * @brief   app_azure_rtos config header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef APP_AZURE_RTOS_CONFIG_H
#define APP_AZURE_RTOS_CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif

/* Using static memory allocation via threadX Byte memory pools */

#define USE_STATIC_ALLOCATION                	1
#define UX_DEVICE_APP_MEM_POOL_SIZE             1024 * 16	//important for USBX_DEVICE_MEMORY_STACK_SIZE
#define TX_APP_MEM_POOL_SIZE                    1024 * 2
#define USBPD_DEVICE_APP_MEM_POOL_SIZE          1024 * 6

#ifdef __cplusplus
}
#endif

#endif /* APP_AZURE_RTOS_CONFIG_H */

