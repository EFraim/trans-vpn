#include <stdint.h>
#include <string.h>
#include <signal.h>

#define MAC_ADDRESS {0x00, 0x18, 0x0A, 0x01, 0xDF, 0x44}

uint8_t MACAddress[6] = MAC_ADDRESS;

#include "log.h"

#include "usbnet.h"
#include "drivers/enc28j60.h"

#include "app.h"
#include "util.h"

void sigint_handler(int s) {
    LOG_DEBUG("Caught SIGINT, exiting...");
    exit_application = 1;
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        printf("Usage: ./ethapp <eth-port-my> <eth-port-other> <usb-port-my> <usb-port-other>\n");
        return 1;
    }
    
    LOG_INFO("Initializing USB Stack");
    usbnet_init(argv[3], argv[4]);
    
	LOG_INFO("Initializing Ethernet stack");
	enc28j60_init(0, 0, MACAddress, argv[1], argv[2]);
	
	LOG_INFO("Starting USB Stack");
    
    LOG_INFO("Entering main loop");
    
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);
    
    app_loop();
	return 0;
}
