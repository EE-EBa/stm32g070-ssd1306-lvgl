#include "bsp.h"
#include "eba_export.h"
#include "eio_pwm.h"
#include "qassert.h"
#include "stdlib.h"

Q_DEFINE_THIS_FILE

/* private config */
#define PWM_LED_POLL_PERIOD_MS (5)

/* private variables */
struct eio_obj *led4 = NULL;

/* private functions */
static void pwm_led_init(void) {
  led4 = eio_find("pwmled4");
  Q_ASSERT(led4 != NULL);
}
INIT_EXPORT(pwm_led_init, 3);

static void pwm_led_poll(void) {
  uint16_t ms = elab_time_ms() % 1000;
  uint8_t duty_ratio;
  if (ms <= 500) {
    duty_ratio = ms / 5;
  } else {
    duty_ratio = (1000 - ms) / 5;
  }
  //eio_pwm_set_duty(led3, duty_ratio);
  eio_pwm_set_duty(led4, duty_ratio);
}
POLL_EXPORT(pwm_led_poll, PWM_LED_POLL_PERIOD_MS);
