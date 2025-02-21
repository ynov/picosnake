#include <FreeRTOS.h>
#include <task.h>

#include <stdio.h>
#include <string.h>

#include "pico/stdio.h"
#include "pico/stdlib.h"

#define UART_BAUD_RATE 19200
#define RX_BUFFER_SIZE 4096
#define LEN(x) (sizeof(x) / sizeof(x[0]))
#define MAX_CALLBACKS 8

static uint8_t num_callbacks = 0;
static void (*on_message_callbacks[MAX_CALLBACKS])(const char* message) = { NULL };

void register_on_message_callback(void (*callback)(const char* message))
{
    if (num_callbacks >= MAX_CALLBACKS) {
        return;
    }

    on_message_callbacks[num_callbacks++] = callback;
}

void uart_printf(uart_inst_t* uart, const char* str, ...)
{
    static uint8_t buffer[4096];

    va_list args;
    va_start(args, str);
    vsprintf(buffer, str, args);
    va_end(args);

    uart_puts(uart, buffer);
}

static void on_message_received(const char* message)
{
    for (uint8_t i = 0; i < num_callbacks; i++) {
        on_message_callbacks[i](message);
    }
}

static void uart_event()
{
    static uint8_t buffer[RX_BUFFER_SIZE];
    static uint16_t buffer_index = 0;
    static TickType_t last_buffer_not_empty = 0;

    while (uart_is_readable(uart0)) {
        last_buffer_not_empty = xTaskGetTickCount();
        uint8_t ch = uart_getc(uart0);

        if (buffer_index >= LEN(buffer)) {
            buffer_index = 0;
        }

        if (ch == '\b') {
            if (buffer_index > 0) {
                buffer_index--;
                buffer[buffer_index] = '\0';
            }
        } else if (ch == '\r' || ch == '\n') {
            buffer[buffer_index] = '\0';
            buffer_index = 0;

            on_message_received(buffer);
        } else if (ch != '\r' && ch != '\n') {
            buffer[buffer_index++] = ch;
        }
    }

    // If we haven't read anything for a while, reset the buffer
    if (buffer_index > 0 && xTaskGetTickCount() - last_buffer_not_empty > 1000) {
        buffer_index = 0;
    }
}

static void serial_init()
{
    uart_init(uart0, UART_BAUD_RATE);

    gpio_set_function(12, UART_FUNCSEL_NUM(uart0, 12));
    gpio_set_function(13, UART_FUNCSEL_NUM(uart0, 13));
    // Enable pull-up on RX pin
    gpio_set_pulls(13, true, false);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(uart0, false, false);
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);

    uart_printf(uart0, "UART0 initialized.\r\n");
}

void serial_task(void* params)
{
    serial_init();

    while (true) {
        uart_event();
        vTaskDelay(1);
    }
}
