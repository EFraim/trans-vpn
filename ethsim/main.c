#include <stdint.h>
#include <string.h>

#define MAC_ADDRESS {0x00, 0x18, 0x0A, 0x01, 0xDF, 0x44}

uint8_t MACAddress[6] = MAC_ADDRESS;

#include "log.h"

#include "usbnet.h"
#include "drivers/enc28j60.h"

#include "app.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: ./ethapp <eth-port> <host-port>\n");
        return 1;
    }
    
    LOG_INFO("Initializing USB Stack");
    usbnet_init(argv[2]);
    
	LOG_INFO("Initializing Ethernet stack");
	enc28j60_init(0, 0, MACAddress, argv[1]);
	
	// Print MAC address
    LOG_INFO("MAC address: %X-%X-%X-%X-%X-%X\n", MACAddress[0], MACAddress[1], MACAddress[2], MACAddress[3], MACAddress[4], MACAddress[5]);

	LOG_INFO("Starting USB Stack");

    
    LOG_INFO("Entering main loop");
    
    app_loop();
	return 0;
}

