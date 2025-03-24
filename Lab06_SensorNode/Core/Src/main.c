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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "VL53L1_Handler.h"
#include "CAN_Handler.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
VL53L1X sensor1, sensor2;
char lcd[100];
uint8_t LEFT[] 			 = { 1, 0, 0, 0, 0, 0, 0, 0 };
uint8_t RIGHT[] 		 = { 2, 0, 0, 0, 0, 0, 0, 0 };
uint8_t FORWARD[] 		 = { 3, 0, 0, 0, 0, 0, 0, 0 };
uint8_t BACKWARD_LEFT[]	 = { 4, 1, 0, 0, 0, 0, 0, 0 };
uint8_t BACKWARD_RIGHT[] = { 4, 2, 0, 0, 0, 0, 0, 0 };
uint8_t FORWARD_LEFT[]   = { 3, 1, 0, 0, 0, 0, 0, 0 };
uint8_t FORWARD_RIGHT[]  = { 3, 2, 0, 0, 0, 0, 0, 0 };

uint8_t returnSignal = 0;

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
uint8_t state = 0, prevstate = 0, forceChange = 0;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);
	returnSignal = RxData[0];
}

uint8_t Get_State(int d1, int d2, uint8_t *forceChange, uint8_t state);
uint8_t Get_CounterState(uint8_t state);

void SentDatatoAct()
{
	switch (state) {
			case 1:
				forceChange = 0;
				prevstate = state;
				while (returnSignal != 1) {
					CAN_Transmit(&hcan, &TxHeader, LEFT);
				}
				break;
			case 2:
				forceChange = 0;
				prevstate = state;
				while (returnSignal != 2) {
					CAN_Transmit(&hcan, &TxHeader, RIGHT);
				}
				break;
			case 3:
				forceChange = 0;
				CAN_Transmit(&hcan, &TxHeader, FORWARD);
				break;
			case 4:
				CAN_Transmit(&hcan, &TxHeader, BACKWARD_LEFT);
				forceChange = 4;
				break;
			case 5:
				CAN_Transmit(&hcan, &TxHeader, BACKWARD_RIGHT);
				forceChange = 5;
				break;
			case 8:
				sprintf(lcd, "\n\rSensor 1\n\r  D1: %dmm\n\n\rSensor 2\n\r  D2: %dmm", sensor1.distance, sensor2.distance);
				Print(lcd, 200);
				break;
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
	MX_I2C1_Init();
	MX_USART1_UART_Init();
	MX_CAN_Init();
	MX_TIM3_Init();
	/* USER CODE BEGIN 2 */
	TOF_InitStruct(&sensor1, &hi2c1, 0x22, XSHUT1_GPIO_Port, XSHUT1_Pin);
	TOF_InitStruct(&sensor2, &hi2c1, 0x26, XSHUT2_GPIO_Port, XSHUT2_Pin);
	int status = VL53L1_BootDualSensors(&sensor1, &sensor2);
	if (VL53L1_BootDualSensors(&sensor1, &sensor2) != 0) {
		VL53L1_Error_Hanlder(status);
	}
	CAN_FilterInit_SingleFF0(&hcan, &canfilterconfig, ACTUATOR_ADDR);
	CAN_ComInit_Std(&TxHeader, &hcan, SENSOR_ADDR, 2);
	//HAL_TIM_Base_Start_IT(&htim3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		if (forceChange != 0) {
			if (forceChange == 5) {
				state = 1;
			} else {
				state = 2;
			}
			forceChange = 0;
		} else {
			VL53L1_GetDistance(&sensor1);
			VL53L1_GetDistance(&sensor2);
			if (sensor2.distance >= 55) {
				sensor2.distance += 50;
			}

			state = Get_State(sensor1.distance, sensor2.distance, 0, prevstate);
		}
		SentDatatoAct();
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
uint8_t Get_State(int distance1, int distance2, uint8_t *forceChange, uint8_t state)
{
	if (*forceChange == 0) {
		if (distance2 < distance1) {
			if (distance2 <= 100)
				return 5;
			else if (distance2 <= 900)
				return 1;
		}
		else if (distance1 < distance2) {
			if (distance1 <= 100)
				return 5;
			else if (distance1 <= 900)
				return 2;
		}

	}
	else if (*forceChange == 1) {
		*forceChange = 0;
		return Get_CounterState(state);
	}
	return 3;
}

uint8_t Get_CounterState(uint8_t state)
{
	if (state == 2) {
		return 1;
	} else if (state == 1) {
		return 2;
	} else {
		return 1;
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
