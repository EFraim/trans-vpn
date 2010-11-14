#define LPC2148
#include <stdint.h>
#include "lpc214x.c"
#include "interrupt.c"
#include "vic.c"

volatile int volVar=0;

void setLed(int num, int val)
{
  if(val)
    IOPIN0 &= ~(1 << (10+(num&3)));
  else
    IOPIN0 |= (1 << (10+(num&3)));
}

int getLed(int num)
{
  return !(IOPIN0 & (1 << (10+(num&3))));
}

void __attribute__((interrupt("IRQ"))) vic_isr() {
  while(!volVar)
    ;
  setLed(0, !getLed(0));
  vicUpdatePriority();
  volVar = 0;
  T1IR = BIT0;
}

void __attribute__((interrupt("FIQ"))) fiq_isr() {
  volVar = 1;
  EXTINT = BIT1;
}

int main() {
  IODIR0 = BIT10 | BIT11 | BIT12 | BIT13 | BIT7;	
  
  PINSEL0 = BIT29;      /* select EINT1 for P0.14      */ 
  EXTMODE = BIT1;       /* INT1 is edge sensitive      */
                        /* falling edge by default     */
  EXTINT  = BIT1;       /* clear the interrupt flag    */


  T1MR0 = 10000*3000;
  T1MCR = BIT0 | BIT1; //Reset & interrupt
  T1TCR = BIT0;
  for(int i=0; i<4; i++)
    setLed(i, 0);


  vicInit();
  vicEnableIRQ(INT_SOURCE_TIMER1, 0, vic_isr);
  vicEnableFIQ(INT_SOURCE_EINT1);
  interruptsEnable();
  for(;;);
}
