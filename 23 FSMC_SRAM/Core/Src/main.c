/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "dma.h"
#include "rng.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SRAM_ADDR_BEGIN		0x68000000UL //Bank1子区3的 SRAM起始地址
#define SRAM_ADDR_HALF		0x68080000UL //SRAM中间地址 512K字节
#define SRAM_ADDR_END			0x680FFFFFUL //SRAM结束地址 1024K字节
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void SRAM_ReadByPointer(void);
void SRAM_WriteByPointer(void);
void SRAM_ReadByFunc(void);
void SRAM_WriteByFunc(void);
void SRAM_WriteDMA(void);
void SRAM_ReadDMA(void);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define COUNT	5 //缓存区数据个数
uint32_t txBuffer[COUNT]; //DMA发送缓存区
uint32_t rxBuffer[COUNT]; //DMA接收缓存区
uint8_t DMA_Direction = 1; //DMA传输方向，1=write, 0=read
uint8_t DMA_Busy = 0; //DMA工作状态，1=busy, 0=idle
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  MX_RNG_Init();
  /* USER CODE BEGIN 2 */
	printf("Reset!\r\n");
	//将外设与DMA流关联
	__HAL_LINKDMA(&hsram3, hdma, hdma_memtomem_dma2_stream0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		/*WK_UP被按下*/
		if(HAL_GPIO_ReadPin(WK_UP_GPIO_Port,WK_UP_Pin) == GPIO_PIN_SET)
		{
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(WK_UP_GPIO_Port,WK_UP_Pin) == GPIO_PIN_SET)
			{
				SRAM_WriteDMA();
				while(HAL_GPIO_ReadPin(WK_UP_GPIO_Port,WK_UP_Pin));
			}
		}
		
		/*KEY2被按下*/
		if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin) == GPIO_PIN_RESET)
		{
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin) == GPIO_PIN_RESET)
			{
				SRAM_ReadDMA();
				while(!HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin));
			}
		}
		
		/*KEY1被按下*/
		if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin) == GPIO_PIN_RESET)
		{
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin) == GPIO_PIN_RESET)
			{
				//SRAM DMA写测试函数
				SRAM_WriteDMA();
				while(!HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin));
			}
		}
		
		/*KEY0被按下*/
		if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin) == GPIO_PIN_RESET)
		{
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin) == GPIO_PIN_RESET)
			{
				//SRAM DMA读测试函数
				SRAM_ReadDMA();
				while(!HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin));
			}
		}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/*用HAL函数写入数据*/
void SRAM_WriteByFunc(void)
{
	//1.写入字符串
	uint32_t *pAddr = (uint32_t *)(SRAM_ADDR_BEGIN);			//给指针赋值
	uint8_t strIn[] = "Moment in UPC";
	uint16_t dataLen = sizeof(strIn); 										//数据长度，字节数，包括最后的结束符'\0’
	if(HAL_SRAM_Write_8b(&hsram3, pAddr, strIn, dataLen) == HAL_OK)
	{
		printf("Write string at 0x6800 0000:");
		printf("%s\r\n",strIn);
	}

	//2.写入一个随机数
	uint32_t	num=0;
	pAddr=(uint32_t *)(SRAM_ADDR_BEGIN+256);							//指针重新赋值
	HAL_RNG_GenerateRandomNumber(&hrng, &num);						//产生32位随机数
	if(HAL_SRAM_Write_32b(&hsram3, pAddr, &num, 1) == HAL_OK)
	{
		printf("Write 32b number at 0x6800 0100");
		printf("0x%x\r\n", num);	
	}
	
	printf("-----------------------------------------\r\n");
}

/*用HAL函数读取数据*/
void SRAM_ReadByFunc(void)
{
	//1.读取字符串
	uint32_t *pAddr = (uint32_t *)(SRAM_ADDR_BEGIN);			//给指针赋值
	uint8_t strOut[30];
	uint16_t dataLen = 30;
	if(HAL_SRAM_Read_8b(&hsram3, pAddr, strOut, dataLen) == HAL_OK)
	{
		printf("Read string at 0x6800 0000:");
		printf("%s\r\n", strOut); 
	}

	//2.读取一个uint32_t数
	uint32_t	num=0;
	pAddr=(uint32_t *)(SRAM_ADDR_BEGIN+256);							//指针重新赋值,指向一个新的地址
	if(HAL_SRAM_Read_32b(&hsram3, pAddr, &num, 1) == HAL_OK)
	{
		printf("Read 32b number at 0x6800 0100:");
		printf("0x%x\r\n", num);
	}
	
	printf("-----------------------------------------\r\n");
}

/*以DMA方式写入数据*/
void SRAM_WriteDMA(void)													
{
    printf("Write 32bit array by DMA:");
    uint32_t Value=3000;	
    for(uint8_t i=0; i<COUNT; i++)
    {
        txBuffer[i] = Value;
        printf("%d,",Value);
        Value += 6;

    }
    printf("\r\n");
    //DMA传输方向，1=write, 0=read
    DMA_Direction=1;					
    //表示DMA正在传输，1=working, 0=idle	
    DMA_Busy=1;																			
    //给指针赋值
    uint32_t *pAddr_32b=(uint32_t *)(SRAM_ADDR_BEGIN);			
    //DMA方式写入SRAM	
    HAL_SRAM_Write_DMA(&hsram3, pAddr_32b, txBuffer, COUNT);	

}


/*以DMA方式读取数据*/
void SRAM_ReadDMA(void)														
{
	printf("Read 32bit array by DMA\r\n");
	DMA_Direction=0;													
	DMA_Busy=1;			
	
	uint32_t *pAddr_32b=(uint32_t *)(SRAM_ADDR_BEGIN);	
	//以DMA方式读取SRAM
	HAL_SRAM_Read_DMA(&hsram3, pAddr_32b, rxBuffer, COUNT);
}

/*DMA传输完成回调函数*/
void HAL_SRAM_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma)
{
	//DMA传输方向，1=write, 0=read
	if(DMA_Direction) 															
	{
		printf("Written by DMA complete.\r\n");
		printf("-----------------------------------------\r\n");
	}
	else
	{
		printf("Read by DMA complete.");
		for(uint8_t i=0; i<COUNT; i++)
			printf("%d,", rxBuffer[i]);
		printf("\r\n-----------------------------------------\r\n");
	}
	//表示DMA结束了传输，1=working, 0=idle
	DMA_Busy=0;																			
}


/*直接通过指针写数据*/
void	SRAM_WriteByPointer(void)
{
	printf("Write five uint16_t numbers");
	printf("start from 0x6808 0000");

	uint16_t num = 1000;
	uint16_t *pAddr_16b = (uint16_t *)(SRAM_ADDR_HALF);	//uint16_t 类型数据指针
	for(uint8_t i=0; i<5; i++) 	//连续写入5个16位整数
	{
		num += 5;
		*pAddr_16b = num;	//直接向指针所指的地址写入数据
		pAddr_16b ++;		//++一次，地址加2, 因为是uint16_t类型
		printf("%d\r\n", num);
	}
}

/*直接通过指针读取数据*/
void	SRAM_ReadByPointer(void)
{
	printf("Read five uint16_t numbers");
	printf("start from 0x6808 0000");

	uint16_t num=0;
	uint16_t *pAddr_16b = (uint16_t *)(SRAM_ADDR_HALF);	  //uint16_t 类型数据指针
	for(uint8_t i=0; i<5; i++)
	{
		num = *pAddr_16b;		//直接从指针所指的地址读数
		pAddr_16b ++;		//++一次，地址加2, 因为是uint16_t类型
		printf("%d\r\n", num);
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
