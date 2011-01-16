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

void BusyWait(int ms) {
    volatile uint32_t busyWaitMax = 800 * (ms);\
    volatile uint32_t busyWaitdummy=0; \
    while(busyWaitdummy < busyWaitMax) busyWaitdummy++;\
}
// #define BusyWait(ms) \
// {\
// 	volatile uint32_t busyWaitMax = 800 * (ms);\
// 	volatile uint32_t busyWaitdummy=0; \
// 	while(busyWaitdummy < busyWaitMax) busyWaitdummy++;\
// }

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

#define ETHERNET_FRAME_SIZE_MAX  1536
#define MAC_ADDRESS {0x00, 0x18, 0x0A, 0x01, 0xDF, 0x44}

uint8_t MACAddress[6] = MAC_ADDRESS;

/* lpc2148 board */
#include <lpc2000/io.h>
#include <lpc2000/interrupt.h>

/* interrupts driver */
#include "drivers/vic.h"

/* serial driver */
#define CLOCKS_PCLK 60000000
#define UART0_BAUD_RATE 19200
#include "drivers/uart0-polling.h"

//#define putchar(c) uart0SendByte(c)
//#include "printf.h"
#include "debug.h"


//#define DBG(fmt, args...) printf(fmt "%s", ##args, "\n")
//#define DBG(fmt, args...)

typedef int bool;

// typedef enum
// {
// 	RX_QUEUE,
// 	TX_QUEUE
// } EQueueType;


// #define BUFFER_LENGTH 3500
// typedef struct
// {
// 	uint8_t buffer[BUFFER_LENGTH];
// 	volatile uint8_t* startPtr;
// 	volatile uint8_t* endPtr;
// } packetQueue_t;
// 
// static packetQueue_t rxPacketQueue;
// static packetQueue_t txPacketQueue;

/* USB */
#include "drivers/usb.h"

#include "usbnet.h"
//#include "rndis.c"
//#include "drivers/usb.c"

/* Ethernet driver */
#include "drivers/enc28j60.h"

//#include "transfer.c"

typedef struct {
    uint8_t data[ETHERNET_FRAME_SIZE_MAX];
    uint16_t length;
} packet_t;

#define RING_SIZE 4

typedef struct {
    packet_t pkt[RING_SIZE];
    int begin;
    int size;
} ring_t;

ring_t rx_ring;
ring_t tx_ring;

void ring_init(ring_t* ring) {
    ring->begin = 0;
    ring->size = 0;
}
int ring_is_empty(ring_t* ring) {
    return ring->size == 0;
}
int ring_is_full(ring_t* ring) {
    return ring->size == RING_SIZE;
}
void ring_append(ring_t* ring) {
    ring->size++;
}
packet_t* ring_next(ring_t* ring) {
    return &(ring->pkt[(ring->begin + ring->size) & (RING_SIZE - 1)]);
}
packet_t* ring_top(ring_t* ring) {
    return &(ring->pkt[ring->begin]);
}
void ring_pop(ring_t* ring) {
    ring->begin = (ring->begin + 1) & (RING_SIZE - 1);
    ring->size--;
}


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
		uart0Init(CLOCKS_PCLK, UART0_BAUD_RATE);

		DBG("Initializing USB Stack");
		usbInit();

		DBG("Initializing Ethernet stack");
		enc28j60_init(&IODIR, &IOPIN, MACAddress);

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

	//TransferInit();

	// Start
	DBG("Starting USB Stack");
	interruptsEnable();

	usbConnect();

    usbnet_init();
    DBG("Entering main loop");
    
	/* Main loop */
	while(1) 
	{	
		// Keep alive LED toggle
		i++;

		if (i == 50000)
		{
			i = 0;
			toggle_led(KEEP_ALIVE_LED);
            DBG("Alive!");
		}

        if (!ring_is_full(&rx_ring)) {
            packet_t* pkt = ring_next(&rx_ring);
            int recv_len = enc28j60_packet_receive(sizeof(pkt->data), pkt->data);
            if (recv_len > 0) {
                DBG("Received ethernet frame of size %d", recv_len);
                if (usbnet_send(pkt->data, recv_len)) {
                    pkt->length = recv_len;
                    ring_append(&rx_ring);
                }
            }
        }
        
        //DBG("Trying to post recieve buffers");
        while (!ring_is_full(&tx_ring)) {
            packet_t* pkt = ring_next(&tx_ring);
            pkt->length = sizeof(pkt->data);
            //DBG("Posting buffer to usbnet...");
            if (usbnet_recv(pkt->data, pkt->length)) {
                DBG("Posted buffer to usbnet");
                ring_append(&tx_ring);
            }
        }
        
        //DBG("Probing for sent buffers");
        while (usbnet_pop_completed_send()) {
            DBG("Sending full frame to USB done");
            ring_pop(&rx_ring);
        }
        
        //DBG("Probing for received buffers, and send them over Eth");
        int recv_len;        //assert(len > 0);
        while ((recv_len = usbnet_pop_completed_recv())) {
            DBG("Got full frame from USB");
            packet_t* pkt = ring_top(&tx_ring);
            pkt->length = recv_len;
            enc28j60_packet_send(pkt->length, pkt->data);
            DBG("Sent ethernet frame of size %d", pkt->length);
            ring_pop(&tx_ring);
        }

                
// 			// Receive ethernet packet
// 			uint16_t bytes = enc28j60_packet_receive_ex();
// 			if (bytes > 0) 
// 			{
// 				//DBG("[Main][Rx][enc] %u", bytes);
// 				eth_nak_interrupts |= INACK_BI;
// 				usbEnableNAKInterrupts(eth_nak_interrupts);
// 			}
// 			
// 			// Send ethernet packet
// 			bytes = enc28j60_packet_send_ex();
// 			if (bytes > 0)
// 			{
// 				//DBG("[Main][Tx][enc] %u", bytes);
// 			}
// 
// 			// A packet was received over the USB - copy to Tx queue
// 			if (tx_packet_state == TX_PACKET_STATE_READY)
// 			{
// 				uint8_t add = AddPacketToTxQueue(RNDISPacketHeaderTx->MessageLength, data_tx_buffer);
// 				
// 				if (add)
// 				{
// 					// Free the buffer for the next packet from the USB
// 					tx_packet_state = TX_PACKET_STATE_EMPTY;
// 					//DBG("[Main][Tx]Add to queue %d", add);
// 				}
// 			}

			// Sending packets to the USB is done in interrupt context
		//}
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

static char* abort_message[3] = {"Data abort\n", "Prefetch abort\n", "Undefined opcode\n"};

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
