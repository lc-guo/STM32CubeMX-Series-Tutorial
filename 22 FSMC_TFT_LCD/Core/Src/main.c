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
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tftlcd.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t lcd_id[12]; 
void LCD_TEST(void);
void testlines(void);
void testdrawrects(void);
void testfillcircles(uint8_t radius);
void tftPrintTest(void);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
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
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

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
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	printf("Reset\r\n");
	HAL_Delay(50);
	LCD_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		LCD_TEST();
		HAL_Delay(4000);
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
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
/*LCD≤‚ ‘∫Ø ˝*/
void LCD_TEST(void)
{
    /*ª≠œﬂ≤‚ ‘*/
    testlines();
    HAL_Delay(1000);
    /*ª≠’˝∑Ω–Œ≤‚ ‘*/
    testdrawrects();
    HAL_Delay(1000);
    /*ª≠‘≤≤‚ ‘*/
    testfillcircles(15);
    HAL_Delay(1000);
    /*◊÷ÃÂ≤‚ ‘*/
    tftPrintTest();

}
/*ª≠œﬂ≤‚ ‘*/
void testlines(void) 
{
    LCD_Clear(WHITE);
    POINT_COLOR=BLACK;
    for (int16_t x=0; x < SSD_VER_RESOLUTION; x+=6) 
    {
        LCD_DrawLine(0, 0, x, SSD_HOR_RESOLUTION-1);
        HAL_Delay(3);

    }
    for (int16_t y=0; y < SSD_HOR_RESOLUTION; y+=6) 
    {
        LCD_DrawLine(0, 0, SSD_VER_RESOLUTION-1, y);
        HAL_Delay(3);

    }
    LCD_Clear(WHITE);
    POINT_COLOR=BLUE;
    for (int16_t x=0; x < SSD_VER_RESOLUTION; x+=6)
    {
        LCD_DrawLine(SSD_VER_RESOLUTION-1, 0, x, SSD_HOR_RESOLUTION-1);
        HAL_Delay(3);

    }
    for (int16_t y=0; y < SSD_HOR_RESOLUTION; y+=6)
    {
        LCD_DrawLine(SSD_VER_RESOLUTION-1, 0, 0, y);
        HAL_Delay(3);

    }
    LCD_Clear(WHITE);
    POINT_COLOR=RED;
    for (int16_t y=0; y < SSD_HOR_RESOLUTION; y+=6) 
    {
        LCD_DrawLine(0, SSD_HOR_RESOLUTION-1, SSD_VER_RESOLUTION-1, y);
        HAL_Delay(3);

    }
    for (int16_t x=0; x < SSD_VER_RESOLUTION; x+=6) 
    {
        LCD_DrawLine(0, SSD_HOR_RESOLUTION-1, x, 0);
        HAL_Delay(3);

    }
    LCD_Clear(WHITE);
    POINT_COLOR=MAGENTA;
    for (int16_t x=0; x < SSD_VER_RESOLUTION; x+=6) 
    {
        LCD_DrawLine(SSD_VER_RESOLUTION-1, SSD_HOR_RESOLUTION-1, x, 0);
        HAL_Delay(3);

    }
    for (int16_t y=0; y < SSD_HOR_RESOLUTION; y+=6)
    {
        LCD_DrawLine(SSD_VER_RESOLUTION-1, SSD_HOR_RESOLUTION-1, 0, y);
        HAL_Delay(3);

    }

}
/*ª≠’˝∑Ω–Œ≤‚ ‘*/
void testdrawrects(void) 
{
    LCD_Clear(WHITE);
    POINT_COLOR=BLACK;
    for (int16_t x=0; x < SSD_VER_RESOLUTION; x+=6)
    {
        LCD_DrawRectangle(SSD_VER_RESOLUTION/2 -x/2, SSD_HOR_RESOLUTION/2 -x/2 , x, x);

    }

}
/*ª≠‘≤≤‚ ‘*/
void testfillcircles(uint8_t radius) 
{
    LCD_Clear(WHITE);
    POINT_COLOR=BLUE;
    for(int16_t x=radius; x < SSD_VER_RESOLUTION; x+=radius*2)
    {
        for(int16_t y=radius; y < SSD_HOR_RESOLUTION; y+=radius*2)
        {
            LCD_Draw_Circle(x, y, radius);

        }

    }
    HAL_Delay(1500);
    LCD_Clear(WHITE);
    POINT_COLOR=MAGENTA;
    radius = radius / 2;
    for(int16_t x=radius; x < SSD_VER_RESOLUTION; x+=radius*2)
    {
        for(int16_t y=radius; y < SSD_HOR_RESOLUTION; y+=radius*2)
        {
            LCD_Draw_Circle(x, y, radius);

        }

    }

}
/*◊÷ÃÂ≤‚ ‘*/
void tftPrintTest(void) 
{
    LCD_Clear(WHITE);
    LCD_SetCursor(0, 30);
    POINT_COLOR=RED;
    LCD_ShowString(0,30,200,16,16,(uint8_t *)"Hello World!");
    POINT_COLOR=YELLOW;
    LCD_ShowString(0,60,200,16,24,(uint8_t *)"Hello World!");
    POINT_COLOR=GREEN;
    LCD_ShowString(0,90,200,16,32,(uint8_t *)"Hello World!");
    POINT_COLOR=BLUE;
    LCD_ShowxNum(0,120,1234,4,24,1);
    HAL_Delay(1500);
    POINT_COLOR=YELLOW;
    LCD_ShowString(0,180,200,16,24,(uint8_t *)"Hello World!");
    POINT_COLOR=GREEN;
    LCD_ShowString(0,210,200,16,32,(uint8_t *)" Want pi?");
    LCD_ShowString(0,240,200,16,32,(uint8_t *)" Print HEX!");
    POINT_COLOR=BLUE;
    LCD_ShowString(0,270,200,16,16,(uint8_t *)"Sketch has been");
    LCD_ShowString(0,300,200,16,24,(uint8_t *)"running for: ");
    POINT_COLOR=MAGENTA;
    LCD_ShowString(0,330,200,16,24,(uint8_t *)" seconds.");

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
