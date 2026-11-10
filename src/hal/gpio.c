
#include "hal/gpio.h"
#include "stm32l4xx.h"
#include <stddef.h>

#define LED_PIN      7U
#define BUTTON_PIN  13U

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

/* Initialisation assurée par MX_GPIO_Init() */
void gpio_init(void)
{
    /*
     * PC7 et PC13 sont déjà configurés
     * par STM32CubeIDE dans MX_GPIO_Init().
     *
     * Ne pas reconfigurer PC13 ici :
     * il est utilisé par EXTI13.
     */
}

/* Mettre une sortie à 1 */
void gpio_set(gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *regs = gpio_regs(port);

    if ((regs == NULL) || (pin >= 16U))
    {
        return;
    }

    regs->BSRR = (1UL << pin);
}

/* Mettre une sortie à 0 */
void gpio_clear(gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *regs = gpio_regs(port);

    if ((regs == NULL) || (pin >= 16U))
    {
        return;
    }

    regs->BSRR = (1UL << (pin + 16U));
}

/* Inverser une sortie */
void gpio_toggle(gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *regs = gpio_regs(port);

    if ((regs == NULL) || (pin >= 16U))
    {
        return;
    }

    uint32_t mask = (1UL << pin);

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

    return (regs->IDR & (1UL << pin)) != 0U;
}

/* Fonctions de notre Mini-ECU */

void mini_ecu_led_write(bool on)
{
    if (on)
    {
        gpio_set(GPIO_PORT_C, LED_PIN);
    }
    else
    {
        gpio_clear(GPIO_PORT_C, LED_PIN);
    }
}

void mini_ecu_led_toggle(void)
{
    gpio_toggle(GPIO_PORT_C, LED_PIN);
}

bool mini_ecu_button_pressed(void)
{
    return gpio_read(GPIO_PORT_C, BUTTON_PIN);
}
