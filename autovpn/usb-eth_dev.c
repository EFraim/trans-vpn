
#include <string.h>


#define CDC_RNDIS
#define CDC_ECM


//#define usb_SOF_handler(frame_number) FALSE

static void usb_SOF_handler(int frame_number) {
  //if (frame_number % 512 == 0)
  //  DBG(".");
}

//#define usb_device_status_handler(status) FALSE
//#define usb_control_class_handler() FALSE
#define usb_control_standard_custom_handler() FALSE
#define usb_control_vendor_handler() FALSE
#define usb_control_reserved_handler() FALSE

//#define INT_EP  1
#define MAX_PACKET_SIZE (64)

#define ETH_DEV_DEBUG

#define ETH_DEV_DBG_ERROR(fmt, args...) DBG("USB DEV ERROR:" fmt, ##args)

#ifdef ETH_DEV_DEBUG
#define ETH_DEV_DBG_INFO(fmt, args...) DBG("USB DEV INFO:" fmt, ##args)
#else
#define ETH_DEV_DBG_INFO(fmt, args...)
#endif

#ifdef CDC_ECM_SUBSET
#define DEVICE_TYPE

#define ETHERNET_FRAME_SIZE_MAX (64)

static const uint8_t usb_descriptors[] = {
  0x12,                /* length                      */
  DESC_DEVICE,
  LE_WORD(0x0110),     /* USB version (BCD)           */
  0x02,                /* Class = CDC (Communication) */
  0x00,                /* Device Sub Class            */
  0x00,                /* Device Protocol             */
  MAX_PACKET_SIZE0,    /* max packet size for EP 0    */
  LE_WORD(0x050d),     /* Vendor: Belkin              */
  LE_WORD(0x0004),     /* Product: Belkin             */
  LE_WORD(0x0201),     /* Device release number (BCD) */
  1,                   /* Manufacturer String Index   */
  2,                   /* Product String Index        */
  3,                   /* SerialNumber String Index   */
  1,                   /* Number of Configurations    */

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
  0x00,               /* Interface Class    ??? should be CDC         */
  0x00,               /* Interface Subclass ???           */
  0x00,               /* Interface Protocol ???          */
  0x00,               /* Interface String Index       */

  0x07,
  DESC_ENDPOINT,
  USB_EP_OUT | BULK_EP, /* Endpoint Address           */
  0x02,               /* Attributes = bulk            */
  LE_WORD(MAX_PACKET_SIZE),
  0x00,               /* Interval (not used for bulk) */

  0x07,
  DESC_ENDPOINT,
  USB_EP_IN | BULK_EP,/* Endpoint Address             */
  0x02,               /* Attributes = bulk            */
  LE_WORD(MAX_PACKET_SIZE),
  0x00,               /* Interval (not used for bulk) */

  0x04,
  DESC_STRING,
  LE_WORD(0x0409),    /* Language = US English        */

  0x0E,
  DESC_STRING,
  'L', 0, 'P', 0, 'C', 0, 'U', 0, 'S', 0, 'B', 0,

  0x0E,
  DESC_STRING,
  'U', 0, 'S', 0, 'B', 0, 'e', 0, 't', 0, 'h', 0,

  0x12,
  DESC_STRING,
  'D', 0, 'E', 0, 'A', 0, 'D', 0, 'B', 0, 'E', 0, 'E', 0, 'F', 0,

  0 /* terminator */
};
#endif

//#define ETHERNET_FRAME_SIZE_MAX               1536
#define ETHERNET_FRAME_SIZE_MAX               576
//#define RNDIS_MESSAGE_BUFFER_SIZE             128

#define REMOTE_NDIS_PACKET_MSG                0x00000001UL
#define REMOTE_NDIS_INITIALIZE_MSG            0x00000002UL
#define REMOTE_NDIS_HALT_MSG                  0x00000003UL
#define REMOTE_NDIS_QUERY_MSG                 0x00000004UL
#define REMOTE_NDIS_SET_MSG                   0x00000005UL
#define REMOTE_NDIS_RESET_MSG                 0x00000006UL
#define REMOTE_NDIS_INDICATE_STATUS_MSG       0x00000007UL
#define REMOTE_NDIS_KEEPALIVE_MSG             0x00000008UL

#define REMOTE_NDIS_INITIALIZE_CMPLT          0x80000002UL
#define REMOTE_NDIS_QUERY_CMPLT               0x80000004UL
#define REMOTE_NDIS_SET_CMPLT                 0x80000005UL
#define REMOTE_NDIS_RESET_CMPLT               0x80000006UL
#define REMOTE_NDIS_KEEPALIVE_CMPLT           0x80000008UL

#define REMOTE_NDIS_STATUS_SUCCESS            0x00000000UL
#define REMOTE_NDIS_STATUS_FAILURE            0xC0000001UL
#define REMOTE_NDIS_STATUS_INVALID_DATA       0xC0010015UL
#define REMOTE_NDIS_STATUS_NOT_SUPPORTED      0xC00000BBUL
#define REMOTE_NDIS_STATUS_MEDIA_CONNECT      0x4001000BUL
#define REMOTE_NDIS_STATUS_MEDIA_DISCONNECT   0x4001000CUL

#define REMOTE_NDIS_MEDIA_STATE_CONNECTED     0x00000000UL
#define REMOTE_NDIS_MEDIA_STATE_DISCONNECTED  0x00000001UL

#define REMOTE_NDIS_MEDIUM_802_3              0x00000000UL

#define REMOTE_NDIS_DF_CONNECTIONLESS       0x00000001UL
#define REMOTE_NDIS_DF_CONNECTION_ORIENTED    0x00000002UL

#define REMOTE_NDIS_PACKET_DIRECTED           0x00000001UL
#define REMOTE_NDIS_PACKET_MULTICAST          0x00000002UL
#define REMOTE_NDIS_PACKET_ALL_MULTICAST      0x00000004UL
#define REMOTE_NDIS_PACKET_BROADCAST          0x00000008UL
#define REMOTE_NDIS_PACKET_SOURCE_ROUTING     0x00000010UL
#define REMOTE_NDIS_PACKET_PROMISCUOUS        0x00000020UL
#define REMOTE_NDIS_PACKET_SMT                0x00000040UL
#define REMOTE_NDIS_PACKET_ALL_LOCAL          0x00000080UL
#define REMOTE_NDIS_PACKET_GROUP              0x00001000UL
#define REMOTE_NDIS_PACKET_ALL_FUNCTIONAL     0x00002000UL
#define REMOTE_NDIS_PACKET_FUNCTIONAL         0x00004000UL
#define REMOTE_NDIS_PACKET_MAC_FRAME          0x00008000UL

#define OID_GEN_SUPPORTED_LIST                0x00010101UL
#define OID_GEN_HARDWARE_STATUS               0x00010102UL
#define OID_GEN_MEDIA_SUPPORTED               0x00010103UL
#define OID_GEN_MEDIA_IN_USE                  0x00010104UL
#define OID_GEN_MAXIMUM_FRAME_SIZE            0x00010106UL
#define OID_GEN_MAXIMUM_TOTAL_SIZE            0x00010111UL
#define OID_GEN_LINK_SPEED                    0x00010107UL
#define OID_GEN_TRANSMIT_BLOCK_SIZE           0x0001010AUL
#define OID_GEN_RECEIVE_BLOCK_SIZE            0x0001010BUL
#define OID_GEN_VENDOR_ID                     0x0001010CUL
#define OID_GEN_VENDOR_DESCRIPTION            0x0001010DUL
#define OID_GEN_CURRENT_PACKET_FILTER         0x0001010EUL
#define OID_GEN_MAXIMUM_TOTAL_SIZE            0x00010111UL
#define OID_GEN_MEDIA_CONNECT_STATUS          0x00010114UL
#define OID_GEN_PHYSICAL_MEDIUM               0x00010202UL
#define OID_GEN_XMIT_OK                       0x00020101UL
#define OID_GEN_RCV_OK                        0x00020102UL
#define OID_GEN_XMIT_ERROR                    0x00020103UL
#define OID_GEN_RCV_ERROR                     0x00020104UL
#define OID_GEN_RCV_NO_BUFFER                 0x00020105UL
#define OID_802_3_PERMANENT_ADDRESS           0x01010101UL
#define OID_802_3_CURRENT_ADDRESS             0x01010102UL
#define OID_802_3_MULTICAST_LIST              0x01010103UL
#define OID_802_3_MAXIMUM_LIST_SIZE           0x01010104UL
#define OID_802_3_MAC_OPTIONS                 0x01010105UL

#define OID_802_3_RCV_ERROR_ALIGNMENT         0x01020101UL
#define OID_802_3_XMIT_ONE_COLLISION          0x01020102UL
#define OID_802_3_XMIT_MORE_COLLISIONS        0x01020103UL


/* Sivan: the code did not work when the notification endpoint
 * was endpoint 3, not 1. The host (Windows XP) still sent an in
 * packet to EP 1. Perhaps this is a requirement of CDC, or just
 * a quirck in RNDIS.
 */

#define RNDIS_NOTIFICATION_EP 1
// sivan testing removal of bidi endpoints
#define RNDIS_BULK_EP_IN   2
#define RNDIS_BULK_EP_OUT  2
//#define RNDIS_BULK_EP_OUT 5

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
#if defined(CDC_RNDIS) && defined(CDC_ECM)
  2,                   /* Number of Configurations    */
#else
  1,                   /* Number of Configurations    */
#endif

#if defined(CDC_RNDIS)
#define DEVICE_TYPE
  0x09,
  DESC_CONFIGURATION,
//  LE_WORD(67),         /* total length                */
  LE_WORD(48),         /* total length                */
  0x02,                /* number of interfaces        */
  0x01,                /* This Configuration          */
  0,                   /* Configuration String Index  */
  USB_ATTR_BUS_POWERED,/* Attributes                  */
  50,                  /* Max Power (unit is 2mA)     */

  0x09,
  DESC_INTERFACE,     /* This is the Data Interface   */
  0,                  /* Interface Number             */
  0,                  /* Alternate Setting            */
  1,                  /* NumEndPoints                 */
  0x02,               /* Interface Class = CDC        */
  0x02,               /* Interface Subclass = ACM     */
  0xFF,               /* Interface Protocol = Vendor Specific (RNDIS) */
  0x00,               /* Interface String Index       */

#if 0
  0x05,
  CS_INTERFACE,
  0x00,                /* Subtype = Header Descriptor */
  LE_WORD(0x0110),     /* CDC Release Number          */

  0x05,
  CS_INTERFACE,
  0x01,                /* Subtype = Call Management   */
  0x00,                /* Capabilities = device
                              handles call management (does not) */
  1,                   /* bDataInterface ??             */

  0x04,
  CS_INTERFACE,
  0x02,                /* Subtype = Abs. Ctl. Manage. */
  0x00,

  0x05,
  CS_INTERFACE,
  0x02,                /* Subtype = ACM               */
  0x00,
  0x01,

#endif

  //
  0x07,                /* length                      */
  DESC_ENDPOINT,
  USB_EP_IN | RNDIS_NOTIFICATION_EP,  /* Endpoint Address            */
  USB_EP_ATTR_INTERRUPT, /* Attributes                */
  LE_WORD(8),          /* Max Packet Size             */
  2,                   /* Polling Interval (in ms)    */

  0x09,
  DESC_INTERFACE,     /* This is the Data Interface   */
  1,                  /* Interface Number             */
  0,                  /* Alternate Setting            */
  2,                  /* NumEndPoints                 */
  0x0A,               /* Interface Class = Data       */
  0x00,               /* Interface Subclass           */
  0x00,               /* Interface Protocol           */
  0x00,               /* Interface String Index       */

  0x07,
  DESC_ENDPOINT,
  USB_EP_IN | RNDIS_BULK_EP_IN,/* Endpoint Address       */
  0x02,               /* Attributes = bulk            */
  LE_WORD(MAX_PACKET_SIZE),
  0x00,               /* Interval (not used for bulk) */

  0x07,
  DESC_ENDPOINT,
  USB_EP_OUT | RNDIS_BULK_EP_OUT, /* Endpoint Address           */
  0x02,               /* Attributes = bulk            */
  LE_WORD(MAX_PACKET_SIZE),
  0x00,               /* Interval (not used for bulk) */
#endif /* RNDIS */

  // CDC Ethernet Networking Control Model" (CDC ECM)
#ifdef CDC_ECM
#define DEVICE_TYPE

  0x09,
  DESC_CONFIGURATION,
  //LE_WORD(64),         /* total length                */
  LE_WORD(73),         /* total length                */
  //LE_WORD(80),         /* total length                */
  0x02,                /* number of interfaces        */
#ifdef CDC_RNDIS
  0x02,                /* This Configuration          */
#else
  0x01,                /* This Configuration          */
#endif
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
  LE_WORD(1200),       /* Max segment size            */
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
  LE_WORD(MAX_PACKET_SIZE),
  0x00,               /* Interval (not used for bulk) */

  0x07,
  DESC_ENDPOINT,
  USB_EP_IN | CDC_ECM_BULK_EP_IN,/* Endpoint Address             */
  0x02,               /* Attributes = bulk            */
  LE_WORD(MAX_PACKET_SIZE),
  0x00,               /* Interval (not used for bulk) */

#endif

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

#ifndef DEVICE_TYPE
#error "this code compiles as either CDC_ECM or CDC_RNDIS"
#endif
void usb_eth_rx(uint8_t ep, uint8_t stat);
void usb_eth_tx(uint8_t ep, uint8_t stat);
void usb_eth_handle_recv(uint8_t *buffer, uint16_t size);

void usb_cdc_ecm_rx(uint8_t ep, uint8_t stat);
void usb_cdc_ecm_tx(uint8_t ep, uint8_t stat);

void usb_rndis_rx(uint8_t ep, uint8_t stat);
void usb_rndis_tx(uint8_t ep, uint8_t stat);
void usb_rndis_notification(uint8_t ep, uint8_t stat);

#ifdef CDC_ECM_SUBSET
static usb_ep_handler_t* const usb_ep_handlers[32] = {
   usbEP0OutHandler, usbEP0InHandler, /* EP  0 Out, In */
   0, 0,                              /* EP  1 Out, In */
   &usb_eth_rx, &usb_eth_tx,          /* EP  2 Out, In */
   0,&usb_rndis_notification,         /* EP  3 Out, In */
   &usb_rndis_rx, &usb_rndis_tx,      /* EP  4 Out, In */
   0,
};
#endif

#if defined(CDC_RNDIS) || defined(CDC_ECM)
static usb_ep_handler_t* const usb_ep_handlers[32] = {
   usbEP0OutHandler, usbEP0InHandler, /* EP  0 Out, In */
   0,&usb_rndis_notification,         /* EP  1 Out, In */
   &usb_rndis_rx, &usb_rndis_tx,      /* EP  2 Out, In */
   0, 0,
   0, 0,
   &usb_cdc_ecm_rx, &usb_cdc_ecm_tx,  /* EP  5 Out, In */
   0, 0,
   0, 0
};
#endif

static volatile uint8_t eth_nak_interrupts = 0;

/*** Old CDC ECM Code ***/

/*******************************************************/
/* Buffers for packet                                  */
/*******************************************************/

//#define PACKET_SIZE (512)
#define RECEIVE_RING_SIZE (2)
typedef struct packet_s {
  uint16_t transferred;
  //uint16_t size;
  uint16_t size;
  uint8_t data[ETHERNET_FRAME_SIZE_MAX];
} packet_t;

static void packet_init(packet_t* packet)
{
  int i;
  packet->size = 0;
  packet->transferred = 0;
  //for (i=0 ;i<ETHERNET_FRAME_SIZE_MAX; i++ ) {
  //  packet->data[i] = 0;
  //}
}

#define RING_IS_FULL(head, tail, size) ((((tail) + 1) % (size)) == head)
#define RING_IS_EMPTY(head, tail, size) ((tail) == (head))
#define RING_INC(index, size)                   \
do {                                            \
  index = (index + 1) % size;                   \
}while (0);

static packet_t receive_ring[RECEIVE_RING_SIZE];
volatile static packet_t *receive_packet;
volatile static uint16_t receive_ring_head;
volatile static uint16_t receive_ring_tail;

volatile static uint16_t bytes_to_send;
volatile static uint16_t bytes_sent;
static uint8_t send_buffer[ETHERNET_FRAME_SIZE_MAX];
volatile static uint8_t packet_pending;

/*******************************************************/
/* CDC ECM Transmit and Receive                        */
/*******************************************************/

void usb_eth_rx_subset(uint8_t ep, uint8_t stat) {
  int len;
  int ip_length;
  uint16_t eth_hdr_type;

  if (receive_packet == 0) {
    if (RING_IS_FULL(receive_ring_head, receive_ring_tail, RECEIVE_RING_SIZE)) {
      ETH_DEV_DBG_ERROR("usb_eth_rx: receive ring is full, head=%d, tail=%d",
                        receive_ring_head, receive_ring_tail);
      return;
    }
    receive_packet = &receive_ring[receive_ring_tail];
    packet_init(receive_packet);
  }

  if (receive_packet->transferred < ETHERNET_FRAME_SIZE_MAX) {
    len = usbRead(ep,
                  &receive_packet->data[0] + receive_packet->transferred,
                  ETHERNET_FRAME_SIZE_MAX - receive_packet->transferred);
  } else {
    /* discard the bytes */
    len = usbRead(ep, 0, 0);
  }
  receive_packet->transferred += len;

  ETH_DEV_DBG_INFO("usb_eth_rx: read %d bytes, head=%d, tail=%d",
                   len,
                   receive_ring_head,
                   receive_ring_tail);

  if (receive_packet->transferred < UIP_LLH_LEN) {
    /* ethernet header was not received yet got some garbage which we need
     to ignore */
    ETH_DEV_DBG_ERROR("got part of packet with %d bytes", receive_packet->transferred);
    receive_packet->transferred = 0;
    return;
  }

  if (receive_packet->size == 0) {
    eth_hdr_type = (receive_packet->data[0xc] << 8) | receive_packet->data[0xd];

    if (eth_hdr_type == UIP_ETHTYPE_IP) {
      ip_length = receive_packet->data[0x10]*0xff + receive_packet->data[0x11];
      receive_packet->size = ip_length + UIP_LLH_LEN;
    }
    else if (eth_hdr_type == UIP_ETHTYPE_ARP) {
      receive_packet->size = receive_packet->transferred;
    }
    else {
      ETH_DEV_DBG_ERROR("Unknown ethernet frame 0x%x", eth_hdr_type);
      receive_packet->size = receive_packet->transferred;
    }
  }

  if (receive_packet->transferred >= receive_packet->size) {
    if (receive_packet->transferred >= receive_packet->size) {
      if (receive_packet->transferred >= ETHERNET_FRAME_SIZE_MAX) {
        ETH_DEV_DBG_ERROR("Discarding packet of size %d", receive_packet->transferred);
        packet_init(receive_packet);
        receive_packet = 0;
      }
      else {
        ETH_DEV_DBG_INFO("Received packet of size %d", receive_packet->transferred);
        receive_packet = 0;
        RING_INC(receive_ring_tail, RECEIVE_RING_SIZE);
      }
    }
  }
}

void usb_eth_tx_subset(uint8_t ep, uint8_t stat)
{
  int len;
  int i;

  if (!packet_pending || bytes_to_send == 0) {
    ETH_DEV_DBG_INFO("usb_eth_tx, send buffer is empty, packet_pending=%d", packet_pending);
    usbEnableNAKInterrupts(0);
    return;
  }

  len = MIN(bytes_to_send - bytes_sent, MAX_PACKET_SIZE);

  usbWrite(ep, ((uint8_t*) &send_buffer)+bytes_sent, len);
  bytes_sent += len;

  ETH_DEV_DBG_INFO("usb_eth_tx: send %d bytes, frame data sent %d/%d",
                   len, bytes_sent, bytes_to_send);


  if (bytes_sent >= bytes_to_send) {
    /* finished sending data */
    for (i=0; i< bytes_to_send;i++) {
      send_buffer[i] = 0;
    }
    bytes_to_send = 0;
    bytes_sent = 0;
    packet_pending = 0;
  }
}

/*******************************************************/
/* UIP interface                                       */
/*******************************************************/

uint16_t xxx_network_device_read() {
  uint16_t ret = 0;
  int i;
  packet_t *packet;

  if (!RING_IS_EMPTY(receive_ring_head, receive_ring_tail, RECEIVE_RING_SIZE)) {
    /* we got ip packet, copy it and notify uip framework */
    //ETH_DEV_DBG_INFO("Got a packet!");
    DBG("Got a packet!");
    packet = &receive_ring[receive_ring_head];
    for (i=0; i<packet->transferred; i++) {
      uip_buf[i] = packet->data[i];
      packet->data[i] = 0;
    }
    ret = packet->transferred;
    packet->transferred = 0;
    RING_INC(receive_ring_head, RECEIVE_RING_SIZE);
  }
  return ret;
}

uint16_t network_device_read() {
  uint16_t ret = 0;
  int i;
  packet_t *packet = 0;
  //if (xxx) DBG("device read");

  vicDisable(INT_CHANNEL_USB);
  if (!RING_IS_EMPTY(receive_ring_head, receive_ring_tail, RECEIVE_RING_SIZE)) {
    packet = &receive_ring[receive_ring_head];
    if (packet->size == 0) packet = 0;
  }
  vicEnable(INT_CHANNEL_USB);

  if (packet) {
    /* we got ip packet, copy it and notify uip framework */
    //ETH_DEV_DBG_INFO("Got a packet!");
    DBG("Got a packet! (%d bytes)",packet->size);
    packet = &receive_ring[receive_ring_head];
    for (i=0; i<packet->size; i++) {
      uip_buf[i] = packet->data[i];
      //packet->data[i] = 0;
    }
    ret = packet->size;
    packet->size = 0;

    vicDisable(INT_CHANNEL_USB);
    RING_INC(receive_ring_head, RECEIVE_RING_SIZE);
    vicEnable(INT_CHANNEL_USB);
  }
  return ret;
}

void network_device_send()
{
  int i;
  uint16_t len;

  DBG("network device send! (%d bytes)",uip_len);

  if (uip_len == 0) {
    ETH_DEV_DBG_ERROR("network_device_send: uip_len == 0");
    return;
  }

  vicDisable(INT_CHANNEL_USB);
  uint8_t packet_pending_copy = packet_pending;
  vicEnable(INT_CHANNEL_USB);

  if (packet_pending_copy) {
    ETH_DEV_DBG_ERROR("network_device_send: already pending packet, discarding request");
    return;
  }

  len = MIN(sizeof(send_buffer), uip_len);

  for (i=0; i<len; i++) {
    send_buffer[i] = uip_buf[i];
  }

  vicDisable(INT_CHANNEL_USB);

  bytes_sent=0;
  bytes_to_send = len;
  packet_pending = 1;

  eth_nak_interrupts |= INACK_BI;
  usbEnableNAKInterrupts(eth_nak_interrupts);

  vicEnable(INT_CHANNEL_USB);

  //ETH_DEV_DBG_ERROR("network_device_send: NACK Interrupts enable on bulk in");

  //DBG("leaving network send");
}

void network_device_init()
{
  int i,j;

  for (i=0; i<sizeof(send_buffer); i++) {
    send_buffer[i]=0;
  }
  bytes_to_send=0;
  bytes_sent = 0;
  packet_pending = 0;

  receive_ring_head = 0;
  receive_ring_tail = 0;
  receive_packet = 0;
  for (i=0; i<RECEIVE_RING_SIZE; i++) {
    for (j=0; j<ETHERNET_FRAME_SIZE_MAX; j++) {
      receive_ring[i].data[j] = 0;
    }
    receive_ring[i].transferred = 0;
  }
}

/*** Sivan RNDIS CODE ***/

static enum {
    RNDIS_UNINITIALIZED,
    RNDIS_INITIALIZED,
    RNDIS_DATA_INITIALIZED
} rndisState = RNDIS_UNINITIALIZED;

uint8_t rndisResponse[256]; /* how large should it be? */
uint8_t rndisResponseReady = 0;
uint8_t rndisResponseLen = 0;

uint8_t rndisResponseAvailable[8] = { 1, 0 , 0 , 0, 0, 0, 0, 0}; /* 0x00000001, 0, in little endian */
uint8_t rndisResponseNotAvailable = 0;

uint32_t rndisCurrentPacketFilter = 0;
uint32_t rndisTxPackets = 0;
uint32_t rndisRxPackets = 0;

#define RNDIS_STATUS_SUCCESS 0
#define RNDIS_STATUS_FAILURE 1
#define RNDIS_STATUS_NOT_SUPPORTED 0xC00100BB
#define RNDIS_STATUS_INVALID_DATA 0xC0010015
#define RNDIS_STATUS_MEDIA_CONNECT 0x4001000B
#define RNDIS_STATUS_MEDIA_DISCONNECT 0x4001000C

static const uint32_t rndisSupportedOIDs[] = {
  OID_GEN_SUPPORTED_LIST,
  OID_GEN_PHYSICAL_MEDIUM,
  OID_GEN_HARDWARE_STATUS,
  OID_GEN_MEDIA_SUPPORTED,
  OID_GEN_MEDIA_IN_USE,
  OID_GEN_MAXIMUM_FRAME_SIZE,
  OID_GEN_MAXIMUM_TOTAL_SIZE,
  OID_GEN_LINK_SPEED,
  OID_GEN_TRANSMIT_BLOCK_SIZE,
  OID_GEN_RECEIVE_BLOCK_SIZE,
  OID_GEN_VENDOR_ID,
  OID_GEN_VENDOR_DESCRIPTION,
  OID_GEN_CURRENT_PACKET_FILTER,
  OID_GEN_MAXIMUM_TOTAL_SIZE,
  OID_GEN_MEDIA_CONNECT_STATUS,
  OID_GEN_XMIT_OK,
  OID_GEN_RCV_OK,
  OID_GEN_XMIT_ERROR,
  OID_GEN_RCV_ERROR,
  OID_GEN_RCV_NO_BUFFER,
  OID_802_3_PERMANENT_ADDRESS,
  OID_802_3_CURRENT_ADDRESS,
  OID_802_3_MULTICAST_LIST,
  OID_802_3_MAXIMUM_LIST_SIZE,
  OID_802_3_MAC_OPTIONS,
  OID_802_3_RCV_ERROR_ALIGNMENT,
  OID_802_3_XMIT_ONE_COLLISION,
  OID_802_3_XMIT_MORE_COLLISIONS,
};

/*******************************************************/
/* RNDIS Control Class Handler                         */
/*******************************************************/

static void rndisReset(void);

static bool usb_control_class_handler(void) {
  uint32_t oid;
  uint32_t buflen;
  uint32_t bufptr;

  //printf("control_class_handler!!!");
  ETH_DEV_DBG_INFO("class control request=%d interface=%u len=%u dir=%d\n",usbRequest.request,usbRequest.index,usbRequest.length,REQTYPE_GET_DIR(usbRequest.type));
  DBG("class control request=%d interface=%u len=%u dir=%d\n",usbRequest.request,usbRequest.index,usbRequest.length,REQTYPE_GET_DIR(usbRequest.type));

  if (REQTYPE_GET_DIR(usbRequest.type) == REQTYPE_DIR_TO_HOST) {
    if (rndisResponseReady) {
      //ETH_DEV_DBG_INFO("sending data back to the host, %d bytes\n",rndisResponseLen);
      usbControlTransferPtr = rndisResponse;
      usbControlTransferLen = rndisResponseLen;
      rndisResponseReady = 0;
    } else {
      //ETH_DEV_DBG_INFO("sending NO RESPONE AVAILABLE to the host");
      usbControlTransferPtr = &rndisResponseNotAvailable;
      usbControlTransferLen = 1;
    }
    return TRUE;
  }

  uint32_t type = uint32FromLittleEndian(usbControlTransferPtr);
  uint32_t len  = uint32FromLittleEndian(usbControlTransferPtr+4);
  uint32_t id   = uint32FromLittleEndian(usbControlTransferPtr+8);

  //ETH_DEV_DBG_INFO("RNDIS request type = %x len=%u id=%u\n",type,len,id);
  if (type > 10 || len > 200) {
    ETH_DEV_DBG_INFO("Woopa!!!\n");
    oid    = uint32FromLittleEndian(usbControlTransferPtr+12);
    buflen = uint32FromLittleEndian(usbControlTransferPtr+16);
    bufptr = uint32FromLittleEndian(usbControlTransferPtr+20);
    if (type==4) ETH_DEV_DBG_INFO("QUERY OID=%x infolen=%d infooffset=%d",oid,buflen,bufptr);
    int iii;
    for (iii=0; iii<usbRequest.length; iii++)
      ETH_DEV_DBG_INFO("%02x",usbControlTransferPtr[iii]);
    while (1);
  }

  switch (type) {
  case 0x02: /* REMOTE_NDIS_INITIALIZE_MSG */
    ETH_DEV_DBG_INFO("  INITIALIZE_MSG version=%u.%u max device->host transfer size = %u",
        uint32FromLittleEndian(usbControlTransferPtr+12),
        uint32FromLittleEndian(usbControlTransferPtr+16),
        uint32FromLittleEndian(usbControlTransferPtr+20));


    if (rndisState != RNDIS_UNINITIALIZED) {
      ETH_DEV_DBG_ERROR("INITIALIZE message when we are not in the uninitialized state (state=%d)",rndisState);
      break;
    }
    //int i;
    //for (i=0; i<len; i++) {
    //  ETH_DEV_DBG_INFO("%d %02x",i,usbControlTransferPtr[i]);
    //}

    uint32ToLittleEndian(0x80000002,rndisResponse); /* INITIALIZE_CMPLT */
    uint32ToLittleEndian(52,rndisResponse+4);         /* length */
    uint32ToLittleEndian(id,rndisResponse+8);         /* request id to which we are responding */
    uint32ToLittleEndian(RNDIS_STATUS_SUCCESS,rndisResponse+12); /* status */
    uint32ToLittleEndian(1,rndisResponse+16);          /* version, major */
    uint32ToLittleEndian(0,rndisResponse+20);          /* version, minor */
    uint32ToLittleEndian(1,rndisResponse+24);          /* flags = connectionless */
    uint32ToLittleEndian(0,rndisResponse+28);          /* medium = 802.3 */
    uint32ToLittleEndian(1,rndisResponse+32);          /* max packets per transfer */
    /* the 22 is some voodoo from Linux drivers/gadget */
    uint32ToLittleEndian((11*4+ETHERNET_FRAME_SIZE_MAX+22),rndisResponse+36);  /* max transfer size; rndis header + ethernet packet */
    uint32ToLittleEndian(0,rndisResponse+40);          /* packet alignment for multi-packet transfers */
    uint32ToLittleEndian(0,rndisResponse+44);          /* reserved */
    uint32ToLittleEndian(0,rndisResponse+48);          /* reserved */
    rndisResponseLen   = 52;

    //usbControlTransferPtr = rndisResponse;
    //usbControlTransferLen = 24;

    rndisResponseReady = 1;
    eth_nak_interrupts |= INACK_II;
    usbEnableNAKInterrupts(eth_nak_interrupts);
    //usbEnableNAKInterrupts(INACK_II);

    rndisState = RNDIS_INITIALIZED; /* we should really do this only after sending the response */

    break;
  case 0x03: /* HALT */
    ETH_DEV_DBG_INFO("HALT!?!");

    rndisState = RNDIS_UNINITIALIZED; /* we should really do this only after sending the response */
    break;
  case 0x04: /* REMOTE_NDIS_QUERY_MSG */
    oid    = uint32FromLittleEndian(usbControlTransferPtr+12);
    buflen = uint32FromLittleEndian(usbControlTransferPtr+16);
    bufptr = uint32FromLittleEndian(usbControlTransferPtr+20);

    ETH_DEV_DBG_INFO("QUERY OID=%x infolen=%d infooffset=%d",oid,buflen,bufptr);
    if (buflen > 0) {
      //ETH_DEV_DBG_INFO("parameters???");
      //int iii;
      //for (iii=0; iii<usbRequest.length; iii++)
      //  ETH_DEV_DBG_INFO("%02x",usbControlTransferPtr[iii]);
      //while(1);
    }

    if (buflen+28 != len) {
      ETH_DEV_DBG_INFO("wrong size?");
      int iii;
      for (iii=0; iii<usbRequest.length; iii++)
        ETH_DEV_DBG_INFO("%02x",usbControlTransferPtr[iii]);
      while(1);
    }

    if (buflen>0 && bufptr != 28-8) {
      ETH_DEV_DBG_INFO("wrong offset?");
      int iii;
      for (iii=0; iii<usbRequest.length; iii++)
        ETH_DEV_DBG_INFO("%02x",usbControlTransferPtr[iii]);
      while(1);

    }

    uint32ToLittleEndian(0x80000004,rndisResponse); /* INITIALIZE_CMPLT */
    uint32ToLittleEndian(id,rndisResponse+8);         /* request id to which we are responding */

    rndisResponseLen   = 24;

    uint32_t response_size = 0;
    switch (oid) {

    case OID_GEN_SUPPORTED_LIST:
      rndisResponseLen += sizeof(rndisSupportedOIDs);
      memcpy(rndisResponse + 24, rndisSupportedOIDs, sizeof(rndisSupportedOIDs));
      break;

    case OID_GEN_PHYSICAL_MEDIUM:
      rndisResponseLen += sizeof(uint32_t);
      /* Indicate that the device is a true ethernet link */
      uint32ToLittleEndian(0,rndisResponse+24);

      break;
    case OID_GEN_HARDWARE_STATUS:
      rndisResponseLen += sizeof(uint32_t);
      uint32ToLittleEndian(0 /* ready */,rndisResponse+24);
      break;

    case OID_GEN_MEDIA_SUPPORTED:
    case OID_GEN_MEDIA_IN_USE:
      rndisResponseLen += sizeof(uint32_t);
      uint32ToLittleEndian(REMOTE_NDIS_MEDIUM_802_3,rndisResponse+24);
      break;

    case OID_GEN_VENDOR_ID:
      rndisResponseLen += sizeof(uint32_t);
      /* Vendor ID 0x0xFFFFFF is reserved for vendors who have not purchased a NDIS VID */
      uint32ToLittleEndian(0x00FFFFFF,rndisResponse+24);
      break;

    case OID_GEN_MAXIMUM_TOTAL_SIZE:
      rndisResponseLen += sizeof(uint32_t);
      /* Indicate maximum overall buffer (Ethernet frame and RNDIS header) the adapter can handle */
      uint32ToLittleEndian(11*4 + ETHERNET_FRAME_SIZE_MAX,rndisResponse+24);
      break;

    case OID_GEN_TRANSMIT_BLOCK_SIZE:
    case OID_GEN_RECEIVE_BLOCK_SIZE:
    case OID_GEN_MAXIMUM_FRAME_SIZE:
      rndisResponseLen += sizeof(uint32_t);
      uint32ToLittleEndian(ETHERNET_FRAME_SIZE_MAX,rndisResponse+24);
      //uint32ToLittleEndian(256,rndisResponse+24);
      break;

    //case OID_GEN_TRANSMIT_BLOCK_SIZE:
    //case OID_GEN_RECEIVE_BLOCK_SIZE:
    //  rndisResponseLen += sizeof(uint32_t);
    //  uint32ToLittleEndian(102 /* from contiki */,rndisResponse+24);
    //  break;

    case OID_GEN_VENDOR_DESCRIPTION:
      rndisResponseLen += 16;
      memcpy(rndisResponse+24, "Sivan Toledo\0\0\0\0",16);
      break;

    case OID_GEN_MEDIA_CONNECT_STATUS:
      rndisResponseLen += sizeof(uint32_t);
      uint32ToLittleEndian(REMOTE_NDIS_MEDIA_STATE_CONNECTED,rndisResponse+24);
      break;

    case OID_GEN_LINK_SPEED:
      rndisResponseLen += sizeof(uint32_t);
      /* Indicate 10Mb/s link speed */
      uint32ToLittleEndian(100000,rndisResponse+24);
      //DBG("get link speed");
      break;

    case OID_802_3_PERMANENT_ADDRESS:
    case OID_802_3_CURRENT_ADDRESS:
      rndisResponseLen += 6; // mac address
      memcpy(rndisResponse + 24, "\x22\x33\x44\x55\x66\x77", 6);
      // MAC address reused from some old Meraki Mini
      //memcpy(rndisResponse + 24, "\x00\x18\x0A\x01\xDF\x44", 6);

      break;

    case OID_802_3_MAXIMUM_LIST_SIZE:
      rndisResponseLen += sizeof(uint32_t);
      /* Indicate only one multicast address supported */
      uint32ToLittleEndian(1,rndisResponse+24);
      break;

    case OID_GEN_CURRENT_PACKET_FILTER:
      rndisResponseLen += sizeof(uint32_t);
      uint32ToLittleEndian(rndisCurrentPacketFilter,rndisResponse+24);
      break;

    case OID_GEN_RCV_ERROR:
    case OID_GEN_XMIT_ERROR:
    case OID_GEN_RCV_NO_BUFFER:
    case OID_802_3_MAC_OPTIONS:
    case OID_802_3_RCV_ERROR_ALIGNMENT:
    case OID_802_3_XMIT_ONE_COLLISION:
    case OID_802_3_XMIT_MORE_COLLISIONS:
      rndisResponseLen += sizeof(uint32_t);
      /* Unused statistic OIDs - always return 0 for each */
      uint32ToLittleEndian(0,rndisResponse+24);

      break;
    case OID_GEN_RCV_OK: // received by the interface means we sent the packet to teh host.
      rndisResponseLen += sizeof(uint32_t);
      uint32ToLittleEndian(rndisTxPackets,rndisResponse+24);
      break;
    case OID_GEN_XMIT_OK:
      rndisResponseLen += sizeof(uint32_t);
      uint32ToLittleEndian(rndisRxPackets,rndisResponse+24);
      break;
    case OID_802_3_MULTICAST_LIST:
      rndisResponseLen += sizeof(uint32_t);
      /* from Linux drivers/gadget */
      uint32ToLittleEndian(0xE0000000,rndisResponse+24);
      break;
    default:
      break;
    }

    if (rndisResponseLen == 24) {
      DBG("No response to query!?!");
      DBG("QUERY OID=%x infolen=%d infooffset=%d",oid,buflen,bufptr);
      uint32ToLittleEndian(RNDIS_STATUS_NOT_SUPPORTED,rndisResponse+12); /* status */
      //uint32ToLittleEndian(RNDIS_STATUS_FAILURE,rndisResponse+12); /* status */
      uint32ToLittleEndian(0,rndisResponse+16);         /* buffer length */
      uint32ToLittleEndian(0,rndisResponse+20);         /* buffer ptr */
    } else {
      ETH_DEV_DBG_INFO("response with %d bytes",rndisResponseLen);
      uint32ToLittleEndian(RNDIS_STATUS_SUCCESS,rndisResponse+12); /* status */
      uint32ToLittleEndian(rndisResponseLen-24,rndisResponse+16);         /* buffer length */
      uint32ToLittleEndian(24-8,rndisResponse+20);         /* buffer ptr */
    }
    uint32ToLittleEndian(rndisResponseLen,rndisResponse+4);         /* length */

    // xxx wrong!!!

    rndisResponseReady = 1;
    eth_nak_interrupts |= INACK_II;
    usbEnableNAKInterrupts(eth_nak_interrupts);
    //usbEnableNAKInterrupts(INACK_II);

    break;
  case 0x05: /* REMOTE_NDIS_SET_MSG */
    uint32ToLittleEndian(0x80000005,rndisResponse); /* INITIALIZE_CMPLT */
    uint32ToLittleEndian(16,rndisResponse+4);         /* length */
    uint32ToLittleEndian(id,rndisResponse+8);         /* request id to which we are responding */
    uint32ToLittleEndian(RNDIS_STATUS_SUCCESS,rndisResponse+12); /* status */
    rndisResponseLen   = 16;

    oid    = uint32FromLittleEndian(usbControlTransferPtr+12);
    buflen = uint32FromLittleEndian(usbControlTransferPtr+16);
    bufptr = uint32FromLittleEndian(usbControlTransferPtr+20);
    ETH_DEV_DBG_INFO("SET OID=%x infolen=%d infooffset=%d",oid,buflen,bufptr);
    switch (oid) {
    case OID_GEN_CURRENT_PACKET_FILTER:
      // the 8 byte offset is for the header, which is the first two words in the message
      rndisCurrentPacketFilter = uint32FromLittleEndian(usbControlTransferPtr+8+bufptr);
      ETH_DEV_DBG_INFO("packet filter = %x",rndisCurrentPacketFilter);
      if (rndisCurrentPacketFilter) rndisState = RNDIS_DATA_INITIALIZED;
      else                          rndisState = RNDIS_INITIALIZED;
      break;
    case OID_802_3_MULTICAST_LIST:
      // do nothing
      break;
    default:
      ETH_DEV_DBG_INFO("cannot set this...");
      while(1);
      break;
    }
    rndisResponseReady = 1;
    eth_nak_interrupts |= INACK_II;
    usbEnableNAKInterrupts(eth_nak_interrupts);
    //usbEnableNAKInterrupts(INACK_II);

    break;

  case 0x06: /* RESET */
    ETH_DEV_DBG_INFO("RESET");
    DBG("rndis RESET");
    //while (1); // for testing
    uint32ToLittleEndian(0x80000006,rndisResponse); /* INITIALIZE_CMPLT */
    uint32ToLittleEndian(16,rndisResponse+4);         /* length */
    uint32ToLittleEndian(RNDIS_STATUS_SUCCESS,rndisResponse+8); /* status */
    uint32ToLittleEndian(0,rndisResponse+8); /* addressing reset: no, don't send again */
    rndisResponseLen   = 16;

    rndisResponseReady = 1;
    eth_nak_interrupts |= INACK_II;
    usbEnableNAKInterrupts(eth_nak_interrupts);
    //usbEnableNAKInterrupts(INACK_II);

    rndisReset();

    break;
  case 0x07: /* INDICATE_STATUS */
    // wrong direction; we need to send this to the host...
    ETH_DEV_DBG_INFO("INDICATE STATUS");
    break;
  case 0x08: /* KEEPALIVE */
    ETH_DEV_DBG_INFO("KEEPALIVE");
    uint32ToLittleEndian(0x80000008,rndisResponse); /* INITIALIZE_CMPLT */
    uint32ToLittleEndian(12,rndisResponse+4);         /* length */
    uint32ToLittleEndian(id,rndisResponse+8);         /* request id to which we are responding */

    rndisResponseReady = 1;
    eth_nak_interrupts |= INACK_II;
    usbEnableNAKInterrupts(eth_nak_interrupts);
    break;
  default:
    ETH_DEV_DBG_INFO("class control request=%x interface=%u len=%u dir=%d\n",usbRequest.request,usbRequest.index,usbRequest.length,REQTYPE_GET_DIR(usbRequest.type));
    ETH_DEV_DBG_INFO("RNDIS request type = %x len=%u id=%u\n",type,len,id);
    break;

  }

  return TRUE;
}

/*******************************************************/
/* RNDIS Bus Reset                                     */
/*******************************************************/

static void usb_device_status_handler(uint8_t dev_status) {
  static count = 0;
  if (dev_status & DEV_STATUS_RESET) {
    //printf("bus reset");
    DBG("USB Bus Reset status=%x\n\n",dev_status);
    if (0 & ++count == 3) {
      DBG("That's it, stopping");
      while(1);
    }
    rndisState = RNDIS_UNINITIALIZED;

    rndisReset();
  }
}

//#define usb_SOF_handler()

/*******************************************************/
/* RNDIS Notifications                                 */
/*******************************************************/

void usb_rndis_notification(uint8_t ep, uint8_t stat) {

  if (stat & EP_STATUS_DATA) return; /* next buffer is full, wait */

  if (rndisResponseReady) {
    //ETH_DEV_DBG_INFO("rndis notification: Yes! status=%02x\n",stat);
    usbWrite(ep, rndisResponseAvailable, 8);
  } else {
    //ETH_DEV_DBG_INFO("rndis notification: no...  status=%02x\n",stat);
  }
  eth_nak_interrupts &= ~INACK_II;
  usbEnableNAKInterrupts(eth_nak_interrupts);
  //usbEnableNAKInterrupts(0);
  return;
}

/*******************************************************/
/* RNDIS Receive and Transmit Handlers                 */
/*******************************************************/

uint8_t rndisPacketHeader[64];
uint32_t rndisRxTransferCount;
uint32_t rndisRxPacketSize;
enum {
  RNDIS_RX_IDLE,
  RNDIS_RX_DROP,
  RNDIS_RX_RECV
} rndisRxState = RNDIS_RX_IDLE;
uint32_t rndisRxPtr;

void usb_rndis_rx(uint8_t ep, uint8_t stat) {
  int len;
  int ip_length;
  int i;
  uint16_t eth_hdr_type;

  if (!(stat & EP_STATUS_DATA)) {
    ETH_DEV_DBG_ERROR("rndis rx, no data...");
    return;
  }

  //DBG("rndis rx ep-state=%x state=%d packetsize=%d transferred=%d",stat,rndisRxState,rndisRxPacketSize,rndisRxTransferCount);

  switch (rndisRxState) {
  case RNDIS_RX_IDLE:
  //if (rndisRxState == RNDIS_RX_IDLE) {
    len = usbRead(ep, rndisPacketHeader, 64);
    if (len==1 && rndisPacketHeader[0] == 0) {
      DBG("single byte USB packet (this is normal for rndis");
      return;
    }

    if (len < 44) {
      ETH_DEV_DBG_ERROR("Whoops, USB packet length shorter than RNDIS message header (only %d bytes)",len);
      return;
    }

    uint32_t type     = uint32FromLittleEndian(rndisPacketHeader);
    uint32_t rndislen = uint32FromLittleEndian(rndisPacketHeader+4);
    uint32_t dataoff  = uint32FromLittleEndian(rndisPacketHeader+8);
    uint32_t datalen  = uint32FromLittleEndian(rndisPacketHeader+12);
    uint32_t ooboff   = uint32FromLittleEndian(rndisPacketHeader+16);
    uint32_t ooblen   = uint32FromLittleEndian(rndisPacketHeader+20);
    uint32_t numoob   = uint32FromLittleEndian(rndisPacketHeader+24);
    uint32_t perpacket_infooff = uint32FromLittleEndian(rndisPacketHeader+28);
    uint32_t perpacket_infolen = uint32FromLittleEndian(rndisPacketHeader+32);

    //DBG("rndis rx type=%x len=%d dataoff=%d datalen=%d ooblen=%d numoob=%d perpacket_infooff=%d perpacket_infolen=%d",
    //    type,len,dataoff,datalen,ooblen,numoob,perpacket_infooff,perpacket_infolen);

    if (type != 1) while(1);

    rndisRxTransferCount = 0;
    rndisRxPacketSize    = datalen;
    /* hopefully, it follows the header immediately... */

    if (receive_packet == 0) {
      if (RING_IS_FULL(receive_ring_head, receive_ring_tail, RECEIVE_RING_SIZE)) {
        DBG("usb_eth_rx: receive ring is full, head=%d, tail=%d",receive_ring_head, receive_ring_tail);
        rndisRxState = RNDIS_RX_DROP;
        for (i=0; i<(len-44); i++) {
           rndisRxTransferCount++;
        }
      } else {
        rndisRxState = RNDIS_RX_RECV;
        receive_packet = &receive_ring[receive_ring_tail];
        packet_init(receive_packet);

        for (i=0; i<(len-44); i++) {
          (receive_packet->data)[ rndisRxTransferCount ] = rndisPacketHeader[44+i];
           rndisRxTransferCount++;
        }
      }
    }
    break;

  case RNDIS_RX_DROP:
  //if (rndisRxState == RNDIS_RX_DROP) {
    len = usbRead(ep, NULL, rndisRxPacketSize - rndisRxTransferCount);
    //ETH_DEV_DBG_INFO("rndis rx read %d bytes more in drop state",len);

    if (len >= 0)
      rndisRxTransferCount += len;
    break;
//  }

  case RNDIS_RX_RECV:
  //if (rndisRxState == RNDIS_RX_RECV) {
    //DBG("rndis rx recv read from endpoint");
    len = usbRead(ep, (receive_packet->data) + rndisRxTransferCount, rndisRxPacketSize - rndisRxTransferCount);
    //DBG("rndis rx read %d bytes more in recv state",len);

    if (len >= 0)
      rndisRxTransferCount += len;
    break;
  }

  if (rndisRxTransferCount == rndisRxPacketSize) {
    rndisRxPackets++;
    DBG("rndis rx done with a packet (%d bytes)",rndisRxPacketSize);
    //for (i=0; i<rndisRxPacketSize; i++) DBG("%d %02x",i,(receive_packet->data)[i]);
    //if (rndisRxPacketSize==54) while(1);
    if (rndisRxState == RNDIS_RX_RECV) {
      receive_packet->size = rndisRxPacketSize;
      receive_packet = 0;
      RING_INC(receive_ring_tail, RECEIVE_RING_SIZE);
      xxx = 1;
    }
    rndisRxState = RNDIS_RX_IDLE;
  }

}

void usb_rndis_tx(uint8_t ep, uint8_t stat) {
  int len;
  int i;

  //DBG("rndis tx\n");
  if (!stat & EP_STATUS_NACKED) {
    DBG("tx endpoint interrupt, but not NAK");
    return;
  }

  if (!packet_pending || bytes_to_send == 0) {
    ETH_DEV_DBG_INFO("usb_eth_tx, send buffer is empty, packet_pending=%d", packet_pending);

    eth_nak_interrupts &= ~INACK_BI;
    usbEnableNAKInterrupts(eth_nak_interrupts);
    return;
  }

  if (bytes_sent == 0) {
    /* prepend rndis header */
    memset(rndisPacketHeader,0,44);
    uint32ToLittleEndian(1,rndisPacketHeader); // type
    uint32ToLittleEndian(44+bytes_to_send,rndisPacketHeader+4);
    uint32ToLittleEndian(44-8,rndisPacketHeader+8); // type
    uint32ToLittleEndian(bytes_to_send,rndisPacketHeader+12); // type

    /* copy start of frame to buffer */
    len = MIN(bytes_to_send, 64-44);
    memcpy(rndisPacketHeader + 44, send_buffer, len);
    usbWrite(ep, rndisPacketHeader, 44+len);
    bytes_sent += len;
    //DBG("sent header and %d bytes out of %d",len,bytes_to_send);
  } else {

    len = MIN(bytes_to_send - bytes_sent, MAX_PACKET_SIZE);

    usbWrite(ep, ((uint8_t*) &send_buffer)+bytes_sent, len);
    bytes_sent += len;

    //DBG("usb_eth_tx: send %d bytes, frame data sent %d/%d",
    //    len, bytes_sent, bytes_to_send);

  }

  if (bytes_sent >= bytes_to_send) {
    /* finished sending data */
    //DBG("usb_rndis_tx: send done... (%d bytes)",bytes_to_send);
    //for (i=0; i< bytes_to_send;i++) {
    //  send_buffer[i] = 0;
    //}

    rndisTxPackets++;

    bytes_to_send = 0;
    bytes_sent = 0;
    packet_pending = 0;

    //eth_nak_interrupts &= ~INACK_BI;
    //usbEnableNAKInterrupts(eth_nak_interrupts);
  }
}

void rndisReset(void) {
  ETH_DEV_DBG_INFO("rndis reset start");

  rndisRxState = RNDIS_RX_IDLE;

  network_device_init();

  //usbClear(USB_EP_OUT | RNDIS_BULK_EP);
  //usbClear(USB_EP_OUT | RNDIS_NOTIFICATION_EP);

  ETH_DEV_DBG_INFO("rndis reset end");

  eth_nak_interrupts = 0;
  usbEnableNAKInterrupts(eth_nak_interrupts);
}

static void rndisInit(void) {

  //uint32ToLittleEndian(1, rndisResponseAvailable  );
  //uint32ToLittleEndian(0, rndisResponseAvailable+4);
  //rndisResponseNotAvailable[0] = 0;

  //usbEnableNackInterrupts(INACK_II);
}

/****************************************/
void usb_cdc_ecm_rx(uint8_t ep, uint8_t stat) {
  int len;
  int ip_length;
  int i;
  uint16_t eth_hdr_type;

  //printf("cdc ecm rx ep-state=%x state=%d transferred=%d",stat,rndisRxState,rndisRxTransferCount);

  switch (rndisRxState) {
  case RNDIS_RX_IDLE:
    rndisRxTransferCount = 0;
    rndisRxPacketSize    = 0; /* it may increase later */
    /* hopefully, it follows the header immediately... */

    if (receive_packet == 0) {
      if (RING_IS_FULL(receive_ring_head, receive_ring_tail, RECEIVE_RING_SIZE)) {
        rndisRxState = RNDIS_RX_DROP;
        DBG("usb_eth_rx: receive ring is full, head=%d, tail=%d",receive_ring_head, receive_ring_tail);
        len = usbRead(ep, NULL, 64);
      } else {
        rndisRxState = RNDIS_RX_RECV;

        receive_packet = &receive_ring[receive_ring_tail];
        packet_init(receive_packet);

        len = usbRead(ep, receive_packet->data , 64);
        rndisRxTransferCount += len;
      }
    }
    break;

  case RNDIS_RX_DROP:
    len = usbRead(ep, NULL, 64);
    //ETH_DEV_DBG_INFO("rndis rx read %d bytes more in drop state",len);
    if (len >= 0) rndisRxTransferCount += len;
    break;

  case RNDIS_RX_RECV:
    len = usbRead(ep, (receive_packet->data) + rndisRxTransferCount, 64 /* maybe we need to limit this */);

    if (len >= 0) rndisRxTransferCount += len;
    break;
  }

  if (len < 64) { /* a short packet terminates the Ethernet frame */
    rndisRxPackets++;
    DBG("cdc ecm rx done with a packet (%d bytes)",rndisRxTransferCount);
    //for (i=0; i<rndisRxPacketSize; i++) DBG("%d %02x",i,(receive_packet->data)[i]);
    //if (rndisRxPacketSize==54) while(1);
    if (rndisRxState == RNDIS_RX_RECV) {
      receive_packet->size = rndisRxTransferCount;
      receive_packet = 0;
      RING_INC(receive_ring_tail, RECEIVE_RING_SIZE);
    }
    rndisRxState = RNDIS_RX_IDLE;
  }
}

void usb_cdc_ecm_tx(uint8_t ep, uint8_t stat) {
  int len;
  int i;

  //printf(" cdc_ecm tx ");
  if (!stat & EP_STATUS_NACKED) {
    DBG("tx endpoint interrupt, but not NAK");
    return;
  }

  if (!packet_pending || bytes_to_send == 0) {
    ETH_DEV_DBG_INFO("usb_eth_tx, send buffer is empty, packet_pending=%d", packet_pending);

    eth_nak_interrupts &= ~INACK_BI;
    usbEnableNAKInterrupts(eth_nak_interrupts);
    return;
  }


  len = MIN(bytes_to_send - bytes_sent, MAX_PACKET_SIZE);

  usbWrite(ep, ((uint8_t*) &send_buffer)+bytes_sent, len);
  bytes_sent += len;

  if (bytes_sent >= bytes_to_send   /* we sent all the data */
      && len < MAX_PACKET_SIZE)     /* and we don't need to send a zero-length termination packet */
  {
    /* finished sending data */

    rndisTxPackets++;

    bytes_to_send = 0;
    bytes_sent = 0;
    packet_pending = 0;

    //eth_nak_interrupts &= ~INACK_BI;
    //usbEnableNAKInterrupts(eth_nak_interrupts);
  }
}
