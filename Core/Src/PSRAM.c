/*
 * PSRAM.c
 *
 *  Created on: Mar 3, 2024
 *      Author: tj
 */

#include "main.h"
#include "PSRAM.h"

typedef uint8_t QSPI_Type;

#if 1
//TEST
QSPI_Type aTxBuffer[BUFFERSIZE] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F};
#define QSPI_TEST_SIZE (sizeof(aTxBuffer) / sizeof(QSPI_Type))
#endif

extern OSPI_HandleTypeDef hospi1;
LL_DLYB_CfgTypeDef dlyb_cfg,dlyb_cfg_test;
OSPI_MemoryMappedTypeDef sMemMappedCfg;

void EnableCompensationCell(void);

////#define TEST_QSPI_INDIRECT
#ifdef TEST_QSPI_INDIRECT
static QSPI_Type X[BUFFERSIZE];
#endif

void Change_QSPISpeed(void)
{
  /* OCTOSPI1 parameter configuration for faster speed*/
  HAL_OSPI_DeInit(&hospi1);

  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThreshold = 1;
  hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
  hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MICRON;
  hospi1.Init.DeviceSize = 24;		//number of address bits!
  hospi1.Init.ChipSelectHighTime = 1;
  hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;	//HAL_OSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
  hospi1.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;	//or configure for HAL_OSPI_WRAP_32_BYTES?
  /* with HAL_OSPI_WRAP_32_BYTES the debugger disconnects ! */
  hospi1.Init.ClockPrescaler = 2;			//with PLL2N = 200 MHz = 100 MHz (104 MHz average) for QSPI PSRAM in QSPI mode
  hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_NONE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE;
  hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_USED;
#ifdef ENABLE_DCACHE
  /* with DCache: align with cache line size (32bytes: U5A5, U575 is just 16bytes) */
  hospi1.Init.ChipSelectBoundary = 8;		/* 4 is 4 words per chunk = 16bytes: U575, 8 is 8 words per chunk = 32bytes: U5A5 */
  hospi1.Init.MaxTran = 0;		/* just used of other OCTOSPI is needed */
  hospi1.Init.Refresh = 256;
#else
  /* without DCache, but avoid page wrap - assuming AHB bus is 23bit */
  hospi1.Init.ChipSelectBoundary = 4;	/* should it be 1 for 32but bus, 4 bytes? */
  hospi1.Init.MaxTran = 0;
  hospi1.Init.Refresh = 256;
#endif
  if (HAL_OSPI_Init(&hospi1) != HAL_OK)
  {
    Error_Handler();
  }
}

int PSRAM_Init(void)
{
#ifdef TEST_QSPI_INDIRECT
	int i;
#endif
	OSPI_RegularCmdTypeDef sCommand = {0};

	  /* Enable Compensation cell */
	  EnableCompensationCell();
	  /* Delay block configuration ------------------------------------------------ */
	  if (HAL_OSPI_DLYB_GetClockPeriod(&hospi1,&dlyb_cfg) != HAL_OK)
	  {
		  return -1;
	  }

	  /*when DTR, PhaseSel is divided by 4 (emperic value)*/
	  dlyb_cfg.PhaseSel /= 4;	//4

	  /* save the present configuration for check*/
	  dlyb_cfg_test = dlyb_cfg;

	  /*set delay block configuration*/
	  HAL_OSPI_DLYB_SetConfig(&hospi1, &dlyb_cfg);

	  /*check the set value*/
	  HAL_OSPI_DLYB_GetConfig(&hospi1, &dlyb_cfg);
	  if ((dlyb_cfg.PhaseSel != dlyb_cfg_test.PhaseSel) || (dlyb_cfg.Units != dlyb_cfg_test.Units))
	  {
		  return -1;
	  }

	  /*Configure QSPI mode: afterwards 4-4-4 */
	  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;	//HAL_OSPI_OPTYPE_WRITE_CFG;
	  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
	  sCommand.Instruction        = ENTER_QPI;
	  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
	  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
	  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
	  sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
	  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
	  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
	  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
	  sCommand.DummyCycles        = DUMMY_CLOCK_CYCLES_WRITE;
	  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
	  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

	  if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	  {
		  return -1;
	  }

	  Change_QSPISpeed();		/* change to faster speed in QSPI mode, max. 104 MHz, we should have 100 MHz with PLL2N = 200 MHz */

#ifdef TEST_QSPI_INDIRECT
	  /* test QSPI Write*/
	  for (i = 0; i < 16; i++)
		  X[i] = 0x20 + i;
	  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;	//HAL_OSPI_OPTYPE_WRITE_CFG;
	  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
	  sCommand.Instruction        = WRITE_CMD;
	  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_4_LINES;
	  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
	  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	  sCommand.AddressMode        = HAL_OSPI_ADDRESS_4_LINES;
	  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
	  sCommand.Address			  = 0x0;
	  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
	  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	  sCommand.DataMode           = HAL_OSPI_DATA_4_LINES;
	  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
	  sCommand.NbData			  = 16 * sizeof(QSPI_Type);
	  sCommand.DummyCycles        = DUMMY_CLOCK_CYCLES_WRITE;
	  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
	  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

	  if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	  {
		  return -1;
	  }

	  if (HAL_OSPI_Transmit(&hospi1, (uint8_t *)X, 1000) != HAL_OK)
	  {
		  return -1;
	  }

	  /*test QSPI Read*/
	  for (i = 0; i < 16; i++)
		  X[i] = 0;

	  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;	//HAL_OSPI_OPTYPE_WRITE_CFG;
	  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
	  sCommand.Instruction        = READ_CMD;
	  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_4_LINES;
	  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
	  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	  sCommand.AddressMode        = HAL_OSPI_ADDRESS_4_LINES;
	  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
	  sCommand.Address			  = 0x0;
	  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
	  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	  sCommand.DataMode           = HAL_OSPI_DATA_4_LINES;
	  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
	  sCommand.NbData			  = 16 * sizeof(QSPI_Type);
	  sCommand.DummyCycles        = DUMMY_CLOCK_CYCLES_READ;
	  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
	  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

	  if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	  {
		  return -1;
	  }

	  if (HAL_OSPI_Receive(&hospi1, (uint8_t *)X, 1000) != HAL_OK)
	  {
		  return -1;
	  }
	  /* OK - this works! */
#endif

	  /*Configure Memory Mapped mode*/
	  sCommand.OperationType      = HAL_OSPI_OPTYPE_WRITE_CFG;
	  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
	  sCommand.Instruction        = WRITE_CMD;
	  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_4_LINES;
	  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
	  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	  sCommand.AddressMode        = HAL_OSPI_ADDRESS_4_LINES;
	  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
	  sCommand.Address			  = 0x0;
	  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
	  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	  sCommand.DataMode           = HAL_OSPI_DATA_4_LINES;
	  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
	  sCommand.DummyCycles        = DUMMY_CLOCK_CYCLES_WRITE;
	  ////sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
	  /* VERY IMPORTANT! */
	  sCommand.DQSMode       	  = HAL_OSPI_DQS_ENABLE;			//ERRATA: ENABLE should fix - but it does not!
	  	  	  	  	  	  	  	  	  	  	  	  	  	  			//the debugger is disconnected and program even do not proceed!
	  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

	  if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  sCommand.OperationType = HAL_OSPI_OPTYPE_READ_CFG;
	  sCommand.Instruction   = READ_CMD;
	  sCommand.DummyCycles   = DUMMY_CLOCK_CYCLES_READ;
	  sCommand.DQSMode       = HAL_OSPI_DQS_DISABLE;
	  /* Remark: if you disable here DQSMode - the debugger will disconnect! */

	  if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  sMemMappedCfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_ENABLE;
	  sMemMappedCfg.TimeOutPeriod     = 0x34;

	  LED_Status(0);

	  if (HAL_OSPI_MemoryMapped(&hospi1, &sMemMappedCfg) != HAL_OK)
	  {
		  return -1;
	  }

#ifdef TEST_QSPI_MEMORY_MAPPED
	  //TEST
	  {
		  uint32_t index, index_K;
		  __IO QSPI_Type *mem_addr;

#if 1
		  /* we can write only via memory mapped mode if Command.DQSMode = HAL_OSPI_DQS_ENABLE ! */
		  index_K = 0;
		  mem_addr = (QSPI_Type *)(OCTOSPI1_BASE + (index_K * KByte));
		  ////for (index_K = 0; index_K < 10; index_K++)
		  {
		    ////mem_addr = (QSPI_Type *)(OCTOSPI1_BASE + (index_K * KByte));
		    ////for (index = (index_K * KByte); index < ((index_K +1) * KByte); index++)
		    for (index = 0; index < QSPI_TEST_SIZE; index++)
		    {
		      *mem_addr = aTxBuffer[index];
		      mem_addr++;
		    }

		    LED_Status(1);
		  }
#endif

		  /* Reading Sequence ----------------------------------------------- */
		  index_K = 0;
		  mem_addr = (QSPI_Type *)(OCTOSPI1_BASE + (index_K * KByte));
		  ////for (index_K = 0; index_K < 10; index_K++)
		  {
		    ////mem_addr = (QSPI_Typet *)(OCTOSPI1_BASE + (index_K * KByte));
		    ////for (index = (index_K * KByte); index < ((index_K +1) * KByte); index++)
			for (index = 0; index < QSPI_TEST_SIZE; index++)
		    {
		      if (*mem_addr != aTxBuffer[index])
		      {
		    	  LED_Status(3);
		      }
		      mem_addr++;
		    }
		  }

		  LED_Status(2);
	  }
#endif

	  return 0;
}

/**
  * @brief Enable Compensation Cell
  * @retval None
  */
void EnableCompensationCell(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;
  HAL_SYSCFG_EnableVddCompensationCell();
}
