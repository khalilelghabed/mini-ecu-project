
#ifndef MINI_ECU_EXTI_H
#define MINI_ECU_EXTI_H

#include <stdbool.h>
#include <stdint.h>

/* Duree de stabilisation du bouton */
#define EXTI_DEBOUNCE_MS 30U

/* Appelee depuis le callback d'interruption */
void Exti_OnInterrupt(void);

/* Traite l'evenement dans la boucle principale */
void Exti_Process(uint32_t now_ms, bool button_high);

/* Recupere un evenement d'urgence valide */
bool Exti_TakeEmergencyEvent(void);

#endif /* MINI_ECU_EXTI_H */

