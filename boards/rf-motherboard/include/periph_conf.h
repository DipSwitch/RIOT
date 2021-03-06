/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_rf-motherboard
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the rf-motherboard board
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 **/
#define CLOCK_HSE           (8000000U)              /* frequency of external oscillator */
#define CLOCK_CORECLOCK     (32000000U)             /* targeted core clock frequency */

/* configuration of PLL prescaler and multiply values */
/* CORECLOCK := HSI / PLL_HSI_DIV * PLL_HSI_MUL */
#define CLOCK_PLL_DIV       RCC_CFGR_PLLDIV2
#define CLOCK_PLL_MUL       RCC_CFGR_PLLMUL8
/* configuration of peripheral bus clock prescalers */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1      /* AHB clock -> 32MHz */
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1     /* APB2 clock -> 32MHz */
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV1     /* APB1 clock -> 32MHz */
/* configuration of flash access cycles */
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY
/** @} */

/**
 * @brief Timer configuration
 * @{
 */
#if 1
#define TIMER_NUMOF         (1U)
#define TIMER_0_EN          1
#else
#define TIMER_NUMOF         (2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
#endif

/* Timer 0 configuration */
#if 0
#define TIMER_0_DEV_0       TIM4
#define TIMER_0_DEV_1       TIM5
#define TIMER_0_CHANNELS    4
#define TIMER_0_PRESCALER   (32U)
#define TIMER_0_MAX_VALUE   (0xffff)
#define TIMER_0_CLKEN()     (RCC->APB1ENR |= (RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM5EN))
#define TIMER_0_ISR_0       isr_tim4
#define TIMER_0_ISR_1       isr_tim5
#define TIMER_0_IRQ_CHAN_0  TIM4_IRQn
#define TIMER_0_IRQ_CHAN_1  TIM5_IRQn
#define TIMER_0_IRQ_PRIO    1
#define TIMER_0_TRIG_SEL    TIM_SMCR_TS_1
#else
#define TIMER_0_DEV_0       TIM2
#define TIMER_0_DEV_1       TIM3
#define TIMER_0_CHANNELS    4
#define TIMER_0_PRESCALER   (32U)
#define TIMER_0_MAX_VALUE   (0xffff)
#define TIMER_0_CLKEN()     (RCC->APB1ENR |= (RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN))
#define TIMER_0_ISR_0       isr_tim2
#define TIMER_0_ISR_1       isr_tim3
#define TIMER_0_IRQ_CHAN_0  TIM2_IRQn
#define TIMER_0_IRQ_CHAN_1  TIM3_IRQn
#define TIMER_0_IRQ_PRIO    1
#define TIMER_0_TRIG_SEL    TIM_SMCR_TS_0
#endif
/** @} */

/* Timer 1 configuration */
#if 0
#define TIMER_1_DEV_0       TIM2
#define TIMER_1_DEV_1       TIM3
#define TIMER_1_CHANNELS    4
#define TIMER_1_PRESCALER   (32U)
#define TIMER_1_MAX_VALUE   (0xffff)
#define TIMER_1_CLKEN()     (RCC->APB1ENR |= (RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN))
#define TIMER_1_ISR_0       isr_tim2
#define TIMER_1_ISR_1       isr_tim3
#define TIMER_1_IRQ_CHAN_0  TIM2_IRQn
#define TIMER_1_IRQ_CHAN_1  TIM3_IRQn
#define TIMER_1_IRQ_PRIO    1
#define TIMER_1_TRIG_SEL    TIM_SMCR_TS_0
#endif

/**
 * @brief UART configuration
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          USART2
#define UART_0_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define UART_0_CLK          (CLOCK_CORECLOCK)   /* UART clock runs with 32MHz (F_CPU / 1) */
#define UART_0_IRQ          USART2_IRQn
#define UART_0_ISR          isr_usart2
#define UART_0_BUS_FREQ     32000000
/* UART 0 pin configuration */
#define UART_0_PORT         GPIOA
#define UART_0_PORT_CLKEN() (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define UART_0_RX_PIN       3
#define UART_0_TX_PIN       2
#define UART_0_AF           7

/**
 * @brief GPIO configuration
 */
#define GPIO_0_EN           01
#define GPIO_1_EN           01
#define GPIO_2_EN           01
#define GPIO_3_EN           01
#define GPIO_4_EN           01
#define GPIO_5_EN           01
#define GPIO_6_EN           01
#define GPIO_7_EN           01
#define GPIO_8_EN           01
#define GPIO_9_EN           01
#define GPIO_10_EN          01
#define GPIO_11_EN          01
#define GPIO_12_EN          01
#define GPIO_13_EN          01
#define GPIO_14_EN          01
#define GPIO_15_EN          01
#define GPIO_IRQ_PRIO       1

/* IRQ config */
#define GPIO_IRQ_0          GPIO_3
#define GPIO_IRQ_1          GPIO_5
//#define GPIO_IRQ_2          GPIO_7 // no IRQ
//#define GPIO_IRQ_3          GPIO_0 // no IRQ
//#define GPIO_IRQ_4          GPIO_5 // no IRQ
#define GPIO_IRQ_5          GPIO_13
#define GPIO_IRQ_6          GPIO_1
#define GPIO_IRQ_7          GPIO_6
#define GPIO_IRQ_8          GPIO_7
#define GPIO_IRQ_9          GPIO_8
#define GPIO_IRQ_10         GPIO_9
#define GPIO_IRQ_11         GPIO_12
//#define GPIO_IRQ_12         GPIO_15 // no IRQ
//#define GPIO_IRQ_13         GPIO_8  // no IRQ
//#define GPIO_IRQ_14         GPIO_9  // not used
//#define GPIO_IRQ_15         GPIO_10 // not used

/* GPIO channel 0 config (BUTTON) */
#define GPIO_0_PORT         GPIOC
#define GPIO_0_PIN          6
#define GPIO_0_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
#define GPIO_0_EXTI_CFG()   (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI6_PC)
#define GPIO_0_IRQ          EXTI9_5_IRQn
/* GPIO channel 1 config (JOY_UP) */
#define GPIO_1_PORT         GPIOA
#define GPIO_1_PIN          10
#define GPIO_1_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define GPIO_1_EXTI_CFG()   //(SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PA)
#define GPIO_1_IRQ          EXTI15_10_IRQn
/* GPIO channel 2 config (JOY_DOWN) */
#define GPIO_2_PORT         GPIOB
#define GPIO_2_PIN          0
#define GPIO_2_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_2_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB)
#define GPIO_2_IRQ          EXTI0_IRQn
/* GPIO channel 3 config (JOY_LEFT) */
#define GPIO_3_PORT         GPIOA
#define GPIO_3_PIN          8
#define GPIO_3_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define GPIO_3_EXTI_CFG()   //(SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PA)
#define GPIO_3_IRQ          EXTI9_5_IRQn
/* GPIO channel 4 config (JOY_RIGHT) */
#define GPIO_4_PORT         GPIOB
#define GPIO_4_PIN          1
#define GPIO_4_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_4_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PB)
#define GPIO_4_IRQ          EXTI1_IRQn
/* GPIO channel 5 config (JOY_CENTER) */
#define GPIO_5_PORT         GPIOA
#define GPIO_5_PIN          9
#define GPIO_5_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define GPIO_5_EXTI_CFG()   //(SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PB)
#define GPIO_5_IRQ          EXTI9_5_IRQn
/* GPIO channel 6 config (SPIRIT1 GPIO0) */
#define GPIO_6_PORT         GPIOC
#define GPIO_6_PIN          7
#define GPIO_6_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
#define GPIO_6_EXTI_CFG()   (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI7_PC)
#define GPIO_6_IRQ          EXTI9_5_IRQn
/* GPIO channel 7 config (SPIRIT1 GPIO1) */
#define GPIO_7_PORT         GPIOC
#define GPIO_7_PIN          8
#define GPIO_7_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
#define GPIO_7_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PC)
#define GPIO_7_IRQ          EXTI9_5_IRQn
/* GPIO channel 8 config (SPIRIT1 GPIO2) */
#define GPIO_8_PORT         GPIOC
#define GPIO_8_PIN          9
#define GPIO_8_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
#define GPIO_8_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI9_PC)
#define GPIO_8_IRQ          EXTI9_5_IRQn
/* GPIO channel 9 config (SPIRIT1 GPIO3) */
#define GPIO_9_PORT         GPIOC
#define GPIO_9_PIN          10
#define GPIO_9_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
#define GPIO_9_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PC)
#define GPIO_9_IRQ          EXTI15_10_IRQn
/* GPIO channel 10 config (INT1 ACCL) */
#define GPIO_10_PORT        GPIOB
#define GPIO_10_PIN         10
#define GPIO_10_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_10_EXTI_CFG()  //(SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PB)
#define GPIO_10_IRQ         EXTI15_10_IRQn
/* GPIO channel 11 config (INT2 ACCL) */
#define GPIO_11_PORT        GPIOB
#define GPIO_11_PIN         11
#define GPIO_11_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_11_EXTI_CFG()  (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PB)
#define GPIO_11_IRQ         EXTI15_10_IRQn
/* GPIO channel 12 config (INT TEMP) */
#define GPIO_12_PORT        GPIOB
#define GPIO_12_PIN         5
#define GPIO_12_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_12_EXTI_CFG()  (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PB)
#define GPIO_12_IRQ         EXTI9_5_IRQn
/* GPIO channel 13 config (SPIRIT1 CS) */
#define GPIO_13_PORT        GPIOB
#define GPIO_13_PIN         12
#define GPIO_13_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_13_EXTI_CFG()  //(SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_13_IRQ         EXTI15_10_IRQn
/* GPIO channel 14 config (ACCEL CS) */
#define GPIO_14_PORT        GPIOA
#define GPIO_14_PIN         4
#define GPIO_14_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define GPIO_14_EXTI_CFG()  //(SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA)
#define GPIO_14_IRQ         EXTI4_IRQn
/* GPIO channel 15 config */
#define GPIO_15_PORT        GPIOC
#define GPIO_15_PIN         13
#define GPIO_15_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
#define GPIO_15_EXTI_CFG()  //(SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PC)
#define GPIO_15_IRQ         EXTI4_IRQn

/**
 * @brief SPI configuration
 * @{
 */
#define SPI_NUMOF           (2U)
#define SPI_0_EN            1
#define SPI_1_EN            1

/* SPI 0 device configuration */
#define SPI_0_DEV           SPI1
#define SPI_0_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define SPI_0_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN))
#define SPI_0_BUS_DIV       1   /* 1 -> SPI runs with full CPU clock, 0 -> half CPU clock */
/* SPI 0 pin configuration */
#define SPI_0_PORT_CLKEN()  (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define SPI_0_PORT          GPIOA
#define SPI_0_PIN_SCK       5
#define SPI_0_PIN_MOSI      7
#define SPI_0_PIN_MISO      6
#define SPI_0_PIN_AF        5

/* SPI 1 device configuration */
#define SPI_1_DEV           SPI2
#define SPI_1_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_SPI2EN)
#define SPI_1_CLKDIS()      (RCC->APB1ENR &= ~(RCC_APB1ENR_SPI2EN))
#define SPI_1_BUS_DIV       1   /* 1 -> SPI runs with full CPU clock, 0 -> half CPU clock */
/* SPI 1 pin configuration */
#define SPI_1_PORT_CLKEN()  (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define SPI_1_PORT          GPIOB
#define SPI_1_PIN_SCK       13
#define SPI_1_PIN_MOSI      15
#define SPI_1_PIN_MISO      14
#define SPI_1_PIN_AF        5
/** @} */

/**
 * @name I2C configuration
  * @{
 */
#define I2C_NUMOF           (1U)
#define I2C_0_EN            1
#define I2C_IRQ_PRIO        1
#define I2C_APBCLK          (36000000U)

/* I2C 0 device configuration */
#define I2C_0_DEV           I2C1
#define I2C_0_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_I2C1EN)
#define I2C_0_CLKDIS()      (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C1EN))
#define I2C_0_EVT_IRQ       I2C1_EV_IRQn
#define I2C_0_EVT_ISR       isr_i2c1_ev
#define I2C_0_ERR_IRQ       I2C1_ER_IRQn
#define I2C_0_ERR_ISR       isr_i2c1_er
/* I2C 0 pin configuration */
#define I2C_0_PORT_CLKEN()  (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define I2C_0_PORT          GPIOB
#define I2C_0_SCL_PIN       6
#define I2C_0_SCL_AF        4
#define I2C_0_SDA_PIN       7
#define I2C_0_SDA_AF        4
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
