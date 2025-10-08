#ifndef EIO_PWM_H
#define EIO_PWM_H
#include "eio_object.h"
#include <stdint.h>

/* public typedef */
struct eio_pwm {
  struct eio_obj super;

  uint8_t duty_ratio;
  const struct eio_pwm_ops *ops;
};

struct eio_pwm_ops {
  void (*init)(struct eio_pwm *const me);
  void (*set_duty)(struct eio_pwm *const me, uint8_t duty_ratio);
};

/* public functions */
/* -----for low-level driver */
void eio_pwm_register(struct eio_pwm *const me, const char *name,
                      const struct eio_pwm_ops *ops, void *user_data);
/* ----- for high-level code */
void eio_pwm_set_duty(struct eio_obj *const me, uint8_t duty_ratio);

#endif
