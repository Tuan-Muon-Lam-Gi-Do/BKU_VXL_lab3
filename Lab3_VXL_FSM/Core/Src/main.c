/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  * opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
// #include "software_timer.h" // --- XÓA ---
#include "button.h"
#include "fsm.h"
#include "scheduler.h" // --- THÊM ---

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_TIM2_Init(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
// --- XÓA --- (Đã chuyển sang fsm.h)
// void display_state(void);
// void fsm_update(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// --- XÓA TOÀN BỘ CÁC HÀM LOGIC ĐÃ CHUYỂN SANG fsm.c ---
// uint8_t led7segBuffer[4];
// const uint8_t SEGMENTS[10] = {...};
// void updateLedBuffer(...) { ... }
// void fsm_update(void) { ... }


// --- GIỮ LẠI CÁC HÀM LÁI LED 7-SEG CẤP THẤP ---
// (Bạn cũng có thể chuyển các hàm này sang fsm.c nếu muốn)

void display7SEG_off(void) {
    HAL_GPIO_WritePin(GPIOA, EN1_Pin|EN2_Pin|EN3_Pin|EN4_Pin, GPIO_PIN_SET);
}


void output7SEG(uint8_t value) {
    HAL_GPIO_WritePin(GPIOB, a_Pin, (value & 0x01) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, b_Pin, (value & 0x02) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, c_Pin, (value & 0x04) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, d_Pin, (value & 0x08) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, e_Pin, (value & 0x10) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, f_Pin, (value & 0x20) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, g_Pin, (value & 0x40) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}


void update7SEG(void) {
    // Lấy buffer từ fsm.c (nếu bạn khai báo nó là extern)
    extern uint8_t led7segBuffer[4];
    static uint8_t digit = 0;

    display7SEG_off();

    switch(digit) {
        case 0: output7SEG(led7segBuffer[0]); HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_RESET); break;
        case 1: output7SEG(led7segBuffer[1]); HAL_GPIO_WritePin(GPIOA, EN2_Pin, GPIO_PIN_RESET); break;
        case 2: output7SEG(led7segBuffer[2]); HAL_GPIO_WritePin(GPIOA, EN3_Pin, GPIO_PIN_RESET); break;
        case 3: output7SEG(led7segBuffer[3]); HAL_GPIO_WritePin(GPIOA, EN4_Pin, GPIO_PIN_RESET); break;
    }

    digit = (digit + 1) % 4;
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
  MX_TIM2_Init();
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);

  // --- THÊM PHẦN KHỞI TẠO SCHEDULER ---
  SCH_Init();
  fsm_init(); // Khởi tạo FSM

  // Tick timer là 10ms (từ MX_TIM2_Init)
  // 1. Task quét nút nhấn: chạy mỗi 10ms (1 tick)
  SCH_Add_Task(getKeyInput, 0, 1);

  // 2. Task quét LED 7-seg: chạy mỗi 10ms (1 tick), offset 1 tick
  // nếu muốn mượt hơn, bạn cần cấu hình TIM2 tick 1ms)
  SCH_Add_Task(update7SEG, 1, 1);

  // 3. Task FSM chính: chạy mỗi 10ms (1 tick), offset 2 ticks
  SCH_Add_Task(fsm_run, 2, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    SCH_Dispatch_Tasks();

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  // (Giữ nguyên)
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{
  // (Giữ nguyên - đây là 10ms tick)
  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  // (Giữ nguyên)
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Red_1_Pin|Amber_1_Pin|Green_1_Pin|Red_2_Pin
                          |Amber_2_Pin|Green_2_Pin|EN1_Pin|EN2_Pin
                          |EN3_Pin|EN4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, a_Pin|b_Pin|c1_Pin|d1_Pin
                          |e1_Pin|f1_Pin|g1_Pin|c_Pin
                          |d_Pin|e_Pin|f_Pin|g_Pin
                          |a1_Pin|b1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BUTTON_1_Pin */
  GPIO_InitStruct.Pin = BUTTON_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BUTTON_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Red_1_Pin Amber_1_Pin Green_1_Pin Red_2_Pin
                           Amber_2_Pin Green_2_Pin EN1_Pin EN2_Pin
                           EN3_Pin EN4_Pin */
  GPIO_InitStruct.Pin = Red_1_Pin|Amber_1_Pin|Green_1_Pin|Red_2_Pin
                          |Amber_2_Pin|Green_2_Pin|EN1_Pin|EN2_Pin
                          |EN3_Pin|EN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : a_Pin b_Pin c1_Pin d1_Pin
                           e1_Pin f1_Pin g1_Pin c_Pin
                           d_Pin e_Pin f_Pin g_Pin
                           a1_Pin b1_Pin */
  GPIO_InitStruct.Pin = a_Pin|b_Pin|c1_Pin|d1_Pin
                          |e1_Pin|f1_Pin|g1_Pin|c_Pin
                          |d_Pin|e_Pin|f_Pin|g_Pin
                          |a1_Pin|b1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON_2_Pin BUTTON_3_Pin */
  GPIO_InitStruct.Pin = BUTTON_2_Pin|BUTTON_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

// --- SỬA LẠI NGẮT TIMER ---
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if (htim->Instance == TIM2) {
    // Báo cho bộ lập lịch biết 1 tick đã trôi qua
    SCH_Update();

  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  // (Giữ nguyên)
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
  * where the assert_param error has occurred.
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
