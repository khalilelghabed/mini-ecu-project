
#include "hal/uart.h"
#include "hal/log.h"
#include "stm32h7xx_nucleo.h"
#include <string.h>
#include "app/ringbuf.h"

static uint8_t rx_storage[64];
static ringbuf_t rx_buffer;
static uint32_t rx_overflow_count = 0U;

void uart_rx_init(void){
  ringbuf_init(&rx_buffer,rx_storage,sizeof(rx_storage));
  rx_overflow_count = 0U;
}

void uart_rx_poll(void){
  uint8_t Data;

  HAL_StatusTypeDef status=HAL_UART_Receive(
    &hcom_uart[COM1] ,
    &Data,
    1U,
    1U 
  );
  if(status==HAL_OK){
    if (!ringbuf_put(&rx_buffer, Data))
        {
            rx_overflow_count++;
        }
  }
  
}
uint32_t uart_rx_overflow_count(void)
{
    return rx_overflow_count;
}

bool uart_read_byte(uint8_t *byte){
    if(byte==NULL){
        return false;
    }

    return ringbuf_get(&rx_buffer, byte);


}


bool uart_write(const uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0U))
    {
        return false;
    }

    HAL_StatusTypeDef status = HAL_UART_Transmit(
        &hcom_uart[COM1],
        (uint8_t *)data,
        length,
        1000U
    );

    return (status == HAL_OK);
}

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

