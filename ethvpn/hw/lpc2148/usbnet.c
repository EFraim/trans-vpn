#include <string.h>

#include "log.h"
#include "drivers/vic.h"
#include "drivers/usb.h"
#include "usbnet.h"


static void usb_SOF_handler(uint16_t frame_number) {}
static bool usb_control_standard_custom_handler() { return FALSE; }
static bool usb_control_vendor_handler() { return FALSE; }
static bool usb_control_reserved_handler() { return FALSE; }

#define MAX_USB_PACKET_SIZE (64)
#define ETHERNET_FRAME_SIZE_MAX               1536


#define CDC_ECM_BULK_EP_IN  5
#define CDC_ECM_BULK_EP_OUT 5


#define CS_INTERFACE            0x24
static const uint8_t usb_descriptors[] = {
  0x12,                /* length                      */
  DESC_DEVICE,
  LE_WORD(0x0110),     /* USB version (BCD)           */
  0x02,                /* Class = CDC (Communication) */
  0x00,                /* Device Sub Class            */
  0x00,                /* Device Protocol             */
  MAX_PACKET_SIZE0,    /* max packet size for EP 0    */
  LE_WORD(0x16C0),     /* Vendor: Wouter van Ooijen   */
  LE_WORD(  1002),     /* Product: lab-use only #2    */
  LE_WORD(0x0101),     /* Device release number (BCD) */
  1,                   /* Manufacturer String Index   */
  2,                   /* Product String Index        */
  3,                   /* SerialNumber String Index   */
  1,                   /* Number of Configurations    */

  // CDC Ethernet Networking Control Model" (CDC ECM)

  0x09,
  DESC_CONFIGURATION,
  LE_WORD(73),         /* total length                */
  0x02,                /* number of interfaces        */
  0x01,                /* This Configuration          */
  0,                   /* Configuration String Index  */
  USB_ATTR_BUS_POWERED,/* Attributes                  */
  50,                  /* Max Power (unit is 2mA)     */

  0x09,
  DESC_INTERFACE,     /* This is the Data Interface   */
  0,                  /* Interface Number             */
  0,                  /* Alternate Setting            */
  0,                  /* NumEndPoints                 */
  0x02,               /* Interface class CDC          */
  0x06,               /* Interface Subclass Ethernet  */
  0x00,               /* Interface Protocol (none)    */
  0x00,               /* Interface String Index       */

  0x05,
  CS_INTERFACE,
  0x00,                /* Subtype = Header Descriptor */
  LE_WORD(0x0110),     /* CDC Release Number          */

  0x05,
  CS_INTERFACE,
  0x06,                /* Subtype = Union             */
  0,                   /* Master (control) interface  */
  1,                   /* Slave (data) interface      */

  13,
  CS_INTERFACE,
  0x0f,                /* Subtype = Ethernet          */
  4,                   /* MAC address string index    */
  0,0,0,0,              /* Statistics (none)           */
  LE_WORD(1442),       /* Max segment size            */
  LE_WORD(0),          /* Number of multicast filters */
  0,                   /* number of power filters     */

  /* Default data interface has no endpoints (the alternate is used) */
  0x09,
  DESC_INTERFACE,     /* This is the Data Interface   */
  1,                  /* Interface Number             */
  0,                  /* Alternate Setting            */
  0,                  /* NumEndPoints                 */
  0x0a,               /* CDC data                     */
  0x00,               /* Interface Subclass           */
  0x00,               /* Interface Protocol           */
  0x00,               /* Interface String Index       */

  0x09,
  DESC_INTERFACE,     /* This is the Data Interface   */
  1,                  /* Interface Number             */
  1,                  /* Alternate Setting            */
  2,                  /* NumEndPoints                 */
  0x0a,               /* CDC data                     */
  0x00,               /* Interface Subclass           */
  0x00,               /* Interface Protocol           */
  0x00,               /* Interface String Index       */

  0x07,
  DESC_ENDPOINT,
  USB_EP_OUT | CDC_ECM_BULK_EP_OUT, /* Endpoint Address           */
  0x02,               /* Attributes = bulk            */
  LE_WORD(MAX_USB_PACKET_SIZE),
  0x00,               /* Interval (not used for bulk) */

  0x07,
  DESC_ENDPOINT,
  USB_EP_IN | CDC_ECM_BULK_EP_IN,/* Endpoint Address             */
  0x02,               /* Attributes = bulk            */
  LE_WORD(MAX_USB_PACKET_SIZE),
  0x00,               /* Interval (not used for bulk) */

  0x04,
  DESC_STRING,
  LE_WORD(0x0409),    /* Language = US English        */

  0x0E,
  DESC_STRING,
  'L', 0, 'P', 0, 'C', 0, 'U', 0, 'S', 0, 'B', 0,

  0x0C,
  DESC_STRING,
  'R', 0, 'N', 0, 'D', 0, 'I', 0, 'S', 0,

  0x12,
  DESC_STRING,
  'D', 0, 'E', 0, 'A', 0, 'D', 0, 'B', 0, 'E', 0, 'E', 0, 'F', 0,

  // from an old meraki: "\x00\x18\x0A\x01\xDF\x44"
  2+24,
  DESC_STRING,
  '0', 0, '0', 0, '1', 0, '8', 0, '0', 0, 'A', 0, '0', 0, '1', 0,
  'D', 0, 'F', 0, '4', 0, '4', 0,

  0 /* terminator */
};

static void usb_cdc_ecm_rx(uint8_t ep, uint8_t stat);
static void usb_cdc_ecm_tx(uint8_t ep, uint8_t stat);

static const usb_ep_handler_t usb_ep_handlers[32] = {
   usbEP0OutHandler, usbEP0InHandler, /* EP  0 Out, In */
   0, 0,         /* EP  1 Out, In */
   0, 0,      /* EP  2 Out, In */
   0, 0,
   0, 0,
   &usb_cdc_ecm_rx, &usb_cdc_ecm_tx,  /* EP  5 Out, In */
   0, 0,
   0, 0
};

static volatile uint8_t eth_nak_interrupts = 0;


static bool usb_control_class_handler(void) { return TRUE; }

#include "usbring.h"

/*
 * Send and Recieve rings
 */ 

static volatile usb_ring_t send_ring;
static volatile usb_ring_t recv_ring;
static volatile int recv_ring_drop;

/*
 * API implementation
 */

static void usbnet_init() {
    usbring_init(&send_ring);
    usbring_init(&recv_ring);
    recv_ring_drop = 0;
}

int usbnet_send(uint8_t* buffer, uint16_t length) {
    vicDisable(INT_CHANNEL_USB);
    int success = usbring_post_buffer(&send_ring, buffer, length);
    vicEnable(INT_CHANNEL_USB);
    
    if (success) {
        eth_nak_interrupts |= INACK_BI;
        usbEnableNAKInterrupts(eth_nak_interrupts);
    }

    return success;
}

int usbnet_recv(uint8_t* buffer, uint16_t length) {
    vicDisable(INT_CHANNEL_USB);
    int success = usbring_post_buffer(&recv_ring, buffer, length);
    vicEnable(INT_CHANNEL_USB);
    
    return success;
}

int usbnet_pop_completed_send() {
    vicDisable(INT_CHANNEL_USB);
    int len = usbring_pop_freed(&send_ring);
    vicEnable(INT_CHANNEL_USB);
    
    return len;
}

int usbnet_pop_completed_recv() {
    vicDisable(INT_CHANNEL_USB);
    int len = usbring_pop_freed(&recv_ring);
    vicEnable(INT_CHANNEL_USB);
    
    return len;
}

/*******************************************************/
/* RNDIS Bus Reset                                     */
/*******************************************************/

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

static void usb_cdc_ecm_rx(uint8_t ep, uint8_t stat) {
    int recv_len;
    
    // if no buffers available or we're already dropping the frame,
    // drop this one too
    if (recv_ring.size == 0 || recv_ring_drop) {
        LOG_DEBUG("Got usb packet - dropping");
        recv_len = usbRead(ep, NULL, MAX_USB_PACKET_SIZE);
        recv_ring_drop = 1;
    } else {
        LOG_DEBUG("Got usb packet - recieving");
        // recieve data for the current frame. Here we assume that
        // frames being recieved do not exceed length of supplied buffer
        // (otherwise we will enter into invalid state...)
	//TODO: This is security vurnelability for potentially sensitive product...
        volatile usb_buffer_t* buffer = &recv_ring.buffers[recv_ring.begin];
        int len = MIN(buffer->length - buffer->current, MAX_USB_PACKET_SIZE);
        recv_len = usbRead(ep, buffer->data + buffer->current, len);
        buffer->current += recv_len;
    }
    
    // if short packet recieved - full frame is received
    if (recv_len < MAX_USB_PACKET_SIZE) {
        if (!recv_ring_drop) {
            usbring_free_buffer(&recv_ring);
        }
        recv_ring_drop = 0;
    }
    
}

static void usb_cdc_ecm_tx(uint8_t ep, uint8_t stat) {
    int len;
    
    // Not a NAK interrupt - irrelevant
    //if (!stat & EP_STATUS_NACKED) {
    //    return;
    //}
    
    // if nothing to send anymore - disable NAK interrupts
    if (send_ring.size == 0) {
        eth_nak_interrupts &= ~INACK_BI;
        usbEnableNAKInterrupts(eth_nak_interrupts);
        return;
    }
    
    if (stat & EP_STATUS_DATA) {
        return;
    }
    
    // get first buffer from the ring and send it starting from current point
    volatile usb_buffer_t* buffer = &send_ring.buffers[send_ring.begin];
    // calculate how much bytes are left and limit it with maximal USB packet size
    len = MIN(buffer->length - buffer->current, MAX_USB_PACKET_SIZE);
    // send the bytes and update current position inside the buffer
    usbWrite(ep, buffer->data + buffer->current, len);
    buffer->current += len;
    
    // in case we finished sending the current buffer,
    // check whether the sent USB packet was shorter than maximal - that's how
    // we notify the host about end of frame.
    // If so, free the buffer from the ring (host is already notified)
    // Otherwise do nothing, so the next time zero length packet will be sent.
    if (buffer->length == buffer->current && len < MAX_USB_PACKET_SIZE) {
        usbring_free_buffer(&send_ring);
    }
}

usb_device_logic_t usbNetDriver = {
  .init = usbnet_init,
  .SOF_handler = usb_SOF_handler,
  .device_status_handler = usb_device_status_handler,
  .ep_handlers = usb_ep_handlers,
  .descriptors = usb_descriptors,
  .control_standard_custom_handler = usb_control_standard_custom_handler,
  .control_class_handler = usb_control_class_handler,
  .control_vendor_handler = usb_control_vendor_handler,
  .control_reserved_handler = usb_control_reserved_handler
};
