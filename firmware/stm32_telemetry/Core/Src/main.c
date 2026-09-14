/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "adc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LED_TOGGLE_INTERVAL_MS 250U
#define UART_HEARTBEAT_INTERVAL_MS 2000U

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
static void UART_Write(const char *message, uint16_t length);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void UART_Write(const char *message, uint16_t length)
{
  (void)HAL_UART_Transmit(&huart2, (const uint8_t *)message, length, 100U);
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
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  uint32_t last_uart_heartbeat_ms = HAL_GetTick();
  static const char startup_message[] =
      "\r\nSTM32 telemetry smoke test ready\r\n"
      "LD2 is blinking; press blue B1 to test input.\r\n";
  GPIO_PinState previous_button_state = HAL_GPIO_ReadPin(B1_USER_GPIO_Port,
                                                          B1_USER_Pin);
  uint32_t last_led_toggle_ms = HAL_GetTick();
  char adc_message[96];
  UART_Write(startup_message, (uint16_t)(sizeof(startup_message) - 1U));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    const uint32_t now_ms = HAL_GetTick();
    if ((now_ms - last_uart_heartbeat_ms) >= UART_HEARTBEAT_INTERVAL_MS)
    {
      (void)HAL_ADC_Start(&hadc1);
      if (HAL_ADC_PollForConversion(&hadc1, 100U) == HAL_OK)
      {
        const uint32_t temp_raw = HAL_ADC_GetValue(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 100U) == HAL_OK)
        {
          const uint32_t light_raw = HAL_ADC_GetValue(&hadc1);
          uint32_t voltage_mv = (temp_raw * 3300U) / 4095U;
          int32_t temperature_tenths_c = (int32_t)voltage_mv - 500;
          const int message_length = snprintf(adc_message,
                                            sizeof(adc_message),
                                            "ADC raw=%lu, voltage_mv=%lu,  temp_x10_C=%ld, light_raw=%lu\r\n", (unsigned long)temp_raw, (unsigned long)voltage_mv, (long)temperature_tenths_c, (unsigned long)light_raw);
          if ((message_length > 0) &&
            (message_length < (int)sizeof(adc_message)))
          {
            UART_Write(adc_message, (uint16_t)message_length);
          }
        }
      }
      (void)HAL_ADC_Stop(&hadc1);
      last_uart_heartbeat_ms = now_ms;
    }
    const GPIO_PinState button_state = HAL_GPIO_ReadPin(B1_USER_GPIO_Port,
                                                        B1_USER_Pin);

    if (button_state == GPIO_PIN_RESET)
    {
      HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    }
    else if ((now_ms - last_led_toggle_ms) >= LED_TOGGLE_INTERVAL_MS)
    {
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      last_led_toggle_ms = now_ms;
    }

    if (button_state != previous_button_state)
    {
      HAL_Delay(20U);
      if (HAL_GPIO_ReadPin(B1_USER_GPIO_Port, B1_USER_Pin) == button_state)
      {
        if (button_state == GPIO_PIN_RESET)
        {
          static const char pressed_message[] = "B1 pressed\r\n";
          UART_Write(pressed_message,
                     (uint16_t)(sizeof(pressed_message) - 1U));
        }
        else
        {
          static const char released_message[] = "B1 released\r\n";
          UART_Write(released_message,
                     (uint16_t)(sizeof(released_message) - 1U));
        }
        previous_button_state = button_state;
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

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

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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
