#include <string.h>

#include "hardware/spi.h"

#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "framebuffer.h"
#include "ledmatrix.h"

/**
 * SPI0 GPIO Configuration
 *
 * GP17 (pin 22) ------> Chip Select    ------> CS on Max7219 board
 * GP18 (pin 24) ------> SCK / SPI0_SCK ------> CLK on Max7219 board
 * GP19 (pin 25) ------> MOSI / SPI0_TX ------> DIN on Max7219 board
 **/

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
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0); // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
    asm volatile("nop \n nop \n nop");
}

void lm_write_register_all(uint8_t reg, uint8_t data)
{
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;

    cs_select();
    for (int i = 0; i < NUM_MODULES; i++) {
        spi_write_blocking(spi_default, buf, 2);
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
        spi_write_blocking(spi_default, buf, 2);
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

static void lm_gpio_init()
{
    // Use SPI0 at 10MHz
    spi_init(spi_default, 10 * 1000 * 1000);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    // Make the SPI pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI));

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);

    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(PICO_DEFAULT_SPI_CSN_PIN, "SPI CS"));
}

void lm_init()
{
    lm_gpio_init();

    // Send init sequence to device
    lm_write_register_all(CMD_SHUTDOWN, 0);
    lm_write_register_all(CMD_DISPLAYTEST, 0);
    lm_write_register_all(CMD_SCANLIMIT, 7); // Use all lines
    lm_write_register_all(CMD_DECODEMODE, 0); // No BCD decode, just use bit pattern.
    lm_write_register_all(CMD_SHUTDOWN, 1);
    lm_write_register_all(CMD_BRIGHTNESS, MIN_BRIGHTNESS);
}
