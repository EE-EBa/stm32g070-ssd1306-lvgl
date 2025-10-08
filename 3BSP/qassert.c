#include "qassert.h"
#include "stdio.h"
Q_NORETURN Q_onAssert(char const *module, int_t location) {
  /* TBD: damage control */
  (void)module;   /* avoid the "unused parameter" compile */
  (void)location; /* avoid the "unused parameter" compile */
  printf("%s's line %d have problem!\n",module,location);
  for (;;) {
  }
}
