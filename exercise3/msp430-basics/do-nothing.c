#include <io.h>
#include <signal.h>

interrupt(PORT2_VECTOR) port2_isr() {
}

typedef enum { RED=4, GREEN=5, BLUE=6 } Color;

void setLed(Color led, int val) {
  if(!val)
    P5OUT |= 1 << led;
  else
    P5OUT &= ~(1 << led);
}

void delay() {
  volatile uint32_t i;
  while(P2IN & 0x80)
    ;
  for(i=0; i<1000; i++)
    ;
}

int main(void) { 
  WDTCTL = WDTPW | WDTHOLD; /* stop the watchdog timer    */
  
  eint();                   /* enable interrupts          */
  P5DIR = 0x70;
  P5OUT = 0x70;
  
  while (1) {
    setLed(RED, 1);
    delay();
    setLed(RED, 0);
    setLed(GREEN, 1);
    delay();
    setLed(GREEN, 0);
    setLed(BLUE, 1);
    delay();
    setLed(BLUE, 0);
    delay();
  }
}
