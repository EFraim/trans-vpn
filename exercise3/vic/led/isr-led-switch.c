#define LPC2148
#include <stdint.h>
#include <lpc2000/io.h>
#include "interrupt.c"
#include "../vic.c"

enum STATE { OFF, ON, BLINKING } state = 0;
unsigned ledOffset=0;
const vic_source_t INT_SAMPLING_TIMER = INT_SOURCE_TIMER0, INT_PERIODIC_TIMER = INT_SOURCE_TIMER1, INT_EXT_BUTTON = INT_SOURCE_EINT1;

void setPeriodicInterruptIn_ms(int ms)
{
  T1MR0 = ms*3000;
  T1MCR = BIT0 | BIT1; //Reset & interrupt
  T1TCR = BIT0;
}

void setSamplingInterruptIn_ms(int ms)
{
  T0MR0 = ms*3000;
  T0MCR = BIT0 | BIT1; //Reset & interrupt
  T0TCR = BIT0;
}

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

void keyPressed()
{
  switch(state) {
  case OFF:
    state = ON;
    setLed(ledOffset, 1);
    break;
  case ON:
    state = BLINKING;
    vicEnable(INT_PERIODIC_TIMER);
    break;
  case BLINKING:
    state = OFF;
    vicDisable(INT_PERIODIC_TIMER);
    setLed(ledOffset, 0);
  }
}

void setBuzzer(int val)
{
  if(val)
    IOPIN0 &= ~BIT7;
  else
    IOPIN0 |= BIT7;
}

void samplingFired()
{
  int prevLedOffset=ledOffset;
  if(~IOPIN0 & BIT19)
    ledOffset++;
  if(~IOPIN0 & BIT18)
    ledOffset--;
  if(prevLedOffset != ledOffset)
    {
      setLed(ledOffset, getLed(prevLedOffset));
      setLed(prevLedOffset, 0);
    }
}

void periodicFired()
{
  if(state == BLINKING)
    setLed(ledOffset, !getLed(ledOffset)); 
}

static int samplesToUnstick = 0;
void __attribute__ ((interrupt("IRQ"))) handleButton() {
  if(!samplesToUnstick)
    {
      setBuzzer(1);
      keyPressed();
      samplesToUnstick = 2;
      EXTINT = BIT1;
    }
  else
    EXTINT = BIT1;
  vicUpdatePriority();
}

void __attribute__ ((interrupt("IRQ"))) handlePeriodicTimer() {
  periodicFired();
  T1IR = BIT0;
  vicUpdatePriority();
}

void __attribute__ ((interrupt("IRQ"))) handleSamplingTimer() {
  samplingFired();
  T0IR = BIT0;
  if(samplesToUnstick)
    samplesToUnstick --;
  else
    setBuzzer(0);
  vicUpdatePriority();
  vicEnable(INT_SAMPLING_TIMER);
}

int main(void) {
  IODIR0 = BIT10 | BIT11 | BIT12 | BIT13 | BIT7;	
  
  PINSEL0 = BIT29;      /* select EINT1 for P0.14      */ 
  EXTMODE = BIT1;       /* INT1 is edge sensitive      */
                        /* falling edge by default     */
  EXTINT  = BIT1;       /* clear the interrupt flag    */
  for(int i=0; i<4; i++)
    setLed(i, 0);

  vicInit();
  setPeriodicInterruptIn_ms(1000);
  setSamplingInterruptIn_ms(200);

  vicEnableIRQ(INT_EXT_BUTTON, 0, handleButton);
  vicEnableIRQ(INT_SAMPLING_TIMER, 1, handleSamplingTimer);
  vicEnableIRQ(INT_PERIODIC_TIMER, 2, handlePeriodicTimer);

  interruptsEnable();
  
  while (1) {
  }
}
