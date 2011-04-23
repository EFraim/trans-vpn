#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "appcon.h"

#include "log.h"
#include "drivers/usb.h"
#include "drivers/iap.h"
#include "usbcon.h"
#include "vpnConfig.h"

extern int CONFIG_LOCATION;
extern int CONFIG_SECTOR;

char configArea[512] __attribute__((aligned(64)));

vpnConfig* liveCopy;

static bool recall() {
  memcpy(&configArea, &CONFIG_SECTOR, sizeof(configArea));
  return true;
}

static bool save() {
  const int SECTOR = (int)&CONFIG_SECTOR;
  return eraseSector(SECTOR)
    && writeSector(&CONFIG_LOCATION, SECTOR, configArea, sizeof(configArea));
}

void appcon_loop() {
  const int SECTOR = (int)&CONFIG_SECTOR;
  liveCopy = (vpnConfig*)(configArea + ((int)&CONFIG-(int)CONFIG_LOCATION));
  recall();
  LOG_INFO("Config address is %X, configuration starts at %X, sector %d\n", (int)&CONFIG, (int)&CONFIG_LOCATION, SECTOR);
  for(;;) {
    LOG_DEBUG("I am still alive!\n");
  }
}
