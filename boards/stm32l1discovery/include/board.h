/*
 * Copyright (C) 2016 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_stm32l1discovery stm32l1discovery
 * @ingroup     boards
 * @brief       Board specific files for the stm32l1discovery board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the stm32l1discovery  board.
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
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED_BLUE_PIN        GPIO_PIN(PORT_B, 6)
#define LED_GREEN_PIN       GPIO_PIN(PORT_B, 7)
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
