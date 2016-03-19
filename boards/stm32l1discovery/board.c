/*
 * Copyright (C) 2016 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32l1discovery
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32L1Discovery board
 *
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"


void board_init(void)
{
    /* initialize the boards LEDs */
    gpio_init(LED_BLUE_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN, GPIO_OUT);
    /* initialize the CPU */
    cpu_init();
}
