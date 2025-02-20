#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snake.h"

static SnakePoint board[HEIGHT * WIDTH];

void snake_init_board()
{
    for (int row = 0; row < HEIGHT; row++) {
        for (int col = 0; col < WIDTH; col++) {
            SnakePoint* pt = &board[(HEIGHT - row - 1) * WIDTH + col];

            pt->x = col;
            pt->y = (HEIGHT - row - 1);

            pt->is_food = false;
            pt->is_body = false;

            pt->next = NULL;
        }
    }
}

void snake_print_board()
{
    static int count = 0;
    printf("Board %d:\n", count++);

    for (int row = 0; row < HEIGHT; row++) {
        for (int col = 0; col < WIDTH; col++) {
            if (board[(HEIGHT - row - 1) * WIDTH + col].is_food) {
                printf("* ");
            } else if (board[(HEIGHT - row - 1) * WIDTH + col].is_body) {
                printf("X ");
            } else {
                printf(". ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

SnakePoint* snake_at(uint16_t x, uint16_t y) { return &board[y * WIDTH + x]; }

SnakePoint* snake_up(SnakePoint* pt)
{
    if (pt->y == HEIGHT - 1) {
        return snake_at(pt->x, 0);
    }
    return snake_at(pt->x, pt->y + 1);
}

SnakePoint* snake_down(SnakePoint* pt)
{
    if (pt->y == 0) {
        return snake_at(pt->x, HEIGHT - 1);
    }
    return snake_at(pt->x, pt->y - 1);
}

SnakePoint* snake_right(SnakePoint* pt)
{
    if (pt->x == WIDTH - 1) {
        return snake_at(0, pt->y);
    }
    return snake_at(pt->x + 1, pt->y);
}

SnakePoint* snake_left(SnakePoint* pt)
{
    if (pt->x == 0) {
        return snake_at(WIDTH - 1, pt->y);
    }
    return snake_at(pt->x - 1, pt->y);
}

SnakePoint* snake_still(SnakePoint* pt) { return pt; }

void snake_grow(Snake* snake, SnakePoint* (*direction_fn)(SnakePoint*) )
{
    SnakePoint* next = direction_fn(snake->head);
    next->is_body = true;

    snake->head->next = next;
    if (next != NULL) {
        snake->head = next;
    }

    snake->length++;
}

void snake_move(Snake* snake, SnakePoint* (*direction_fn)(SnakePoint*) )
{
    SnakePoint* next = direction_fn(snake->head);
    next->is_body = true;

    snake->head->next = next;
    if (next != NULL) {
        snake->head = next;
    }

    if (next->is_food) {
        next->is_food = false;
        snake->length++;
    } else {
        snake->tail->is_body = false;
        snake->tail = snake->tail->next;
    }
}

void snake_spawn_food(uint16_t x, uint16_t y)
{
    snake_at(x, y)->is_food = true;
}

void snake_spawn_food_random(Snake* snake)
{
    if (snake->length == HEIGHT * WIDTH) {
        return;
    }

    while (true) {
        uint16_t x = rand() % WIDTH;
        uint16_t y = rand() % HEIGHT;

        if (!snake_at(x, y)->is_body && !snake_at(x, y)->is_food) {
            snake_spawn_food(x, y);
            break;
        }
    }
}

void snake_init(Snake* snake, SnakePoint* starting_point, int grow_num, SnakePoint* (*direction_fn)(SnakePoint*))
{
    snake->head = starting_point;
    snake->tail = starting_point;
    snake->length = 1;
    starting_point->is_body = true;

    for (int i = 0; i < grow_num; i++) {
        snake_grow(snake, direction_fn);
    }
}

bool snake_can_move(Snake* snake, SnakePoint* (*direction_fn)(SnakePoint*) )
{
    return !direction_fn(snake->head)->is_body;
}

SnakePoint* (*snake_get_direction_fn(uint8_t direction))(SnakePoint*)
{
    switch (direction) {
        case SNAKE_DIRECTION_UP:
            return snake_up;
        case SNAKE_DIRECTION_DOWN:
            return snake_down;
        case SNAKE_DIRECTION_LEFT:
            return snake_left;
        case SNAKE_DIRECTION_RIGHT:
            return snake_right;
        default:
            return snake_still;
    }
}

SnakePoint* (*snake_get_direction_fn_str(const char* direction))(SnakePoint*)
{
    if (strcmp(direction, SNAKE_DIRECTION_UP_STR) == 0) {
        return snake_up;
    } else if (strcmp(direction, SNAKE_DIRECTION_DOWN_STR) == 0) {
        return snake_down;
    } else if (strcmp(direction, SNAKE_DIRECTION_LEFT_STR) == 0) {
        return snake_left;
    } else if (strcmp(direction, SNAKE_DIRECTION_RIGHT_STR) == 0) {
        return snake_right;
    } else {
        return snake_still;
    }
}

void snake_board_to_buffer(uint8_t* dest)
{
    for (uint16_t y = 0; y < HEIGHT; y++) {
        for (uint16_t x = 0; x < WIDTH; x++) {
            SnakePoint* pt = snake_at(x, (HEIGHT - y - 1));
            dest[y * WIDTH + x] = pt->is_body || pt->is_food ? 1 : 0;
        }
    }
}
