#include <stdint.h>

void ms_sleep(int ms) {}

typedef unsigned int _uint32_t;

inline void BusyWait(int ms) {
    volatile uint32_t busyWaitMax = 800 * (ms);
    volatile uint32_t busyWaitdummy=0;
    while(busyWaitdummy < busyWaitMax) busyWaitdummy++;
}

/* Debug LEDs */
#define KEEP_ALIVE_LED	BIT10
#define EP2_IN_LED		BIT11
#define EP2_OUT_LED		BIT12
#define EP1_IN_LED		BIT13
#define ABORT_LED		BIT15
#define USB_LED			BIT9

#define MAC_ADDRESS {0x00, 0x18, 0x0A, 0x01, 0xDF, 0x44}

uint8_t MACAddress[6] = MAC_ADDRESS;

/* lpc2148 board */
#include <lpc2000/io.h>
#include <lpc2000/interrupt.h>
#include <lpc2000/lpc214x.h>


static inline void INIT_GPIO() { SCS = BIT0 | BIT1; }
#define IODIR FIO0DIR
#define IOPIN FIO0PIN

static inline void toggle_led(int _led) {  FIO0PIN ^= _led; }
static inline void led_on(int _led) { FIO0PIN &= ~_led; }
static inline void led_off(int _led) { FIO0PIN |= _led; }

/* interrupts driver */
#include "drivers/vic.h"

/* serial driver */
#define CLOCKS_PCLK 60000000
#define UART0_BAUD_RATE 19200
#include "drivers/uart0-polling.h"

#include "log.h"

typedef int bool;

/* USB */
#include "drivers/usb.h"

#include "usbnet.h"

/* Ethernet driver */
#include "drivers/enc28j60.h"

#include "app.h"

void initPLL()
{
  VPBDIV = 0x00000001;  /* PCLK = CCLK */
  PLLCFG  = 0x00000024; /* Fosc=12MHz, CCLK=60MHz */
  PLLCON  = 0x00000001; /* enable the PLL  */
  PLLFEED = 0x000000AA; /* feed sequence   */
  PLLFEED = 0x00000055;
  while (!(PLLSTAT & 0x00000400));
  PLLCON = 3;     // enable and connect
  PLLFEED = 0xAA;
  PLLFEED = 0x55;
}

void initGPIO()
{
  // Use slow/fast I/O registers and 
  INIT_GPIO();
  
  // Enable out direction for LEDs
  IODIR |= (KEEP_ALIVE_LED | EP2_IN_LED | EP2_OUT_LED | EP1_IN_LED | ABORT_LED | USB_LED);
  
  // Turn off all LEDs
  IOPIN |= (KEEP_ALIVE_LED | EP2_IN_LED | EP2_OUT_LED | EP1_IN_LED | ABORT_LED | USB_LED);
}

int main(int argc, char *argv[])
{
    initPLL();
    initGPIO();
	vicInit();
	uart0Init(CLOCKS_PCLK, UART0_BAUD_RATE);
    
    LOG_INFO("Initializing USB Stack");
	usbInit();
    usbnet_init();
    
	LOG_INFO("Initializing Ethernet stack");
	enc28j60_init(&IODIR, &IOPIN, MACAddress);
	
	// Print MAC address
    enc28j60_get_mac_address((uint8_t*)MACAddress);
    LOG_INFO("MAC address: %X-%X-%X-%X-%X-%X\n", MACAddress[0], MACAddress[1], MACAddress[2], MACAddress[3], MACAddress[4], MACAddress[5]);

	LOG_INFO("Starting USB Stack");
	interruptsEnable();

	usbConnect();
    
    LOG_INFO("Entering main loop");
    
    app_loop();

	return 0;
}


/* 
	Exception handling
*/
typedef enum
{
	EXCEPTION_DATA_ABORT,
	EXCEPTION_PREFETCH_ABORT,
	EXCEPTION_UNDEFINED_OPCODE
} EXCEPTION_TYPE;

static char* abort_message[3] = {"Data abort\n", "Prefetch abort\n", "Undefined opcode\n"};

static void abort_handler(uint8_t abort_type)
{
  for(uint32_t i=0; ;i++)
    if (i % 50000 == 0)
      {
	toggle_led(ABORT_LED);
	LOG_FATAL("[ABORT] %s", abort_message[abort_type]);
      }
  
  vicUpdatePriority();
}

void __attribute__ ((interrupt("IRQ"))) dabt_isr(void) 
{
	abort_handler(EXCEPTION_DATA_ABORT);
}

void __attribute__ ((interrupt("IRQ"))) pabt_isr(void) 
{
	abort_handler(EXCEPTION_PREFETCH_ABORT);
}

void __attribute__ ((interrupt("IRQ"))) undef_isr(void) 
{
	abort_handler(EXCEPTION_UNDEFINED_OPCODE);
}
