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
  //LE_WORD(64),         /* total length                */
  LE_WORD(73),         /* total length                */
  //LE_WORD(80),         /* total length                */
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
  LE_WORD(1514),       /* Max segment size            */
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

static const usb_ep_handler_t usb_ep_handlers[32] = {
   usbEP0OutHandler, usbEP0InHandler, /* EP  0 Out, In */
   0, 0,         /* EP  1 Out, In */
   0, 0,      /* EP  2 Out, In */
   0, 0,
   0, 0,
   0, 0,  /* EP  5 Out, In */ //TODO: write
   0, 0,
   0, 0
};

static volatile uint8_t eth_nak_interrupts = 0;


static bool usb_control_class_handler(void) { return TRUE; }


/*
 * USB ring definition
 */

typedef struct {
    uint8_t* data;
    uint16_t length;
    uint16_t current;
} usb_buffer_t;

#define USB_RING_CAPACITY 4
typedef struct {
    usb_buffer_t buffers[USB_RING_CAPACITY];
    int begin;
    int size;
    int nfreed;
} usb_ring_t;


/*
 * API implementation
 */

static void usbcon_init() {}


/*******************************************************/
/* RNDIS Bus Reset                                     */
/*******************************************************/

static void usb_device_status_handler(uint8_t dev_status) { }


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
