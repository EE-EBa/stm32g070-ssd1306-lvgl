#ifndef EIO_PIN_H
#define EIO_PIN_H

/* includes */
#include "eio_object.h"
#include "stdbool.h"
#include "stdint.h"

/* public define */
enum pin_mode {
  PIN_MODE_INPUT = 0,
  PIN_MODE_INPUT_PULLUP,
  PIN_MODE_INPUT_PULLDOWN,
  PIN_MODE_OUTPUT_PP,
  PIN_MODE_OUTPUT_OD,

  PIN_MODE_MAX

};

/* public structure */
struct eio_pin {
  struct eio_obj super; /* casting can occur between structure "super" and
                        structure eio_pin because their address is identical*/

  const struct eio_pin_ops *ops;
  uint8_t mode; /* device's mode , for gpio , the mode is input or output */
  bool status;  /* get device  status ,for  gpio ,the status is  high or low */
};

struct eio_pin_ops {
  void (*init)(struct eio_pin *const me);
  void (*set_mode)(struct eio_pin *const me, uint8_t mode);
  bool (*get_status)(struct eio_pin *const me);
  void (*set_status)(struct eio_pin *const me, bool status);
};

/* public functions */
/*      for low-level driver */
void eio_pin_register(struct eio_pin *const me, const char *name,
                      struct eio_pin_ops const *ops, void *user_data);

/*      for high-level driver */
void eio_pin_set_mode(struct eio_obj *const me, uint8_t mode);
bool eio_pin_get_status(struct eio_obj *const me);
void eio_pin_set_status(struct eio_obj *const me, bool status);

#endif
