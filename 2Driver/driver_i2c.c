#include "eba_export.h"
#include "eio_i2c.h"
#include "qassert.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_hal_i2c.h"
#include "stm32g0xx_hal_rcc.h"
#include "stdint.h"
#include <stdint.h>

Q_DEFINE_THIS_FILE

struct eio_i2c_data {
  struct eio_i2c    *device;
  const char        *name;
  I2C_TypeDef       *i2c_instance;
  GPIO_TypeDef      *scl_gpio;
  uint16_t           scl_pin;
  uint32_t           scl_alternate;
  GPIO_TypeDef      *sda_gpio;
  uint16_t           sda_pin;
  uint32_t           sda_alternate;
  I2C_HandleTypeDef *hi2c;
};

static void    _init(struct eio_i2c *const me);
static int32_t _write(struct eio_i2c *const me, uint16_t address,
                      const uint8_t *data, uint16_t size, uint32_t timeout);
static int32_t _read(struct eio_i2c *const me, uint16_t address, uint8_t *data,
                     uint16_t size, uint32_t timeout);
static int32_t _mem_write(struct eio_i2c *const me, uint16_t address,
                          uint16_t mem_address, uint8_t mem_size,
                          const uint8_t *data, uint16_t size, uint32_t timeout);
static int32_t _mem_read(struct eio_i2c *const me, uint16_t address,
                         uint16_t mem_address, uint8_t mem_size, uint8_t *data,
                         uint16_t size, uint32_t timeout);
static void    _gpio_init(struct eio_i2c_data *const me);
static void    _i2c_hareware_init(struct eio_i2c_data *const me);

static struct eio_i2c      i2c1_device;
static I2C_HandleTypeDef   hi2c1;
static struct eio_i2c_data i2c_driver_data[] = {
    {
        .device        = &i2c1_device,
        .name          = "I2C1",
        .i2c_instance  = I2C1,
        .scl_gpio      = GPIOB,
        .scl_pin       = GPIO_PIN_8,
        .scl_alternate = GPIO_AF6_I2C1,
        .sda_gpio      = GPIOB,
        .sda_pin       = GPIO_PIN_9,
        .sda_alternate = GPIO_AF6_I2C1,
        .hi2c          = &hi2c1,
    },
};

static const struct eio_i2c_ops i2c_driver_ops = {
    .init      = _init,
    .write     = _write,
    .read      = _read,
    .mem_write = _mem_write,
    .mem_read  = _mem_read,
};

static void driver_i2c_init(void) {
  __HAL_RCC_I2C1_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  hi2c1.Instance              = I2C1;
  hi2c1.Init.Timing           = 0x00303D5B;
  hi2c1.Init.OwnAddress1      = 0;
  hi2c1.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2      = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

  for (uint32_t i = 0;
       i < sizeof(i2c_driver_data) / sizeof(struct eio_i2c_data); i++) {

    eio_i2c_register(i2c_driver_data[i].device, i2c_driver_data[i].name,
                     &i2c_driver_ops, &i2c_driver_data[i]);
  }
}
INIT_EXPORT(driver_i2c_init, 2);

static void _init(struct eio_i2c *const me) {
  Q_ASSERT(me != NULL);

  struct eio_i2c_data *driver_data =
      (struct eio_i2c_data *)me->super.attr.user_data;
  _gpio_init(driver_data);
  _i2c_hareware_init(driver_data);

  me->clock_speed = 400000;
}

static int32_t _write(struct eio_i2c *const me, uint16_t address,
                      const uint8_t *data, uint16_t size, uint32_t timeout) {
  Q_ASSERT(me != NULL);
  Q_ASSERT(data != NULL);
  Q_ASSERT(size > 0);

  struct eio_i2c_data *driver_data =
      (struct eio_i2c_data *)me->super.attr.user_data;

  if (HAL_I2C_Master_Transmit(driver_data->hi2c, address << 1, (uint8_t *)data,
                              size, timeout) != HAL_OK) {
    return -1;
  }

  return size;
}
