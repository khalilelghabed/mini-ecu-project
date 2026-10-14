
#include <stdio.h>
#include "types.h"
#include "hal/can_types.h"

#define CHECK(c) do { \
    if (!(c)) { \
        printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); \
        return 1; \
    } \
} while (0)

int main(void)
{
    can_frame_t f = {0};
    ecu_state_t s = ECU_STATE_INIT;

    f.id = 0x100;
    f.dlc = 2;
    f.data[0] = 0x32;

    CHECK(f.dlc <= CAN_MAX_DLC);
    CHECK(sizeof f.data == CAN_MAX_DLC);
    CHECK(s == ECU_STATE_INIT);

    printf("OK : types valides\n");
    return 0;
}

