#include "eio_pin.h"
#include "eio_object.h"
#include "qassert.h"
#include "string.h"
#include <string.h>

Q_DEFINE_THIS_FILE
/* private variables */
static struct eio_ops _obj_ops = {
    .open = NULL,
    .close = NULL,
    .read = NULL,
    .write = NULL,
};

/* private functions */

/* public functions */

void eio_pin_register(struct eio_pin *const me, const char *name,
                      struct eio_pin_ops const *ops, void *user_data) {
  Q_ASSERT(me != NULL);
  Q_ASSERT(name != NULL);
  Q_ASSERT(ops != NULL);
  Q_ASSERT(user_data != NULL);

  struct eio_obj_attr attr = {
      .user_data = user_data,
      .standalone = true,
      .type = EIO_OBJ_PIN,
  };

  eio_register(&me->super, name, &_obj_ops, &attr);
  me->ops = ops;
  me->ops->init(me);
  me->status = me->ops->get_status(me);
}

void eio_pin_set_mode(struct eio_obj *const me, uint8_t mode) {
  Q_ASSERT(me != NULL);

  struct eio_pin *pin = (struct eio_pin *)me;
  if (pin->mode != mode) {
    pin->ops->set_mode(pin, mode);
    pin->mode = mode;
  }
}

bool eio_pin_get_status(struct eio_obj *const me) {
  Q_ASSERT(me != NULL);

  struct eio_pin *pin = (struct eio_pin *)me;
  pin->status = pin->ops->get_status(pin);
  return pin->status;
}

void eio_pin_set_status(struct eio_obj *const me, bool status) {
  Q_ASSERT(me != NULL);

  struct eio_pin *pin = (struct eio_pin *)me;
  Q_ASSERT(pin->mode == PIN_MODE_OUTPUT_PP || pin->mode == PIN_MODE_OUTPUT_OD);

  if (status != pin->status) {
    pin->ops->set_status(pin, status);
    eio_pin_get_status(me);
    Q_ASSERT(pin->status == status);
  }
}
