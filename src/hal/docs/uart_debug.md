
# UART Communication and Debug Logging — Day 9

## 1. Objective

Implement UART transmission and reception for the Mini-ECU
Sensor Node based on STM32H753ZI.

The project provides:
- UART transmission and reception
- Formatted debug logging
- Periodic vehicle data display
- Circular receive buffer
- UART character echo
- RX buffer overflow detection

## 2. Development environment

- MCU: STM32H753ZIT6
- Board target: NUCLEO-H753ZI
- IDE: STM32CubeIDE 1.18.1
- Simulation: Renode
- Serial interface: USART3 / BSP COM1
- Configuration: 115200 baud, 8N1

## 3. Software architecture

main.c
  |
  +-- uart_rx_init()
  +-- uart_rx_poll()
  +-- uart_read_byte()
  +-- uart_write()
  +-- log_print()
          |
          v
       uart.c
          |
          +-- STM32 HAL UART
          +-- ringbuf.c

The UART module manages hardware communication and RX buffering.

The application uses the UART interface without directly
accessing the circular buffer.

## 4. UART transmission

uart_write():
  Transmits a byte array through USART3.

uart_write_text():
  Transmits a null-terminated text string.

log_print():
  Formats debug messages with vsnprintf() into a fixed-size
  buffer before UART transmission.

## 5. UART reception

uart_rx_init():
  Initializes the circular receive buffer.

uart_rx_poll():
  Attempts to receive one byte through HAL_UART_Receive().
  A short timeout is used.

uart_read_byte():
  Retrieves a buffered byte.

uart_rx_overflow_count():
  Returns the number of bytes rejected by the full RX buffer.

## 6. Ring Buffer

Physical storage: 64 bytes
Usable capacity: 63 bytes

The implementation uses FIFO ordering.

One storage position remains unused to distinguish a full
buffer from an empty buffer.

## 7. Periodic data output

HAL_GetTick() is used to schedule debug messages approximately
every 1000 ms of simulated MCU time.

Example output:

[TIME] 1000 ms | Vitesse = 50 km/h | Temperature = 25 C
[UART] RX Overflows = 0

[TIME] 2000 ms | Vitesse = 55 km/h | Temperature = 26 C
[UART] RX Overflows = 0

## 8. Verification

Validated in Renode:
- USART3 transmission
- USART3 reception
- Character echo
- Periodic UART logging
- RX buffer overflow detection

During the overflow experiment, buffer reads were temporarily
disabled to deliberately fill the receive buffer.

Observed final overflow counter: 18 rejected bytes.

The normal application behavior was restored after the test.

## 9. Unit tests

Host-side tests:
- 11 signal tests
- 4 ring buffer tests

Total: 15 successful unit tests.

## 10. Current limitations

- The RX implementation uses polling.
- HAL_UART_Receive() may block for a short time.
- Incoming bytes may be lost before reaching the ring buffer.
- The overflow counter detects only rejected buffer insertions.
- UART transmission is blocking.
- UART timing was not validated on physical hardware.
- The logging function uses a shared static buffer and is
  not designed for concurrent or interrupt-based calls.

## 11. Future improvements

- Interrupt-driven UART reception
- DMA-based UART communication
- Improved UART error monitoring
- Non-blocking transmission
- Integration with a second ECU
- Physical hardware validation
