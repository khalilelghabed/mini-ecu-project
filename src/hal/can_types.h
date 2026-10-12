#ifndef CAN_TYPES_H
#define CAN_TYPES_H

#include <stdint.h>

#define CAN_MAX_DLC 8U      /* octets de données maximum d'une trame */

typedef struct {
    uint32_t id;                    /* identifiant du message           */
    uint8_t  dlc;                   /* nombre d'octets utiles (0 à 8)   */
    uint8_t  data[CAN_MAX_DLC];     /* octets de données                */
} can_frame_t;

#endif /* CAN_TYPES_H */
