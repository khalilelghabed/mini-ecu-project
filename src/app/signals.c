#include "signals.h"
#define SIG_TEMP_BYTE    4U
#define SIG_TEMP_OFFSET  (-SIG_TEMP_MIN)
#include <stddef.h>



void signals_init(can_frame_t *f)
{
    if (f == NULL) {
        return;
    }
   
    f->id = 0x000;
    f->dlc = CAN_MAX_DLC;

    for (uint8_t i = 0; i < CAN_MAX_DLC; i++) {
        f->data[i] = 0;
    }
}


void signals_set_speed(can_frame_t *f ,uint16_t v){
   if (f==NULL){
      return;
}
  
 
 f->data[0]=  v & 0xff;

 f->data[1]= v>>8;

   

}


uint16_t signals_get_speed(const can_frame_t *f)
{
    if (f == NULL) {
        return 0;
    }

    uint16_t v;

    v = (uint16_t)f->data[0] |
        ((uint16_t)f->data[1] << 8);

    return v;
}

void signals_set_setpoint(can_frame_t *f , uint16_t v){

          if (f==NULL){
           return ;

 }
        f->data[2]= v & 0xFF;
        f->data[3]= v >>  8;
}


uint16_t signals_get_setpoint ( const can_frame_t *f){
    if (f==NULL){
           return 0 ;

 }

   uint16_t    v= (uint16_t)f->data[3] << 8 | (uint16_t)f->data[2] ;
   return v;
}


bool signals_set_temp(can_frame_t *f,int16_t celsius){
     if (f==NULL){
      return false;
}
     if(celsius < SIG_TEMP_MIN || celsius > SIG_TEMP_MAX){
     return false  ;
}
     uint8_t temp= celsius +  SIG_TEMP_OFFSET ; 
     f->data[SIG_TEMP_BYTE]=temp; 
     return true ; 

}


int16_t signals_get_temp(const can_frame_t *f)
{
    if (f == NULL) {
        return SIG_TEMP_MIN;                /* valeur de repli */
    }
    return (int16_t)((int16_t)f->data[SIG_TEMP_BYTE] - SIG_TEMP_OFFSET);
}

void     signals_set_flag(can_frame_t *f, uint8_t flag){
     if (f == NULL) {
        return ;                
    }
    f->data[5]=(uint8_t)(f->data[5] |  flag);

}


void     signals_clear_flag(can_frame_t *f, uint8_t flag){
    if (f == NULL) {
        return ;
    }
   f->data[5]=(uint8_t)(f->data[5] &  ~flag);

}


void signals_toggle_flag(can_frame_t *f, uint8_t flag)
{
    if (f == NULL) {
        return;
    }

    f->data[5] ^= flag;
 
}


bool signals_get_flag(const can_frame_t *f, uint8_t flag)
{
    if (f == NULL) {
        return false;
    }

    return (f->data[5] & flag) != 0U;
}
