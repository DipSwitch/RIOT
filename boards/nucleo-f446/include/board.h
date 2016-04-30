/*
 * Copyright (C) 2016 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_nucleo-f446 Nucleo-F446
 * @ingroup     boards
 * @brief       Board specific files for the nucleo-f446 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nucleo-f446 board
 *
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name xtimer configuration
 * @{
 */
#define XTIMER              TIMER_0
#define XTIMER_CHAN         (0)
#define XTIMER_OVERHEAD     (6)
#define XTIMER_BACKOFF      (5)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
