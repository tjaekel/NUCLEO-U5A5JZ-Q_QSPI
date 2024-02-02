/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_cdc_acm.c
  * @author  MCD Application Team
  * @brief   USBX Device applicative file
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

/* Includes ------------------------------------------------------------------*/
#include "ux_device_cdc_acm.h"

#include "main.h"
#include "cmd_dec.h"
#include "QSPI.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define APP_RX_DATA_SIZE   2048
#define APP_TX_DATA_SIZE   2048

/* Rx/TX flag */
#define RX_NEW_RECEIVED_DATA      0x01
#define TX_NEW_TRANSMITTED_DATA   0x02

/* Data length for vcp */
#define VCP_WORDLENGTH8  8
#define VCP_WORDLENGTH9  9

/* the minimum baudrate */
#define MIN_BAUDRATE     9600

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

UX_SLAVE_CLASS_CDC_ACM  *cdc_acm;

/* Data to send over USB CDC are stored in this buffer   */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

uint32_t UserTxBufPtrIn;
uint32_t UserTxBufPtrOut;

UART_HandleTypeDef *uart_handler;
extern TX_EVENT_FLAGS_GROUP EventFlag;

UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARAMETER CDC_VCP_LineCoding =
{
  1843200,	//115200, /* baud rate */
  0x00,   /* stop bits-1 */
  0x00,   /* parity - none */
  0x08    /* nb. of bits 8 */
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
extern VOID USBX_APP_UART_Init(UART_HandleTypeDef **huart);
#if 0
static void USBD_CDC_VCP_Config(UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARAMETER *);
#endif

void UART_Send(const uint8_t *p, size_t l);
void VCP_UART_Send(const uint8_t *p, size_t l);

uint8_t CmdLine[2][CMD_LINE_LEN];
int CmdLineIdx[2] = {0, 0};
extern uint8_t LastCmdLine[2][CMD_LINE_LEN];

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void SendPrompt(int idx)
{
	if (idx != 0)
	{
		UART_Send((uint8_t *)"> \003", 3);
	}
	else
	{
		VCP_UART_Send((uint8_t *)"> \003", 3);
	}
}

void PutCmdLine(int idx, uint8_t *str)
{
	size_t l, oldIdx;
	int i;
	l = strlen((char *)str);

	oldIdx = CmdLineIdx[idx];
	if (l)
	{
		strcpy((char *)&CmdLine[idx][0], (char *)str);
		CmdLineIdx[idx] = l;
		if (idx != 0)
		{
			//delete all existing characters
			for (i = 0; i < oldIdx; i++)
				UART_Send((uint8_t *)"\010 \010", 3);
			UART_Send((uint8_t *)str, l);
		}
		else
		{
			for (i = 0; i < oldIdx; i++)
				VCP_UART_Send((uint8_t *)"\010 \010", 3);
			VCP_UART_Send((uint8_t *)str, l);
		}
	}
}

int GetCmdLine(int idx, uint8_t c)
{
	CmdLine[idx][CmdLineIdx[idx]] = c;
	int l;

	if (CmdLineIdx[idx] >= (CMD_LINE_LEN - 1))
	{
		//full command line - call CMD processor
		l = CmdLineIdx[idx];
		CmdLineIdx[idx] = 0;
		//send newline
		if (idx != 0)
		{
			UART_Send((uint8_t *)"\r\n", 2);
		}
		else
		{
			VCP_UART_Send((uint8_t *)"\r\n", 2);
		}
		return l;
	}

	//handle backspace
	if (c == 0x08)
	{
		//backspace - delete last character
		if (CmdLineIdx[idx] > 0)
		{
			CmdLineIdx[idx]--;
			//where to send?
			if (idx != 0)
			{
				UART_Send((uint8_t *)"\010 \010", 3);
			}
			else
			{
				VCP_UART_Send((uint8_t *)"\010 \010", 3);
			}
		}

		return 0;
	}

	//handle DEL as "repeat last command"
	if (c == 0x7F)
	{
		PutCmdLine(idx, &LastCmdLine[idx][0]);
		return 0;
	}

	//check for NL, CR as end of string
	if ((c == '\r') || (c == '\n'))
	{
		CmdLine[idx][CmdLineIdx[idx]] = c;
		(CmdLineIdx[idx])++;
		CmdLine[idx][CmdLineIdx[idx]] = '\0';
		l = CmdLineIdx[idx];
		CmdLineIdx[idx] = 0;
		//print newline
		if (idx != 0)
		{
			UART_Send((uint8_t *)"\r\n", 2);
		}
		else
		{
			VCP_UART_Send((uint8_t *)"\r\n", 2);
		}
		return l;
	}

	//echo the character
	//where to send?
	if (idx != 0)
	{
		UART_Send((uint8_t *)&c, 1);
	}
	else
	{
		VCP_UART_Send((uint8_t *)&c, 1);
	}

	CmdLineIdx[idx]++;

	return 0;
}

/* USER CODE END 0 */

/**
  * @brief  USBD_CDC_ACM_Activate
  *         This function is called when insertion of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_Activate(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_Activate */

  /* Save the CDC instance */
  cdc_acm = (UX_SLAVE_CLASS_CDC_ACM*) cdc_acm_instance;

  /* ATT: regular UART works only if USB-C UART is connected! */

#if 0
  /* Configure the UART peripheral */
  USBX_APP_UART_Init(&uart_handler);
#endif

  /* Get default UART parameters */
  CDC_VCP_LineCoding.ux_slave_class_cdc_acm_parameter_baudrate = uart_handler->Init.BaudRate;

  /* Set the UART data type : only 8bits and 9bits are supported */
  switch (uart_handler->Init.WordLength)
  {
    case UART_WORDLENGTH_8B:
    {
      /* Set UART data bit to 8 */
      CDC_VCP_LineCoding.ux_slave_class_cdc_acm_parameter_data_bit = VCP_WORDLENGTH8;
      break;
    }

    case UART_WORDLENGTH_9B:
    {
      /* Set UART data bit to 9 */
      CDC_VCP_LineCoding.ux_slave_class_cdc_acm_parameter_data_bit = VCP_WORDLENGTH9;
      break;
    }

    default :
    {
      /* By default set UART data bit to 8 */
      CDC_VCP_LineCoding.ux_slave_class_cdc_acm_parameter_data_bit = VCP_WORDLENGTH8;
      break;
    }
  }

  /* Get UART Parity */
  CDC_VCP_LineCoding.ux_slave_class_cdc_acm_parameter_parity = uart_handler->Init.Parity;

  /* Get UART StopBits */
  CDC_VCP_LineCoding.ux_slave_class_cdc_acm_parameter_stop_bit = uart_handler->Init.StopBits;

#if 0
  /* Set device class_cdc_acm with default parameters */
  if (ux_device_class_cdc_acm_ioctl(cdc_acm, UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_LINE_CODING,
                                    &CDC_VCP_LineCoding) != UX_SUCCESS)
  {
    Error_Handler();
  }
#endif

#if 0
  /* Receive an amount of data in interrupt mode */
  if (HAL_UART_Receive_IT(uart_handler, (uint8_t *)UserTxBufferFS, 1) != HAL_OK)
  {
    /* Transfer error in reception process */
    Error_Handler();
  }
#endif

  /* USER CODE END USBD_CDC_ACM_Activate */

  return;
}

/**
  * @brief  USBD_CDC_ACM_Deactivate
  *         This function is called when extraction of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_Deactivate(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_Deactivate */
  UX_PARAMETER_NOT_USED(cdc_acm_instance);

  /* Reset the cdc acm instance */
  cdc_acm = UX_NULL;

  /* DeInitialize the UART peripheral */
  HAL_UART_DeInit(uart_handler);

  /* USER CODE END USBD_CDC_ACM_Deactivate */

  return;
}

/**
  * @brief  USBD_CDC_ACM_ParameterChange
  *         This function is invoked to manage the CDC ACM class requests.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_ParameterChange(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_ParameterChange */
  UX_PARAMETER_NOT_USED(cdc_acm_instance);

  ULONG request;
  UX_SLAVE_TRANSFER *transfer_request;
  UX_SLAVE_DEVICE *device;

  /* Get the pointer to the device.  */
  device = &_ux_system_slave -> ux_system_slave_device;

  /* Get the pointer to the transfer request associated with the control endpoint. */
  transfer_request = &device -> ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request;

  request = *(transfer_request -> ux_slave_transfer_request_setup + UX_SETUP_REQUEST);

  switch (request)
  {
    case UX_SLAVE_CLASS_CDC_ACM_SET_LINE_CODING :

      /* Get the Line Coding parameters */
      if (ux_device_class_cdc_acm_ioctl(cdc_acm, UX_SLAVE_CLASS_CDC_ACM_IOCTL_GET_LINE_CODING,
                                        &CDC_VCP_LineCoding) != UX_SUCCESS)
      {
        Error_Handler();
      }

      /* Check if baudrate < 9600) then set it to 9600 */
      if (CDC_VCP_LineCoding.ux_slave_class_cdc_acm_parameter_baudrate < MIN_BAUDRATE)
      {
        CDC_VCP_LineCoding.ux_slave_class_cdc_acm_parameter_baudrate = MIN_BAUDRATE;

#if 0
        /* Set the new configuration of ComPort */
        USBD_CDC_VCP_Config(&CDC_VCP_LineCoding);
#endif
      }
      else
      {
#if 0
        /* Set the new configuration of ComPort */
        USBD_CDC_VCP_Config(&CDC_VCP_LineCoding);
#endif
      }

      break;

    case UX_SLAVE_CLASS_CDC_ACM_GET_LINE_CODING :

      /* Set the Line Coding parameters */
      if (ux_device_class_cdc_acm_ioctl(cdc_acm, UX_SLAVE_CLASS_CDC_ACM_IOCTL_SET_LINE_CODING,
                                        &CDC_VCP_LineCoding) != UX_SUCCESS)
      {
        Error_Handler();
      }

      break;

    case UX_SLAVE_CLASS_CDC_ACM_SET_CONTROL_LINE_STATE :
    default :
      break;
  }

  /* USER CODE END USBD_CDC_ACM_ParameterChange */

  return;
}

/* USER CODE BEGIN 1 */

/**
  * @brief  Function implementing usbx_cdc_acm_thread_entry.
  * @param  thread_input: Not used
  * @retval none
  */
VOID usbx_cdc_acm_read_thread_entry(ULONG thread_input)
{
  extern void LED_Toggle(int dly);
  int l;
  ULONG i;

  ULONG actual_length;
#if 0
  ULONG senddataflag = 0;
#endif
  UX_SLAVE_DEVICE *device;

  UX_PARAMETER_NOT_USED(thread_input);

  device = &_ux_system_slave->ux_system_slave_device;

  while (1)
  {
    /* Check if device is configured */
    if ((device->ux_slave_device_state == UX_DEVICE_CONFIGURED) && (cdc_acm != UX_NULL))
    {

#ifndef UX_DEVICE_CLASS_CDC_ACM_TRANSMISSION_DISABLE

      /* Set transmission_status to UX_FALSE for the first time */
      cdc_acm -> ux_slave_class_cdc_acm_transmission_status = UX_FALSE;

#endif /* UX_DEVICE_CLASS_CDC_ACM_TRANSMISSION_DISABLE */

      /* Read the received data in blocking mode */
      ux_device_class_cdc_acm_read(cdc_acm, (UCHAR *)UserRxBufferFS, 64,
                                   &actual_length);
      if (actual_length != 0)
      {
#if 0
        /* Send the data via UART */
        if (HAL_UART_Transmit_DMA(uart_handler, (uint8_t *)UserRxBufferFS, actual_length) != HAL_OK)
        {
          Error_Handler();
        }

        /* Wait until the requested flag TX_NEW_TRANSMITTED_DATA is received */
        if (tx_event_flags_get(&EventFlag, TX_NEW_TRANSMITTED_DATA, TX_OR_CLEAR,
                               &senddataflag, TX_WAIT_FOREVER) != TX_SUCCESS)
        {
          Error_Handler();
        }
#else
        //echo back on same VCP UART
#if 0
        ULONG actual_lengthTx;
        if (ux_device_class_cdc_acm_write(cdc_acm, (UCHAR *)UserRxBufferFS,
                                                actual_length, &actual_lengthTx) == UX_SUCCESS)
		{

		}
#else
        //VCP_UART_Send(UserRxBufferFS, actual_length);
        l = 0;
        for (i = 0; i < actual_length; i++)
        {
        	l = GetCmdLine(0, UserRxBufferFS[i]);
        	if (l)
        		break;
        }
        if (l)
        {
        	////VCP_UART_Send((uint8_t *)&CmdLine[0][0], l);
        	CMD_DEC_execute((char *)&CmdLine[0][0], VCP_OUT);
        	SendPrompt(0);
        }
#endif
#endif

        LED_Toggle(0);
      }
      else
      {
        /* Sleep thread for 10ms if no data received */
        tx_thread_sleep(MS_TO_TICK(10));
        LED_Toggle(100);
      }
    }
    else
    {
      /* Sleep thread for 10ms */
      tx_thread_sleep(MS_TO_TICK(10));
      LED_Toggle(100);
    }
  }
}

void VCP_UART_Send(const uint8_t *p, size_t l)
{
    ULONG actual_lengthTx;
    if (ux_device_class_cdc_acm_write(cdc_acm, (UCHAR *)p,
                                            l, &actual_lengthTx) == UX_SUCCESS)
	{

	}
}

/**
  * @brief  Function implementing usbx_cdc_acm_write_thread_entry.
  * @param  thread_input: Not used
  * @retval none
  */
VOID usbx_cdc_acm_write_thread_entry(ULONG thread_input)
{
  ULONG receivedataflag = 0;
#if 0
  ULONG actual_length, buffptr, buffsize;
#else
  ULONG buffptr, buffsize;
#if 0
  ULONG senddataflag = 0;
#endif
#endif
  int l;
  ULONG i;

  UX_PARAMETER_NOT_USED(thread_input);

  //XXXX
  /* Configure the UART peripheral */
  USBX_APP_UART_Init(&uart_handler);

  /* Receive an amount of data in interrupt mode */
  if (HAL_UART_Receive_IT(uart_handler, (uint8_t *)UserTxBufferFS, 1) != HAL_OK)
  {
    /* Transfer error in reception process */
    Error_Handler();
  }

  while (1)
  {
    /* Wait until the requested flag RX_NEW_RECEIVED_DATA is received */
    if (tx_event_flags_get(&EventFlag, RX_NEW_RECEIVED_DATA, TX_OR_CLEAR,
                           &receivedataflag, TX_WAIT_FOREVER) != TX_SUCCESS)
    {
      Error_Handler();
    }

#ifndef UX_DEVICE_CLASS_CDC_ACM_TRANSMISSION_DISABLE

    /* Set transmission_status to UX_FALSE for the first time */
    cdc_acm -> ux_slave_class_cdc_acm_transmission_status = UX_FALSE;

#endif

    /* Check if there is a new data to send */
    if (UserTxBufPtrOut != UserTxBufPtrIn)
    {
      /* Check buffer overflow and Rollback */
      if (UserTxBufPtrOut > UserTxBufPtrIn)
      {
        buffsize = APP_RX_DATA_SIZE - UserTxBufPtrOut;
      }
      else
      {
        /* Calculate data size */
        buffsize = UserTxBufPtrIn - UserTxBufPtrOut;
      }

      /* Copy UserTxBufPtrOut in buffptr */
      buffptr = UserTxBufPtrOut;

#if 0
      /* Send data over the class cdc_acm_write */
      if (ux_device_class_cdc_acm_write(cdc_acm, (UCHAR *)(&UserTxBufferFS[buffptr]),
                                        buffsize, &actual_length) == UX_SUCCESS)
      {
        /* Increment the UserTxBufPtrOut pointer */
        UserTxBufPtrOut += buffsize;

        /* Rollback UserTxBufPtrOut if it equal to APP_TX_DATA_SIZE */
        if (UserTxBufPtrOut == APP_TX_DATA_SIZE)
        {
          UserTxBufPtrOut = 0;
        }
      }
#else
      //send back over same UART
      /* Send the data via UART */
#if 0
      if (HAL_UART_Transmit_DMA(uart_handler, (uint8_t *)(&UserTxBufferFS[buffptr]), buffsize) != HAL_OK)
      {
        Error_Handler();
      }
#else
      //UART_Send((uint8_t *)(&UserTxBufferFS[buffptr]), buffsize);
      l = 0;
      for (i = 0; i < buffsize; i++)
      {
    	  l = GetCmdLine(1, UserTxBufferFS[buffptr + i]);
    	  if (l)
    		  break;
      }
      if (l)
      {
      	////UART_Send((uint8_t *)&CmdLine[1][0], l);
    	CMD_DEC_execute((char *)&CmdLine[1][0], UART_OUT);
        SendPrompt(1);
      }
#endif

      /* Increment the UserTxBufPtrOut pointer */
      UserTxBufPtrOut += buffsize;

      /* Rollback UserTxBufPtrOut if it equal to APP_TX_DATA_SIZE */
      if (UserTxBufPtrOut == APP_TX_DATA_SIZE)
      {
        UserTxBufPtrOut = 0;
      }

#if 0
      /* Wait until the requested flag TX_NEW_TRANSMITTED_DATA is received */
      if (tx_event_flags_get(&EventFlag, TX_NEW_TRANSMITTED_DATA, TX_OR_CLEAR,
                             &senddataflag, TX_WAIT_FOREVER) != TX_SUCCESS)
      {
        Error_Handler();
      }
#endif

#endif
    }
  }
}

void UART_Send(const uint8_t *p, size_t l)
{
	ULONG senddataflag = 0;

    if (HAL_UART_Transmit_DMA(uart_handler, p, l) != HAL_OK)
    {
      Error_Handler();
    }

    /* Wait until the requested flag TX_NEW_TRANSMITTED_DATA is received */
    if (tx_event_flags_get(&EventFlag, TX_NEW_TRANSMITTED_DATA, TX_OR_CLEAR,
                           &senddataflag, TX_WAIT_FOREVER) != TX_SUCCESS)
    {
      Error_Handler();
    }
}

/**
  * @brief  Tx Transfer completed callback.
  * @param  huart UART handle.
  * @retval none
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Set TX_NEW_TRANSMITTED_DATA flag */
  if (tx_event_flags_set(&EventFlag, TX_NEW_TRANSMITTED_DATA, TX_OR) != TX_SUCCESS)
  {
    Error_Handler();
  }
}

/**
  * @brief  Rx Transfer completed callback
  * @param  huart: UART handle
  * @retval none
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Set RX_NEW_RECEIVED_DATA flag */
  if (tx_event_flags_set(&EventFlag, RX_NEW_RECEIVED_DATA, TX_OR) != TX_SUCCESS)
  {
    Error_Handler();
  }

  Generic_SetRxFlag(UART_OUT);

  /* Increment the UserTxBufPtrIn pointer */
  UserTxBufPtrIn++;

  /* Rollback the UserTxBufPtrIn if it equal to APP_TX_DATA_SIZE */
  if (UserTxBufPtrIn == APP_TX_DATA_SIZE)
  {
    UserTxBufPtrIn = 0;
  }

  /* Start another reception: provide the buffer pointer with offset and the buffer size */
  if (HAL_UART_Receive_IT(uart_handler, (uint8_t *)UserTxBufferFS + UserTxBufPtrIn, 1) != HAL_OK)
  {
    /* Transfer error in reception process */
    Error_Handler();
  }

}

/**
  * @brief  UART error callbacks
            Transfer error occurred in reception and/or transmission process.
  * @param  UartHandle: UART handle
  * @retval none
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  Error_Handler();
}

#if 0
/**
  * @brief  USBD_CDC_VCP_Config
            Configure the COM Port with the parameters received from host.
  * @param  UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARAMETER: linecoding struct.
  * @param  CDC_VCP_LineCoding: CDC VCP line coding.
  * @retval none
  */
static VOID USBD_CDC_VCP_Config(UX_SLAVE_CLASS_CDC_ACM_LINE_CODING_PARAMETER
                                *CDC_VCP_LineCoding)
{
  /* Deinitialization UART */
  if (HAL_UART_DeInit(uart_handler) != HAL_OK)
  {
    /* Deinitialization Error */
    Error_Handler();
  }

  /* Check stop bit parameter */
  switch (CDC_VCP_LineCoding->ux_slave_class_cdc_acm_parameter_stop_bit)
  {
    case 0:

      /* Set the UART Stop bit to 1 */
      uart_handler->Init.StopBits = UART_STOPBITS_1;

      break;

    case 2:

      /* Set the UART Stop bit to 2 */
      uart_handler->Init.StopBits = UART_STOPBITS_2;

      break;

    default :

      /* By default set the UART Stop bit to 1 */
      uart_handler->Init.StopBits = UART_STOPBITS_1;

      break;
  }

  /* Check parity parameter */
  switch (CDC_VCP_LineCoding->ux_slave_class_cdc_acm_parameter_parity)
  {
    case 0:

      /* Set the UART parity bit to none */
      uart_handler->Init.Parity = UART_PARITY_NONE;

      break;

    case 1:

      /* Set the UART parity bit to ODD */
      uart_handler->Init.Parity = UART_PARITY_ODD;

      break;

    case 2:

      /* Set the UART parity bit to even */
      uart_handler->Init.Parity = UART_PARITY_EVEN;

      break;

    default :

      /* By default set the UART parity bit to none */
      uart_handler->Init.Parity = UART_PARITY_NONE;

      break;
  }

  /* Set the UART data type : only 8bits and 9bits is supported */
  switch (CDC_VCP_LineCoding->ux_slave_class_cdc_acm_parameter_data_bit)
  {
    case 0x07:

      /* With this configuration a parity (Even or Odd) must be set */
      uart_handler->Init.WordLength = UART_WORDLENGTH_8B;

      break;

    case 0x08:

      if (uart_handler->Init.Parity == UART_PARITY_NONE)
      {
        uart_handler->Init.WordLength = UART_WORDLENGTH_8B;
      }
      else
      {
        uart_handler->Init.WordLength = UART_WORDLENGTH_9B;
      }

      break;

    default :

      uart_handler->Init.WordLength = UART_WORDLENGTH_8B;

      break;
  }

  /* Get the UART baudrate from vcp */
  uart_handler->Init.BaudRate = CDC_VCP_LineCoding->ux_slave_class_cdc_acm_parameter_baudrate;

  /* Set the UART Hw flow control to none */
  uart_handler->Init.HwFlowCtl = UART_HWCONTROL_NONE;

  /* Set the UART mode */
  uart_handler->Init.Mode = UART_MODE_TX_RX;

  /* Set the UART sampling */
  uart_handler->Init.OverSampling = UART_OVERSAMPLING_16;

  /* Initialization UART */
  if (HAL_UART_Init(uart_handler) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Start reception: provide the buffer pointer with offset and the buffer size */
  HAL_UART_Receive_IT(uart_handler, (uint8_t *)(UserTxBufferFS + UserTxBufPtrIn), 1);
}
#endif

/* USER CODE END 1 */
