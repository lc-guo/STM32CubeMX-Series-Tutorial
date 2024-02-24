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
#define SRAM_ADDR_BEGIN		0x68000000UL //Bank1����3�� SRAM��ʼ��ַ
#define SRAM_ADDR_HALF		0x68080000UL //SRAM�м��ַ 512K�ֽ�
#define SRAM_ADDR_END			0x680FFFFFUL //SRAM������ַ 1024K�ֽ�
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
#define COUNT	5 //���������ݸ���
uint32_t txBuffer[COUNT]; //DMA���ͻ�����
uint32_t rxBuffer[COUNT]; //DMA���ջ�����
uint8_t DMA_Direction = 1; //DMA���䷽��1=write, 0=read
uint8_t DMA_Busy = 0; //DMA����״̬��1=busy, 0=idle
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
	//��������DMA������
	__HAL_LINKDMA(&hsram3, hdma, hdma_memtomem_dma2_stream0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		/*WK_UP������*/
		if(HAL_GPIO_ReadPin(WK_UP_GPIO_Port,WK_UP_Pin) == GPIO_PIN_SET)
		{
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(WK_UP_GPIO_Port,WK_UP_Pin) == GPIO_PIN_SET)
			{
				SRAM_WriteDMA();
				while(HAL_GPIO_ReadPin(WK_UP_GPIO_Port,WK_UP_Pin));
			}
		}
		
		/*KEY2������*/
		if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin) == GPIO_PIN_RESET)
		{
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin) == GPIO_PIN_RESET)
			{
				SRAM_ReadDMA();
				while(!HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin));
			}
		}
		
		/*KEY1������*/
		if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin) == GPIO_PIN_RESET)
		{
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin) == GPIO_PIN_RESET)
			{
				//SRAM DMAд���Ժ���
				SRAM_WriteDMA();
				while(!HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin));
			}
		}
		
		/*KEY0������*/
		if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin) == GPIO_PIN_RESET)
		{
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin) == GPIO_PIN_RESET)
			{
				//SRAM DMA�����Ժ���
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
/*��HAL����д������*/
void SRAM_WriteByFunc(void)
{
	//1.д���ַ���
	uint32_t *pAddr = (uint32_t *)(SRAM_ADDR_BEGIN);			//��ָ�븳ֵ
	uint8_t strIn[] = "Moment in UPC";
	uint16_t dataLen = sizeof(strIn); 										//���ݳ��ȣ��ֽ������������Ľ�����'\0��
	if(HAL_SRAM_Write_8b(&hsram3, pAddr, strIn, dataLen) == HAL_OK)
	{
		printf("Write string at 0x6800 0000:");
		printf("%s\r\n",strIn);
	}

	//2.д��һ�������
	uint32_t	num=0;
	pAddr=(uint32_t *)(SRAM_ADDR_BEGIN+256);							//ָ�����¸�ֵ
	HAL_RNG_GenerateRandomNumber(&hrng, &num);						//����32λ�����
	if(HAL_SRAM_Write_32b(&hsram3, pAddr, &num, 1) == HAL_OK)
	{
		printf("Write 32b number at 0x6800 0100");
		printf("0x%x\r\n", num);	
	}
	
	printf("-----------------------------------------\r\n");
}

/*��HAL������ȡ����*/
void SRAM_ReadByFunc(void)
{
	//1.��ȡ�ַ���
	uint32_t *pAddr = (uint32_t *)(SRAM_ADDR_BEGIN);			//��ָ�븳ֵ
	uint8_t strOut[30];
	uint16_t dataLen = 30;
	if(HAL_SRAM_Read_8b(&hsram3, pAddr, strOut, dataLen) == HAL_OK)
	{
		printf("Read string at 0x6800 0000:");
		printf("%s\r\n", strOut); 
	}

	//2.��ȡһ��uint32_t��
	uint32_t	num=0;
	pAddr=(uint32_t *)(SRAM_ADDR_BEGIN+256);							//ָ�����¸�ֵ,ָ��һ���µĵ�ַ
	if(HAL_SRAM_Read_32b(&hsram3, pAddr, &num, 1) == HAL_OK)
	{
		printf("Read 32b number at 0x6800 0100:");
		printf("0x%x\r\n", num);
	}
	
	printf("-----------------------------------------\r\n");
}

/*��DMA��ʽд������*/
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
    //DMA���䷽��1=write, 0=read
    DMA_Direction=1;					
    //��ʾDMA���ڴ��䣬1=working, 0=idle	
    DMA_Busy=1;																			
    //��ָ�븳ֵ
    uint32_t *pAddr_32b=(uint32_t *)(SRAM_ADDR_BEGIN);			
    //DMA��ʽд��SRAM	
    HAL_SRAM_Write_DMA(&hsram3, pAddr_32b, txBuffer, COUNT);	

}


/*��DMA��ʽ��ȡ����*/
void SRAM_ReadDMA(void)														
{
	printf("Read 32bit array by DMA\r\n");
	DMA_Direction=0;													
	DMA_Busy=1;			
	
	uint32_t *pAddr_32b=(uint32_t *)(SRAM_ADDR_BEGIN);	
	//��DMA��ʽ��ȡSRAM
	HAL_SRAM_Read_DMA(&hsram3, pAddr_32b, rxBuffer, COUNT);
}

/*DMA������ɻص�����*/
void HAL_SRAM_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma)
{
	//DMA���䷽��1=write, 0=read
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
	//��ʾDMA�����˴��䣬1=working, 0=idle
	DMA_Busy=0;																			
}


/*ֱ��ͨ��ָ��д����*/
void	SRAM_WriteByPointer(void)
{
	printf("Write five uint16_t numbers");
	printf("start from 0x6808 0000");

	uint16_t num = 1000;
	uint16_t *pAddr_16b = (uint16_t *)(SRAM_ADDR_HALF);	//uint16_t ��������ָ��
	for(uint8_t i=0; i<5; i++) 	//����д��5��16λ����
	{
		num += 5;
		*pAddr_16b = num;	//ֱ����ָ����ָ�ĵ�ַд������
		pAddr_16b ++;		//++һ�Σ���ַ��2, ��Ϊ��uint16_t����
		printf("%d\r\n", num);
	}
}

/*ֱ��ͨ��ָ���ȡ����*/
void	SRAM_ReadByPointer(void)
{
	printf("Read five uint16_t numbers");
	printf("start from 0x6808 0000");

	uint16_t num=0;
	uint16_t *pAddr_16b = (uint16_t *)(SRAM_ADDR_HALF);	  //uint16_t ��������ָ��
	for(uint8_t i=0; i<5; i++)
	{
		num = *pAddr_16b;		//ֱ�Ӵ�ָ����ָ�ĵ�ַ����
		pAddr_16b ++;		//++һ�Σ���ַ��2, ��Ϊ��uint16_t����
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
