#include <stdint.h>

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

#define ETHERNET_FRAME_SIZE_MAX  1536
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

typedef struct {
    uint8_t data[ETHERNET_FRAME_SIZE_MAX + 4];
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
	
    /* Main loop */
	for(uint32_t iteration=0; ; iteration++) 
	{	
        // Keep alive LED toggle
        if (iteration % 10000 == 0)
            toggle_led(KEEP_ALIVE_LED);

        // if the receive ring is not full, try to recieve a new frame
        if (!ring_is_full(&rx_ring)) {
            packet_t* pkt = ring_next(&rx_ring); // get the next free packet
            int recv_len = enc28j60_packet_receive(sizeof(pkt->data), pkt->data);
            if (recv_len > 0) {
                LOG_DEBUG("Received ethernet frame of size %d", recv_len);
                if (usbnet_send(pkt->data, recv_len)) {
                    pkt->length = recv_len;
                    ring_append(&rx_ring);
                } else {
                    LOG_ERROR("*** Failed to send frame over USB! Probably rings desynchronization!");
                }
            }
        } else {
            //LOG_WARNING("** Rx Ring is full - cannot store received ethernet frame!");
        }
        
        // while there are free buffers in tx queue, post them to USB engine
        while (!ring_is_full(&tx_ring)) {
            packet_t* pkt = ring_next(&tx_ring);
            pkt->length = sizeof(pkt->data);
            if (usbnet_recv(pkt->data, pkt->length)) {
                ring_append(&tx_ring);
            } else {
                LOG_ERROR("*** Posting tx buffer to USB failed! Probably rings desynchronization!");
            }
        }
        
        // free rx buffers that were successfully delivered to host
        while (usbnet_pop_completed_send()) {
            LOG_DEBUG("Sending full frame to USB done");
            ring_pop(&rx_ring);
        }
        
        int recv_len;
        // go over received buffers from host, and send them over Ethernet
        while ((recv_len = usbnet_pop_completed_recv())) {
            packet_t* pkt = ring_top(&tx_ring);
            pkt->length = recv_len;
            enc28j60_packet_send(pkt->length, pkt->data);
            LOG_DEBUG("Sent ethernet frame of size %d", pkt->length);
            ring_pop(&tx_ring);
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
