
#ifndef STUB_STM32H7XX_H
#define STUB_STM32H7XX_H

#include <stdint.h>

/* Registres GPIO simulés */
typedef struct
{
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
} GPIO_TypeDef;

/* Registre RCC nécessaire au test */
typedef struct
{
    volatile uint32_t AHB4ENR;
} RCC_TypeDef;

extern GPIO_TypeDef fake_gpiob;
extern GPIO_TypeDef fake_gpioc;
extern RCC_TypeDef fake_rcc;

#define GPIOB (&fake_gpiob)
#define GPIOC (&fake_gpioc)
#define RCC   (&fake_rcc)

#define RCC_AHB4ENR_GPIOBEN (1UL << 1)
#define RCC_AHB4ENR_GPIOCEN (1UL << 2)

#endif
