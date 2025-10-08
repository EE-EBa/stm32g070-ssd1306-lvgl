#include "bsp.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_def.h"
#include "stm32g0xx_hal_pwr_ex.h"
#include "stm32g0xx_hal_rcc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* private function prototype */
static void _error_handler(void);
static void _system_clock_config(void);

/* BSP initialization */
void bsp_init() {
  HAL_Init();
  _system_clock_config();
}

/* System clock configuration */
static void _system_clock_config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitstruct = {0};

  /* configure the main internal regualator output voltage */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /* Initializes The RCC Oscillators according to the specified parameters  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    _error_handler();
  }

  /* Initializes the CPU,AHB and APB buses clocks */
  RCC_ClkInitstruct.ClockType =
      RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitstruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitstruct, FLASH_LATENCY_2) != HAL_OK) {
    _error_handler();
  }
}

/* assert function */
static void _error_handler(void) {

  __disable_irq();
  while (1) {
  }
}

#ifdef __cplusplus
}
#endif
