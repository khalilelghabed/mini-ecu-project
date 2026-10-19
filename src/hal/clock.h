
#ifndef MINI_ECU_CLOCK_H
#define MINI_ECU_CLOCK_H

#include <stdint.h>

/* Fréquences d'horloge en Hz */
typedef struct
{
    uint32_t sysclk_hz;
    uint32_t hclk_hz;
    uint32_t pclk1_hz;
    uint32_t pclk2_hz;

} clock_info_t;

/* Lire les fréquences configurées */
void clock_get_info(clock_info_t *info);

#endif
