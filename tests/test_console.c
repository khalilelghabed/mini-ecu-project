
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include "app/console.h"

#define CHECK(c) do { \
    if (!(c)) { \
        printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); \
        return 1; \
    } \
} while (0)

/* Test 1 : Initialisation */
static int test_console_init(void)
{
    console_t console;

    console_init(&console);

    CHECK(console.state == CONSOLE_WAITING);
    CHECK(console.length == 0U);
    CHECK(console.buffer[0] == '\0');

    return 0;
}

/* Test 2 : Pointeur NULL */
static int test_console_null(void)
{
    console_init(NULL);

    CHECK(console_process_char(NULL, 'A') == CMD_NONE);

    return 0;
}

/* Test 3 : Etat WAITING */
static int test_console_waiting(void)
{
    console_t console;

    console_init(&console);

    CHECK(console_process_char(&console, '\r') == CMD_NONE);
    CHECK(console_process_char(&console, '\n') == CMD_NONE);
    CHECK(console_process_char(&console, '\b') == CMD_NONE);
    CHECK(console_process_char(&console, '\x7F') == CMD_NONE);

    CHECK(console.state == CONSOLE_WAITING);
    CHECK(console.length == 0U);

    CHECK(console_process_char(&console, 'l') == CMD_NONE);

    CHECK(console.state == CONSOLE_RECEIVING);
    CHECK(console.length == 1U);
    CHECK(console.buffer[0] == 'l');
    CHECK(console.buffer[1] == '\0');

    return 0;
}

/* Test 4 : Reception de plusieurs caracteres */
static int test_console_receiving(void)
{
    console_t console;
    const char text[] = "led on";

    console_init(&console);

    for (uint8_t i = 0U; i < sizeof(text) - 1U; i++)
    {
        CHECK(console_process_char(&console, text[i]) == CMD_NONE);
    }

    CHECK(console.state == CONSOLE_RECEIVING);
    CHECK(console.length == 6U);

    CHECK(console.buffer[0] == 'l');
    CHECK(console.buffer[1] == 'e');
    CHECK(console.buffer[2] == 'd');
    CHECK(console.buffer[3] == ' ');
    CHECK(console.buffer[4] == 'o');
    CHECK(console.buffer[5] == 'n');
    CHECK(console.buffer[6] == '\0');

    return 0;
}

/* Test 5 : Backspace et DEL */
static int test_console_backspace(void)
{
    console_t console;
    const char text[] = "led onn";

    console_init(&console);

    for (uint8_t i = 0U; i < sizeof(text) - 1U; i++)
    {
        CHECK(console_process_char(&console, text[i]) == CMD_NONE);
    }

    CHECK(console.length == 7U);

    CHECK(console_process_char(&console, '\b') == CMD_NONE);

    CHECK(console.length == 6U);
    CHECK(console.buffer[5] == 'n');
    CHECK(console.buffer[6] == '\0');
    CHECK(console.state == CONSOLE_RECEIVING);

    CHECK(console_process_char(&console, '\x7F') == CMD_NONE);

    CHECK(console.length == 5U);
    CHECK(console.buffer[5] == '\0');

    return 0;
}

/* Test 6 : Effacer tous les caracteres */
static int test_console_backspace_empty(void)
{
    console_t console;

    console_init(&console);

    CHECK(console_process_char(&console, 'A') == CMD_NONE);
    CHECK(console.state == CONSOLE_RECEIVING);
    CHECK(console.length == 1U);

    CHECK(console_process_char(&console, '\b') == CMD_NONE);

    CHECK(console.state == CONSOLE_WAITING);
    CHECK(console.length == 0U);
    CHECK(console.buffer[0] == '\0');

    CHECK(console_process_char(&console, '\b') == CMD_NONE);
    CHECK(console.length == 0U);

    return 0;
}

/* Test 7 : Fin de ligne CR, LF et CRLF */
static int test_console_endline(void)
{
    console_t console;

    console_init(&console);

    CHECK(console_process_char(&console, 'A') == CMD_NONE);
    CHECK(console_process_char(&console, '\r') == CMD_UNKNOWN);

    CHECK(console.state == CONSOLE_WAITING);
    CHECK(console.length == 0U);
    CHECK(console.buffer[0] == '\0');

    /* Le LF supplementaire est ignore */
    CHECK(console_process_char(&console, '\n') == CMD_NONE);
    CHECK(console.state == CONSOLE_WAITING);

    /* Nouvelle commande terminee par LF */
    CHECK(console_process_char(&console, 'B') == CMD_NONE);
    CHECK(console_process_char(&console, '\n') == CMD_UNKNOWN);

    CHECK(console.state == CONSOLE_WAITING);
    CHECK(console.length == 0U);

    return 0;
}

/* Test 8 : Capacite maximale du buffer */
static int test_console_capacity(void)
{
    console_t console;

    console_init(&console);

    /* Remplir exactement 31 caracteres */
    for (uint8_t i = 0U; i < CONSOLE_BUFFER_SIZE - 1U; i++)
    {
        CHECK(console_process_char(&console, 'A') == CMD_NONE);
    }

    CHECK(console.state == CONSOLE_RECEIVING);
    CHECK(console.length == CONSOLE_BUFFER_SIZE - 1U);
    CHECK(console.buffer[CONSOLE_BUFFER_SIZE - 1U] == '\0');

    /* 31 caracteres sont autorises */
    CHECK(console_process_char(&console, '\r') == CMD_UNKNOWN);

    CHECK(console.state == CONSOLE_WAITING);
    CHECK(console.length == 0U);

    return 0;
}

/* Test 9 : Debordement du buffer */
static int test_console_too_long(void)
{
    console_t console;

    console_init(&console);

    /* Envoyer 40 caracteres */
    for (int i = 0; i < 40; i++)
    {
        CHECK(console_process_char(&console, 'A') == CMD_NONE);
    }

    CHECK(console.state == CONSOLE_DISCARDING);

    /* Ignorer les caracteres supplementaires */
    CHECK(console_process_char(&console, 'B') == CMD_NONE);
    CHECK(console_process_char(&console, '\b') == CMD_NONE);

    CHECK(console.state == CONSOLE_DISCARDING);

    /* Signaler l'erreur a la fin de ligne */
    CHECK(console_process_char(&console, '\r') == CMD_TOO_LONG);

    CHECK(console.state == CONSOLE_WAITING);
    CHECK(console.length == 0U);
    CHECK(console.buffer[0] == '\0');

    /* Ne pas signaler deux fois l'erreur */
    CHECK(console_process_char(&console, '\n') == CMD_NONE);

    return 0;
}

/* Test 10 : Recuperation apres debordement */
static int test_console_recovery(void)
{
    console_t console;

    console_init(&console);

    for (int i = 0; i < 40; i++)
    {
        CHECK(console_process_char(&console, 'X') == CMD_NONE);
    }

    CHECK(console.state == CONSOLE_DISCARDING);

    CHECK(console_process_char(&console, '\n') == CMD_TOO_LONG);

    CHECK(console.state == CONSOLE_WAITING);

    /* La console accepte une nouvelle commande */
    CHECK(console_process_char(&console, 'l') == CMD_NONE);
    CHECK(console_process_char(&console, 'e') == CMD_NONE);
    CHECK(console_process_char(&console, 'd') == CMD_NONE);

    CHECK(console.state == CONSOLE_RECEIVING);
    CHECK(console.length == 3U);

    CHECK(console.buffer[0] == 'l');
    CHECK(console.buffer[1] == 'e');
    CHECK(console.buffer[2] == 'd');
    CHECK(console.buffer[3] == '\0');

    return 0;
}

/* Test 11 : Reconnaissance des commandes */
static int test_console_commands(void)
{
    console_t console;

    const char *commands[] = {
        "led on",
        "led off",
        "status",
        "version",
        "led",
        "led blabla"
    };

    const console_command_t expected[] = {
        CMD_LED_ON,
        CMD_LED_OFF,
        CMD_STATUS,
        CMD_VERSION,
        CMD_UNKNOWN,
        CMD_UNKNOWN
    };

    console_init(&console);

    /* Tester toutes les commandes */
    for (uint8_t i = 0U; i < 6U; i++)
    {
        const char *text = commands[i];

        for (uint8_t j = 0U; text[j] != '\0'; j++)
        {
            CHECK(console_process_char(&console, text[j]) == CMD_NONE);
        }

        CHECK(console_process_char(&console, '\r') == expected[i]);

        /* Eviter une double execution avec CRLF */
        CHECK(console_process_char(&console, '\n') == CMD_NONE);

        CHECK(console.state == CONSOLE_WAITING);
        CHECK(console.length == 0U);
    }

    /* Tester la correction d'une commande */
    const char *text = "led onn";

    for (uint8_t i = 0U; text[i] != '\0'; i++)
    {
        CHECK(console_process_char(&console, text[i]) == CMD_NONE);
    }

    CHECK(console_process_char(&console, '\b') == CMD_NONE);
    CHECK(console_process_char(&console, '\r') == CMD_LED_ON);

    CHECK(console.state == CONSOLE_WAITING);
    CHECK(console.length == 0U);

    return 0;
}

int main(void)
{
    int fails = 0;

    fails += test_console_init();
    fails += test_console_null();
    fails += test_console_waiting();
    fails += test_console_receiving();
    fails += test_console_backspace();
    fails += test_console_backspace_empty();
    fails += test_console_endline();
    fails += test_console_capacity();
    fails += test_console_too_long();
    fails += test_console_recovery();
    fails += test_console_commands();

    if (fails == 0)
    {
        printf("OK : 11 tests console reussis\n");
    }
    else
    {
        printf("ECHEC : %d test(s) console\n", fails);
    }

    return fails;
}
