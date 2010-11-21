#include <stdint.h>
#include "lpc2000/io.h"
#include "lpc2000/lpc214x.h"
#include "lpc2000/interrupt.h"
#include <interrupt.c>

static const uint32_t LED_MASK = 1U << 10;
static const uint8_t PCON_IDL = 0x1, PCON_PD = 0x2;
static const uint32_t PCTIM0 = 0x2;
static const uint32_t EXTWAKE0 = 0x1, EXTWAKE1 = 0x2;

static inline void triggerLed()
{
  IOPIN0 ^= LED_MASK;
}

extern "C" void __attribute__ ((interrupt("FIQ"))) fiq_isr(void) {
  if(VICFIQStatus & BIT4)
    {
      static const int TTL = 4;
      static int TicksToLive = TTL*2;
      triggerLed();
      T0IR = BIT0;
      if(!--TicksToLive) {
	TicksToLive = TTL*2;	
	PCON = PCON_PD;
	EXTINT  = BIT1;       /* clear the interrupt flag    */
      }
    }
}

void enableInterruptSource(int mask)
{
  VICIntEnable = mask;
}

void configure_periodic_shift() {
  T0MR0 = 500*3000;
  T0MCR = BIT0 | BIT1; //Reset & interrupt
  T0TCR = BIT0;
  VICIntSelect = BIT4;
  enableInterruptSource(BIT4);
  enable_interrupts();
}

void feed_PLL() {
  disable_interrupts();
  PLLFEED = 0xAA;
  PLLFEED = 0x55;
  enable_interrupts();
}


int main() {
  PLLCFG = (0x1 << 5) | 0x4;
  PLLCON = 0x1;
  feed_PLL();
  while(!(PLLSTAT & (1<<10)))
    ;
  PLLCON = 0x3;
  feed_PLL();
  INTWAKE = EXTWAKE1;
  PINSEL0 = BIT29;      /* select EINT1 for P0.14      */ 
  EXTMODE = BIT1;       /* INT1 is edge sensitive      */
                        /* falling edge by default     */
  EXTPOLAR = 0;
  EXTINT  = BIT1;       /* clear the interrupt flag    */
  IODIR0 |= LED_MASK;
  IOPIN0 |= LED_MASK;
  configure_periodic_shift();
  PCONP = PCTIM0;
  for(;;)
    PCON = PCON_IDL;
  return 0;
}
