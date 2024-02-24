/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* USER CODE BEGIN 0 */
#include "w25flash.h"
#include "string.h"
#include "stdio.h"
/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PB3     ------> SPI1_SCK
    PB4     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI1 interrupt Init */
    HAL_NVIC_SetPriority(SPI1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);
  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PB3     ------> SPI1_SCK
    PB4     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);

    /* SPI1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SPI1_IRQn);
  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//����д��Page0��Page1
uint8_t num = 0;
//ע�⣺һ��Pageд��֮ǰ�����Ǳ��������ģ�д��֮��Ͳ������ظ�д
void Flash_TestWrite(void)
{
	uint8_t blobkNo = 0;
	uint16_t sectorNo = 0;
	uint16_t pageNo = 0;
	uint32_t memAddress = 0;
	
	printf("---------------------\r\n");
	//д��Page0�����ַ���
	memAddress = Flash_Addr_byBlockSectorPage(blobkNo, sectorNo, pageNo);		//Page0�ĵ�ַ
	uint8_t	bufStr1[] = "Hello from beginning";
	uint16_t len = 1 + strlen("Hello from beginning"); 											//����������'\0'
	Flash_WriteInPage(memAddress, bufStr1, len);   													//��Page0����ʼλ��д������
	printf("Write in Page0:0\r\n%s\r\n", bufStr1);

	uint8_t	bufStr2[]="Hello in page";
	len = 1 + strlen("Hello in page"); 																			//����������'\0'
	Flash_WriteInPage(memAddress+100, bufStr2, len);   											//Page0��ƫ��100
	printf("Write in Page0:100\r\n%s\r\n", bufStr2);

	//д��Page1��0-255����
	uint8_t	bufPage[FLASH_PAGE_SIZE];																				//EN25Q_PAGE_SIZE=256
	for (uint16_t i=0;i<FLASH_PAGE_SIZE;i++)
		bufPage[i] = i+num;																												//׼������
	num++;
	pageNo = 1; 																														//Page 1
	memAddress = Flash_Addr_byBlockSectorPage(blobkNo, sectorNo, pageNo);		//page1�ĵ�ַ
	Flash_WriteInPage(memAddress, bufPage, FLASH_PAGE_SIZE);   							//дһ��Page
	printf("Write 0-255 in Page1\r\n");
	printf("---------------------\r\n");
}

//���Զ�ȡPage0 �� Page1������
void Flash_TestRead(void)
{
	uint8_t blobkNo=0;
	uint16_t sectorNo=0;
	uint16_t pageNo=0;
	
	printf("---------------------\r\n");
	//��ȡPage0
	uint8_t bufStr[50];																											//Page0����������
	uint32_t memAddress = Flash_Addr_byBlockSectorPage(blobkNo, sectorNo,pageNo);
	Flash_ReadBytes(memAddress, bufStr, 50);																//��ȡ50���ַ�
	printf("Read from Page0:0\r\n%s\r\n",bufStr);

	Flash_ReadBytes(memAddress+100, bufStr, 50);														//��ַƫ��100���50�����ֽ�
	printf("Read from Page0:100\r\n%s\r\n",bufStr);

	//��ȡPage1
	uint8_t	randData = 0;
	pageNo = 1;
	memAddress = Flash_Addr_byBlockSectorPage(blobkNo, sectorNo,pageNo);

	randData = Flash_ReadOneByte(memAddress+12);														//��ȡ1���ֽ����ݣ�ҳ�ڵ�ַƫ��12
	printf("Page1[12] = %d\r\n",randData);

	randData = Flash_ReadOneByte(memAddress+136);														//ҳ�ڵ�ַƫ��136
	printf("Page1[136] = %d\r\n",randData);

	randData = Flash_ReadOneByte(memAddress+210);														//ҳ�ڵ�ַƫ��210
	printf("Page1[210] = %d\r\n",randData);
	printf("---------------------\r\n");
}
/* USER CODE END 1 */
