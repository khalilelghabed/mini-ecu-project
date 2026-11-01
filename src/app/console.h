
#ifndef APP_CONSOLE_H
#define APP_CONSOLE_H
#include <stdbool.h>
#include <stdint.h>

#define CONSOLE_BUFFER_SIZE 32U

typedef enum
{
    CONSOLE_WAITING,
    CONSOLE_RECEIVING,
    CONSOLE_DISCARDING
} console_state_t;

typedef enum
{
    CMD_NONE,
    CMD_LED_ON,
    CMD_LED_OFF,
    CMD_STATUS,
    CMD_VERSION,
    CMD_UNKNOWN,
    CMD_TOO_LONG
} console_command_t;

typedef struct
{
    console_state_t state;
    char buffer[CONSOLE_BUFFER_SIZE];
    uint8_t length;
} console_t;

void console_init(console_t *console);

console_command_t console_process_char(
    console_t *console,
    char character
);

const char *console_response(
    console_command_t cmd,
    bool led_on
);

#endif

