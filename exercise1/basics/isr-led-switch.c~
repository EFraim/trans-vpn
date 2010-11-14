#include <lpc2000/io.h>
#include <lpc2000/interrupt.h>

void __attribute__ ((interrupt("FIQ"))) fiq_isr(void) {
  EXTINT  = BIT1;       /* clear the interrupt flag    */

                        /* toggle the LED              */
  if (IOPIN0 & BIT10) IOPIN0 &= ~BIT10; 
  else                IOPIN0 |=  BIT10;
}

int main(void) {
  IODIR0 = BIT10;	
  
  PINSEL0 = BIT29;      /* select EINT1 for P0.14      */ 
  EXTMODE = BIT1;       /* INT1 is edge sensitive      */
                        /* falling edge by default     */
  EXTINT  = BIT1;       /* clear the interrupt flag    */

  VICIntSelect = BIT15; /* EINT1 is the fast interrupt */
  VICIntEnable = BIT15; /* enable EINT1                */
  
  enable_interrupts();
  
  while (1) {
  }
}