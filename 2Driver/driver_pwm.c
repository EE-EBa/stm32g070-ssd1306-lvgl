#include "eba_export.h"
#include "eio_pwm.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_hal_rcc.h"
#include "stm32g0xx_hal_tim.h"
#include "stm32g0xx_hal_tim_ex.h"
#include <stdint.h>

/* public typedef */
struct eio_pwm_data {
  struct eio_pwm *device;
  const char *name;
  GPIO_TypeDef *gpio_x;
  uint16_t pin;
  uint32_t alternate;
  TIM_HandleTypeDef *htim;
  uint32_t channel;
};

/* private function prototype */
static void _init(struct eio_pwm *const me);
static void _set_duty(struct eio_pwm *const me, uint8_t duty_ratio);
static void _timer_pwm_init(struct eio_pwm_data *data);
static void _gpio_pwm_init(struct eio_pwm_data *data);

/* private variables */
static struct eio_pwm pwm_led3;
static struct eio_pwm pwm_led4;
static struct eio_pwm pwm_adc_in;

static TIM_HandleTypeDef htim1;
static TIM_HandleTypeDef htim15;

static struct eio_pwm_data pwm_driver_data[] = {

    {
        &pwm_led4,
        "pwmled4",
        GPIOC,
        GPIO_PIN_9,
        GPIO_AF2_TIM1,
        &htim1,
        TIM_CHANNEL_2,
    },

    {
        &pwm_adc_in,
        "adc_in",
        GPIOC,
        GPIO_PIN_1,
        GPIO_AF2_TIM15,
        &htim15,
        TIM_CHANNEL_1,
    }

};

static const struct eio_pwm_ops pwm_driver_ops = {
    .init = _init,
    .set_duty = _set_duty,
};

/* private functions */
static void driver_pwm_init(void) {
  __HAL_RCC_TIM1_CLK_ENABLE();
  __HAL_RCC_TIM15_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 63;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_PWM_Init(&htim1);

  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 63;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 999;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_PWM_Init(&htim15);

  for (uint32_t i = 0;
       i < sizeof(pwm_driver_data) / sizeof(struct eio_pwm_data); i++) {

    /* Device registering */
    eio_pwm_register(pwm_driver_data[i].device, pwm_driver_data[i].name,
                     &pwm_driver_ops, &pwm_driver_data[i]);
  }
}
INIT_EXPORT(driver_pwm_init, 0);

static void _init(struct eio_pwm *const me) {

  struct eio_pwm_data *data = me->super.attr.user_data;

  _timer_pwm_init(data);
  _gpio_pwm_init(data);
  HAL_TIM_PWM_Start(data->htim, data->channel);
}

static void _set_duty(struct eio_pwm *const me, uint8_t duty) {
  struct eio_pwm_data *data = me->super.attr.user_data;

  HAL_TIM_PWM_Stop(data->htim, data->channel);

  TIM_OC_InitTypeDef sConfigOC = {0};
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = duty * 10;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  HAL_TIM_PWM_ConfigChannel(data->htim, &sConfigOC, data->channel);

  HAL_TIM_PWM_Start(data->htim, data->channel);
}

static void _timer_pwm_init(struct eio_pwm_data *data) {

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(data->htim, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  HAL_TIM_PWM_ConfigChannel(data->htim, &sConfigOC, data->channel);

  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  HAL_TIMEx_ConfigBreakDeadTime(data->htim, &sBreakDeadTimeConfig);
}

static void _gpio_pwm_init(struct eio_pwm_data *data) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = data->pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = data->alternate;
  HAL_GPIO_Init(data->gpio_x, &GPIO_InitStruct);
}
