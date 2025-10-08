#include "eio_pwm.h"
#include "eio_object.h"
#include "qassert.h"
#include <string.h>

Q_DEFINE_THIS_FILE

/* private variables */
static struct eio_ops _obj_ops = {
    .open = NULL,
    .close = NULL,
    .read = NULL,
    .write = NULL,
};

/* public functions */
void eio_pwm_register(struct eio_pwm *const me, const char *name,
                      const struct eio_pwm_ops *ops, void *user_data) {
  Q_ASSERT(me != NULL);
  Q_ASSERT(name != NULL);
  Q_ASSERT(ops != NULL);

  struct eio_obj_attr attr = {
      .user_data = user_data,
      .standalone = true,
      .type = EIO_OBJ_PWM,
  };


  eio_register(&me->super, name, &_obj_ops, &attr);
  me->ops = ops;
  me->duty_ratio = 0;
  me->ops->init(me);
}

void eio_pwm_set_duty(struct eio_obj *const me, uint8_t duty_ratio) {
  Q_ASSERT(me != NULL);
  Q_ASSERT(duty_ratio <= 100);
  Q_ASSERT(me->attr.type == EIO_OBJ_PWM);

  struct eio_pwm *pwm = (struct eio_pwm *)me;
  if (duty_ratio != pwm->duty_ratio) {
    pwm->ops->set_duty(pwm, duty_ratio);
    pwm->duty_ratio = duty_ratio;
  }
}
