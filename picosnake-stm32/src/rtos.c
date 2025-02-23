#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "hal.h"
#include "serial.h"

// Add these required FreeRTOS callback functions
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
    serial_printf("Stack overflow in task: %s\r\n", pcTaskName);
    while (true) { }
}

void vApplicationMallocFailedHook(void)
{
    serial_printf("Malloc failed!\r\n");
    while (true) { }
}

void vApplicationIdleHook(void) { }

void vApplicationTickHook(void) { }
