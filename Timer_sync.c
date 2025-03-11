#include "timer_sync.h"
/*----------------------------------------------------------------------------
 *      Practica 2 Timer
 *---------------------------------------------------------------------------*/
 
// One-Shoot Timer Function
static void Timer1_Callback (void const *arg) {
  osTimerStart(tim_idsync,30000U);
	get_time();
}


int Init_Timers (void) {
  osStatus_t status;                           
 
  // Create one-shoot timer
  exec1 = 1U;
  tim_idsync = osTimerNew((osTimerFunc_t)&Timer1_Callback, osTimerOnce, &exec1, NULL);
  if (tim_idsync != NULL) {  // One-shot timer created
    
    status = osTimerStart(tim_idsync, 5000U); 
    if (status != osOK) {
      return -1;
    }
  }
  
  return NULL;
}
