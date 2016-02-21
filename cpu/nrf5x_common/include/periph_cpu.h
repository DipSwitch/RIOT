/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief           CPU specific definitions for handling peripherals
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef CPU_PERIPH_H
#define CPU_PERIPH_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Iron out some differences in register and IRQ channel naming between
 *          the different nRFx family members
 * @{
 */
#if defined(CPU_FAM_NRF51)
#define GPIO_BASE           (NRF_GPIO)
#define UART_IRQN           (UART0_IRQn)
#elif defined(CPU_FAM_NRF52)
#define GPIO_BASE           (NRF_P0)
#define UART_IRQN           (UARTE0_UART0_IRQn)
#else
#error "nrf5x_common: no valid value for CPU_FAM_XX defined"
#endif
/** @} */

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (8U)

/**
 * @brief   Override macro for defining GPIO pins
 *
 * The port definition is used (and zeroed) to suppress compiler warnings
 */
#define GPIO_PIN(x,y)       ((x & 0) | y)

/**
 * @brief   Override GPIO pull register select values
 * @{
 */
#define HAVE_GPIO_PP_T
typedef enum {
    GPIO_NOPULL = 0,        /**< do not use internal pull resistors */
    GPIO_PULLUP = 3,        /**< enable internal pull-up resistor */
    GPIO_PULLDOWN = 1       /**< enable internal pull-down resistor */
} gpio_pp_t;
/** @} */

/**
 * @brief   Override GPIO active flank values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_RISING  = 1,       /**< emit interrupt on rising flank */
    GPIO_BOTH    = 3        /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */


#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = 0,                 /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = SPI_CONFIG_CPHA_Msk,                 /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = SPI_CONFIG_CPOL_Msk,                 /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (SPI_CONFIG_CPOL_Msk | SPI_CONFIG_CPHA_Msk)                  /**< CPOL=1, CPHA=1 */
} spi_mode_t;

#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = SPI_FREQUENCY_FREQUENCY_K125,  /**< 100KHz */
    SPI_CLK_400KHZ = SPI_FREQUENCY_FREQUENCY_K500,  /**< 400KHz */
    SPI_CLK_1MHZ   = SPI_FREQUENCY_FREQUENCY_M1,    /**< 1MHz */
    SPI_CLK_5MHZ   = SPI_FREQUENCY_FREQUENCY_M4,    /**< 5MHz */
    SPI_CLK_10MHZ  = SPI_FREQUENCY_FREQUENCY_M8     /**< 10MHz */
} spi_clk_t;


typedef struct {
    NRF_SPI_Type *dev;
    uint8_t csk;
    uint8_t mosi;
    uint8_t miso;
} spi_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* CPU_PERIPH_H */
/** @} */
