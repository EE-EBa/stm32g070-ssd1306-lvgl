#include "eba_export.h"
#include "bsp.h"
#include "qassert.h"
#include "stdint.h"
#include "string.h"
#include <stdint.h>
#include <stdio.h>

/* private define */
#define POLL_PERIOD_MAX (2592000000) /* 30 days */

Q_DEFINE_THIS_FILE

/* private function prototype */
static void module_null_init(void);
static void _init_func_execute(int8_t level);
static void _get_poll_export_table(void);
static void _get_init_export_table(void);

static void _poll_func_execute(void);

/* private variables */
INIT_EXPORT(module_null_init, 0);
POLL_EXPORT(module_null_init, (1000 * 60 * 60));

static struct export *export_init_table = NULL;
static uint32_t count_export_init = 0;
static struct export *export_poll_table = NULL;
static uint32_t count_export_poll = 0;
static int8_t export_level_max = INT8_MIN;

/* public functions */
void run(void) {
  _get_init_export_table();
  _get_poll_export_table();

  for (uint8_t level = 0; level <= export_level_max; level++) {
    _init_func_execute(level);
  }



  while (1) {
    _poll_func_execute();
  }
}

/* private functions */
static void _get_init_export_table(void) {
  struct export *func_block = (struct export *)&init_module_null_init;
  uint32_t address_last;

  while (1) {
    address_last = (uint32_t)func_block - sizeof(struct export);
    struct export *table = (struct export *)address_last;
    if ((table->magic_head != EXPORT_ID_INIT) ||
        table->magic_tail != EXPORT_ID_INIT) {
      break;
    }
    func_block = table;
  }

  export_init_table = func_block;
  uint32_t i = 0;
  while (1) {

    if ((export_init_table[i].magic_head == EXPORT_ID_INIT) &&
        export_init_table[i].magic_tail == EXPORT_ID_INIT) {
      if (export_init_table[i].level > export_level_max) {
        export_level_max = export_init_table[i].level;
      }
      i++;
    } else {
      break;
    }
  }
  count_export_init = i;
}

static void _get_poll_export_table(void) {
  struct export *func_block = (struct export *)&poll_module_null_init;
  uint32_t address_last;

  while (1) {
    address_last = (uint32_t)func_block - sizeof(struct export);
    struct export *table = (struct export *)address_last;
    if (table->magic_head != EXPORT_ID_POLL ||
        table->magic_tail != EXPORT_ID_POLL) {
      break;
    }
    func_block = table;
  }

  export_poll_table = func_block;

  uint32_t i = 0;
  while (1) {
    if (export_poll_table[i].magic_head == EXPORT_ID_POLL &&
        export_poll_table[i].magic_tail == EXPORT_ID_POLL) {
      Q_ASSERT((export_poll_table[i].period_ms <= POLL_PERIOD_MAX));

      struct export_poll_data *data =
          (struct export_poll_data *)export_poll_table[i].data;

      data->timeout_ms = elab_time_ms() + export_poll_table[i].period_ms;
      i++;
    } else {
      break;
    }
  }

  count_export_poll = i;
}

/* Execute the initial functions in specific level */
static void _init_func_execute(int8_t level) {
  for (uint32_t i = 0; i < count_export_init; i++) {
    if (export_init_table[i].level == level) {
      if (!export_init_table[i].exit) {
        if (level != EXPORT_UNIT_TEST) {
          printf("Export init %s.\n", export_init_table[i].name);
        }
        ((void (*)(void))export_init_table[i].func)();
      }
    }
  }
}

/* polling  functions executing */
static void _poll_func_execute(void) {
  struct export_poll_data *data;

  for (uint32_t i = 0; i < count_export_poll; i++) {
    data = export_poll_table[i].data;

    while (1) {
      uint32_t _time = elab_time_ms();
      if (((_time >= data->timeout_ms) &&
           (_time - data->timeout_ms < POLL_PERIOD_MAX)) ||
          ((_time < data->timeout_ms) &&
           (data->timeout_ms - _time > POLL_PERIOD_MAX))) {
        data->timeout_ms += export_poll_table[i].period_ms;
        ((void (*)(void))export_poll_table[i].func)();
      } else {
        break;
      }
    }
  }
}

/* null function to mark the section of exporting */
static void module_null_init(void) {}
