/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "wave_form.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CLK_INTERNO 84000000
#define PSC_TIM10 84
#define SERIAL_TIMEOUT 1000
#define MSG_SIZE 200

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

int newARR;

int buttonStates[7];
const int notas[7] = {264, 296, 332, 352, 396, 444, 498};

int waveOption, lastWave, optionInput;
char msg[MSG_SIZE];

int waveCounter;
int16_t count;
int16_t position;

uint8_t i, j;
uint8_t desligar;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	waveCounter = __HAL_TIM_GET_COUNTER(&htim2);
}

int CalculaARR_TIM10 (int freq_nota)
{
	int ARR_TIM10 = CLK_INTERNO / PSC_TIM10 / freq_nota / SIZE;

	return ARR_TIM10 - 1;
}
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
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_TIM10_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim10);
  HAL_UART_Receive_IT(&huart2, &optionInput, 1);

  HAL_TIM_Encoder_Start_IT(&htim2, TIM_CHANNEL_ALL);
  /* Infinite loop */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  waveCounter = waveCounter;

      buttonStates[0] = HAL_GPIO_ReadPin(bt0_GPIO_Port, bt0_Pin) == GPIO_PIN_SET;
      buttonStates[1] = HAL_GPIO_ReadPin(bt1_GPIO_Port, bt1_Pin) == GPIO_PIN_SET;
      buttonStates[2] = HAL_GPIO_ReadPin(bt2_GPIO_Port, bt2_Pin) == GPIO_PIN_SET;
      buttonStates[3] = HAL_GPIO_ReadPin(bt3_GPIO_Port, bt3_Pin) == GPIO_PIN_SET;
      buttonStates[4] = HAL_GPIO_ReadPin(bt4_GPIO_Port, bt4_Pin) == GPIO_PIN_SET;
      buttonStates[5] = HAL_GPIO_ReadPin(bt5_GPIO_Port, bt5_Pin) == GPIO_PIN_SET;
      buttonStates[6] = HAL_GPIO_ReadPin(bt6_GPIO_Port, bt6_Pin) == GPIO_PIN_SET;
      /*
      buttonStates[7] = HAL_GPIO_ReadPin(bt7_GPIO_Port, bt7_Pin) == GPIO_PIN_SET;
      buttonStates[8] = HAL_GPIO_ReadPin(bt8_GPIO_Port, bt8_Pin) == GPIO_PIN_SET;
      buttonStates[9] = HAL_GPIO_ReadPin(bt9_GPIO_Port, bt9_Pin) == GPIO_PIN_SET;
      buttonStates[10] = HAL_GPIO_ReadPin(bt10_GPIO_Port, bt10_Pin) == GPIO_PIN_SET;
      buttonStates[11] = HAL_GPIO_ReadPin(bt11_GPIO_Port, bt11_Pin) == GPIO_PIN_SET;
      */

      for (j = 0; j <= 6; j++)
      {
          if (buttonStates[j])
          {
              // #CALCULA E SETA ARR
        	  newARR = CalculaARR_TIM10(notas[j]);
        	  __HAL_TIM_SET_AUTORELOAD(&htim10, newARR);

        	  // #HABILITA TIMERS
        	  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
        	  HAL_TIM_Base_Start_IT(&htim10);

          } else if((buttonStates[0]||buttonStates[1]||buttonStates[2]||buttonStates[3]||buttonStates[4]||buttonStates[5]||buttonStates[6])!=1)
          {
        	  HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
        	  HAL_TIM_Base_Stop_IT(&htim10);
          }
      }

      waveOption = waveCounter/5;
      switch(waveOption)
      {
      	  case SINE:
      		  sprintf(msg, "\n\rSINE");
      		  break;
      	  case SQUARE:
      		  sprintf(msg, "\n\rSQUARE");
      		  break;
      	  case TRIANGLE:
      		  sprintf(msg, "\n\rTRIANGLE");
      		  break;
      	  case SAW:
      		  sprintf(msg, "\n\rSAW");
      		  break;
      	  case FLUTE:
      		  sprintf(msg, "\n\rFLUTE");
      		  break;
      }

      if(waveOption!=lastWave)
      {
    	  HAL_UART_Transmit(&huart2, msg, strlen(msg), SERIAL_TIMEOUT);
    	  msg[0] = '\0';
    	  lastWave=waveOption;
      }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM10)
	{
		if(htim->Channel==TIM_CHANNEL_1)
		{
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, wave[waveOption][i]);
			i++;
			if(i>=SIZE)
			{
				i=0;
			}
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

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
