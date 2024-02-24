/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sdio.h
  * @brief   This file contains all the function prototypes for
  *          the sdio.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SDIO_H__
#define __SDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SD_HandleTypeDef hsd;

/* USER CODE BEGIN Private defines */
void SDCard_TestRead(void);
void SDCard_TestWrite(void);
void SDCard_ShowInfo(void);
void SDCard_EraseBlocks(void);
void SDCard_ShowCSD(void);
void SDCard_ShowCID(void);
void SDCard_TestWrite_IT(void);
void SDCard_TestRead_IT(void);
void SDCard_TestWrite_DMA(void);
void SDCard_TestRead_DMA(void);
/* USER CODE END Private defines */

void MX_SDIO_SD_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SDIO_H__ */

