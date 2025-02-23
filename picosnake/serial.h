#ifndef PICOSNAKE_SERIAL_H
#define PICOSNAKE_SERIAL_H

#include "pico/stdio.h"
#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

void serial_task(void* params);
void serial_printf(const char* str, ...);
void register_on_message_callback(void (*callback)(const char* message));

#ifdef __cplusplus
}
#endif

#endif
