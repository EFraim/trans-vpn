#include "RNDISConstants.h"
#include "common_RNDIS.h"

#define usb_SOF_handler(frame_number) FALSE

#define usb_control_standard_custom_handler() FALSE
#define usb_control_vendor_handler() FALSE
#define usb_control_reserved_handler() FALSE

#define INT_EP  1
#define BULK_EP 2

#define MAX_PACKET_SIZE (64)

//#define RNDIS_MESSAGE_BUFFER_SIZE             128

#define RNDIS_NOTIFICATION_EP 1
#define RNDIS_BULK_EP 2

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
                              handles call management (does not?) */
  0,                   /* bDataInterface ??             */

  0x04,
  CS_INTERFACE,
  0x02,                /* Subtype = Abs. Ctl. Manage. */
  0x00,

  0x05,
  CS_INTERFACE,
  0x06,                /* Subtype = Union             */
  0x00,                /* MasterInterface             */
  0x01,                /* Slave Interface             */

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
  USB_EP_IN | RNDIS_BULK_EP,/* Endpoint Address       */
  0x02,               /* Attributes = bulk            */
  LE_WORD(MAX_PACKET_SIZE),
  0x00,               /* Interval (not used for bulk) */

  0x07,
  DESC_ENDPOINT,
  USB_EP_OUT | RNDIS_BULK_EP, /* Endpoint Address           */
  0x02,               /* Attributes = bulk            */
  LE_WORD(MAX_PACKET_SIZE),
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

  0 /* terminator */
};

void usb_rndis_rx(uint8_t ep, uint8_t stat);
void usb_rndis_tx_ex(uint8_t ep, uint8_t stat);
void usb_rndis_notification(uint8_t ep, uint8_t stat);

static uint8_t control_rx_buffer[256 + ETHERNET_FRAME_SIZE_MAX];
static uint32_t control_rx_buffer_len;

static uint32_t response_ready = FALSE;

static usb_ep_handler_t* const usb_ep_handlers[32] = {
   usbEP0OutHandler, usbEP0InHandler, /* EP  0 Out, In */
   0,&usb_rndis_notification,         /* EP  1 Out, In */
   &usb_rndis_rx, &usb_rndis_tx_ex,      /* EP  2 Out, In */
   0, 0
};

static volatile uint8_t eth_nak_interrupts = 0;

/*** Sivan RNDIS CODE ***/

typedef enum
{
	RNDIS_STATE_UNINITIALIZED,
	RNDIS_STATE_INITIALIZED,
	RNDIS_STATE_DATA_INITIALIZED
} RNDIS_STATE;

static RNDIS_STATE rndis_state = RNDIS_STATE_UNINITIALIZED;

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
	OID_802_3_RCV_ERROR_ALIGNMENT,
	OID_802_3_XMIT_ONE_COLLISION,
	OID_802_3_XMIT_MORE_COLLISIONS,
};

/*******************************************************/
/* RNDIS Control Class Handler                         */
/*******************************************************/

static void rndis_reset(void);

static bool RNDIS_Device_ProcessNDISQuery(const uint32_t OId, void* const QueryData, const uint16_t QuerySize,
                                          void* ResponseData, uint16_t* const ResponseSize)
{
	(void)QueryData;
	(void)QuerySize;

	switch (OId)
	{
		case OID_GEN_SUPPORTED_LIST:
			DBG("[RNDIS] OID: SUPPORTED_LIST");
			*ResponseSize = sizeof(rndisSupportedOIDs);
			
			memcpy(ResponseData, rndisSupportedOIDs, sizeof(rndisSupportedOIDs));
			
			return true;
		case OID_GEN_PHYSICAL_MEDIUM:
			DBG("[RNDIS] OID: PHYSICAL_MEDIUM");
			*ResponseSize = sizeof(uint32_t);
			
			/* Indicate that the device is a true ethernet link */
			*((uint32_t*)ResponseData) = 0;
			
			return true;
		case OID_GEN_HARDWARE_STATUS:
			DBG("[RNDIS] OID: HARDWARE_STATUS");
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = NDIS_HardwareStatus_Ready;
			
			return true;
		case OID_GEN_MEDIA_SUPPORTED:
		case OID_GEN_MEDIA_IN_USE:
			DBG("[RNDIS] OID: MEDIA_SUPPORTED_IN_USE");
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = REMOTE_NDIS_MEDIUM_802_3;
			
			return true;
		case OID_GEN_VENDOR_ID:
			DBG("[RNDIS] OID: VENDOR_ID");
			*ResponseSize = sizeof(uint32_t);
			
			/* Vendor ID 0x0xFFFFFF is reserved for vendors who have not purchased a NDIS VID */
			*((uint32_t*)ResponseData) = 0x00FFFFFF;
			
			return true;
		case OID_GEN_MAXIMUM_FRAME_SIZE:
		case OID_GEN_TRANSMIT_BLOCK_SIZE:
		case OID_GEN_RECEIVE_BLOCK_SIZE:
			DBG("[RNDIS] OID: FRAME_OR_BLOCK_SIZE");
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = ETHERNET_FRAME_SIZE_MAX;
			
			return true;
		case OID_GEN_VENDOR_DESCRIPTION:
			DBG("[RNDIS] OID: VENDOR_DESCRIPTION");
			*ResponseSize = (strlen("Ron & Ido's Remote NDIS Based Device") + 1);
			
			memcpy(ResponseData, "Ron & Ido's Remote NDIS Based Device", *ResponseSize - 1);
			
			return true;
		case OID_GEN_MEDIA_CONNECT_STATUS:
			//DBG("[RNDIS] OID: MEDIA_CONNECT_STATUS");
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = REMOTE_NDIS_MEDIA_STATE_CONNECTED;
			
			return true;
		case OID_GEN_LINK_SPEED:
			//DBG("[RNDIS] OID: LINK_SPEED");
			*ResponseSize = sizeof(uint32_t);
			
			/* Indicate 10Mb/s link speed */
			*((uint32_t*)ResponseData) = 100000;

			return true;
		case OID_802_3_PERMANENT_ADDRESS:
		case OID_802_3_CURRENT_ADDRESS:
			DBG("[RNDIS] OID: PERMANENT_OR_CURRENT_ADDRESS");
			*ResponseSize = 6; // sizeof(MAC_Address_t);
			
			memcpy(ResponseData, MACAddress, 6); //sizeof(MAC_Address_t));

			return true;
		case OID_802_3_MAXIMUM_LIST_SIZE:
			DBG("[RNDIS] OID: MAXIMUM_LIST_SIZE");
			*ResponseSize = sizeof(uint32_t);
			
			/* Indicate only one multicast address supported */
			*((uint32_t*)ResponseData) = 1;
		
			return true;
		case OID_GEN_CURRENT_PACKET_FILTER:
			DBG("[RNDIS] OID: CURRENT_PACKET_FILTER");
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = rndisCurrentPacketFilter;
		
			return true;			
		case OID_GEN_XMIT_OK:
		case OID_GEN_RCV_OK:
		case OID_GEN_XMIT_ERROR:
		case OID_GEN_RCV_ERROR:
		case OID_GEN_RCV_NO_BUFFER:
		case OID_802_3_RCV_ERROR_ALIGNMENT:
		case OID_802_3_XMIT_ONE_COLLISION:
		case OID_802_3_XMIT_MORE_COLLISIONS:
			//DBG("[RNDIS] OID: Unused statistic OIDs");
			*ResponseSize = sizeof(uint32_t);
			
			/* Unused statistic OIDs - always return 0 for each */
			*((uint32_t*)ResponseData) = 0;
		
			return true;
		case OID_GEN_MAXIMUM_TOTAL_SIZE:
			DBG("[RNDIS] OID: MAXIMUM_TOTAL_SIZE");
			*ResponseSize = sizeof(uint32_t);
			
			/* Indicate maximum overall buffer (Ethernet frame and RNDIS header) the adapter can handle */
			*((uint32_t*)ResponseData) = (RNDIS_MESSAGE_BUFFER_SIZE + ETHERNET_FRAME_SIZE_MAX);
		
			return true;
		default:
			DBG("[RNDIS] OID: Unknown #### ERROR ####");
			return false;
	}
}

static bool RNDIS_Device_ProcessNDISSet(const uint32_t OId,
                                        void* SetData, const uint16_t SetSize)
{
	(void)SetSize;

	switch (OId)
	{
		case OID_GEN_CURRENT_PACKET_FILTER:
			DBG("[RNDIS] OID: GEN_CUR_PCKT_FLTR");
			DBG("[RNDIS] STATE: DATA_INITIALIZED");

			rndisCurrentPacketFilter = *(uint32_t*)SetData;

			rndis_state = RNDIS_STATE_DATA_INITIALIZED;
			
			return true;
		case OID_802_3_MULTICAST_LIST:
			DBG("[RNDIS] OID: GEN_CUR_PCKT_FLTR");
			/* Do nothing - throw away the value from the host as it is unused */
		
			return true;
		default:
			DBG("[RNDIS] OID: Unknown #### ERROR ####");
			return false;
	}
}

static bool usb_control_class_handler(void) 
{
	uint32_t rndis_message_type;

	if (REQTYPE_GET_DIR(usbRequest.type) == REQTYPE_DIR_TO_DEVICE)
	{
		rndis_message_type = *((uint32_t*)usbControlTransferPtr);

		switch(rndis_message_type)
		{
			case REMOTE_NDIS_INITIALIZE_MSG:
				{
					DBG("[RNDIS] MSG: REMOTE_NDIS_INITIALIZE_MSG");
					RNDIS_Initialize_Message_t*  INITIALIZE_Message  =
								   (RNDIS_Initialize_Message_t*)usbControlTransferPtr;
					RNDIS_Initialize_Complete_t* INITIALIZE_Response =
								   (RNDIS_Initialize_Complete_t*)&control_rx_buffer;
					
					INITIALIZE_Response->MessageType           = REMOTE_NDIS_INITIALIZE_CMPLT;
					INITIALIZE_Response->MessageLength         = sizeof(RNDIS_Initialize_Complete_t);
					INITIALIZE_Response->RequestId             = INITIALIZE_Message->RequestId;
					INITIALIZE_Response->Status                = REMOTE_NDIS_STATUS_SUCCESS;
					
					INITIALIZE_Response->MajorVersion          = REMOTE_NDIS_VERSION_MAJOR;
					INITIALIZE_Response->MinorVersion          = REMOTE_NDIS_VERSION_MINOR;			
					INITIALIZE_Response->DeviceFlags           = REMOTE_NDIS_DF_CONNECTIONLESS;
					INITIALIZE_Response->Medium                = REMOTE_NDIS_MEDIUM_802_3;
					INITIALIZE_Response->MaxPacketsPerTransfer = 1;
					INITIALIZE_Response->MaxTransferSize       = (sizeof(RNDIS_Packet_Message_t) + ETHERNET_FRAME_SIZE_MAX); // INITIALIZE_Message->MaxTransferSize; // (sizeof(RNDIS_Packet_Message_t) + ETHERNET_FRAME_SIZE_MAX);
					INITIALIZE_Response->PacketAlignmentFactor = 0;
					INITIALIZE_Response->AFListOffset          = 0;
					INITIALIZE_Response->AFListSize            = 0;

					control_rx_buffer_len = sizeof(RNDIS_Initialize_Complete_t);

					response_ready = TRUE;

					rndis_state = RNDIS_STATE_INITIALIZED;
					DBG("[RNDIS] STATE: INITIALIZED");
				}
				break;
			case REMOTE_NDIS_HALT_MSG:
				DBG("[RNDIS] MSG: REMOTE_NDIS_HALT_MSG");
				break;
			case REMOTE_NDIS_RESET_MSG:
				{
					DBG("[RNDIS] MSG: REMOTE_NDIS_RESET_MSG");
					RNDIS_Reset_Complete_t* RESET_Response = (RNDIS_Reset_Complete_t*)usbControlTransferPtr;

					RESET_Response->MessageType     = REMOTE_NDIS_RESET_CMPLT;
					RESET_Response->MessageLength   = sizeof(RNDIS_Reset_Complete_t);
					RESET_Response->Status          = REMOTE_NDIS_STATUS_SUCCESS;
					RESET_Response->AddressingReset = 0;

					rndis_reset();

					response_ready = true;
				}
				break;
			case REMOTE_NDIS_KEEPALIVE_MSG:
				{
					DBG("[RNDIS] MSG: REMOTE_NDIS_KEEPALIVE_MSG");
					RNDIS_KeepAlive_Message_t*  KEEPALIVE_Message  =
									(RNDIS_KeepAlive_Message_t*)usbControlTransferPtr;
					RNDIS_KeepAlive_Complete_t* KEEPALIVE_Response =
									(RNDIS_KeepAlive_Complete_t*)&control_rx_buffer;

					KEEPALIVE_Response->MessageType   = REMOTE_NDIS_KEEPALIVE_CMPLT;
					KEEPALIVE_Response->MessageLength = sizeof(RNDIS_KeepAlive_Complete_t);
					KEEPALIVE_Response->RequestId     = KEEPALIVE_Message->RequestId;
					KEEPALIVE_Response->Status        = REMOTE_NDIS_STATUS_SUCCESS;
				
					response_ready = true;
				}		
				break;
			case REMOTE_NDIS_QUERY_MSG:
				{	
					//DBG("[RNDIS] MSG: REMOTE_NDIS_QUERY_MSG");
					RNDIS_Query_Message_t*  QUERY_Message  = (RNDIS_Query_Message_t*)usbControlTransferPtr;
					RNDIS_Query_Complete_t* QUERY_Response = (RNDIS_Query_Complete_t*)&control_rx_buffer;
					uint32_t                Query_Oid      = QUERY_Message->Oid;
								
					void*     QueryData = &usbControlTransferPtr[sizeof(RNDIS_Message_Header_t) /*+ sizeof(uint32_t)*/ + QUERY_Message->InformationBufferOffset];
					void*     ResponseData = &control_rx_buffer[sizeof(RNDIS_Query_Complete_t)];		
					uint16_t  ResponseSize;

					QUERY_Response->MessageType   = REMOTE_NDIS_QUERY_CMPLT;
					QUERY_Response->MessageLength = sizeof(RNDIS_Query_Complete_t);
					QUERY_Response->RequestId		= QUERY_Message->RequestId;

					if (RNDIS_Device_ProcessNDISQuery(Query_Oid, QueryData, QUERY_Message->InformationBufferLength,
													  ResponseData, &ResponseSize))
					{
						QUERY_Response->Status                  = REMOTE_NDIS_STATUS_SUCCESS;
						QUERY_Response->MessageLength          += ResponseSize;
									
						QUERY_Response->InformationBufferLength = ResponseSize;
						QUERY_Response->InformationBufferOffset = (sizeof(RNDIS_Query_Complete_t) - sizeof(RNDIS_Message_Header_t) /*- sizeof(uint32_t)*/);
					}
					else
					{				
						QUERY_Response->Status                  = REMOTE_NDIS_STATUS_NOT_SUPPORTED;

						QUERY_Response->InformationBufferLength = 0;
						QUERY_Response->InformationBufferOffset = 0;
					}

					control_rx_buffer_len = QUERY_Response->MessageLength;

					response_ready = true;
				}				
				break;
			case REMOTE_NDIS_SET_MSG:
				{
					DBG("[RNDIS] MSG: REMOTE_NDIS_SET_MSG");
					RNDIS_Set_Message_t*  SET_Message  = (RNDIS_Set_Message_t*)usbControlTransferPtr;
					RNDIS_Set_Complete_t* SET_Response = (RNDIS_Set_Complete_t*)&control_rx_buffer;
					uint32_t              SET_Oid      = SET_Message->Oid;

					SET_Response->MessageType   = REMOTE_NDIS_SET_CMPLT;
					SET_Response->MessageLength = sizeof(RNDIS_Set_Complete_t);
					SET_Response->RequestId     = SET_Message->RequestId;

					void* SetData = &control_rx_buffer[sizeof(RNDIS_Message_Header_t) + SET_Message->InformationBufferOffset];
								
					SET_Response->Status = RNDIS_Device_ProcessNDISSet(SET_Oid, SetData,
																	   SET_Message->InformationBufferLength) ?
																	   REMOTE_NDIS_STATUS_SUCCESS : REMOTE_NDIS_STATUS_NOT_SUPPORTED;
					response_ready = true;

				}
				break;

		}
		
		if (response_ready == true)
		{
			eth_nak_interrupts |= INACK_II;
			usbEnableNAKInterrupts(eth_nak_interrupts);
		}

		return TRUE;
	}
	else if (REQTYPE_GET_DIR(usbRequest.type) == REQTYPE_DIR_TO_HOST)
	{

		if (response_ready == true)
		{
			usbControlTransferPtr = (uint8_t*)&control_rx_buffer;
			usbControlTransferLen = control_rx_buffer_len;
			//DBG("[RNDIS] Sending response");
			response_ready = false;
		}
		else
		{
			usbControlTransferPtr = &rndisResponseNotAvailable;
			usbControlTransferLen = 1;
			// DBG("[RNDIS] ERROR: Received 0xA1 when response is not ready");
		}

		return TRUE;
	}

	DBG("rndis ERROR");

	return FALSE;
}

/*******************************************************/
/* RNDIS Notifications                                 */
/*******************************************************/

void usb_rndis_notification(uint8_t ep, uint8_t stat) 
{
	if (stat & EP_STATUS_DATA) 
	  return; /* next buffer is full, wait */

	if (response_ready == true) 
	{
		usbWrite(ep, rndisResponseAvailable, 8);

		// Disable NAKs interrupts for Interrupt EPs
		eth_nak_interrupts &= ~INACK_II;
		usbEnableNAKInterrupts(eth_nak_interrupts);
	}

	return;
}

/*******************************************************/
/* RNDIS Receive and Transmit Handlers                 */
/*******************************************************/


//static uint8_t data_rx_buffer[256 + ETHERNET_FRAME_SIZE_MAX];
static int32_t data_rx_buffer_remaining_length;
static uint8_t * data_rx_buffer_current_location;

static uint8_t data_tx_buffer[256 + ETHERNET_FRAME_SIZE_MAX]; // (sizeof(RNDIS_Packet_Message_t) + ETHERNET_FRAME_SIZE_MAX)
static int32_t data_tx_buffer_remaining_length;
static uint8_t * data_tx_buffer_current_location = data_tx_buffer;

typedef enum
{
	TX_PACKET_STATE_EMPTY,		// No packet in the tx buffer
	TX_PACKET_STATE_RECEIVING,	// Packet is being received into the tx buffer
	TX_PACKET_STATE_READY		// Packet fully received and is ready to be sent over the Ethernet interface
} TX_PACKET_STATE;

static volatile TX_PACKET_STATE tx_packet_state = TX_PACKET_STATE_EMPTY;

RNDIS_Packet_Message_t * RNDISPacketHeaderTx = (RNDIS_Packet_Message_t*)&data_tx_buffer;

typedef enum
{
	RX_PACKET_STATE_EMPTY,		// No packet in the rx buffer
	RX_PACKET_STATE_SENDING,	// Packet is being sent to the host from the rx buffer
	RX_PACKET_STATE_READY		// Packet is fully received from the Ethernet interface and is ready to be sent to the host
} RX_PACKET_STATE;

static volatile RX_PACKET_STATE rx_packet_state = RX_PACKET_STATE_EMPTY;

//RNDIS_Packet_Message_t * RNDISPacketHeaderRx = (RNDIS_Packet_Message_t*)&data_rx_buffer;


// out
void usb_rndis_rx(uint8_t ep, uint8_t stat) 
{
	int8_t read_bytes;

	led_on(EP2_OUT_LED);
	
	// Tx packet is ready to be sent - do not accpet new packets for transmittion
	if (tx_packet_state == TX_PACKET_STATE_READY)
	{
		// Absort zero-length packet
		read_bytes = usbRead(USB_EP_OUT | 0x02, (uint8_t*)NULL, 0);
		//DBG(read_bytes > 0 ? "[DATA][OUT][1] ?\n" : "[DATA][OUT][1] -\n");
		led_off(EP2_OUT_LED);
		return;
	}

	// Start receiving a new packet for transmittion
	if (tx_packet_state == TX_PACKET_STATE_EMPTY)
	{
		// Read first chunk
		read_bytes = usbRead(USB_EP_OUT | 0x02, data_tx_buffer, 64); //sizeof(RNDIS_Packet_Message_t));

		if (read_bytes < 0)
		{
			DBG("[DATA][OUT] ERROR1");
			led_off(EP2_OUT_LED);
			return;
		}

		// If read succesful, set new state
		tx_packet_state = TX_PACKET_STATE_RECEIVING;
		//DBG("[DATA][OUT] TX_STATE: RECEIVING");

		//DBG("[DATA][OUT] Receive %u (%u)", RNDISPacketHeaderTx->MessageLength, RNDISPacketHeaderTx->DataLength);

		//DBG("[DATA][OUT] %u", (_uint32_t)read_bytes);

		// Set current location in the buffer (Further data will be written to that location)
		data_tx_buffer_current_location = data_tx_buffer + read_bytes; //sizeof(RNDIS_Packet_Message_t);

		// Update the current transfer's remaining length
		data_tx_buffer_remaining_length = RNDISPacketHeaderTx->MessageLength - read_bytes; //sizeof(RNDIS_Packet_Message_t);

		// If this is the last chunk of the transfer
		if (data_tx_buffer_remaining_length == 0)
		{
			DBG("[DATA][OUT] Completed");

			// Print packet data
			#ifdef DBG_PACKET_DATA
			{
				uint32_t i;
				for (i = 0; i < RNDISPacketHeaderTx->DataLength; i++)
				{
					DBG(&data_tx_buffer[sizeof(RNDIS_Message_Header_t) + RNDISPacketHeaderTx->DataOffset] + i);
				}
				DBG("");
			}
			#endif

			tx_packet_state = TX_PACKET_STATE_READY;
			//DBG("[DATA][OUT] TX_STATE: READY");
		}
	}
	else // tx_packet_state == TX_PACKET_STATE_RECEIVING
	{
		// If there are more transactions in the transfer
		if (data_tx_buffer_remaining_length > 0)
		{
			// Read next transaction's data
			read_bytes = usbRead(USB_EP_OUT | 0x02, data_tx_buffer_current_location, data_tx_buffer_remaining_length);

			if (read_bytes < 0)
			{
				DBG("[DATA][OUT] ERROR2");
				led_off(EP2_OUT_LED);
				return;
			}

			//DBG("[DATA][OUT] %u read", read_bytes);

			// Update current location in the tx buffer and the remaining length of the transfer
			data_tx_buffer_current_location += read_bytes;
			data_tx_buffer_remaining_length -= read_bytes;

			if (data_tx_buffer_remaining_length < 0)
			{
				DBG("[DATA][OUT] ERROR3");
				led_off(EP2_OUT_LED);
				return;
			}

			// If this is the last chunk of the transfer
			if (data_tx_buffer_remaining_length == 0)
			{
				//DBG("[DATA][OUT] Completed");

				// Print packet data
				#ifdef DBG_PACKET_DATA
				{
					uint32_t i;
					for (i = 0; i < RNDISPacketHeaderTx->DataLength; i++)
					{
						DBG(&data_tx_buffer[sizeof(RNDIS_Message_Header_t) + RNDISPacketHeaderTx->DataOffset] + i);
					}
					DBG("");
				}
				#endif

				tx_packet_state = TX_PACKET_STATE_READY;
				//DBG("[DATA][OUT] TX_STATE: READY");
			}
		}
		else
		{
			// Absort zero-length packet
			read_bytes = usbRead(USB_EP_OUT | 0x02, (uint8_t*)NULL, 0);
			DBG(read_bytes > 0 ? "[DATA][OUT][2] ?\n" : "[DATA][OUT][2] -\n");
		}
	}

	led_off(EP2_OUT_LED);

}


void usb_rndis_tx_ex(uint8_t ep, uint8_t stat) {
	int32_t len;
	RNDIS_Packet_Message_t* pRNDISHeader;

	led_on(EP2_IN_LED);

	// If there is no packet to be sent to the host, return immediately (return NAK)
	if (rxPacketQueue.startPtr == rxPacketQueue.endPtr)
	{
		//DBG("[DATA][IN] EMPTY");
		led_off(EP2_IN_LED);
		return;
	}

	// If the packet is fully received from the Ethernet interface
	// and is ready to be sent to the host
	if (rx_packet_state != RX_PACKET_STATE_SENDING && rxPacketQueue.startPtr != rxPacketQueue.endPtr)
	{
		rx_packet_state = RX_PACKET_STATE_SENDING;

		data_rx_buffer_current_location = rxPacketQueue.startPtr;

		if (data_rx_buffer_current_location + sizeof(RNDIS_Packet_Message_t) - 1 >= &rxPacketQueue.buffer[BUFFER_LENGTH])
		{
			data_rx_buffer_current_location = rxPacketQueue.buffer;
		}

		pRNDISHeader = (RNDIS_Packet_Message_t*) data_rx_buffer_current_location;
		data_rx_buffer_remaining_length = pRNDISHeader->MessageLength;

		//DBG("[DATA][IN] RX_STATE: SENDING");

		//DBG("[DATA][IN] Send %u", data_rx_buffer_remaining_length);
	}

	if (rx_packet_state == RX_PACKET_STATE_SENDING)
	{

		len = MIN(64, data_rx_buffer_remaining_length);
		len = MIN(len, &rxPacketQueue.buffer[BUFFER_LENGTH] - data_rx_buffer_current_location);
		
		usbWrite(USB_EP_IN | 0x02, data_rx_buffer_current_location, len);
		data_rx_buffer_current_location += len;
		data_rx_buffer_remaining_length -= len;

		if (data_rx_buffer_current_location >= &rxPacketQueue.buffer[BUFFER_LENGTH])
		{
			data_rx_buffer_current_location = rxPacketQueue.buffer;
		}

		//DBG("[DATA][IN] %u left", data_rx_buffer_remaining_length);

		// This is the last chunk
		if (data_rx_buffer_remaining_length == 0)
		{
			//DBG("[DATA][IN] Completed");

			rxPacketQueue.startPtr = data_rx_buffer_current_location;

			rx_packet_state = RX_PACKET_STATE_READY;
			eth_nak_interrupts &= ~INACK_BI;
			usbEnableNAKInterrupts(eth_nak_interrupts);

			//DBG("[DATA][IN] RX_STATE: EMPTY");

		}
	}
	

	led_off(EP2_IN_LED);

}


/*******************************************************/
/* RNDIS Bus Reset                                     */
/*******************************************************/
static void usb_device_status_handler(uint8_t dev_status) 
{
	if (dev_status & DEV_STATUS_CONNECT)
	{
		DBG("############ DEVICE CONNECT ############");
	}

	if (dev_status & DEV_STATUS_RESET)
	{
		DBG("############ DEVICE RESET ############");
		rndis_reset();
	}

	if (dev_status & DEV_STATUS_SUSPEND)
	{
		DBG("############ DEVICE SUSPEND ############");
	}
}

void rndis_reset(void) 
{
	data_tx_buffer_current_location = data_tx_buffer;
	//data_rx_buffer_current_location = data_rx_buffer;

	tx_packet_state = TX_PACKET_STATE_EMPTY;
	rx_packet_state = RX_PACKET_STATE_EMPTY;

	eth_nak_interrupts = 0;
	usbEnableNAKInterrupts(eth_nak_interrupts);

	rndis_state = RNDIS_STATE_UNINITIALIZED;
}
