/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_axavior AxAvior Module
 * @ingroup     boards
 * @brief       Board specific files for the AxAvior Module.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the AxAvior module.
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Define the nominal CPU core clock in this board
 */
#define F_CPU               CLOCK_CORECLOCK

/**
 * @name xtimer configuration
 * @{
 */
#define XTIMER              TIMER_DEV(0)
#define XTIMER_CHAN         (0)
#define XTIMER_OVERHEAD     (6)
#define XTIMER_BACKOFF      (3)
#define XTIMER_SHOOT_EARLY  (3)
#define XTIMER_MASK         (0xffff0000)
/** @} */

/**
 * @name Define the UART to be used as stdio, its baudrate, and the size of
 *       receiving ringbuffer
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name Define the interface to the AT86RF212B radio
 * @{
 */
#define AT86RF212B_SPI       SPI_1
#define AT86RF212B_CS        GPIO(PORT_B,12) // SPI CS
#define AT86RF212B_INT       GPIO(PORT_C,10) // IO3
#define AT86RF212B_RESET     GPIO(PORT_C,7)  // IO0
#define AT86RF212B_SLEEP     GPIO(PORT_C,9)  // IO2
#define AT86RF212B_SPI_CLK   SPI_SPEED_5MHZ
/** @} */

/**
 * @name Define the interface for the connected flash memory
 * @{
 */
#define EXTFLASH_SPI        SPI_1
#define EXTFLASH_CS         GPIO_PIN(PORT_A,11)
#define EXTFLASH_WRITE      GPIO_PIN(PORT_C,6)
#define EXTFLASH_HOLD       GPIO_PIN(PORT_C,9)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_GREEN_PORT      (GPIOB)
#define LED_GREEN_PIN       (3)
#define LED_GREEN_GPIO      GPIO_PIN(PORT_B,3)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_GREEN_ON        (LED_GREEN_PORT->ODR &= ~(1<<LED_GREEN_PIN))
#define LED_GREEN_OFF       (LED_GREEN_PORT->ODR |= (1<<LED_GREEN_PIN))
#define LED_GREEN_TOGGLE    (LED_GREEN_PORT->ODR ^= (1<<LED_GREEN_PIN))
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
