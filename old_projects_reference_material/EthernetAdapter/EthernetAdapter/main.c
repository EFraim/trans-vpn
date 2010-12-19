#include <stdint.h>

typedef unsigned int _uint32_t;

#ifdef FAST_GPIO
	#define INIT_GPIO() SCS = BIT0 | BIT1;
	#define IODIR FIO0DIR
	#define IOPIN FIO0PIN
#else
	#define INIT_GPIO() SCS = 0;
	#define IODIR IODIR0
	#define IOPIN IOPIN0
#endif

#define BusyWait(ms) \
{\
	volatile uint32_t busyWaitMax = 800 * (ms);\
	volatile uint32_t busyWaitdummy=0; \
	while(busyWaitdummy < busyWaitMax) busyWaitdummy++;\
}

/* Debug LEDs */
#define KEEP_ALIVE_LED	BIT10
#define EP2_IN_LED		BIT11
#define EP2_OUT_LED		BIT12
#define EP1_IN_LED		BIT13
#define ABORT_LED		BIT15
#define USB_LED			BIT9

#define toggle_led(_led)	\
	if (FIO0PIN & _led)		\
	{						\
		FIO0PIN &= ~_led;	\
	}						\
	else					\
	{						\
		FIO0PIN |= _led;	\
	}

#define led_on(_led) \
	FIO0PIN &= ~_led;

#define led_off(_led) \
	FIO0PIN |= _led;

#define MAC_ADDRESS {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}

uint8_t MACAddress[6] = MAC_ADDRESS;

/* lpc2148 board */
#include <lpc2000/io.h>
#include <lpc2000/interrupt.h>

/* interrupts driver */
#include "drivers/vic.c"

/* serial driver */
#define CLOCKS_PCLK 60000000
#define UART0_BAUD_RATE 19200
#include "drivers/uart0-polling.c"

#define putchar(c) uart0SendByte(c)
#include "printf.c"

#define DBG(fmt, args...) printf(fmt "%s", ##args, "\n")
//#define DBG(fmt, args...)

typedef int bool;

typedef enum
{
	RX_QUEUE,
	TX_QUEUE
} EQueueType;

#define BUFFER_LENGTH 3500
typedef struct
{
	uint8_t buffer[BUFFER_LENGTH];
	volatile uint8_t* startPtr;
	volatile uint8_t* endPtr;
} packetQueue_t;

static packetQueue_t rxPacketQueue;
static packetQueue_t txPacketQueue;

/* USB */
#include "drivers/usb.h"
#include "rndis.c"
#include "drivers/usb.c"

/* Ethernet driver */
#include "drivers/enc28j60.c"

#include "transfer.c"

/*
	Main control loop
*/
int main(int argc, char *argv[])
{
	int32_t i = 0;

	/* Initialize clocks */
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

	// Initialize GPIOs (mainly for debug LEDs)
	{
		// Use slow/fast I/O registers and 
		INIT_GPIO();

		// Enable out direction for LEDs
		IODIR |= (KEEP_ALIVE_LED | EP2_IN_LED | EP2_OUT_LED | EP1_IN_LED | ABORT_LED | USB_LED);

		// Turn off all LEDs
		IOPIN |= (KEEP_ALIVE_LED | EP2_IN_LED | EP2_OUT_LED | EP1_IN_LED | ABORT_LED | USB_LED);
	}

	/* Initialize drivers */
	{
		vicInit();
		uart0Init();

		DBG("Initializing USB Stack");
		usbInit();

		DBG("Initializing Ethernet stack");
		enc28j60_init();

		// Print MAC address

		enc28j60_get_mac_address((uint8_t*)MACAddress);

		DBG("MAC address: %X-%X-%X-%X-%X-%X\n", 
			MACAddress[0],
			MACAddress[1],
			MACAddress[2],
			MACAddress[3],
			MACAddress[4],
			MACAddress[5]);
	}

	TransferInit();

	// Start
	DBG("Starting USB Stack");
	interruptsEnable();

	usbConnect();

	/* Main loop */
	while(1) 
	{	
		// Keep alive LED toggle
		i++;

		if (i == 500000)
		{
			i = 0;
			toggle_led(KEEP_ALIVE_LED);
		}

  		if (rndis_state == RNDIS_STATE_DATA_INITIALIZED)
		{
			// Receive ethernet packet
			uint16_t bytes = enc28j60_packet_receive_ex();
			if (bytes > 0) 
			{
				//DBG("[Main][Rx][enc] %u", bytes);
				eth_nak_interrupts |= INACK_BI;
				usbEnableNAKInterrupts(eth_nak_interrupts);
			}
			
			// Send ethernet packet
			bytes = enc28j60_packet_send_ex();
			if (bytes > 0)
			{
				//DBG("[Main][Tx][enc] %u", bytes);
			}

			// A packet was received over the USB - copy to Tx queue
			if (tx_packet_state == TX_PACKET_STATE_READY)
			{
				uint8_t add = AddPacketToTxQueue(RNDISPacketHeaderTx->MessageLength, data_tx_buffer);
				
				if (add)
				{
					// Free the buffer for the next packet from the USB
					tx_packet_state = TX_PACKET_STATE_EMPTY;
					//DBG("[Main][Tx]Add to queue %d", add);
				}
			}

			// Sending packets to the USB is done in interrupt context
		}
	}

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

static uint8_t* abort_message[3] = {"Data abort\n", "Prefetch abort\n", "Undefined opcode\n"};

static void abort_handler(uint8_t abort_type)
{
	uint32_t i = 0;

	while(1)
	{
		// LED toggle to indicate abort
		i++;

		if (i == 50000)
		{
			i = 0;

			toggle_led(ABORT_LED);

			DBG("[ABORT] %s", abort_message[abort_type]);
		}
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
