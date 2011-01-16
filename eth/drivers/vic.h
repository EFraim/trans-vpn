#ifndef _VIC_H_
#define _VIC_H_

#include <stdint.h>
#include <lpc2000/lpc214x.h>

typedef void (*interrupt_handler_t)(void); 

#if    defined(LPC2119)
typedef enum {
  INT_CHANNEL_WDT    = 0,
  INT_CHANNEL_SWI    = 1,
  INT_CHANNEL_CORE1  = 2,
  INT_CHANNEL_CORE2  = 3,
  INT_CHANNEL_TIMER0 = 4,
  INT_CHANNEL_TIMER1 = 5,
  INT_CHANNEL_UART0  = 6,
  INT_CHANNEL_UART1  = 7,
  INT_CHANNEL_PWM0   = 8,
  INT_CHANNEL_I2C    = 9,
  INT_CHANNEL_SPI0   = 10,
  INT_CHANNEL_SPI1   = 11,
  INT_CHANNEL_PLL    = 12,
  INT_CHANNEL_RTC    = 13,
  INT_CHANNEL_EINT0  = 14,
  INT_CHANNEL_EINT1  = 15,
  INT_CHANNEL_EINT2  = 16,
  INT_CHANNEL_EINT3  = 17,
  INT_CHANNEL_ADC    = 18,
  INT_CHANNEL_CANACC = 19,
  INT_CHANNEL_CAN1TX = 20,
  INT_CHANNEL_CAN2TX = 21,
  INT_CHANNEL_CAN3TX = 22,
  INT_CHANNEL_CAN4TX = 23,
  INT_CHANNEL_CAN1RX = 26,
  INT_CHANNEL_CAN2RX = 27,
  INT_CHANNEL_CAN3RX = 28,
  INT_CHANNEL_CAN4RX = 29
} interrupt_channel_t;
#endif

#if    defined(LPC2148)
typedef enum {
  INT_CHANNEL_WDT    = 0,
  INT_CHANNEL_SWI    = 1,
  INT_CHANNEL_CORE1  = 2,
  INT_CHANNEL_CORE2  = 3,
  INT_CHANNEL_TIMER0 = 4,
  INT_CHANNEL_TIMER1 = 5,
  INT_CHANNEL_UART0  = 6,
  INT_CHANNEL_UART1  = 7,
  INT_CHANNEL_PWM0   = 8,
  INT_CHANNEL_I2C0   = 9,
  INT_CHANNEL_SPI0   = 10,
  INT_CHANNEL_SPI1   = 11,
  INT_CHANNEL_PLL    = 12,
  INT_CHANNEL_RTC    = 13,
  INT_CHANNEL_EINT0  = 14,
  INT_CHANNEL_EINT1  = 15,
  INT_CHANNEL_EINT2  = 16,
  INT_CHANNEL_EINT3  = 17,
  INT_CHANNEL_ADC    = 18,
  INT_CHANNEL_I2C1   = 19,
  INT_CHANNEL_BOD    = 20,
  INT_CHANNEL_ADC1   = 21,
  INT_CHANNEL_USB    = 22
} interrupt_channel_t;
#endif
                        
#define vicUpdatePriority() (VICVectAddr=0xff)


void vicInit(void);

void vicEnableFIQ(interrupt_channel_t c);
void vicEnableIRQ(interrupt_channel_t c, 
                  uint32_t            priority,
                  interrupt_handler_t h);

void vicEnableDefaultIRQ(interrupt_handler_t h);

static inline void vicEnable(interrupt_channel_t c) {
  VICIntEnable = (1 << c);
}

static inline void vicDisable(interrupt_channel_t c) {
  VICIntEnClr = (1 << c);
}

#endif
