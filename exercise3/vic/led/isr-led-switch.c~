#include <lpc2000/io.h>
#include <lpc2000/interrupt.h>

enum STATE { OFF, ON, BLINKING } state = 0;
unsigned ledOffset=0;

void enableInterruptSource(int mask)
{
  VICIntEnable = mask;
}

void disableInterruptSource(int mask)
{
  VICIntEnClr = mask;
}

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
    enableInterruptSource(BIT5);
    break;
  case BLINKING:
    state = OFF;
    disableInterruptSource(BIT5);
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

void __attribute__ ((interrupt("FIQ"))) fiq_isr(void) {
  static int samplesToUnstick = 0;
  if(VICFIQStatus & BIT15)
    if(!samplesToUnstick)
      {
	setBuzzer(1);
	keyPressed();
	samplesToUnstick = 2;
	EXTINT = BIT1;
      }
    else
      EXTINT = BIT1;
      
  if(VICFIQStatus & BIT5)
    {
      periodicFired();
      T1IR = BIT0;
    }

  if(VICFIQStatus & BIT4)
    {
      samplingFired();
      T0IR = BIT0;
      if(samplesToUnstick)
	samplesToUnstick --;
      else
	setBuzzer(0);
    }
}

int main(void) {
  IODIR0 = BIT10 | BIT11 | BIT12 | BIT13 | BIT7;	
  
  PINSEL0 = BIT29;      /* select EINT1 for P0.14      */ 
  EXTMODE = BIT1;       /* INT1 is edge sensitive      */
                        /* falling edge by default     */
  EXTINT  = BIT1;       /* clear the interrupt flag    */
  for(int i=0; i<4; i++)
    setLed(i, 0);

  setPeriodicInterruptIn_ms(1000);
  setSamplingInterruptIn_ms(200);

  VICIntSelect = BIT15 | BIT5 | BIT4;
  enableInterruptSource(BIT15 | BIT4);
  
  enable_interrupts();
  
  while (1) {
  }
}
