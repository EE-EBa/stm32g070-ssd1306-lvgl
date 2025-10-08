#ifndef BSP_H
#define BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
/* public functions */
void bsp_init(void);
void eio_pin_driver_init(void);

uint32_t elab_time_ms(void);
#ifdef __cplusplus
}
#endif
#endif
