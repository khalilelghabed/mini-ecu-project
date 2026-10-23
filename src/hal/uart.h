
#ifndef MINI_ECU_UART_H
#define MINI_ECU_UART_H

#include <stdbool.h>
#include <stdint.h>

/* Envoyer plusieurs octets */
bool uart_write(const uint8_t *data, uint16_t length);

/* Envoyer un message texte */
bool uart_write_text(const char *text);

uint32_t uart_rx_overflow_count(void);
void uart_rx_init(void);
void uart_rx_poll(void);
bool uart_read_byte(uint8_t *byte);
#endif
