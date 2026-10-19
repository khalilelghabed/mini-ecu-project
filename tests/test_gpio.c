
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "stm32h7xx.h"
#include "hal/gpio.h"

/* Registres simulés dans la RAM du PC */
GPIO_TypeDef fake_gpiob = {0};
GPIO_TypeDef fake_gpioc = {0};
RCC_TypeDef fake_rcc = {0};

static void test_gpio_init(void)
{
    gpio_init();

    assert((fake_rcc.AHB4ENR & (3U << 1))
           == (3U << 1));

    assert((fake_gpiob.MODER & 3U) == 1U);

    assert((fake_gpioc.MODER & (3U << 26)) == 0U);

    printf("[TEST] gpio_init          OK\n");
}

static void test_gpio_set_clear(void)
{
    gpio_set(GPIO_PORT_B, 0U);
    assert(fake_gpiob.BSRR == (1U << 0));

    gpio_clear(GPIO_PORT_B, 0U);
    assert(fake_gpiob.BSRR == (1U << 16));

    printf("[TEST] gpio_set_clear     OK\n");
}

static void test_gpio_toggle(void)
{
    /* Simuler une sortie initialement à 0 */
    fake_gpiob.ODR = 0U;

    gpio_toggle(GPIO_PORT_B, 0U);
    assert(fake_gpiob.BSRR == (1U << 0));

    /* Simuler l'effet matériel de l'écriture BSRR */
    fake_gpiob.ODR = 1U;

    gpio_toggle(GPIO_PORT_B, 0U);
    assert(fake_gpiob.BSRR == (1U << 16));

    printf("[TEST] gpio_toggle        OK\n");
}

static void test_gpio_read(void)
{
    fake_gpioc.IDR = 0U;
    assert(gpio_read(GPIO_PORT_C, 13U) == false);

    fake_gpioc.IDR = (1U << 13);
    assert(gpio_read(GPIO_PORT_C, 13U) == true);

    printf("[TEST] gpio_read          OK\n");
}

static void test_application_api(void)
{
    mini_ecu_led_write(true);
    assert(fake_gpiob.BSRR == (1U << 0));

    mini_ecu_led_write(false);
    assert(fake_gpiob.BSRR == (1U << 16));

    fake_gpioc.IDR = (1U << 13);
    assert(mini_ecu_button_pressed() == true);

    fake_gpiob.ODR = 0U;
    mini_ecu_led_toggle();
    assert(fake_gpiob.BSRR == (1U << 0));

    printf("[TEST] application_api    OK\n");
}

static void test_invalid_pin(void)
{
    fake_gpiob.BSRR = 0U;

    gpio_set(GPIO_PORT_B, 16U);

    assert(fake_gpiob.BSRR == 0U);
    assert(gpio_read(GPIO_PORT_C, 16U) == false);

    printf("[TEST] invalid_pin        OK\n");
}

int main(void)
{
    printf("\n=== TESTS GPIO PAR REGISTRES ===\n\n");

    test_gpio_init();
    test_gpio_set_clear();
    test_gpio_toggle();
    test_gpio_read();
    test_application_api();
    test_invalid_pin();

    printf("\nTous les tests ont réussi !\n");

    return 0;
}

