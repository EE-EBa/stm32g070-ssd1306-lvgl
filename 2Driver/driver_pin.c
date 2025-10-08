
#include "eba_export.h"
#include "eio_pin.h"
#include "qassert.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_gpio.h"
#include <stdbool.h>
#include <stdint.h>

Q_DEFINE_THIS_FILE

/* public variables */
struct eio_pin_data {
  struct eio_pin *device;
  char const *name;
  GPIO_TypeDef *gpio_x;
  uint16_t pin;
};

/* private functions of prototype */
static void _init(struct eio_pin *const me);
static void _set_mode(struct eio_pin *const me, uint8_t mode);
static bool _get_status(struct eio_pin *const me);
static void _set_status(struct eio_pin *const me, bool status);

/* private variables */
static struct eio_pin pin_c_08;

static struct eio_pin_ops const pin_driver_ops = {
    .init = _init,
    .set_mode = _set_mode,
    .get_status = _get_status,
    .set_status = _set_status,
};

static struct eio_pin_data eio_pin_driver_data[] = {
    {
        &pin_c_08,
        "LED1",
        GPIOC,
        GPIO_PIN_8,
    },

};
/* private functions */
static void eio_pin_driver_init(void) {
  for (uint32_t i = 0;
       i < sizeof(eio_pin_driver_data) / sizeof(struct eio_pin_data); i++) {
    /* Enable the clock */
    if (eio_pin_driver_data[i].gpio_x == GPIOA) {
      __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (eio_pin_driver_data[i].gpio_x == GPIOB) {
      __HAL_RCC_GPIOB_CLK_ENABLE();
    } else if (eio_pin_driver_data[i].gpio_x == GPIOC) {
      __HAL_RCC_GPIOC_CLK_ENABLE();
    } else if (eio_pin_driver_data[i].gpio_x == GPIOD) {
      __HAL_RCC_GPIOD_CLK_ENABLE();
    }

    /* Device registering */
    eio_pin_register(eio_pin_driver_data[i].device, eio_pin_driver_data[i].name,
                     &pin_driver_ops, &eio_pin_driver_data[i]);
  }
}
INIT_EXPORT(eio_pin_driver_init, 1);

static void _init(struct eio_pin *const me) {
  struct eio_pin_data *driver_data =
      (struct eio_pin_data *)me->super.attr.user_data;

  /* Configure GPIO pins */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = driver_data->pin;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(driver_data->gpio_x, &GPIO_InitStruct);

  HAL_GPIO_WritePin(driver_data->gpio_x, driver_data->pin, GPIO_PIN_RESET);
}

static void _set_mode(struct eio_pin *const me, uint8_t mode) {

  Q_ASSERT(me != NULL);
  Q_ASSERT(mode < PIN_MODE_MAX);

  struct eio_pin_data *driver_data =
      (struct eio_pin_data *)me->super.attr.user_data;

  /* Configure GPIO pins */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (mode == PIN_MODE_INPUT) {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
  } else if (mode == PIN_MODE_INPUT_PULLUP) {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
  } else if (mode == PIN_MODE_INPUT_PULLDOWN) {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  } else if (mode == PIN_MODE_OUTPUT_PP) {
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
  } else if (mode == PIN_MODE_OUTPUT_OD) {
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
  }

  GPIO_InitStruct.Pin = driver_data->pin;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(driver_data->gpio_x, &GPIO_InitStruct);
}

static bool _get_status(struct eio_pin *const me) {
  Q_ASSERT(me != NULL);

  struct eio_pin_data *driver_data =
      (struct eio_pin_data *)me->super.attr.user_data;

  GPIO_PinState status =
      HAL_GPIO_ReadPin(driver_data->gpio_x, driver_data->pin);

  return (status == GPIO_PIN_SET) ? true : false;
}

static void _set_status(struct eio_pin *const me, bool status) {
  Q_ASSERT(me != NULL);

  struct eio_pin_data *driver_data = (struct eio_pin_data *)me->super.attr.user_data;

  HAL_GPIO_WritePin(driver_data->gpio_x, driver_data->pin,
                    status ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
