#include "framebuffer.h"

#define min(a, b) ((a) < (b) ? (a) : (b))

void fb_set_pixel(FrameBuffer* fb, uint16_t x, uint16_t y, uint8_t color)
{
    if (x >= 0 && x < fb->width && y >= 0 && y < fb->height) {
        fb->pixels[(fb->height - y - 1) * fb->width + x] = color;
    }
}

void fb_draw_line(FrameBuffer* fb, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color)
{
    int16_t dx = abs(x2 - x1);
    int16_t dy = abs(y2 - y1);
    int16_t sx = (x1 < x2) ? 1 : -1;
    int16_t sy = (y1 < y2) ? 1 : -1;
    int16_t err = dx - dy;

    while (true) {
        fb_set_pixel(fb, x1, y1, color);

        if (x1 == x2 && y1 == y2) {
            break;
        }

        int16_t e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void fb_draw_rectangle(
    FrameBuffer* fb, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color, bool fill)
{
    if (fill) {
        for (uint16_t i = x; i < x + width; i++) {
            for (int16_t j = y; j < y + height; j++) {
                fb_set_pixel(fb, i, j, color);
            }
        }
    } else {
        for (int16_t i = x; i < x + width; i++) {
            fb_set_pixel(fb, i, y, color);
            fb_set_pixel(fb, i, y + height - 1, color);
        }
        for (int16_t j = y; j < y + height; j++) {
            fb_set_pixel(fb, x, j, color);
            fb_set_pixel(fb, x + width - 1, j, color);
        }
    }
}

void fb_draw_pixels(FrameBuffer* fb, uint8_t* pixels, uint8_t width, uint8_t height, uint16_t pos_x)
{
    if (pos_x < 0 || pos_x >= fb->width) {
        return;
    }

    int16_t copy_width = min(width, fb->width - pos_x);
    int16_t copy_height = min(height, fb->height);

    for (uint16_t y = 0; y < copy_height; y++) {
        uint8_t* dest = &fb->pixels[y * fb->width + pos_x];
        uint8_t* src = &pixels[y * width];
        memcpy(dest, src, copy_width);
    }
}

void fb_scroll(FrameBuffer* fb, uint16_t amount, uint8_t direction)
{
    /* clang-format off */
    if (amount <= 0
        || (direction == FB_DIRECTION_LEFT || direction == FB_DIRECTION_RIGHT) && amount >= fb->width
        || (direction == FB_DIRECTION_UP || direction == FB_DIRECTION_DOWN) && amount >= fb->height) {
        return;
    }
    /* clang-format on */

    switch (direction) {
    case FB_DIRECTION_LEFT:
        for (uint16_t y = 0; y < fb->height; y++) {
            for (uint16_t x = 0; x < fb->width - amount; x++) {
                fb->pixels[y * fb->width + x] = fb->pixels[y * fb->width + x + amount];
            }

            for (uint16_t x = fb->width - amount; x < fb->width; x++) {
                fb->pixels[y * fb->width + x] = 0;
            }
        }
        break;

    case FB_DIRECTION_RIGHT:
        for (uint16_t y = 0; y < fb->height; y++) {
            for (uint16_t x = fb->width - 1; x >= amount; x--) {
                fb->pixels[y * fb->width + x] = fb->pixels[y * fb->width + x - amount];
            }

            for (uint16_t x = 0; x < amount; x++) {
                fb->pixels[y * fb->width + x] = 0;
            }
        }
        break;

    case FB_DIRECTION_UP:
        for (uint16_t y = 0; y < fb->height - amount; y++) {
            memcpy(&fb->pixels[y * fb->width], &fb->pixels[(y + amount) * fb->width], fb->width);
        }

        memset(&fb->pixels[(fb->height - amount) * fb->width], 0, amount * fb->width);
        break;

    case FB_DIRECTION_DOWN:
        for (uint16_t y = fb->height - 1; y >= amount; y--) {
            memcpy(&fb->pixels[y * fb->width], &fb->pixels[(y - amount) * fb->width], fb->width);
        }

        memset(fb->pixels, 0, amount * fb->width);
        break;
    }
}

void fb_scroll_and_swap(FrameBuffer* fb, uint16_t amount, uint8_t direction)
{
    /* clang-format off */
    if (amount <= 0
        || (direction == FB_DIRECTION_LEFT || direction == FB_DIRECTION_RIGHT) && amount >= fb->width
        || (direction == FB_DIRECTION_UP || direction == FB_DIRECTION_DOWN) && amount >= fb->height) {
        return;
    }
    /* clang-format on */

    switch (direction) {
    case FB_DIRECTION_LEFT: {
        for (uint16_t y = 0; y < fb->height; y++) {
            uint8_t temp[amount];
            memcpy(temp, &fb->pixels[y * fb->width], amount);

            for (uint16_t x = 0; x < fb->width - amount; x++) {
                fb->pixels[y * fb->width + x] = fb->pixels[y * fb->width + x + amount];
            }

            memcpy(&fb->pixels[y * fb->width + (fb->width - amount)], temp, amount);
        }
        break;
    }

    case FB_DIRECTION_RIGHT: {
        for (uint16_t y = 0; y < fb->height; y++) {
            uint8_t temp[amount];
            memcpy(temp, &fb->pixels[y * fb->width + (fb->width - amount)], amount);

            for (uint16_t x = fb->width - 1; x >= amount; x--) {
                fb->pixels[y * fb->width + x] = fb->pixels[y * fb->width + x - amount];
            }

            memcpy(&fb->pixels[y * fb->width], temp, amount);
        }
        break;
    }

    case FB_DIRECTION_UP: {
        uint8_t temp[amount * fb->width];
        memcpy(temp, fb->pixels, amount * fb->width);

        for (uint16_t y = 0; y < fb->height - amount; y++) {
            memcpy(&fb->pixels[y * fb->width], &fb->pixels[(y + amount) * fb->width], fb->width);
        }

        memcpy(&fb->pixels[(fb->height - amount) * fb->width], temp, amount * fb->width);
        break;
    }

    case FB_DIRECTION_DOWN: {
        uint8_t temp[amount * fb->width];
        memcpy(temp, &fb->pixels[(fb->height - amount) * fb->width], amount * fb->width);

        for (uint16_t y = fb->height - 1; y >= amount; y--) {
            memcpy(&fb->pixels[y * fb->width], &fb->pixels[(y - amount) * fb->width], fb->width);
        }

        memcpy(fb->pixels, temp, amount * fb->width);
        break;
    }
    }
}

void fb_get_slice(FrameBuffer* fb, uint8_t* slice_dest, uint16_t pos_x, uint16_t width, uint16_t height)
{
    if (!fb || !slice_dest || pos_x < 0 || width <= 0 || height <= 0) {
        return;
    }

    if (pos_x + width > fb->width || height > fb->height) {
        return;
    }

    for (uint16_t y = 0; y < height; y++) {
        uint8_t* src = &fb->pixels[y * fb->width + pos_x];
        uint8_t* dest = &slice_dest[y * width];
        memcpy(dest, src, width);
    }
}

void fb_clear(FrameBuffer* fb) { memset(fb->pixels, 0, fb->width * fb->height); }

void fb_print_framebuffer(FrameBuffer* fb, const char* content, const char* blank)
{
    static char buffer[2048];

    for (uint16_t y = 0; y < fb->height; y++) {
        for (uint16_t x = 0; x < fb->width; x++) {
            sprintf(buffer, "%s", fb->pixels[y * fb->width + x] ? content : blank);
            printf("%s", buffer);
        }
        printf("\r\n");
    }
}
