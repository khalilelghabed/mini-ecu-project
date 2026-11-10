
#include "hal/uart.h"
#include "app/ringbuf.h"

#include "stm32l4xx_hal.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Handle créé par STM32CubeMX */
extern UART_HandleTypeDef hlpuart1;

/* Stockage statique de la réception */
static uint8_t rx_storage[64];
static ringbuf_t rx_buffer;

/* Nombre d'octets perdus lorsque le buffer est plein */
static uint32_t rx_overflow_count = 0U;

/* Initialiser la réception UART */
void uart_rx_init(void)
{
    ringbuf_init(&rx_buffer,
                 rx_storage,
                 sizeof(rx_storage));

    rx_overflow_count = 0U;
}

/* Lire un octet depuis LPUART1 */
void uart_rx_poll(void)
{
    uint8_t data;

    HAL_StatusTypeDef status = HAL_UART_Receive(
        &hlpuart1,
        &data,
        1U,
        1U
    );

    if (status == HAL_OK)
    {
        if (!ringbuf_put(&rx_buffer, data))
        {
            rx_overflow_count++;
        }
    }
}

/* Lire le compteur de dépassement */
uint32_t uart_rx_overflow_count(void)
{
    return rx_overflow_count;
}

/* Extraire un octet du ring buffer */
bool uart_read_byte(uint8_t *byte)
{
    if (byte == NULL)
    {
        return false;
    }

    return ringbuf_get(&rx_buffer, byte);
}

/* Envoyer des données par LPUART1 */
bool uart_write(const uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0U))
    {
        return false;
    }

    HAL_StatusTypeDef status = HAL_UART_Transmit(
        &hlpuart1,
        (uint8_t *)data,
        length,
        1000U
    );

    return (status == HAL_OK);
}

/* Envoyer une chaîne de caractères */
bool uart_write_text(const char *text)
{
    if (text == NULL)
    {
        return false;
    }

    size_t length = strlen(text);

    if ((length == 0U) || (length > UINT16_MAX))
    {
        return false;
    }

    return uart_write(
        (const uint8_t *)text,
        (uint16_t)length
    );
}

/* Transmission de diagnostic sans HAL */
bool uart_fault_write(const char *text)
{
    if (text == NULL)
    {
        return false;
    }

    while (*text != '\0')
    {
        uint32_t timeout = 100000U;

        /* Attendre que LPUART1 accepte un octet */
        while ((LPUART1->ISR & UART_FLAG_TXE) == 0U)
        {
            if (timeout == 0U)
            {
                return false;
            }

            timeout--;
        }

        /* Envoyer directement un caractère */
        LPUART1->TDR = (uint32_t)(uint8_t)(*text);

        text++;
    }

    /* Attendre la fin de la dernière transmission */
    uint32_t timeout = 100000U;

    while ((LPUART1->ISR & UART_FLAG_TC) == 0U)
    {
        if (timeout == 0U)
        {
            return false;
        }

        timeout--;
    }

    return true;
}
