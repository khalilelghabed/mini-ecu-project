
#include <stdint.h>

#include "app/led_blink.h"
#include "hal/timer.h"
#include "main.h"

/* Periode entre deux changements d'etat */
#define LED_TOGGLE_PERIOD_MS 250U

/* Instant du dernier changement */
static uint32_t s_last_toggle_ms = 0U;

/* Initialisation de la LED */
void led_blink_init(void)
{
    HAL_GPIO_WritePin(
        LED_TIMER_GPIO_Port,
        LED_TIMER_Pin,
        GPIO_PIN_RESET
    );

    s_last_toggle_ms = timer_get_ms();
}

/* Traitement periodique non bloquant */
void led_blink_process(void)
{
    uint32_t now_ms = timer_get_ms();

    if (timer_elapsed_ms(
            now_ms,
            s_last_toggle_ms,
            LED_TOGGLE_PERIOD_MS))
    {
        s_last_toggle_ms = now_ms;

        HAL_GPIO_TogglePin(
            LED_TIMER_GPIO_Port,
            LED_TIMER_Pin
        );
    }
}
