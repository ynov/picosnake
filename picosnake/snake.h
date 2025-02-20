#ifndef SNAKE_H
#define SNAKE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define HEIGHT 8
#define WIDTH 32

#define MAX_SNAKE_LENGTH (HEIGHT * WIDTH)

#define SNAKE_DIRECTION_UP ((uint8_t) 1u)
#define SNAKE_DIRECTION_DOWN ((uint8_t) 2u)
#define SNAKE_DIRECTION_LEFT ((uint8_t) 3u)
#define SNAKE_DIRECTION_RIGHT ((uint8_t) 4u)
#define SNAKE_DIRECTION_STOP ((uint8_t) 0u)

#define SNAKE_DIRECTION_UP_STR "up"
#define SNAKE_DIRECTION_DOWN_STR "down"
#define SNAKE_DIRECTION_LEFT_STR "left"
#define SNAKE_DIRECTION_RIGHT_STR "right"
#define SNAKE_DIRECTION_STOP_STR "stop"

typedef struct SnakePoint {
    uint16_t x, y;
    bool is_body;
    bool is_food;
    struct SnakePoint* next;
} SnakePoint;

typedef struct Snake {
    SnakePoint* head;
    SnakePoint* tail;
    uint16_t length;
} Snake;

SnakePoint* snake_at(uint16_t x, uint16_t y);
SnakePoint* snake_up(SnakePoint* pt);
SnakePoint* snake_down(SnakePoint* pt);
SnakePoint* snake_right(SnakePoint* pt);
SnakePoint* snake_left(SnakePoint* pt);

void snake_init_board();
void snake_init(Snake* snake, SnakePoint* starting_point, int grow_num, SnakePoint* (*direction_fn)(SnakePoint*) );
void snake_grow(Snake* snake, SnakePoint* (*direction_fn)(SnakePoint*) );
void snake_move(Snake* snake, SnakePoint* (*direction_fn)(SnakePoint*) );
void snake_spawn_food_random(Snake* snake);
void snake_spawn_food(uint16_t x, uint16_t y);
void snake_print_board();
bool snake_can_move(Snake* snake, SnakePoint* (*direction_fn)(SnakePoint*) );
SnakePoint* (*snake_get_direction_fn(uint8_t direction))(SnakePoint*);
SnakePoint* (*snake_get_direction_fn_str(const char* direction))(SnakePoint*);

void snake_board_to_buffer(uint8_t* dest);

#ifdef __cplusplus
}
#endif

#endif
