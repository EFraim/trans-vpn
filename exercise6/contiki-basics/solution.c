#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
PROCESS(slow_led_blinking_process, "Our leds will be blinking...");
PROCESS(fast_led_blinking_process, "Red rage...");
PROCESS(test_button_process, "Test button counter");
PROCESS(custom_event_process, "Custom event receiver");
AUTOSTART_PROCESSES(&test_button_process, &hello_world_process, &slow_led_blinking_process, &custom_event_process);
/*---------------------------------------------------------------------------*/
const static char roller[] = {'/','-','\\','|'};
static process_event_t customEvent = PROCESS_EVENT_MAX;

PROCESS_THREAD(hello_world_process, ev, data)
{
  static struct etimer periodicEvent;
  static char stage=0;
  static char blinkingUnleashed = 0;
  customEvent = process_alloc_event();
  PROCESS_BEGIN();

  printf("hello Pavel and Evgeny!\n");

  etimer_set(&periodicEvent, CLOCK_SECOND*4);
  for(;;) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER && (struct etimer*)data == &periodicEvent) {
      printf("%c", roller[stage++%sizeof(roller)]);
      etimer_reset(&periodicEvent);
      process_post(&custom_event_process, customEvent, NULL);
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

PROCESS_THREAD(test_button_process, ev, data)
{
  static int accidentCounter = 0;
  PROCESS_BEGIN();

  button_sensor.activate();
  
  for(;;) {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event &&
			     data == &button_sensor);
    printf("A button has been pressed %d times.\n", ++accidentCounter); 
  }
  
  PROCESS_END();
}

PROCESS_THREAD(custom_event_process, ev, data)
{
  PROCESS_BEGIN();
  for(;;) {
    PROCESS_WAIT_EVENT_UNTIL(ev == customEvent);
    printf("Custom event registered.\n");
  }
  PROCESS_END();
}
