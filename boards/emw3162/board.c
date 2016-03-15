/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_emw3162
 * @{
 *
 * @file
 * @brief       Board specific implementations for the EMW3162 board
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "board.h"

static void leds_init(void);

void board_init(void)
{
    /* initialize the boards LEDs, this is done first for debugging purposes */
    leds_init();

    /* initialize the CPU */
    cpu_init();
}

/**
 * @brief Initialize the boards on-board LEDs (D1 and D2)
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - D1: PB0
 * - D2: PB1
 */
static void leds_init(void)
{
#if 0
    /* enable clock for port GPIOD */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* configure pins as general outputs */
    LED_PORT->MODER &= ~(0x0000000f);
    LED_PORT->MODER |= 0x00000005;
    /* set output speed high-speed */
    LED_PORT->OSPEEDR |= 0x0000000f;
    /* set output type to push-pull */
    LED_PORT->OTYPER &= ~(0xf000);
    /* disable pull resistors */
    LED_PORT->PUPDR &= ~(0xff000000);

    /* turn all LEDs off */
    LED_PORT->BSRRH = 0x00000003;
#endif
}