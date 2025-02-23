#ifndef SERIAL_H
#define SERIAL_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

extern UART_HandleTypeDef huart1;

void serial_task(void* parameters);
void serial_printf(const char* format, ...);
void register_on_message_callback(void (*callback)(const char* message));

#ifdef __cplusplus
}
#endif

#endif
