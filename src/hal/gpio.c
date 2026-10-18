#include "hal/gpio.h"
#include "main.h"

#define BUTTON_PRESSED_LEVEL GPIO_PIN_SET

void mini_ecu_led_write(bool on)
{
    HAL_GPIO_WritePin(
        LED_STATUS_GPIO_Port,
        LED_STATUS_Pin,
        on ? GPIO_PIN_SET : GPIO_PIN_RESET
    );
}

void mini_ecu_led_toggle(void)
{
    HAL_GPIO_TogglePin(
        LED_STATUS_GPIO_Port,
        LED_STATUS_Pin
    );
}

bool mini_ecu_button_pressed(void)
{
    return HAL_GPIO_ReadPin(
        BUTTON_USER_GPIO_Port,
        BUTTON_USER_Pin
    ) == BUTTON_PRESSED_LEVEL;
}
