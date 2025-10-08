#ifndef DRIVER_H
#define DRIVER_H

#include "stdbool.h"
#include "stdint.h"

/* public define  */
#define EXPORT_ID_INIT (0xa5a5a5a5)
#define EXPORT_ID_POLL (0xbeefbeef)

enum export_level {
  EXPORT_POLL = -2,
  EXPORT_UNIT_TEST = -1,
  EXPORT_LEVEL_BSP = 0,
  EXPORT_LEVEL_HW_INDEPENDENT = 0,
  EXPORT_DRIVER = 1,
  EXPORT_MIDWARE = 2,
  EXPORT_DEVICE = 3,
  EXPORT_APP = 4,
  EXPORT_USER = 5,
};

/* public typedef */
struct export_poll_data {
  uint32_t timeout_ms;
};

struct export {
  uint32_t magic_head; /*  */
  const char *name;    /* driver's name */
  void *func;
  void *data;
  bool exit;
  int8_t level;
  uint8_t type;
  uint32_t period_ms;
  uint32_t magic_tail;
};

/* private functions */
void run(void);

/* drivers register macro */
#ifdef __GNUC__
#define SECTION(x) __attribute__((section(x)))
#define USED __attribute__((used))
#define ALIGN(n) __attribute__((aligned(n)))
#define WEAK __attribute__((weak))
#endif

#define INIT_EXPORT(_func, _level)                                             \
  USED const struct export init_##_func SECTION("eba_export_section") = {     \
      .name = #_func,                                                          \
      .func = (void *)&_func,                                                  \
      .level = _level,                                                         \
      .exit = false,                                                           \
      .magic_head = EXPORT_ID_INIT,                                            \
      .magic_tail = EXPORT_ID_INIT,                                            \
  }

#define EXIT_EXPORT(_func, _level)                                             \
  USED const struct export exit_##_func SECTION("eba_export_section") = {      \
      .name = #_func,                                                          \
      .func = (void *)&_func,                                                  \
      .level = _level,                                                         \
      .exit = true,                                                            \
      .magic_head = EXPORT_ID_INIT,                                            \
      .magic_tail = EXPORT_ID_INIT,                                            \
  }

#define POLL_EXPORT(_func, _period_ms)                                         \
  static struct export_poll_data poll_##_func##_data = {                       \
      .timeout_ms = 0,                                                         \
  };                                                                           \
  USED const struct export poll_##_func SECTION("eba_expoll_section") = {      \
      .name = "poll",                                                          \
      .func = &_func,                                                          \
      .data = (void *)&poll_##_func##_data,                                    \
      .level = EXPORT_POLL,                                                    \
      .period_ms = (uint32_t)(_period_ms),                                     \
      .magic_head = EXPORT_ID_POLL,                                            \
      .magic_tail = EXPORT_ID_POLL,                                            \
  };
#endif
