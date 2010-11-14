
#include <math.h>
#include <stdbool.h>

uint8_t uart0TxBuffer[81];
int16_t uart0TxBufferSize = 0;
int16_t uart0TxBufferPtr  = 0; // next byte to transmit
uint8_t uart0TxBufferAvailable = 1;
bool uart0RxBufferReadyFlag = false;
int16_t uart0RxBufferSize = 0;
uint8_t uart0RxBuffer[81];


static bool bytesLeftToTx() {
  return uart0TxBufferPtr < uart0TxBufferSize;
}

static void sendNextByte() {
  U0THR = uart0TxBuffer[uart0TxBufferPtr++];
}

static void endTransfer() {
  U0IER &= ~BIT1;
  uart0TxBufferAvailable = 1;
  uart0TxBufferSize = uart0TxBufferPtr = 0;
}

void inline handleUART() {
  if(!uart0TxBufferAvailable&&uart0TxBufferPtr&&(U0LSR & BIT5)) { //We are in the middle of transmission
    sendNextByte();
    if(!bytesLeftToTx())
      endTransfer();
  }
  while(U0LSR & BIT0) {
    if(!uart0RxBufferReadyFlag) {
      uart0RxBuffer[uart0RxBufferSize++] = U0RBR;
      if(uart0RxBufferSize == sizeof(uart0RxBuffer)/sizeof(uint8_t)) //We don't have a good option here, but seems this better than writing all over the stack
	uart0RxBufferSize--;
      if(uart0RxBuffer[uart0RxBufferSize-1] == '\n')
	uart0RxBufferReadyFlag = true;
    }
  }
}

void __attribute__ ((interrupt("FIQ"))) fiq_isr(void) {
  if(VICFIQStatus & BIT6)
    handleUART();
}
    


void uart0QueueByte(char c) {
  uart0TxBuffer[uart0TxBufferSize++] = c;
}

void uart0Send() {
  interrupts_state_t is = interruptsSaveAndDisable(); //We do not want Rx interrupt to happen while we are touching state used in ISR.
  if(bytesLeftToTx())
    {
      while(!(U0LSR & BIT5)); //Initial polling, to make sure there are no leftovers from previous transmission
      sendNextByte();
      if(bytesLeftToTx())
	U0IER |= BIT1;
      else
	endTransfer();
    }
  else
    endTransfer();
  interruptsRestore(is);
}

int uart0TxBufferTrylock() {
  int result;
  interrupts_state_t is = interruptsSaveAndDisable();
  result = uart0TxBufferAvailable;
  if (uart0TxBufferAvailable)
    uart0TxBufferAvailable=0;
  interruptsRestore(is);
  return result; // we failed to acquire the lock :-(
}

int uart0RxBufferReady() {
  return uart0RxBufferReadyFlag;
}

void uart0RxBufferRelease() {
  uart0RxBufferSize = 0;
  uart0RxBufferReadyFlag = false;
}


static void uart0Init() {
  uint32_t divisor;

  U0LCR = BIT1 | BIT0 /* 8-bit words                            */
  | BIT7;             /* enable programming of divisor latches  */

  {
    uint32_t divaddval, mulval, divisor;
    double exact_rate, exact_divisor;
    double error, min_exact_rate, min_error = 1e300;
    uint32_t min_divaddval, min_mulval, min_divisor;

    for (divaddval=0; divaddval <= 15; divaddval++) {
      for (mulval=1; mulval <= 15; mulval++) {
        exact_divisor = (double) CLOCKS_PCLK / ( (double) UART0_BAUD_RATE
            * 16.0 * ( 1.0 + ( (double) divaddval / (double) mulval ) ) );
        divisor = round(exact_divisor);
        exact_rate = (double) CLOCKS_PCLK / ( (double) divisor * 16.0 * ( 1.0
            + ( (double) divaddval / (double) mulval ) ) );
        error = fabs(exact_rate - (double) UART0_BAUD_RATE );
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

  VICIntSelect |= BIT6;
  VICIntEnable = BIT6;
  U0IER = BIT0;

  /* you may need other initialization !!! (in particular, to configure interrupts) */
}
