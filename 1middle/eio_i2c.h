#ifndef EIO_I2C_H
#define EIO_I2C_H

#include <stdbool.h>
#include <stdint.h>
#include "eio_object.h"

struct eio_i2c {
  struct eio_obj super;

  const struct eio_i2c_ops *ops;
  uint32_t                  clock_speed;
  bool                      is_initialized;
};

struct eio_i2c_ops {
  void    (*init)(struct eio_i2c *const me);
  int32_t (*write)(struct eio_i2c *const me, uint16_t dev_addr,
                   const uint8_t *data, uint16_t size, uint32_t timeout);
  int32_t (*read)(struct eio_i2c *const me, uint16_t dev_addr, uint8_t *data,
                  uint16_t size, uint32_t timeout);
  int32_t (*mem_write)(struct eio_i2c *const me, uint16_t dev_addr,
                       uint16_t mem_addr, uint8_t mem_addr_size,
                       const uint8_t *data, uint16_t size, uint32_t timeout);
  int32_t (*mem_read)(struct eio_i2c *const me, uint16_t dev_addr,
                      uint16_t mem_addr, uint8_t mem_addr_size, uint8_t *data,
                      uint16_t size, uint32_t timeout);
};

void eio_i2c_register(struct eio_i2c *const me, const char *name,
                      const struct eio_i2c_ops *ops, void *user_data);

int32_t eio_i2c_write(struct eio_obj *const me, uint16_t dev_addr,
                      const uint8_t *data, uint16_t size, uint32_t timeout);

int32_t eio_i2c_read(struct eio_obj *const me, uint16_t dev_addr, uint8_t *data,
                     uint16_t size, uint32_t timeout);

int32_t eio_i2c_mem_write(struct eio_obj *const me, uint16_t dev_addr,
                          uint16_t mem_addr, uint8_t mem_addr_size,
                          const uint8_t *data, uint16_t size, uint32_t timeout);

int32_t eio_i2c_mem_read(struct eio_obj *const me, uint16_t dev_addr,
                         uint16_t mem_addr, uint8_t mem_addr_size,
                         uint8_t *data, uint16_t size, uint32_t timeout);

#endif
