
#include "hal/timer.h"
#include "stm32l4xx_hal.h"

/* Handle TIM7 genere par STM32CubeMX */
extern TIM_HandleTypeDef htim7;

/* Compteur de millisecondes */
static volatile uint32_t s_timer_ms = 0U;

/* Demarrer TIM7 avec interruption */
bool timer_start(void)
{
    s_timer_ms = 0U;

    /* Repartir avec un compteur propre */
    __HAL_TIM_SET_COUNTER(&htim7, 0U);
    __HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);

    return HAL_TIM_Base_Start_IT(&htim7) == HAL_OK;
}

/* Lire notre compteur */
uint32_t timer_get_ms(void)
{
    return s_timer_ms;
}

/* Verifier une duree ecoulee */
bool timer_elapsed_ms(uint32_t now_ms,
                      uint32_t start_ms,
                      uint32_t duration_ms)
{
    return (uint32_t)(now_ms - start_ms) >= duration_ms;
}

/* Callback appele lors d'une interruption timer */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM7)
    {
        s_timer_ms++;
    }
}
