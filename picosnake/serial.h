#ifndef PICOSNAKE_SERIAL_H
#define PICOSNAKE_SERIAL_H

#include "pico/stdio.h"
#include "pico/stdlib.h"

void serial_task(void* params);
void register_on_message_callback(void (*callback)(const char* message));
void uart_printf(uart_inst_t* uart, const char* str, ...);

#endif
