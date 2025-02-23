#include "hal.h"
#include "ledmatrix.h"

#include <string.h>

SPI_HandleTypeDef hspi1;

/**
 * SPI1 GPIO Configuration
 * PA5 ------> SPI1_SCK
 * PA7 ------> SPI1_MOSI
 */

const uint8_t CMD_NOOP = 0;
const uint8_t CMD_DIGIT0 = 1; // Goes up to 8, for each line
const uint8_t CMD_DECODEMODE = 9;
const uint8_t CMD_BRIGHTNESS = 10;
const uint8_t CMD_SCANLIMIT = 11;
const uint8_t CMD_SHUTDOWN = 12;
const uint8_t CMD_DISPLAYTEST = 15;

static inline void cs_select()
{
    asm volatile("nop \n nop \n nop");
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET); // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect()
{
    asm volatile("nop \n nop \n nop");
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
    asm volatile("nop \n nop \n nop");
}

void lm_write_register_all(uint8_t reg, uint8_t data)
{
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;

    cs_select();
    for (int i = 0; i < NUM_MODULES; i++) {
        HAL_SPI_Transmit(&hspi1, buf, 2, HAL_MAX_DELAY);
    }
    cs_deselect();
}

void lm_write_register(uint8_t reg, uint8_t* data, size_t size)
{
    uint8_t buf[2];
    buf[0] = reg;

    cs_select();
    for (uint8_t i = 0; i < size; i++) {
        buf[1] = data[size - 1 - i];
        HAL_SPI_Transmit(&hspi1, buf, 2, HAL_MAX_DELAY);
    }
    cs_deselect();
}

void lm_write_register_from_framebuffer(FrameBuffer* fb)
{
    // 1 byte = 8 bits = 1 pixel in the framebuffer
    size_t col_in_bytes = fb->width / 8;

    for (uint16_t row = 0; row < fb->height; row++) {
        uint8_t row_data[col_in_bytes];
        memset(row_data, 0, col_in_bytes);

        for (uint8_t col_byte_index = 0; col_byte_index < col_in_bytes; col_byte_index++) {
            uint8_t* pixel_ptr = &fb->pixels[row * fb->width + col_byte_index * 8];
            /* clang-format off */
            // Note that endianness matters here
            row_data[col_byte_index] =
                pixel_ptr[0] << 0u |
                pixel_ptr[1] << 1u |
                pixel_ptr[2] << 2u |
                pixel_ptr[3] << 3u |
                pixel_ptr[4] << 4u |
                pixel_ptr[5] << 5u |
                pixel_ptr[6] << 6u |
                pixel_ptr[7] << 7u;
            /* clang-format on */
        }

        lm_write_register(CMD_DIGIT0 + fb->height - row - 1, row_data, fb->width / 8);
    }
}

void lm_init()
{
    // Send init sequence to device
    lm_write_register_all(CMD_SHUTDOWN, 0);
    lm_write_register_all(CMD_DISPLAYTEST, 0);
    lm_write_register_all(CMD_SCANLIMIT, 7); // Use all lines
    lm_write_register_all(CMD_DECODEMODE, 0); // No BCD decode, just use bit pattern.
    lm_write_register_all(CMD_SHUTDOWN, 1);
    lm_write_register_all(CMD_BRIGHTNESS, MIN_BRIGHTNESS);

    // Clear the matrix
    for (int i = 0; i < NUM_ROWS; i++) {
        lm_write_register_all(CMD_DIGIT0 + i, 0);
    }
}
