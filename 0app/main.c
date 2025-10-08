#include "bsp.h"
#include "eio_pin.h"
#include "stdio.h"
#include <stdbool.h>
#include "eba_export.h"
/* private variables */

/* public functions */
extern void initialise_monitor_handles(void);
int main(void) {
  initialise_monitor_handles();
  printf("semihosting test ok!\n");

  bsp_init();

  run();
}
