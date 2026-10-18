#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "main.h"
#include "hal/gpio.h"

/* Faux ports GPIO */
GPIO_TypeDef fake_gpiob = {0};
GPIO_TypeDef fake_gpioc = {0};

/* États simulés */
static GPIO_PinState led_state = GPIO_PIN_RESET;
static GPIO_PinState button_state = GPIO_PIN_RESET;

/* Fausse fonction de lecture */
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *port,
                                uint16_t pin)
{
    assert(port == BUTTON_USER_GPIO_Port);
    assert(pin == BUTTON_USER_Pin);

    return button_state;
}

/* Fausse fonction d'écriture */
void HAL_GPIO_WritePin(GPIO_TypeDef *port,
                       uint16_t pin,
                       GPIO_PinState state)
{
    assert(port == LED_STATUS_GPIO_Port);
    assert(pin == LED_STATUS_Pin);

    led_state = state;
}

/* Fausse fonction Toggle */
void HAL_GPIO_TogglePin(GPIO_TypeDef *port,
                        uint16_t pin)
{
    assert(port == LED_STATUS_GPIO_Port);
    assert(pin == LED_STATUS_Pin);

    led_state = (led_state == GPIO_PIN_SET)
              ? GPIO_PIN_RESET
              : GPIO_PIN_SET;
}

/* TEST 1 : Allumer et éteindre */
static void test_led_write(void)
{
    mini_ecu_led_write(true);
    assert(led_state == GPIO_PIN_SET);

    mini_ecu_led_write(false);
    assert(led_state == GPIO_PIN_RESET);

    printf("[TEST] led_write        OK\n");
}

/* TEST 2 : Inverser la LED */
static void test_led_toggle(void)
{
    led_state = GPIO_PIN_RESET;

    mini_ecu_led_toggle();
    assert(led_state == GPIO_PIN_SET);

    mini_ecu_led_toggle();
    assert(led_state == GPIO_PIN_RESET);

    printf("[TEST] led_toggle       OK\n");
}

/* TEST 3 : Lire le bouton */
static void test_button_pressed(void)
{
    button_state = GPIO_PIN_RESET;
    assert(mini_ecu_button_pressed() == false);

    button_state = GPIO_PIN_SET;
    assert(mini_ecu_button_pressed() == true);

    printf("[TEST] button_pressed   OK\n");
}

int main(void)
{
    printf("\n=== TESTS UNITAIRES GPIO ===\n\n");

    test_led_write();
    test_led_toggle();
    test_button_pressed();

    printf("\nTous les tests GPIO ont réussi !\n");

    return 0;
}
