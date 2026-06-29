/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * @author         : Cuplis Kei Darma
  ******************************************************************************
  * @attention
  *
  * This is the main application file. Keep this file clean!
  * - All configuration is in config.h
  * - State machine logic is in state_machine.c
  * - Sensor functions are in sensors.c
  *
  * Target Hardware: STM32F103C8T6 "Blue Pill"
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iwdg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "config.h"
#include "state_machine.h"
#include "sensors.h"
#include "error_log.h"
#include "config_storage.h"

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void System_Startup(void);
static void System_Diagnostics(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static volatile uint8_t shutdownRequested = 0;

/**
  * @brief  Initiate graceful shutdown
  */
void System_RequestShutdown(void)
{
  shutdownRequested = 1;
}

/**
  * @brief  Perform graceful shutdown
  */
static void System_Shutdown(void)
{
  // Stop pump immediately
  PUMP_OFF();
  
  // Indicate shutdown with LED pattern
  for(int i = 0; i < 3; i++) {
    PROGRAM_LED_ON();
    STATUS_LED_ON();
    HAL_Delay(200);
    PROGRAM_LED_OFF();
    STATUS_LED_OFF();
    HAL_Delay(200);
  }
  
  // Turn off all peripherals
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_All, GPIO_PIN_RESET);
  
  // Enter low power mode or infinite loop
  while(1) {
    __WFI();  // Wait for interrupt (power off)
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
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  // Initialize system modules
  Sensors_Init();
  StateMachine_Init();
  
  // Run startup sequence
  System_Startup();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  // Non-blocking loop variables
  uint32_t lastLoopTime = 0;
  uint32_t lastIWDGRefresh = 0;
  uint32_t loopInterval = 10; // Default 10ms
  
  while (1)
  {
    // Check for shutdown request (Task 9)
    if(shutdownRequested) {
      System_Shutdown();
    }

    // Diagnostic Mode Trigger (Task 10)
    // Check if door held open for 10 seconds
    static uint32_t doorOpenStartTime = 0;

    if(!Sensors_IsDoorClosed()) {
      if(doorOpenStartTime == 0) {
        doorOpenStartTime = HAL_GetTick();
      } else if((HAL_GetTick() - doorOpenStartTime) > 10000) {
        // Enter diagnostic mode
        System_Diagnostics();
        doorOpenStartTime = 0; // Reset after running
      }
    } else {
      doorOpenStartTime = 0;
    }

    uint32_t currentTime = HAL_GetTick();

    // IWDG refresh every 2 seconds (safe margin for ~3.2s timeout)
    if((currentTime - lastIWDGRefresh) >= 2000) {
      lastIWDGRefresh = currentTime;
      HAL_IWDG_Refresh(&hiwdg);
    }

    // Non-blocking loop timing
    if((currentTime - lastLoopTime) >= loopInterval) {
      lastLoopTime = currentTime;

      // Process state machine
      StateMachine_Process();

      // Update LED indicators
      StateMachine_UpdateLEDs();
      
      // Adaptive rate based on state for power efficiency
      SystemState_t state = StateMachine_GetState();
      if(state == STATE_FILLING) {
        loopInterval = 10; // Fast response needed
      } else if(state == STATE_DOOR_OPEN || state == STATE_ERROR) {
        loopInterval = 20; // Medium response
      } else {
        loopInterval = 50; // Slow response (IDLE/FULL) - saves CPU
      }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
/**
  * @brief  System startup sequence
  * @param  None
  * @retval None
  */
static void System_Startup(void)
{
  // Initialize outputs to safe state
  PUMP_OFF();
  PROGRAM_LED_OFF();
  STATUS_LED_OFF();

  // Power-on stabilization delay
  HAL_Delay(500);
  
  // Self-test sensors (Task 8)
  uint8_t sensorTest = Sensors_SelfTest();
  
  if(sensorTest != 0) {
    // Sensor test failed - indicate with LED pattern
    for(int i = 0; i < 10; i++) {
      PROGRAM_LED_ON();
      STATUS_LED_OFF();
      HAL_Delay(100);
      PROGRAM_LED_OFF();
      STATUS_LED_ON();
      HAL_Delay(100);
    }
    STATUS_LED_OFF();
    // Continue anyway, but user is warned
  }

  // Startup blink sequence (3x fast blink = system starting)
  for(int i = 0; i < 3; i++) {
    PROGRAM_LED_ON();
    STATUS_LED_ON();
    HAL_Delay(150);
    PROGRAM_LED_OFF();
    STATUS_LED_OFF();
    HAL_Delay(150);
  }

  HAL_Delay(500);
}
/**
  * @brief  Display system diagnostics via LED
  * @note   Hold door open for 10 seconds to trigger
  */
static void System_Diagnostics(void)
{
  // Pattern 1: Clock speed indication
  // Fast blink = correct speed
  for(int i = 0; i < 8; i++) {
    PROGRAM_LED_TOGGLE();
    HAL_Delay(100);
  }
  HAL_Delay(500);
  
  // Pattern 2: Sensor status
  if(Sensors_IsDoorClosed()) {
    STATUS_LED_ON();
    HAL_Delay(500);
    STATUS_LED_OFF();
  }
  HAL_Delay(500);
  
  if(Sensors_IsTankFull()) {
    STATUS_LED_ON();
    HAL_Delay(500);
    STATUS_LED_OFF();
  }
  HAL_Delay(500);
  
  // Pattern 3: Error count
  uint8_t errorCount = StateMachine_GetStats()->errorCount;
  for(int i = 0; i < errorCount && i < 10; i++) {
    PROGRAM_LED_ON();
    HAL_Delay(200);
    PROGRAM_LED_OFF();
    HAL_Delay(200);
  }
  
  // Pattern 4: Pump cycle count (tens)
  uint32_t cycles = StateMachine_GetStats()->pumpCycleCount;
  uint8_t tens = (cycles / 10) % 10;
  for(int i = 0; i < tens; i++) {
    STATUS_LED_ON();
    HAL_Delay(200);
    STATUS_LED_OFF();
    HAL_Delay(200);
  }
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
	// Hardware error state - both LEDs blink rapidly together
	PROGRAM_LED_TOGGLE();
	STATUS_LED_TOGGLE();
	HAL_Delay(100);
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
