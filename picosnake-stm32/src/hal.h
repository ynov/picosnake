#ifndef __HAL_H
#define __HAL_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

extern UART_HandleTypeDef huart1;
extern SPI_HandleTypeDef hspi1;

void hal_init();
void uart_init();
void gpio_init();
void spi_init();

#ifdef __cplusplus
}
#endif

#endif
