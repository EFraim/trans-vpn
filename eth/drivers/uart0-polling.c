#include <math.h>

#include "uart0-polling.h"

#include <lpc2000/lpc214x.h>
#include <lpc2000/io.h>

void uart0Init(int pclock, int baud_rate) {

  U0LCR = BIT1 | BIT0 /* 8-bit words                            */
  | BIT7;             /* enable programming of divisor latches  */

  {
    uint32_t divaddval, mulval, divisor;
    double exact_rate, exact_divisor;
    double error, min_exact_rate, min_error = 1e300;
    uint32_t min_divaddval = 0, min_mulval = 0, min_divisor = 0;

    for (divaddval=0; divaddval <= 15; divaddval++) {
      for (mulval=1; mulval <= 15; mulval++) {
        exact_divisor = (double) pclock / ( (double) baud_rate
            * 16.0 * ( 1.0 + ( (double) divaddval / (double) mulval ) ) );
        divisor = round(exact_divisor);
        exact_rate = (double) pclock / ( (double) divisor * 16.0 * ( 1.0
            + ( (double) divaddval / (double) mulval ) ) );
        error = fabs(exact_rate - (double) baud_rate );
        if (error < min_error) {
          min_error = error;
          min_exact_rate = exact_rate;
          min_divaddval = divaddval;
          min_mulval = mulval;
          min_divisor = divisor;
        }
      }
    }
    U0DLM = (min_divisor & 0x0000FF00) >> 8;
    U0DLL = (min_divisor & 0x000000FF);
    U0FDR = min_divaddval | (min_mulval << 4);
  }

  U0LCR &= ~BIT7; /* disable programming of divisor latches */

  U0FCR = BIT0 | BIT1 | BIT2; /* FIFO control: enable & reset    */

  PINSEL0 &= ~(BIT1 | BIT3); /* select UART function               */
  PINSEL0 |= BIT0 | BIT2; /* for pins P0.0 and P0.1             */
}


void uart0SendByte(uint8_t c) {
  while (!(U0LSR & BIT5));
  U0THR = c;
}

