#include <stdio.h>
#include <string.h>

#include "rtos.h"

#include "pico/stdio.h"
#include "pico/stdlib.h"

#define UART_BAUD_RATE 19200

#define BUFFER_SIZE 1024
#define MAX_CALLBACKS 8
#define TX_QUEUE_SIZE 32

static QueueHandle_t tx_queue = NULL;
static uint8_t num_callbacks = 0;
static void (*on_message_callbacks[MAX_CALLBACKS])(const char* message) = { NULL };

void register_on_message_callback(void (*callback)(const char* message))
{
    if (num_callbacks >= MAX_CALLBACKS) {
        return;
    }

    on_message_callbacks[num_callbacks++] = callback;
}

void serial_printf(const char* str, ...)
{
    static uint8_t buffer[BUFFER_SIZE] = { 0 };

    if (tx_queue == NULL) {
        tx_queue = xQueueCreate(TX_QUEUE_SIZE, BUFFER_SIZE);
    }

    va_list args;
    va_start(args, str);
    vsprintf(buffer, str, args);
    va_end(args);

    xQueueSend(tx_queue, buffer, 0);

    buffer[0] = '\0';
}

static void on_message_received(const char* message)
{
    for (uint8_t i = 0; i < num_callbacks; i++) {
        on_message_callbacks[i](message);
    }
}

static void serial_event()
{
    static uint8_t rx_buffer[BUFFER_SIZE];
    static uint8_t tx_buffer[BUFFER_SIZE];
    static uint16_t rx_buffer_index = 0;
    static TickType_t last_rx_buffer_not_empty = 0;

    if (uart_is_readable(uart0)) {
        last_rx_buffer_not_empty = xTaskGetTickCount();
        uint8_t ch = uart_getc(uart0);

        if (rx_buffer_index >= BUFFER_SIZE) {
            rx_buffer_index = 0;
        }

        if (ch == '\b') {
            if (rx_buffer_index > 0) {
                rx_buffer_index--;
                rx_buffer[rx_buffer_index] = '\0';
            }
        } else if (ch == '\r' || ch == '\n') {
            rx_buffer[rx_buffer_index] = '\0';
            rx_buffer_index = 0;

            on_message_received((const char*) rx_buffer);
        } else if (ch != '\r' && ch != '\n') {
            rx_buffer[rx_buffer_index++] = ch;
        }
    }

    // If we haven't read anything for a while, reset the buffer
    if (rx_buffer_index > 0 && xTaskGetTickCount() - last_rx_buffer_not_empty > 1000) {
        rx_buffer_index = 0;
    }

    if (tx_queue != NULL && xQueueReceive(tx_queue, tx_buffer, 0) == pdTRUE) {
        uart_puts(uart0, tx_buffer);
    }
}

static void serial_gpio_init()
{
    uart_init(uart0, UART_BAUD_RATE);

    /**
     * UART0 GPIO Configuration
     * GP12 ------> UART0 TX
     * GP13 ------> UART0 RX
     */
    gpio_set_function(12, UART_FUNCSEL_NUM(uart0, 12));
    gpio_set_function(13, UART_FUNCSEL_NUM(uart0, 13));
    // Enable pull-up on RX pin
    gpio_set_pulls(13, true, false);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(uart0, false, false);
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
}

void serial_task(void* params)
{
    serial_gpio_init();
    serial_printf("serial initialized.\r\n");

    while (true) {
        serial_event();
        vTaskDelay(1);
    }
}
