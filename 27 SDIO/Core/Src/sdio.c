/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sdio.c
  * @brief   This file provides code for the configuration
  *          of the SDIO instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "sdio.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
uint8_t TX[BLOCKSIZE]="Hello, welcome to UPC\0";  
uint8_t RX[BLOCKSIZE];  	
/* USER CODE END 0 */

SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;

/* SDIO init function */

void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 4;
  if (HAL_SD_Init(&hsd) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}

void HAL_SD_MspInit(SD_HandleTypeDef* sdHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(sdHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspInit 0 */

  /* USER CODE END SDIO_MspInit 0 */
    /* SDIO clock enable */
    __HAL_RCC_SDIO_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* SDIO DMA Init */
    /* SDIO_RX Init */
    hdma_sdio_rx.Instance = DMA2_Stream3;
    hdma_sdio_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdio_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_rx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sdio_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_rx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_rx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(sdHandle,hdmarx,hdma_sdio_rx);

    /* SDIO_TX Init */
    hdma_sdio_tx.Instance = DMA2_Stream6;
    hdma_sdio_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sdio_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_tx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sdio_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_tx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_tx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(sdHandle,hdmatx,hdma_sdio_tx);

    /* SDIO interrupt Init */
    HAL_NVIC_SetPriority(SDIO_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspInit 1 */

  /* USER CODE END SDIO_MspInit 1 */
  }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* sdHandle)
{

  if(sdHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspDeInit 0 */

  /* USER CODE END SDIO_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDIO_CLK_DISABLE();

    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* SDIO DMA DeInit */
    HAL_DMA_DeInit(sdHandle->hdmarx);
    HAL_DMA_DeInit(sdHandle->hdmatx);

    /* SDIO interrupt Deinit */
    HAL_NVIC_DisableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspDeInit 1 */

  /* USER CODE END SDIO_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//显示SD卡的信息
void SDCard_ShowInfo(void)
{
	//SD卡信息结构体变量
	HAL_SD_CardInfoTypeDef cardInfo;  
	HAL_StatusTypeDef res = HAL_SD_GetCardInfo(&hsd, &cardInfo);

	if(res!=HAL_OK)
	{
		printf("HAL_SD_GetCardInfo() error\r\n");
		return;
	}

	printf("\r\n*** HAL_SD_GetCardInfo() info ***\r\n");
	printf("Card Type= %d\r\n", cardInfo.CardType);
	printf("Card Version= %d\r\n", cardInfo.CardVersion);
	printf("Card Class= %d\r\n", cardInfo.Class);
	printf("Relative Card Address= %d\r\n", cardInfo.RelCardAdd);
	printf("Block Count= %d\r\n", cardInfo.BlockNbr);
	printf("Block Size(Bytes)= %d\r\n", cardInfo.BlockSize);
	printf("LogiBlockCount= %d\r\n", cardInfo.LogBlockNbr);
	printf("LogiBlockSize(Bytes)= %d\r\n", cardInfo.LogBlockSize);
	printf("SD Card Capacity(MB)= %d\r\n", cardInfo.BlockNbr>>1>>10);
}


void SDCard_ShowCSD(void)
{
	HAL_SD_CardCSDTypeDef cardCSD;
	HAL_StatusTypeDef res = HAL_SD_GetCardCSD(&hsd, &cardCSD);

	if(res!=HAL_OK)
	{
		printf("HAL_SD_GetCardCSD() error\r\n");
		return;
	}

	printf("\r\n*** HAL_SD_GetCardCSD() info ***\r\n");
	printf("Specification Version= %d\r\n", cardCSD.SysSpecVersion);
	printf("Max Bus Clock Freq= %d\r\n", cardCSD.MaxBusClkFrec);
	printf("Read Block Len= %d\r\n", cardCSD.RdBlockLen);
	printf("DeviceSizen= %d\r\n", cardCSD.DeviceSize);
	printf("DeviceSizeMul= %d\r\n", cardCSD.DeviceSizeMul);
	printf("EraseGrSize= %d\r\n", cardCSD.EraseGrSize);
	printf("MaxWrBlockLen= %d\r\n", cardCSD.MaxWrBlockLen);
}


void SDCard_ShowCID(void)
{
	HAL_SD_CardCIDTypeDef cardCID;
	HAL_StatusTypeDef res = HAL_SD_GetCardCID(&hsd, &cardCID);

	if(res!=HAL_OK)
	{
		printf("HAL_SD_GetCardCID() error\r\n");
		return;
	}
	
	printf("\r\n*** HAL_SD_GetCardCID() info ***\r\n");
	printf("ManufacturerID= %d\r\n", cardCID.ManufacturerID);
	printf("ProdName1= %d\r\n", cardCID.ProdName1);
	printf("ProdName2= %d\r\n", cardCID.ProdName2);
	printf("ProdRev= %d\r\n", cardCID.ProdRev);
	printf("ProdSN= %d\r\n", cardCID.ProdSN);
	printf("ManufactDate= %d\r\n", cardCID.ManufactDate);
}

//获取SD卡当前状态
void SDCard_ShowStatus(void)
{
	//SD卡状态结构体变量
	HAL_SD_CardStatusTypeDef cardStatus;
	HAL_StatusTypeDef res = HAL_SD_GetCardStatus(&hsd, &cardStatus);

	if(res!=HAL_OK)
	{
		printf("HAL_SD_GetCardStatus() error\r\n");
		return;
	}

	printf("\r\n*** HAL_SD_GetCardStatus() info ***\r\n");
	printf("DataBusWidth= %d\r\n", cardStatus.DataBusWidth);
	printf("CardType= %d\r\n", cardStatus.CardType);
	printf("SpeedClass= %d\r\n", cardStatus.SpeedClass);
	printf("AllocationUnitSize= %d\r\n", cardStatus.AllocationUnitSize);
	printf("EraseSize= %d\r\n", cardStatus.EraseSize);
	printf("EraseTimeout= %d\r\n", cardStatus.EraseTimeout);
}

/*SD卡擦除测试*/
void SDCard_EraseBlocks(void)
{
	uint32_t BlockAddrStart=0;
	uint32_t BlockAddrEnd=10;
	
	printf("\r\n*** Erasing blocks ***\r\n");

	if(HAL_SD_Erase(&hsd, BlockAddrStart, BlockAddrEnd)==HAL_OK)
		printf("Erasing blocks,OK\r\n");
	else
		printf("Erasing blocks,fail\r\n");

	HAL_SD_CardStateTypeDef cardState=HAL_SD_GetCardState(&hsd);
	printf("GetCardState()= %d\r\n", cardState);

	while(cardState != HAL_SD_CARD_TRANSFER)
	{
		HAL_Delay(1);
		cardState=HAL_SD_GetCardState(&hsd);
	}
	printf("Blocks 0-10 is erased.\r\n");
}

/*SD卡写入测试函数*/
void SDCard_TestWrite(void)
{
	printf("\r\n*** Writing blocks ***\r\n");

	uint8_t pData[BLOCKSIZE]="Hello, welcome to UPC\0";  
	uint32_t BlockAddr=5; 	
	uint32_t BlockCount=1; 
	uint32_t TimeOut=1000;	

	if(HAL_SD_WriteBlocks(&hsd,pData,BlockAddr,BlockCount,TimeOut)==HAL_OK)
	{
		printf("Write to block 5, OK\r\n");
		printf("The string is: %s\r\n", pData);
	}
	else
	{
		printf("Write to block 5, fail ***\r\n");
		return;
	}

	for(uint16_t i=0;i<BLOCKSIZE; i++)
		pData[i]=i; 	

	BlockAddr=6;
	if(HAL_SD_WriteBlocks(&hsd,pData,BlockAddr,BlockCount,TimeOut)==HAL_OK) 
	{
		printf("Write block 6, OK\r\n");
		printf("Data in [10:15] is: ");
		for (uint16_t j=11; j<=15;j++)
		{
			printf("%d,", pData[j]);
		}
		printf("\r\n");
	}
	else
		printf("Write to block 6, fail ***\r\n");
}

/*SD卡读取测试函数*/
void SDCard_TestRead(void)	
{
	printf("\r\n*** Reading blocks ***\r\n");

	uint8_t pData[BLOCKSIZE];
	uint32_t BlockAddr=5;
	uint32_t BlockCount=1;
	uint32_t TimeOut=1000;

	if(HAL_SD_ReadBlocks(&hsd,pData,BlockAddr,BlockCount,TimeOut) == HAL_OK)
	{
		printf("Read block 5, OK\r\n");
		printf("The string is: %s\r\n", pData);
	}
	else
	{
		printf("Read block 5, fail ***\r\n");
		return;
	}

	BlockAddr=6;
	if(HAL_SD_ReadBlocks(&hsd,pData,BlockAddr,BlockCount,TimeOut) == HAL_OK)
	{
		printf("Read block 6, OK\r\n");
		printf("Data in [10:15] is: ");

		for (uint16_t j=11; j<=15;j++)
		{
			printf("%d,", pData[j]);
		}
		printf("\r\n");
	}
}

/*SD卡中断写入测试函数*/
void SDCard_TestWrite_IT(void)
{
	printf("\r\n*** IT Writing blocks ***\r\n");
	uint32_t BlockCount=1; 
	uint16_t BlockAddr=5;
	
	HAL_SD_WriteBlocks_IT(&hsd, TX, BlockAddr, BlockCount);
}

/*SD卡中断读取测试函数*/
void SDCard_TestRead_IT(void)	
{
	printf("\r\n*** IT Reading blocks ***\r\n");
	uint32_t BlockCount=1;
	uint16_t BlockAddr=5;
	
	HAL_SD_ReadBlocks_IT(&hsd, RX, BlockAddr, BlockCount);
}

/*SD卡DMA写入测试函数*/
void SDCard_TestWrite_DMA(void)
{
	printf("\r\n*** DMA Writing blocks ***\r\n");
	uint32_t BlockCount=1; 
	uint16_t BlockAddr=6;
	
	for(uint16_t i=0;i<BLOCKSIZE; i++)
		TX[i]=i; 
	
	HAL_SD_WriteBlocks_DMA(&hsd, TX, BlockAddr, BlockCount);
}

/*SD卡DMA读取测试函数*/
void SDCard_TestRead_DMA(void)	
{
	printf("\r\n*** DMA Reading blocks ***\r\n");
	uint32_t BlockCount=1;
	uint16_t BlockAddr=6;
	
	HAL_SD_ReadBlocks_DMA(&hsd, RX, BlockAddr, BlockCount);
}

/*SD Tx传输完成回调*/
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
//	printf("IT Write to block 5, OK\r\n");
//	printf("The string is: %s\r\n", TX);
		printf("DMA Write to block 6, OK\r\n");
		printf("Data in [10:15] is: ");

		for (uint16_t j=10; j<=15;j++)
		{
			printf("%d,", TX[j]);
		}
		printf("\r\n");
}

/*SD Rx传输完成回调*/
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
//		printf("IT Read block 5, OK\r\n");
//		printf("The string is: %s\r\n", RX);
		printf("DMA Read block 6, OK\r\n");
		printf("Data in [10:15] is: ");

		for (uint16_t j=10; j<=15;j++)
		{
			printf("%d,", RX[j]);
		}
		printf("\r\n");
}

/* USER CODE END 1 */
