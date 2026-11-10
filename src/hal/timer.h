
#ifndef MINI_ECU_TIMER_H
#define MINI_ECU_TIMER_H

#include <stdint.h>
#include <stdbool.h>

/* Demarre le timer materiel TIM7 */
bool timer_start(void);

/* Retourne le temps ecoule en millisecondes */
uint32_t timer_get_ms(void);

/* Verifie si une duree est ecoulee */
bool timer_elapsed_ms(uint32_t now_ms,
                      uint32_t start_ms,
                      uint32_t duration_ms);

#endif /* MINI_ECU_TIMER_H */
