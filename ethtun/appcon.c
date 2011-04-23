#include "appcon.h"

#include "log.h"
#include "drivers/usb.h"
#include "usbcon.h"
#include "vpnConfig.h"

extern int CONFIG_LOCATION;
extern int CONFIG_SECTOR;

void appcon_loop() {
  const int SECTOR = (int)&CONFIG_SECTOR;
  LOG_INFO("Config address is %X, configuration starts at %X, sector %d\n", (int)&CONFIG, (int)&CONFIG_LOCATION, SECTOR);
    for(;;) {
      LOG_DEBUG("I am still alive!\n");
    }
}
