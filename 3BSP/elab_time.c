/* includes */
#include "stdint.h"

#ifdef __cplusplus
extern "C"{
#endif

    /* private variables */
    static uint32_t sys_time_ms = 0;

    /* public functions */

    uint32_t elab_time_ms(void){
        return sys_time_ms;
    }

    /* private variables  */

    void SysTick_Handler(void){
        sys_time_ms ++;
    }

#ifdef __cplusplus
}
#endif
