#ifndef SIGNALS_H
#define SIGNALS_H

#include <stdbool.h>
#include "hal/can_types.h"


#define SIG_FLAG_CRUISE (1U << 0)
#define SIG_FLAG_FAULT (1U << 1)
#define SIG_FLAG_BRAKE (1U << 2)

#define SIG_TEMP_MIN  (-40)
#define SIG_TEMP_MAX  (215)

void signals_init(can_frame_t *f);
void signals_set_speed(can_frame_t *f,uint16_t v);
uint16_t signals_get_speed(const can_frame_t *f);
void signals_set_setpoint(can_frame_t *f , uint16_t v);
uint16_t signals_get_setpoint(const can_frame_t *f);
bool signals_set_temp( can_frame_t *f,int16_t celsius);/* false si hors -40..215 */
int16_t signals_get_temp(const can_frame_t *f);
void signals_set_flag(can_frame_t *f,uint8_t flag);
void signals_clear_flag(can_frame_t *f,uint8_t flag);
void signals_toggle_flag(can_frame_t *f , uint8_t flag); 
bool signals_get_flag(const can_frame_t *f,uint8_t flag);

#endif
