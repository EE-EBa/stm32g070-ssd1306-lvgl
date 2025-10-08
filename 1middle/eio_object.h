#ifndef EBA_OBJECT_H
#define EBA_OBJECT_H

#include "eba_def.h"
#include <stdbool.h>
#include <stdint.h>

/* public define */
enum eio_object {
  EIO_OBJ_PIN = 0,
  EIO_OBJ_PWM,
  EIO_OBJ_ADC,
  EIO_OBJ_DAC,
  EIO_OBJ_UART,
  EIO_OBJ_SPI,
  EIO_OBJ_I2C,
  EIO_OBJ_CAN,

  EIO_OBJ_MAX

};

/* public typedef */
struct eio_obj_attr {
  void *user_data;
  uint8_t type;
  bool standalone;
};

struct eio_obj {
  struct eio_obj *next;
  const char *name;
  const struct eio_ops *ops;
  uint16_t count_open;
  struct eio_obj_attr attr;
};

struct eio_ops {
  enum eba_error (*open)(struct eio_obj *const me);
  enum eba_error (*close)(struct eio_obj *const me);
  int32_t (*read)(struct eio_obj *const me, void *buffer, uint32_t size);
  int32_t (*write)(struct eio_obj *const me, const void *buffer, uint32_t size);
};

/* public functions */
/* -----for io-level driver */
void eio_register(struct eio_obj *const me, const char *name,
                  const struct eio_ops *ops, struct eio_obj_attr *attr);

/* -----for high-level code */
struct eio_obj *eio_find(const char *name);
enum eba_error eio_open(struct eio_obj *const me);
enum eba_error eio_close(struct eio_obj *const me);
int32_t eio_read(struct eio_obj *const me,  void *buffer, uint32_t size);
int32_t eio_write(struct eio_obj *const me, const void *buffer, uint32_t size);

#endif
