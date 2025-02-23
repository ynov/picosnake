#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rtos.h"
#include "serial.h"

#define BUFFER_SIZE 1024
#define MAX_CALLBACKS 8
#define TX_QUEUE_SIZE 32

UART_HandleTypeDef huart1;

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

void serial_printf(const char* format, ...)
{
    static uint8_t buffer[BUFFER_SIZE] = { 0 };

    if (tx_queue == NULL) {
        tx_queue = xQueueCreate(TX_QUEUE_SIZE, BUFFER_SIZE);
    }

    va_list args;
    va_start(args, format);
    vsprintf((char*) buffer, format, args);
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

void serial_event()
{
    static uint8_t rx_buffer[BUFFER_SIZE];
    static uint8_t tx_buffer[BUFFER_SIZE];
    static uint8_t ch_buff[1], ch;
    static uint16_t rx_buffer_index = 0;
    static TickType_t last_rx_buffer_not_empty = 0;

    while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) {
        vTaskDelay(1);
    }

    HAL_StatusTypeDef r = HAL_UART_Receive(&huart1, (uint8_t*) ch_buff, sizeof(ch_buff), 1);

    if (r == HAL_OK) {
        ch = ch_buff[0];
        last_rx_buffer_not_empty = xTaskGetTickCount();

        if (rx_buffer_index >= BUFFER_SIZE) {
            rx_buffer_index = 0;
        }

        if (ch == '\b') {
            if (rx_buffer_index > 0) {
                rx_buffer_index--;
                rx_buffer[rx_buffer_index] = '\0';
            }
        } else if ((ch == '\r' || ch == '\n') && rx_buffer_index > 0) {
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

    if (xQueueReceive(tx_queue, tx_buffer, 0) == pdTRUE) {
        HAL_UART_Transmit(&huart1, tx_buffer, strlen((char*) tx_buffer), HAL_MAX_DELAY);
    }
}

void serial_task(void* parameters)
{
    serial_printf("serial_task started\r\n");

    while (true) {
        serial_event();
    }
}
