/*
 * interrupts.h
 * 
 * Sivan Toledo.
 */

#ifndef LPC2000_INTERRUPT_H
#define LPC2000_INTERRUPT_H

#include <stdint.h>

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

//void interruptEnableFIQ(interrupt_channel_t c);
//void interruptEnableVIC(interrupt_channel_t c, 
//                        uint16_t            priority,
//                        interrupt_handler_t h);
                        
// this must be done near the end of an ISR to
// update the priority hardware
                        
#define interruptUpdatePriority() (VICVectAddr=0xff)

#define disable_interrupts()											\
  asm volatile (															\
		"STMDB	SP!, {R0}		\n\t"	/* Push R0.						*/	\
		"MRS	R0, CPSR		\n\t"	/* Get CPSR.					*/	\
		"ORR	R0, R0, #0xC0	\n\t"	/* Disable IRQ, FIQ.			*/	\
		"MSR	CPSR, R0		\n\t"	/* Write back modified value.	*/	\
		"LDMIA	SP!, {R0}			" )	/* Pop R0.						*/
			
#define enable_interrupts()												\
  asm volatile (															\
		"STMDB	SP!, {R0}		\n\t"	/* Push R0.						*/	\
		"MRS	R0, CPSR		\n\t"	/* Get CPSR.					*/	\
		"BIC	R0, R0, #0xC0	\n\t"	/* Enable IRQ, FIQ.				*/	\
		"MSR	CPSR, R0		\n\t"	/* Write back modified value.	*/	\
		"LDMIA	SP!, {R0}			" )	/* Pop R0.						*/

#endif                             
