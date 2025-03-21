#include <stdbool.h>

#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "rtos.h"
#include "serial.h"

extern void app_task(void* parameters);

int main()
{
    stdio_init_all();

    xTaskCreate(serial_task, "serial_task", 1024, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(app_task, "app_task", 1024, NULL, tskIDLE_PRIORITY, NULL);

    vTaskStartScheduler();

    while (true) { }
}
