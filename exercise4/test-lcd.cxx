#include <stdint.h>
#include <lpc214x.c>
#include "lpc2000/interrupt.h"
#include <interrupt.c>
#include "lcd.cxx"
typedef CommandLayer<MSBusHAL, true> LCD;

static int maxLength = 0;
static volatile int lcdAccess = 1;

void print_char(char c) {
  lcdAccess = 1;
  static int cur_line=0;
  static int lineLength[2]={0};
  if(c != '\n') {
    LCD::write_mem(c);
    lineLength[cur_line]++;
  } else {
    lineLength[cur_line ^= 1] = 0;
    LCD::set_line(cur_line);
  }
  maxLength = lineLength[0] > lineLength[1] ? lineLength[0] : lineLength[1];
  lcdAccess = 0;
}

extern "C" void __attribute__ ((interrupt("FIQ"))) fiq_isr(void) {
  if(VICFIQStatus & BIT4)
    {
      if(!lcdAccess) { //Otherwise we can wait. This is just cosmetic and not urgent
	static const int LINE_LENGTH=16;
	static int curOffset = 0;
	static bool returning = false;
	if(curOffset >= maxLength-LINE_LENGTH)
	  returning = true;
	if(curOffset == 0)
	  returning = false;
	if(curOffset != 0 || maxLength > LINE_LENGTH)
	  {
	    LCD::write_opcode(SHIFT_CMD(SHIFT_CMD::DISPLAY, returning ? SHIFT_CMD::RIGHT : SHIFT_CMD::LEFT));
	    curOffset += returning ? -1 : 1;
	  }
      }
      IOPIN0 ^= BIT10 | BIT11;
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

#define printf_char(c) print_char(c)
#include "print.c"
#include "printf.c"


int main() {
  IODIR0 |= BIT10 | BIT11;
  IOPIN0 &= ~(BIT10 | BIT11);
  LCD::init();
  lcdAccess = 0;
  IOPIN0 |= BIT10;
  printf("Special longer than line code: %d\nHello world!", 777);
  IOPIN0 |= BIT11;
  configure_periodic_shift();
  for(;;)
    ;
  return 0;
}
