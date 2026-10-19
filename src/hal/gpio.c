
#include "hal/gpio.h"
#include "stm32h7xx.h"
#include <stddef.h>

#define LED_PIN       0U
#define BUTTON_PIN   13U

/* Associer un port logique à ses registres */
static GPIO_TypeDef *gpio_regs(gpio_port_t port)
{
    switch (port)
    {
        case GPIO_PORT_B:
            return GPIOB;

        case GPIO_PORT_C:
            return GPIOC;

        default:
            return NULL;
    }
}

/* Initialisation des périphériques GPIO */
void gpio_init(void)
{
    /* 1. Activer les horloges GPIOB et GPIOC */
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN
                  | RCC_AHB4ENR_GPIOCEN;

    (void)RCC->AHB4ENR;

    /* 2. PB0 : sortie (MODER0 = 01) */
    GPIOB->MODER &= ~(3U << 0);
    GPIOB->MODER |=  (1U << 0);

    /* Sortie push-pull, sans pull-up/down */
    GPIOB->OTYPER &= ~(1U << LED_PIN);
    GPIOB->PUPDR  &= ~(3U << 0);

    /* 3. PC13 : entrée (MODER13 = 00) */
    GPIOC->MODER &= ~(3U << 26);
    GPIOC->PUPDR &= ~(3U << 26);

    /* 4. Éteindre initialement la LED */
    GPIOB->BSRR = (1U << 16);
}

/* Mettre une sortie à 1 */
void gpio_set(gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *regs = gpio_regs(port);

    if ((regs == NULL) || (pin >= 16U))
    {
        return;
    }

    regs->BSRR = (1U << pin);
}

/* Mettre une sortie à 0 */
void gpio_clear(gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *regs = gpio_regs(port);

    if ((regs == NULL) || (pin >= 16U))
    {
        return;
    }

    regs->BSRR = (1U << (pin + 16U));
}

/* Inverser une sortie */
void gpio_toggle(gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *regs = gpio_regs(port);

    if ((regs == NULL) || (pin >= 16U))
    {
        return;
    }

    uint32_t mask = (1U << pin);

    if ((regs->ODR & mask) != 0U)
    {
        gpio_clear(port, pin);
    }
    else
    {
        gpio_set(port, pin);
    }
}

/* Lire l'état logique d'une entrée */
bool gpio_read(gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *regs = gpio_regs(port);

    if ((regs == NULL) || (pin >= 16U))
    {
        return false;
    }

    return (regs->IDR & (1U << pin)) != 0U;
}

/* Fonctions de notre Mini-ECU */

void mini_ecu_led_write(bool on)
{
    if (on)
    {
        gpio_set(GPIO_PORT_B, LED_PIN);
    }
    else
    {
        gpio_clear(GPIO_PORT_B, LED_PIN);
    }
}

void mini_ecu_led_toggle(void)
{
    gpio_toggle(GPIO_PORT_B, LED_PIN);
}

bool mini_ecu_button_pressed(void)
{
    return gpio_read(GPIO_PORT_C, BUTTON_PIN);
}

