
#include "hal/log.h"
#include "hal/uart.h"

#include <stdarg.h>
#include <stdio.h>

void log_print(const char *fmt, ...)
{
    // 1. Tampon fixe de 128 octets
    static char buffer[128];

    // 2. Verifier que le format est valide
    if (fmt == NULL)
    {
        return;
    }

    // 3. Preparer les arguments variables
    va_list args;
    va_start(args, fmt);

    // 4. Construire le message dans le tampon
    int resultat = vsnprintf(
        buffer,
        sizeof(buffer),
        fmt,
        args
    );

    // 5. Terminer l'utilisation des arguments
    va_end(args);

    // 6. Verifier les erreurs de formatage
    if (resultat < 0)
    {
        return;
    }

    // 7. Verifier que le message n'a pas ete tronque
    if ((size_t)resultat >= sizeof(buffer))
    {
        return;
    }

    // 8. Envoyer le message par UART
    (void)uart_write_text(buffer);
}

