


#ifndef MINI_ECU_GPIO_H
#define MINI_ECU_GPIO_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    GPIO_PORT_B,
    GPIO_PORT_C
} gpio_port_t;

/* Configuration initiale */
void gpio_init(void);

/* API GPIO */
void gpio_set(gpio_port_t port, uint8_t pin);
void gpio_clear(gpio_port_t port, uint8_t pin);
void gpio_toggle(gpio_port_t port, uint8_t pin);
bool gpio_read(gpio_port_t port, uint8_t pin);

/* Fonctions utilisées par notre application */
void mini_ecu_led_write(bool on);
void mini_ecu_led_toggle(void);
bool mini_ecu_button_pressed(void);

#endif


