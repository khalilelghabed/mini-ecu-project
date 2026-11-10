
#include "hal/clock.h"
#include "stm32l4xx_hal.h"

#include <stddef.h>

/* Lire les fréquences d'horloge du STM32 */
void clock_get_info(clock_info_t *info)
{
    if (info == NULL)
    {
        return;
    }

    info->sysclk_hz = HAL_RCC_GetSysClockFreq();

    info->hclk_hz = HAL_RCC_GetHCLKFreq();

    info->pclk1_hz = HAL_RCC_GetPCLK1Freq();

    info->pclk2_hz = HAL_RCC_GetPCLK2Freq();
}
