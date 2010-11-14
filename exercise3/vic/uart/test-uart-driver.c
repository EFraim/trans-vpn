#include <stdint.h>
#include <lpc214x.c>
#include <interrupt.c>
#define LPC2148
#include "../vic.c"

volatile int dummy;
const int delay = 400000;

#define CLOCKS_PCLK 3000000

#define UART0_BAUD_RATE 19200
#include "uart0.c"

#define  printf_char(x) uart0QueueByte(x)
#include "printf.c"

int main(void) {
  IODIR0 = BIT10;
  uart0Init();
  
  interruptsEnable();

  while (1) {
    uint32_t i;
    for (i=0; i<delay; i++) dummy=1;
    IOPIN0 |=   BIT10;
    for (i=0; i<delay; i++) dummy=1;
    IOPIN0 &=  ~BIT10;

    i = 0;
    while (!uart0TxBufferTrylock()) i++;
    if(uart0RxBufferReady())
      {
	uart0RxBuffer[uart0RxBufferSize-1] = 0;
	printf("Just got from you: %s\n", uart0RxBuffer);
	uart0RxBufferRelease();
      }
    else
      printf("uart tx acquired at %d\n",i);

    uart0Send();
  }
}
