/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
static CAN_TxHeaderTypeDef TxMessage; //CAN发送的消息的消息头
static CAN_RxHeaderTypeDef RxMessage; //CAN接收的消息的消息头
uint32_t TxMailbox;
/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 5;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = ENABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//设置筛选器,要在完成CAN初始化之后调用此函数
HAL_StatusTypeDef	CAN_SetFilters(void)
{
	CAN_FilterTypeDef	canFilter;  //筛选器结构体变量

 // Configure the CAN Filter
	canFilter.FilterBank = 0;		//筛选器组编号
	canFilter.FilterMode = CAN_FILTERMODE_IDMASK;	//ID掩码模式
	canFilter.FilterScale = CAN_FILTERSCALE_32BIT;	//32位长度

//设置1：接收所有帧
//	canFilter.FilterIdHigh = 0x0000;		//CAN_FxR1 的高16位
//	canFilter.FilterIdLow = 0x0000;			//CAN_FxR1 的低16位
//	canFilter.FilterMaskIdHigh = 0x0000;	//CAN_FxR2的高16位。所有位任意
//	canFilter.FilterMaskIdLow = 0x0000;		//CAN_FxR2的低16位，所有位任意

//设置2：只接收stdID为奇数的帧
	canFilter.FilterIdHigh = 0x0020;		//CAN_FxR1 的高16位
	canFilter.FilterIdLow = 0x0000;			//CAN_FxR1 的低16位
	canFilter.FilterMaskIdHigh = 0x0020;	//CAN_FxR2的高16位
	canFilter.FilterMaskIdLow = 0x0000;		//CAN_FxR2的低16位


	canFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;		//应用于FIFO0
	canFilter.FilterActivation = ENABLE;		//使用筛选器
	canFilter.SlaveStartFilterBank = 14;		//从CAN控制器筛选器起始的Bank

	HAL_StatusTypeDef result=HAL_CAN_ConfigFilter(&hcan1, &canFilter);
	return result;
}

/*CAN发送数据测试函数*/
void CAN1_Send_Test(uint32_t msgid, uint8_t *data)
{
	TxMessage.IDE = CAN_ID_STD;     //设置ID类型
	TxMessage.StdId = msgid;        //设置ID号
	TxMessage.RTR = CAN_RTR_DATA;   //设置传送数据帧
	TxMessage.DLC = 4;              //设置数据长度
	
	if(HAL_CAN_AddTxMessage(&hcan1, &TxMessage, data, &TxMailbox) != HAL_OK) 
	{
		printf("CAN send test data fail!\r\n");
		Error_Handler();
	}
	//必须延时要不然进不去发送完成回调函数
	//HAL_Delay(1);
	//printf("CAN send test data success!\r\n");
}

/*CAN接收中断处理函数*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	uint8_t  data[8];
	HAL_StatusTypeDef status;
	if(hcan == &hcan1) 
	{
		status = HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxMessage, data);
		if (HAL_OK == status)
		{                             
				printf("--->Data Receieve!\r\n");
				printf("RxMessage.StdId is %#x\r\n", RxMessage.StdId);
				printf("data[0] is 0x%02x\r\n", data[0]);
				printf("data[1] is 0x%02x\r\n", data[1]);
				printf("data[2] is 0x%02x\r\n", data[2]);
				printf("data[3] is 0x%02x\r\n", data[3]);
				printf("<---\r\n");
		}
	}
}
/*CAN发送完成中断处理函数*/
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	printf("--->Into TxMailbox0CompleteCallback Function!\r\n");
	printf("--->CAN send test data success!\r\n\r\n");
}
/* USER CODE END 1 */
