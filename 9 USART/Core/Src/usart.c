/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
uint8_t	rxBuffer[3]="#0;";	//数据接收缓冲区
uint8_t proBuffer[3]="#1;";	//数据处理缓冲区
uint8_t	rxCompleted=RESET;	//数据接收完成标志
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/*串口结束传输中断*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    printf("Into HAL_UART_TxCpltCallback Function\r\n");
    HAL_GPIO_TogglePin(RED_LED_GPIO_Port,RED_LED_Pin);

}
/*串口接收完成中断*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        //接收到固定长度数据后使能UART_IT_IDLE中断，在UART_IT_IDLE中断里再次接收
        //接收完成标志
        rxCompleted=SET;	
        //复制接收到的数据到缓冲区
        for(uint16_t i=0;i<RX_CMD_LEN;i++)
            proBuffer[i] = rxBuffer[i];

        //接收到数据后才开启IDLE中断
        __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE); 

    }

}
/*串口空闲回调函数*/
void on_UART_IDLE(UART_HandleTypeDef *huart)
{
    //判断IDLE中断是否被开启
    if(__HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE) == RESET) 
        return;

    //清除IDLE标志
    __HAL_UART_CLEAR_IDLEFLAG(huart); 	
    //禁止IDLE中断
    __HAL_UART_DISABLE_IT(huart, UART_IT_IDLE); 	
    //接收完成
    if(rxCompleted)	
    {
        //上传接收到的指令
        printf("Receive CMD is %s\r\n",proBuffer);
        //处理指令
        CMD_PROCESS();
        //再次接收
        rxCompleted = RESET;
        //再次启动串口接收
        HAL_UART_Receive_IT(huart, rxBuffer, RX_CMD_LEN); 
    }
}
/*接收命令处理函数*/
void CMD_PROCESS(void)
{
    //非法的命令格式
    if(proBuffer[0] != '#' && proBuffer[2] != ';')
    {
        printf("Unlawful Orders\r\n");
        return;

    }
    //解析命令
    uint8_t	CMD = proBuffer[1]-0x30;
    //控制GREEN_LED
    if(CMD == 1)
    {
        HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
        printf("GREEN_LED ON\r\n");
    }
    else if(CMD == 0) 
    {
        HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
        printf("GREEN_LED OFF\r\n");
    }
}
/* USER CODE END 1 */
