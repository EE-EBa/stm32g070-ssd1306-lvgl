#include "eio_i2c.h"
#include "eio_object.h"
#include "qassert.h"
#include "stdint.h"
#include "string.h"
#include <stdint.h>

Q_DEFINE_THIS_FILE

void eio_i2c_register(struct eio_i2c *const me, const char *name,
                      const struct eio_i2c_ops *ops, void *user_data) {
  Q_ASSERT(me != NULL);
  Q_ASSERT(name != NULL);
  Q_ASSERT(ops != NULL);

  struct eio_obj_attr attr = {
      .user_data  = user_data,
      .standalone = true,
      .type       = EIO_OBJ_I2C,
  };

  me->ops            = ops;
  me->is_initialized = false;
  me->clock_speed    = 0;
  eio_register(&me->super, name, NULL, &attr);

  if (me->ops->init != NULL) {
    me->ops->init(me);
    me->is_initialized = true;
  }
}

int32_t eio_i2c_write(struct eio_obj *const me, uint16_t dev_addr,
                      const uint8_t *data, uint16_t size, uint32_t timeout) {

  Q_ASSERT(me != NULL);
  Q_ASSERT(me->attr.type == EIO_OBJ_I2C);
  struct eio_i2c *i2c = (struct eio_i2c *)me;

  Q_ASSERT(i2c->ops != NULL);
  Q_ASSERT(i2c->ops->write != NULL);
  return i2c->ops->write(i2c, dev_addr, data, size, timeout);
}

int32_t eio_i2c_read(struct eio_obj *const me, uint16_t dev_addr, uint8_t *data,
                     uint16_t size, uint32_t timeout) {
  Q_ASSERT(me != NULL);
  Q_ASSERT(me->attr.type == EIO_OBJ_I2C);
  struct eio_i2c *i2c = (struct eio_i2c *)me;

  Q_ASSERT(i2c->ops != NULL);
  Q_ASSERT(i2c->ops->read != NULL);
  return i2c->ops->read(i2c, dev_addr, data, size, timeout);
}

int32_t eio_i2c_mem_write(struct eio_obj *const me, uint16_t dev_addr,
                          uint16_t mem_addr, uint8_t mem_addr_size,
                          const uint8_t *data, uint16_t size,
                          uint32_t timeout) {
  Q_ASSERT(me != NULL);
  Q_ASSERT(me->attr.type == EIO_OBJ_I2C);
  struct eio_i2c *i2c = (struct eio_i2c *)me;

  Q_ASSERT(i2c->ops != NULL);
  Q_ASSERT(i2c->ops->mem_write != NULL);
  return i2c->ops->mem_write(i2c, dev_addr, mem_addr, mem_addr_size, data, size,
                             timeout);
}

int32_t eio_i2c_mem_read(struct eio_obj *const me, uint16_t dev_addr,
                         uint16_t mem_addr, uint8_t mem_addr_size,
                         uint8_t *data, uint16_t size, uint32_t timeout) {
  Q_ASSERT(me != NULL);
  Q_ASSERT(me->attr.type == EIO_OBJ_I2C);

  struct eio_i2c *i2c = (struct eio_i2c *)me;
  Q_ASSERT(i2c->ops != NULL);
  Q_ASSERT(i2c->ops->mem_read != NULL);
  return i2c->ops->mem_read(i2c, dev_addr, mem_addr, mem_addr_size, data, size,
                            timeout);
}
