#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    ECU_STATE_INIT,     /* démarrage, auto-tests */
    ECU_STATE_RUN,      /* fonctionnement normal */
    ECU_STATE_ERROR,    /* défaut détecté */
    ECU_STATE_SLEEP     /* veille */
} ecu_state_t;

#endif /* TYPES_H */
