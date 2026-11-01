#include "console.h"
#include <stdio.h>
#include <string.h>

void console_init(console_t *console)
{
    if (console == NULL) {
       return ;
    }
    console->state = CONSOLE_WAITING ;
    console->buffer[0]='\0';
    console->length=0U;
}
static console_command_t console_parse(const char *line)
{
    if (strcmp(line, "led on") == 0)
    {
        return CMD_LED_ON;
    }

    if (strcmp(line, "led off") == 0)
    {
        return CMD_LED_OFF;
    }

    if (strcmp(line, "status") == 0)
    {
        return CMD_STATUS;
    }

    if (strcmp(line, "version") == 0)
    {
        return CMD_VERSION;
    }

    return CMD_UNKNOWN;
}


console_command_t console_process_char(console_t *console, char character)
{
    if (console == NULL)
    {
        return CMD_NONE;
    }

    switch (console->state)
    {
        case CONSOLE_WAITING:
            if (character == '\r' || character == '\n' ||
             character == '\b' || character == '\x7F')
           {
             return CMD_NONE;
           }

    	   console->buffer[0] = character;
           console->length = 1U;
           console->buffer[1] = '\0';
           console->state = CONSOLE_RECEIVING;
            break;

        case CONSOLE_RECEIVING:

            /* 1. Detecter la fin de ligne */
            if (character == '\r' || character == '\n')
            {   
                console_command_t cmd = console_parse(console->buffer);
                console->state = CONSOLE_WAITING;
                console->length = 0U;
                console->buffer[0] = '\0';
                
                return cmd;
            }

            /* 2. Gerer Backspace */
            if (character == '\b' || character == '\x7F')
            {
                if (console->length > 0U)
                {
                    console->length--;
                    console->buffer[console->length] = '\0';
                }

                if (console->length == 0U)
                {
                    console->state = CONSOLE_WAITING;
                }

                return CMD_NONE;
            }

            /* 3. Verifier si le buffer est plein */
            if (console->length >= CONSOLE_BUFFER_SIZE - 1U)
            {
                console->state = CONSOLE_DISCARDING;

                return CMD_NONE;
            }

            /* 4. Ajouter le caractere au buffer */
            console->buffer[console->length] = character;
            console->length++;

            /* 5. Terminer la chaine */
            console->buffer[console->length] = '\0';

            break;

        case CONSOLE_DISCARDING:

            /* Ignorer les caracteres jusqu'a la fin de ligne */
            if (character == '\r' || character == '\n')
            {
                console->state = CONSOLE_WAITING;
                console->length = 0U;
                console->buffer[0] = '\0';

                return CMD_TOO_LONG;
            }

            return CMD_NONE;

        default:
            break;
    }

    return CMD_NONE;
}


const char *console_response(
    console_command_t cmd,
    bool led_on
)
{
    switch (cmd)
    {
        case CMD_LED_ON:
            return "OK: LED ON";

        case CMD_LED_OFF:
            return "OK: LED OFF";

        case CMD_STATUS:
            if (led_on)
            {
                return "LED: ON";
            }
            return "LED: OFF";

        case CMD_VERSION:
            return "Mini ECU Firmware v0.1";

        case CMD_UNKNOWN:
            return "ERROR: Unknown command";

        case CMD_TOO_LONG:
            return "ERROR: Command too long";

        case CMD_NONE:
        default:
            return "";
    }
}

