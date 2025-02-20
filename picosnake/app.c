#include <FreeRTOS.h>
#include <task.h>
#include <stdlib.h>

#include "hardware/adc.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "framebuffer.h"
#include "ledmatrix.h"
#include "serial.h"
#include "snake.h"

#define COLOR 1
#define INITIAL_DIRECTION SNAKE_DIRECTION_RIGHT

static Snake snake;
static FrameBuffer fb;
static uint16_t delay_ms = 150;
static volatile uint8_t move_direction = INITIAL_DIRECTION;

static void init_random()
{
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
    uint16_t raw = adc_read();
    srand(raw);
}

static void on_message(const char* message)
{
    uint8_t next_direction;
    if (strcmp(message, "cmd:move_up") == 0) {
        next_direction = SNAKE_DIRECTION_UP;

        printf("log: moving up...\r\n");
        uart_printf(uart0, "ok:%s\r\n", message);
    } else if (strcmp(message, "cmd:move_down") == 0) {
        next_direction = SNAKE_DIRECTION_DOWN;

        printf("log: moving down...\r\n");
        uart_printf(uart0, "ok:%s\r\n", message);
    } else if (strcmp(message, "cmd:move_left") == 0) {
        next_direction = SNAKE_DIRECTION_LEFT;

        printf("log: moving left...\r\n");
        uart_printf(uart0, "ok:%s\r\n", message);
    } else if (strcmp(message, "cmd:move_right") == 0) {
        next_direction = SNAKE_DIRECTION_RIGHT;

        printf("log: moving right...\r\n");
        uart_printf(uart0, "ok:%s\r\n", message);
    }

    if (snake_can_move(&snake, snake_get_direction_fn(next_direction))) {
        move_direction = next_direction;
    }
}



void app_task(void* parameters)
{
    init_random();
    register_on_message_callback(on_message);

    fb_init(fb, NUM_COLUMNS, NUM_ROWS);

    snake_init_board();
    snake_init(&snake, snake_at(1, 1), 2, snake_get_direction_fn(INITIAL_DIRECTION));
    snake_board_to_buffer(fb.pixels);

    snake_spawn_food_random(&snake);

    lm_init();
    lm_write_register_from_framebuffer(&fb);

    while (true) {
        snake_board_to_buffer(fb.pixels);
        lm_write_register_from_framebuffer(&fb);

        if (snake_can_move(&snake, snake_get_direction_fn(move_direction))) {
            if (snake_get_direction_fn(move_direction)(snake.head)->is_food) {
                snake_spawn_food_random(&snake);
            }

            snake_move(&snake, snake_get_direction_fn(move_direction));
        }

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}
