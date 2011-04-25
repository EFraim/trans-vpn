#include <string.h>

#include "endian.h"
#include "assert.h"
#include "debug.h"
#include "log.h"
#include "drivers/vic.h"
#include "drivers/usb.h"
#include "usbcon.h"


static void usb_SOF_handler(uint16_t frame_number) {}
static bool usb_control_standard_custom_handler() { return FALSE; }
static bool usb_control_vendor_handler() { return FALSE; }
static bool usb_control_reserved_handler() { return FALSE; }

#define MAX_USB_PACKET_SIZE (64)
#define ETHERNET_FRAME_SIZE_MAX               1536


#define CON_BULK_EP_IN  5
#define CON_BULK_EP_OUT 5


#define CS_INTERFACE            0x24
static const uint8_t usb_descriptors[] = {
  0x12,                /* length                      */
  DESC_DEVICE,
  LE_WORD(0x0110),     /* USB version (BCD)           */
  0x00,                /* Defined at Interface level  */
  0x00,                /* Device Sub Class            */
  0x00,                /* Device Protocol             */
  MAX_PACKET_SIZE0,    /* max packet size for EP 0    */
  LE_WORD(0x16C0),     /* Vendor: Wouter van Ooijen   */
  LE_WORD(  1003),     /* Product: lab-use only #3    */
  LE_WORD(0x0101),     /* Device release number (BCD) */
  1,                   /* Manufacturer String Index   */
  2,                   /* Product String Index        */
  3,                   /* SerialNumber String Index   */
  1,                   /* Number of Configurations    */

  // CDC Ethernet Networking Control Model" (CDC ECM)

  0x09,
  DESC_CONFIGURATION,
  LE_WORD(32),         /* total length                */
  0x01,                /* number of interfaces        */
  0x01,                /* This Configuration          */
  0,                   /* Configuration String Index  */
  USB_ATTR_BUS_POWERED,/* Attributes                  */
  50,                  /* Max Power (unit is 2mA)     */

  0x09,
  DESC_INTERFACE,     /* This is the Data Interface   */
  0,                  /* Interface Number             */
  0,                  /* Alternate Setting            */
  2,                  /* NumEndPoints                 */
  0xFF,               /* Vendor specific class	      */
  0xFF,               /* Vendor specific subclass     */
  0xFF,               /* Vendor specific protocol     */
  0x02,               /* Interface String Index       */

  0x07,
  DESC_ENDPOINT,
  USB_EP_IN | CON_BULK_EP_IN, /* Endpoint Address           */
  0x02,               /* Attributes = bulk            */
  LE_WORD(MAX_USB_PACKET_SIZE),
  0x00,               /* Interval (not used for bulk) */

  0x07,
  DESC_ENDPOINT,
  USB_EP_OUT | CON_BULK_EP_OUT,/* Endpoint Address             */
  0x02,               /* Attributes = bulk            */
  LE_WORD(MAX_USB_PACKET_SIZE),
  0x00,               /* Interval (not used for bulk) */

  0x04,
  DESC_STRING,
  LE_WORD(0x0409),    /* Language = US English        */

  0x0E,
  DESC_STRING,
  'L', 0, 'P', 0, 'C', 0, 'U', 0, 'S', 0, 'B', 0,

  0x12,
  DESC_STRING,
  'V', 0, 'P', 0, 'N', 0, ' ', 0, 'C', 0, 'O', 0, 'N', 0, 'F', 0,

  0x12,
  DESC_STRING,
  'D', 0, 'E', 0, 'A', 0, 'D', 0, 'B', 0, 'E', 0, 'E', 0, 'F', 0,


  0 /* terminator */
};

static void usbcon_ep_inHandler(uint8_t bEP, uint8_t bEPStat);
static void usbcon_ep_outHandler(uint8_t bEP, uint8_t bEPStat);

static const usb_ep_handler_t usb_ep_handlers[32] = {
   usbEP0OutHandler, usbEP0InHandler, /* EP  0 Out, In */
   0, 0,         /* EP  1 Out, In */
   0, 0,      /* EP  2 Out, In */
   0, 0,
   0, 0,
   usbcon_ep_inHandler, usbcon_ep_outHandler,  /* EP  5 Out, In */ //TODO: write
   0, 0,
   0, 0
};

static volatile uint8_t eth_nak_interrupts = 0;


static bool usb_control_class_handler(void) { return TRUE; }

#include "usbring.h"

/*
 * Send and Recieve rings
 */ 

static usb_ring_t send_ring;
static usb_ring_t recv_ring;
static int recv_ring_drop;


/*
 * API implementation
 */

static void usbcon_init() {
    usbring_init(&send_ring);
    usbring_init(&recv_ring);
    recv_ring_drop = 0;
}

static const char* coalesce(const char* x, const char* y) { return x ? x : y; }

static void usbcon_ep_inHandler(uint8_t ep, uint8_t stat) {
    static char recvBuf[MAX_USB_PACKET_SIZE];
    int recv_len = usbRead(ep, (uint8_t*)recvBuf, MAX_USB_PACKET_SIZE);
    LOG_INFO("In handler %d bytes, %d stat\n", recv_len, stat);
    for(int st=0, en=coalesce(memchr(recvBuf, '\n', recv_len), recvBuf+recv_len-1)-recvBuf; 
	st < recv_len; st = en+1, en=coalesce(memchr(recvBuf+st+1, '\n', recv_len-st-1), recvBuf+recv_len-1)-recvBuf) {
      int len=en-st+1;
      if (recv_ring.size == 0 || recv_ring_drop ||
	  recv_ring.buffers[recv_ring.begin].length - recv_ring.buffers[recv_ring.begin].current < len+1) {
        LOG_DEBUG("Got string - dropping");
	recv_ring_drop = recvBuf[en] != '\n';
        continue;
      }
      usb_buffer_t* buffer = &recv_ring.buffers[recv_ring.begin];
      memcpy(buffer->data+buffer->current, recvBuf+st, len);
      buffer->current += len;
      if(recvBuf[en] == '\n') {
	LOG_INFO("Buffer done\n");
	buffer->data[buffer->current] = 0;
	buffer->current++;	
	usbring_free_buffer(&recv_ring);
      }
    }
}

static void usbcon_ep_outHandler(uint8_t ep, uint8_t stat) {
  LOG_INFO("Out handler\n");
  if (send_ring.size == 0) {
    eth_nak_interrupts &= ~INACK_BI;
    usbEnableNAKInterrupts(eth_nak_interrupts);
    return;
  }
    
  if (stat & EP_STATUS_DATA)
    return;
    
  // get first buffer from the ring and send it starting from current point
  usb_buffer_t* buffer = &send_ring.buffers[send_ring.begin];
  // calculate how much bytes are left and limit it with maximal USB packet size
  int len = MIN(buffer->length - buffer->current, MAX_USB_PACKET_SIZE);
  // send the bytes and update current position inside the buffer
  usbWrite(ep, buffer->data + buffer->current, len);
  buffer->current += len;
  if(buffer->current == buffer->length)  
    usbring_free_buffer(&send_ring);
}

static void usb_device_status_handler(uint8_t dev_status) {
  if (dev_status & DEV_STATUS_RESET) {
    LOG_INFO("USB Bus Reset status=%x\n\n",dev_status);
    
    recv_ring_drop = 0;
    usbring_reset(&recv_ring);
    usbring_reset(&send_ring);
    
    eth_nak_interrupts = 0;
    if (send_ring.size > 0) {
      eth_nak_interrupts |= INACK_BI;
    }
    usbEnableNAKInterrupts(eth_nak_interrupts);
  }
}

static void waitForRing(usb_ring_t *ring) {
  for(;;) {
    for(volatile int i=0; i<10000; i++) //Busyloop
      ;
    vicDisable(INT_CHANNEL_USB);
    int len = usbring_pop_freed(ring);
    vicEnable(INT_CHANNEL_USB);
    if(len > 0)
      return;
  }
}

void usbcon_send_response_await_query(const char* reply, char* cmd) {
  vicDisable(INT_CHANNEL_USB);
  //-1 for NULL termination
  usbring_post_buffer(&recv_ring, (uint8_t*)cmd, MAX_CMD_LEN-1);
  usbring_post_buffer(&send_ring, (uint8_t*)reply, strlen(reply));
  eth_nak_interrupts |= INACK_BI;
  usbEnableNAKInterrupts(eth_nak_interrupts);
  vicEnable(INT_CHANNEL_USB);

  LOG_INFO("Awaiting send completion\n");
  waitForRing(&send_ring);
  LOG_INFO("Awaiting new command\n");
  waitForRing(&recv_ring);
}


usb_device_logic_t usbConDriver = {
  .init = usbcon_init,
  .SOF_handler = usb_SOF_handler,
  .device_status_handler = usb_device_status_handler,
  .ep_handlers = usb_ep_handlers,
  .descriptors = usb_descriptors,
  .control_standard_custom_handler = usb_control_standard_custom_handler,
  .control_class_handler = usb_control_class_handler,
  .control_vendor_handler = usb_control_vendor_handler,
  .control_reserved_handler = usb_control_reserved_handler
};
