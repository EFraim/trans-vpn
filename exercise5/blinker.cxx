#include <stdint.h>
#include "lpc2000/io.h"
#include "lpc2000/lpc214x.h"
#include "lpc2000/interrupt.h"
#include <interrupt.c>

static const uint32_t LED_MASK = 1U << 10;

static inline void triggerLed()
{
  IOPIN0 ^= LED_MASK;
}

extern "C" void __attribute__ ((interrupt("FIQ"))) fiq_isr(void) {
  if(VICFIQStatus & BIT4)
    {
      triggerLed();
      T0IR = BIT0;
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
  IODIR0 |= LED_MASK;
  configure_periodic_shift();
  for(;;)
    ;
  return 0;
}
