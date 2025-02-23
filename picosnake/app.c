#include "hardware/adc.h"
#include "hardware/spi.h"

#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "framebuffer.h"
#include "ledmatrix.h"
#include "rtos.h"
#include "serial.h"
#include "snake.h"

#define COLOR 1
#define MOVEMENT_DELAY_MS ((uint8_t) 150u)
#define INITIAL_DIRECTION SNAKE_DIRECTION_RIGHT

#define CMD_UP "cmd:u"
#define CMD_LEFT "cmd:l"
#define CMD_RIGHT "cmd:r"
#define CMD_DOWN "cmd:d"
#define CMD_RESET "cmd:reset"

static Snake snake;
static FrameBuffer fb;
static volatile uint8_t move_direction = INITIAL_DIRECTION;

static void init_random()
{
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
    uint16_t raw = adc_read();
    srand(raw);
}

static void reset_game()
{
    snake_init_board();
    snake_init(&snake, snake_at(1, 1), 2, snake_get_direction_fn(INITIAL_DIRECTION));
    snake_spawn_food_random(&snake);
}

static void on_message(const char* message)
{
    uint8_t next_direction;
    if (strcmp(message, CMD_UP) == 0) {
        next_direction = SNAKE_DIRECTION_UP;

        printf("log: moving up...\r\n");
        serial_printf("ok:%s\r\n", message);
    } else if (strcmp(message, CMD_DOWN) == 0) {
        next_direction = SNAKE_DIRECTION_DOWN;

        printf("log: moving down...\r\n");
        serial_printf("ok:%s\r\n", message);
    } else if (strcmp(message, CMD_LEFT) == 0) {
        next_direction = SNAKE_DIRECTION_LEFT;

        printf("log: moving left...\r\n");
        serial_printf("ok:%s\r\n", message);
    } else if (strcmp(message, CMD_RIGHT) == 0) {
        next_direction = SNAKE_DIRECTION_RIGHT;

        printf("log: moving right...\r\n");
        serial_printf("ok:%s\r\n", message);
    } else if (strcmp(message, CMD_RESET) == 0) {
        move_direction = INITIAL_DIRECTION;
        reset_game();
    }

    if (snake_can_move(&snake, snake_get_direction_fn(next_direction))) {
        move_direction = next_direction;
    }
}

void app_task(void* parameters)
{
    init_random();

    register_on_message_callback(on_message);

    reset_game();

    fb_init_from_buffer(fb, NUM_COLUMNS, NUM_ROWS, snake_buffer());

    lm_init();

    serial_printf("game initialized.\r\n");

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(MOVEMENT_DELAY_MS / 2));

        lm_write_register_from_framebuffer(&fb);

        snake_move_and_check(&snake, snake_get_direction_fn(move_direction));

        vTaskDelay(pdMS_TO_TICKS(MOVEMENT_DELAY_MS / 2));
    }
}
