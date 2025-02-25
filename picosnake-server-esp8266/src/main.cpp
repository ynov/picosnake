#include <Arduino.h>

#include "wifi.h"

#define RX_BUFFER_SIZE 4096
#define RX_BUFFER_RESET_CYCLE 10000
#define UART_BAUD_RATE 19200

void serial_task(void* parameters)
{
    static char rx_buffer[RX_BUFFER_SIZE];
    static uint16_t rx_buffer_index = 0;
    static TickType_t last_rx_buffer_not_empty = 0;

    while (true) {
        if (Serial.available()) {
            char ch = Serial.read();
            last_rx_buffer_not_empty = xTaskGetTickCount();

            if (rx_buffer_index >= RX_BUFFER_SIZE) {
                rx_buffer_index = 0;
            }

            if (ch == '\n') {
                rx_buffer[rx_buffer_index] = '\0';
                rx_buffer_index = 0;

                Serial.printf("log: %s\r\n", rx_buffer);
            } else if (ch != '\r' && ch != '\n') {
                rx_buffer[rx_buffer_index++] = ch;
            }
        } else {
            vTaskDelay(1);
        }

        // If we haven't read anything for a while, reset the buffer
        if (rx_buffer_index > 0 && xTaskGetTickCount() - last_rx_buffer_not_empty > 1000) {
            rx_buffer_index = 0;
        }
    }
}

void setup()
{
    // Enable pull-up on RX pin (ESP32-S3-DevKitC-1)
    pinMode(44, INPUT_PULLUP);

    Serial.begin(UART_BAUD_RATE);
    Serial.printf("log: PicoSnake server (ESP32)!\r\n");

    xTaskCreate(wifi_task, "wifi_task", 8192, NULL, 1, NULL);
    xTaskCreate(serial_task, "serial_task", 4096, NULL, 1, NULL);
}

void loop() { }
