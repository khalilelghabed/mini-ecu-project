
# Day 10 — UART Command Console

## Objective
Implement a serial command console for the STM32H753 Mini-ECU.

## Architecture
Renode Terminal
  -> USART3
  -> UART RX polling
  -> Ring Buffer
  -> Console State Machine
  -> Command Parser
  -> Application / GPIO
  -> UART Response

## State Machine
- WAITING: Wait for the first character.
- RECEIVING: Store characters until the end of the line.
- DISCARDING: Ignore an oversized command until the end of line.

## Supported Commands
| Command | Action |
|---------|--------|
| led on | Turn on LED PB0 |
| led off | Turn off LED PB0 |
| status | Read and display LED state |
| version | Display firmware version |

Unknown commands return an error.

## Implementation
- console.c / console.h: command parsing and state machine
- uart.c / uart.h: UART communication
- ringbuf.c / ringbuf.h: FIFO buffering
- gpio.c / gpio.h: LED control and GPIO reading
- main.c: command execution and integration

## Robustness
- Fixed-size 32-byte command buffer
- Maximum 31 useful characters
- CR, LF, and CRLF support
- Backspace and DEL support
- Empty-line handling
- Oversized command detection
- No dynamic memory allocation

## Validation
- 11 console unit tests
- UART console tested in Renode
- LED ON/OFF verified through GPIO PB0
- Commands: status, version, unknown and oversized input tested

## Current Limitations
- UART reception uses polling
- UART transmission uses blocking calls
- Renode terminal input may require an additional Enter press
- Physical hardware validation remains to be performed
