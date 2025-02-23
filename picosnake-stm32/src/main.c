#include <stdbool.h>

#include "hal.h"
#include "serial.h"
#include "rtos.h"

extern void app_task(void* parameters);

int main(void)
{
    hal_init();

    xTaskCreate(serial_task, "serial_task", 1024, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(app_task, "app_task", 1024, NULL, tskIDLE_PRIORITY, NULL);

    vTaskStartScheduler();

    while (true) { }
}
