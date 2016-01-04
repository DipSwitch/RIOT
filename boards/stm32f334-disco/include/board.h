/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_stm32f334-disco STM32F334Discovery
 * @ingroup     boards
 * @brief       Board specific files for the STM32F334-DISCO board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F334-DISCO evaluation board
 *
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
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
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               (72000000UL)

/**
 * @name Define the UART used for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            GPIOB
#define LD3_PIN             (1 << 6)
#define LD4_PIN             (1 << 8)
#define LD5_PIN             (1 << 9)
#define LD6_PIN             (1 << 7)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LD3_ON              (LED_PORT->BSRRL = LD3_PIN)
#define LD3_OFF             (LED_PORT->BSRRH = LD3_PIN)
#define LD3_TOGGLE          (LED_PORT->ODR ^= LD3_PIN)
#define LD4_ON              (LED_PORT->BSRRL = LD4_PIN)
#define LD4_OFF             (LED_PORT->BSRRH = LD4_PIN)
#define LD4_TOGGLE          (LED_PORT->ODR ^= LD4_PIN)
#define LD5_ON              (LED_PORT->BSRRL = LD5_PIN)
#define LD5_OFF             (LED_PORT->BSRRH = LD5_PIN)
#define LD5_TOGGLE          (LED_PORT->ODR ^= LD5_PIN)
#define LD6_ON              (LED_PORT->BSRRL = LD6_PIN)
#define LD6_OFF             (LED_PORT->BSRRH = LD6_PIN)
#define LD6_TOGGLE          (LED_PORT->ODR ^= LD6_PIN)

/* for compatability to other boards */
#define LED_RED_ON          LD3_ON
#define LED_RED_OFF         LD3_OFF
#define LED_RED_TOGGLE      LD3_TOGGLE
#define LED_ORANGE_ON       LD4_ON
#define LED_ORANGE_OFF      LD4_OFF
#define LED_ORANGE_TOGGLE   LD4_TOGGLE
#define LED_GREEN_ON        LD5_ON
#define LED_GREEN_OFF       LD5_OFF
#define LED_GREEN_TOGGLE    LD5_TOGGLE
#define LED_BLUE_ON         LD6_ON
#define LED_BLUE_OFF        LD6_OFF
#define LED_BLUE_TOGGLE     LD6_TOGGLE
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
