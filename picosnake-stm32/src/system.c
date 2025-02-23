#include "hal.h"
#include "rtos.h"

extern void xPortSysTickHandler(void);

void SysTick_Handler(void)
{
    /* The SysTick runs at the lowest interrupt priority, so when this interrupt
    executes, we can be sure that it is time to perform a context switch */
    HAL_IncTick();
#if (INCLUDE_xTaskGetSchedulerState == 1)
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
#endif /* INCLUDE_xTaskGetSchedulerState */
        xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState == 1)
    }
#endif /* INCLUDE_xTaskGetSchedulerState */
}
