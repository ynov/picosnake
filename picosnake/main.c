#include <stdio.h>

#include "FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>

#include "pico/stdio.h"
#include "pico/stdlib.h"

extern void serial_task(void* parameters);
extern void app_task(void* parameters);

int main()
{
    stdio_init_all();

    xTaskCreate(serial_task, "serial_task", 4096, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(app_task, "app_task", 4096, NULL, tskIDLE_PRIORITY, NULL);

    vTaskStartScheduler();

    while (true) { }
}
