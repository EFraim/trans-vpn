#include "contiki.h"
#include "dev/leds.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
PROCESS(slow_led_blinking_process, "Our leds will be blinking...");
PROCESS(fast_led_blinking_process, "Red rage...");
AUTOSTART_PROCESSES(&hello_world_process, &slow_led_blinking_process);
/*---------------------------------------------------------------------------*/
const static char roller[] = {'/','-','\\','|'};
PROCESS_THREAD(hello_world_process, ev, data)
{
  static struct etimer periodicEvent;
  static char stage=0;
  static char blinkingUnleashed = 0;
  PROCESS_BEGIN();

  printf("hello Pavel and Evgeny!\n");

  etimer_set(&periodicEvent, CLOCK_SECOND*4);
  for(;;) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER && (struct etimer*)data == &periodicEvent) {
      printf("%c", roller[stage++%sizeof(roller)]);
      etimer_reset(&periodicEvent);
      if(!blinkingUnleashed) {
	blinkingUnleashed = 1;
	process_start(&fast_led_blinking_process, NULL);
      }
    }
  }
  
  PROCESS_END();
}

PROCESS_THREAD(slow_led_blinking_process, ev, data)
{
  static struct etimer blinkTimer;
  PROCESS_BEGIN();
  etimer_set(&blinkTimer, CLOCK_SECOND/2);
  for(;;) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER && (struct etimer*)data == &blinkTimer) {
      leds_toggle(LEDS_GREEN);
      etimer_reset(&blinkTimer);
    }
  }
  PROCESS_END();
}

PROCESS_THREAD(fast_led_blinking_process, ev, data)
{
  static struct etimer blinkTimer;
  PROCESS_BEGIN();
  etimer_set(&blinkTimer, CLOCK_SECOND/20);
  for(;;) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER && (struct etimer*)data == &blinkTimer) {
      leds_toggle(LEDS_RED);
      etimer_reset(&blinkTimer);
    }
  }
  PROCESS_END();
}
