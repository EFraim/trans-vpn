#include <lpc2000/io.h>
#include <lpc2000/interrupt.h>

enum STATE { OFF, ON, BLINKING } state = 0;

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
  T1MCR = BIT0 | BIT1; //Reset & interrupt & stop
  T1TCR = BIT0;
}

void keyPressed()
{
  switch(state) {
  case OFF:
    state = ON;
    IOPIN0 &= ~BIT10;
    break;
  case ON:
    state = BLINKING;
    enableInterruptSource(BIT5);
    break;
  case BLINKING:
    state = OFF;
    disableInterruptSource(BIT5);
    IOPIN0 |= BIT10;
  }
}

void periodicFired()
{
  if(state == BLINKING)
    IOPIN0 = IOPIN0 ^ BIT10; 
}

void __attribute__ ((interrupt("FIQ"))) fiq_isr(void) {
  if(VICFIQStatus & BIT15)
    if(!(T0TCR & BIT0))
      {
	keyPressed();
	T0TC = 0;
	T0MR0 = 400*3000; //400ms grace period. My hand is slow and seemingly shaky today
	T0MCR = BIT1 | BIT2; //Reset & stop
	T0TCR = BIT0;
	EXTINT = BIT1;
      }
    else
      EXTINT = BIT1;
      
  if(VICFIQStatus & BIT5)
    {
      periodicFired();
      T1IR = BIT0;
    }
}

int main(void) {
  IODIR0 = BIT10;	
  
  PINSEL0 = BIT29;      /* select EINT1 for P0.14      */ 
  EXTMODE = BIT1;       /* INT1 is edge sensitive      */
                        /* falling edge by default     */
  EXTINT  = BIT1;       /* clear the interrupt flag    */
  IOPIN0 |= BIT10;

  setPeriodicInterruptIn_ms(1000);

  VICIntSelect = BIT15 | BIT5;
  enableInterruptSource(BIT15);
  
  enable_interrupts();
  
  while (1) {
  }
}
