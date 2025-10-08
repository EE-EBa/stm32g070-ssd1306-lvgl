#include "eio_object.h"
#include "common/eba_def.h"
#include "qassert.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

Q_DEFINE_THIS_FILE
/* private variables */
static struct eio_obj *eio_list = NULL;

/* public functions */
void eio_register(struct eio_obj *const me, const char *name,
                  const struct eio_ops *ops, struct eio_obj_attr *attr) {

  Q_ASSERT(me != NULL);
  Q_ASSERT(name != NULL);
  Q_ASSERT(ops != NULL);
  Q_ASSERT(attr != NULL);

  me->name = name;
  me->ops = ops;
  me->next = eio_list;
  eio_list = me;

  memcpy(&me->attr, attr, sizeof(struct eio_obj_attr));
}

struct eio_obj *eio_find(const char *name) {
  Q_ASSERT(name != NULL);

  struct eio_obj *obj = eio_list;

  while (obj != NULL) {
    if (strcmp(obj->name, name) == 0) {
      break;
    }
    obj = obj->next;
  }
  return obj;
}

enum eba_error eio_open(struct eio_obj *const me) {
  Q_ASSERT(me->ops->open != NULL);

  enum eba_error ret = EBA_OK;

  if (me->attr.standalone) {
    if (me->count_open != 0) {
      ret = EBA_ERROR;
      goto exit;
    }
  }

  ret = me->ops->open(me);
  if (ret == EBA_OK) {
    me->count_open++;
  }

exit:
  return ret;
}

enum eba_error eio_close(struct eio_obj *const me) {
  Q_ASSERT(me->ops->close != NULL);

  enum eba_error ret = EBA_OK;
  if (me->count_open > 0) {
    ret = me->ops->close(me);
    if (ret == EBA_OK) {
      me->count_open--;
      goto exit;
    }
  } else {
    ret = EBA_ERROR;
  }
exit:
  return ret;
}

int32_t eio_read(struct eio_obj *const me, void *buffer, uint32_t size) {
  Q_ASSERT(me != NULL);
  Q_ASSERT(buffer != NULL);
  Q_ASSERT(size != 0);
  Q_ASSERT(me->ops->read != NULL);

  int32_t ret = 0;
  if (me->count_open != 0) {
    ret = me->ops->read(me, buffer, size);
  } else {
    ret = (int32_t)EBA_ERROR;
  }
  return ret;
}

int32_t eio_write(struct eio_obj *const me, const void *buffer, uint32_t size) {
  Q_ASSERT(me != NULL);
  Q_ASSERT(buffer != NULL);
  Q_ASSERT(size != 0);
  Q_ASSERT(me->ops->write != NULL);

  int32_t ret = 0;
  if (me->count_open != 0) {
    ret = me->ops->write(me, buffer, size);

  } else {
    ret = (int32_t)EBA_ERROR;
  }
  return ret;
}
