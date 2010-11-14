#include <stdint.h>

#include <lpc214x.c>

volatile int dummy;
const int delay = 100000;

#define CLOCKS_PCLK 3000000

#define UART0_BAUD_RATE 19200
#include "uart0-polling.c"

#define  print_char(x) uart0SendByte(x)
#include "print.c"

int main(void) {
  uart0Init();
  
  IODIR0 = BIT10;

  while (1) {
    uint32_t i;
    for (i=0; i<delay; i++) dummy=1;
    IOPIN0 |=   BIT10;
    for (i=0; i<delay; i++) dummy=1;
    IOPIN0 &=  ~BIT10;

    print("uart test\n");
  }
}
