#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FrameBuffer {
    uint16_t width;
    uint16_t height;
    bool is_heap_allocated;
    uint8_t* pixels;
} FrameBuffer;

#define fb_init(_fb, _width, _height)                                                                                  \
    uint8_t _pixels_##_width##_height[_height * _width];                                                               \
    do {                                                                                                               \
        _fb.width = _width;                                                                                            \
        _fb.height = _height;                                                                                          \
        _fb.pixels = _pixels_##_width##_height;                                                                        \
        _fb.is_heap_allocated = false;                                                                                 \
        memset(_fb.pixels, 0, _height* _width * sizeof(uint8_t));                                                      \
    } while (0)

#define fb_init_from_buffer(_fb, _width, _height, _buffer)                                                             \
    do {                                                                                                               \
        _fb.width = _width;                                                                                            \
        _fb.height = _height;                                                                                          \
        _fb.pixels = _buffer;                                                                                          \
        _fb.is_heap_allocated = false;                                                                                 \
    } while (0)

#define fb_heap_init(_fb, _width, _height)                                                                             \
    do {                                                                                                               \
        _fb.width = _width;                                                                                            \
        _fb.height = _height;                                                                                          \
        _fb.pixels = (uint8_t*) malloc(_height * _width * sizeof(uint8_t));                                            \
        _fb.is_heap_allocated = true;                                                                                  \
        memset(_fb.pixels, 0, _height* _width * sizeof(uint8_t));                                                      \
    } while (0)

#define fb_heap_free(_fb)                                                                                              \
    do {                                                                                                               \
        if (_fb.is_heap_allocated) {                                                                                   \
            free(_fb.pixels);                                                                                          \
            _fb.pixels = NULL;                                                                                         \
        }                                                                                                              \
    } while (0)

#define FB_DIRECTION_LEFT ((uint8_t) 0u)
#define FB_DIRECTION_RIGHT ((uint8_t) 1u)
#define FB_DIRECTION_UP ((uint8_t) 2u)
#define FB_DIRECTION_DOWN ((uint8_t) 3u)

void fb_set_pixel(FrameBuffer* fb, uint16_t x, uint16_t y, uint8_t color);
void fb_draw_line(FrameBuffer* fb, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color);
void fb_draw_rectangle(
    FrameBuffer* fb, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color, bool fill);
void fb_draw_pixels(FrameBuffer* fb, uint8_t* pixels, uint8_t width, uint8_t height, uint16_t pos_x);
void fb_get_slice(FrameBuffer* fb, uint8_t* slice_dest, uint16_t pos_x, uint16_t width, uint16_t height);
void fb_scroll(FrameBuffer* fb, uint16_t amount, uint8_t direction);
void fb_scroll_and_swap(FrameBuffer* fb, uint16_t amount, uint8_t direction);
void fb_clear(FrameBuffer* fb);
void fb_print_framebuffer(FrameBuffer* fb, const char*, const char*);

#ifdef __cplusplus
}
#endif

#endif
