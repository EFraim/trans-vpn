#include "contiki.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
const static char roller[] = {'/','-','\\','|'};
PROCESS_THREAD(hello_world_process, ev, data)
{
  static struct etimer periodicEvent;
  static char stage=0;
  PROCESS_BEGIN();

  printf("hello Pavel and Evgeny!\n");

  etimer_set(&periodicEvent, CLOCK_SECOND*4);
  for(;;) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER)
      {
	printf("%c", roller[stage++%sizeof(roller)]);
	etimer_reset(&periodicEvent);
      }
  }
  
  PROCESS_END();
}
