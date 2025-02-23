#ifndef LEDMATRIX_H
#define LEDMATRIX_H

#include <stdint.h>
#include <stddef.h>

#include "framebuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STM32F4
#define CS_PORT GPIOA
#define CS_PIN GPIO_PIN_1
#endif

#define NUM_MODULES 4
#define NUM_ROWS 8
#define NUM_COLUMNS (NUM_MODULES * 8)

#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 2

extern const uint8_t CMD_NOOP;
extern const uint8_t CMD_DIGIT0;
extern const uint8_t CMD_DECODEMODE;
extern const uint8_t CMD_BRIGHTNESS;
extern const uint8_t CMD_SCANLIMIT;
extern const uint8_t CMD_SHUTDOWN;
extern const uint8_t CMD_DISPLAYTEST;

void lm_init();
void lm_write_register_all(uint8_t reg, uint8_t data);
void lm_write_register(uint8_t reg, uint8_t* data, size_t size);
void lm_write_register_from_framebuffer(FrameBuffer* fb);

#ifdef __cplusplus
}
#endif

#endif
