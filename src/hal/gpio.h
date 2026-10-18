#ifndef MINI_ECU_GPIO_H
#define MINI_ECU_GPIO_H

#include <stdbool.h>

/* Commander la LED */
void mini_ecu_led_write(bool on);

/* Inverser l'état de la LED */
void mini_ecu_led_toggle(void);

/* Lire le bouton : true = appuyé */
bool mini_ecu_button_pressed(void);

#endif
