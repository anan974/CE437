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
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "CAN_Handler.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
char buffer2[100];

#define SFORWARD 	1
#define SBACKWARD 	0

#define FAST 1
#define SLOW 0

uint8_t LEFT[] 		= { 1, 0, 0, 0, 0, 0, 0, 0 };
uint8_t RIGHT[] 	= { 2, 0, 0, 0, 0, 0, 0, 0 };
uint8_t FORWARD[] 	= { 3, 0, 0, 0, 0, 0, 0, 0 };
uint8_t BACKWARD[] 	= { 4, 0, 0, 0, 0, 0, 0, 0 };
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

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
uint8_t recv = 0, temp = 7, temp2 = 7;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);
	temp = RxData[0];
	temp2 = RxData[1];
	recv = 1;

}
//


//Motor
void Start_Motor();
void genPwm(TIM_HandleTypeDef *htim, uint32_t channel, float duty_cycle);
void run(uint8_t speed, uint8_t mode);
void stop();

//Servo

uint16_t Angle(double angle);
void To_Default();
void Start_Servo();
void Turn_Left();
void Turn_Right();


//Decoder temp to drive car
void DriveCar()
{
	switch(temp)
	{
	case 1:
	{
		run(31, SFORWARD);
		Turn_Left();
		HAL_Delay(200);
		CAN_Transmit(&hcan, &TxHeader, LEFT);
		break;
	}
	case 2:
	{
		run(31, SFORWARD);
		Turn_Right();
		HAL_Delay(200);
		CAN_Transmit(&hcan, &TxHeader, RIGHT);
		break;
	}
	case 3:
	{
		run(31, SFORWARD);
		if (temp2 == 1)
		{
		Turn_Left();
		}
		else if (temp2 == 2)
		{
		Turn_Right();
		}
		HAL_Delay(200);
		CAN_Transmit(&hcan, &TxHeader, FORWARD);
		break;
	}
	case 4:
	{
		if (temp2 == 1)
		{
			Turn_Left();
		}
		else
		{
			Turn_Right();
		}
		run(31, SBACKWARD);
		CAN_Transmit(&hcan, &TxHeader, BACKWARD);
		HAL_Delay(1500);
		break;
	}

	}
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */
	Start_Motor();
	Start_Servo();
	CAN_FilterInit_SingleFF0(&hcan, &canfilterconfig, SENSOR_ADDR);
	CAN_ComInit_Std(&TxHeader, &hcan, ACTUATOR_ADDR, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	run(35, SFORWARD);
	while (1)
	{
		if (recv == 1) {
		DriveCar();
			recv = 0;
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
//Motor
void Start_Motor()
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
}


void genPwm(TIM_HandleTypeDef *htim, uint32_t channel, float duty_cycle)
{
	float load_value = (duty_cycle / 100) * htim->Instance->ARR;
	__HAL_TIM_SET_COMPARE(htim, channel, (uint16_t)load_value);
}

void run(uint8_t speed, uint8_t mode)
{
	if (mode == SFORWARD) {
		genPwm(&htim1,TIM_CHANNEL_4, speed);
		genPwm(&htim1,TIM_CHANNEL_1, 0);
	}
	else {
		genPwm(&htim1,TIM_CHANNEL_4, 0);
		genPwm(&htim1,TIM_CHANNEL_1, speed);
	}
}


void stop()
{
	genPwm(&htim1,TIM_CHANNEL_4, 0);
	genPwm(&htim1,TIM_CHANNEL_1, 0);
	HAL_Delay(200);
}

//Servo

uint16_t Angle(double angle)
{
	double temp = 250 + angle * 5.56;
	return (int)temp;
}

void To_Default()
{
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1, Angle(90));
}

void Start_Servo()
{
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	To_Default();
	HAL_Delay(100);
}

void Turn_Left()
{
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1, Angle(48));
}

void Turn_Right()
{
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1, Angle(136));
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
