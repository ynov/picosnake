#include <Arduino.h>

#include "wifi.h"

#define RX_BUFFER_SIZE 4096
#define RX_BUFFER_RESET_CYCLE 10000
#define UART_BAUD_RATE 19200

static char rx_buffer[RX_BUFFER_SIZE];
static uint16_t rx_buffer_index, rx_buffer_not_empty_cycle = 0;

static inline void reset_buffer()
{
    rx_buffer_index = 0;
    rx_buffer_not_empty_cycle = 0;
}

void setup()
{
    // Enable pull-up on RX pin
    pinMode(3, INPUT_PULLUP);

    delay(50);

    Serial.begin(UART_BAUD_RATE);
    Serial.printf("log: PicoSnake server (ESP8266)!\r\n");

    wifi_setup();
}

void serialEvent()
{
    while (Serial.available()) {
        char ch = Serial.read();
        if (ch == '\n') {
            rx_buffer[rx_buffer_index] = '\0';
            Serial.printf("log: %s\r\n", rx_buffer);
            reset_buffer();
        } else if (ch != '\r' && ch != '\n') {
            rx_buffer[rx_buffer_index++] = ch;
        }
    }
}

void loop()
{
    wifi_main();

    // After RX_BUFFER_RESET_CYCLE (10,000) loop cycles of non empty buffer, reset the buffer
    if (rx_buffer_index > 0) {
        if (rx_buffer_not_empty_cycle > RX_BUFFER_RESET_CYCLE) {
            reset_buffer();
        } else {
            rx_buffer_not_empty_cycle++;
        }
    }
}
