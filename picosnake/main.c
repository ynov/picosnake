#include <stdio.h>

#include "rtos.h"

#include "pico/stdio.h"
#include "pico/stdlib.h"

extern void serial_task(void* parameters);
extern void app_task(void* parameters);

int main()
{
    stdio_init_all();

    xTaskCreate(serial_task, "serial_task", 1024, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(app_task, "app_task", 1024, NULL, tskIDLE_PRIORITY, NULL);

    vTaskStartScheduler();

    while (true) { }
}
