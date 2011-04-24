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

vpn_config_t* liveCopy;

static bool recall() {
  memcpy(&configArea, &CONFIG_SECTOR, sizeof(configArea));
  return true;
}

static bool save() {
  const int SECTOR = (int)&CONFIG_SECTOR;
  return eraseSector(SECTOR)
    && writeSector(&CONFIG_LOCATION, SECTOR, configArea, sizeof(configArea));
}

static void execute_cmd(const char* cmd, char* reply) {
  strcpy(reply, "BAD COMMAND OR FILE NAME\n");
}

void appcon_loop() {
  const int SECTOR = (int)&CONFIG_SECTOR;
  liveCopy = (vpn_config_t*)(configArea + ((int)&CONFIG-(int)CONFIG_LOCATION));
  recall();
  LOG_INFO("Config address is %X, configuration starts at %X, sector %d\n", (int)&CONFIG, (int)&CONFIG_LOCATION, SECTOR);

  char cmd[MAX_CMD_LEN], reply[MAX_CMD_LEN] = "TransVPN configuration console 1.0.0; Enter help<RET> for syntax\n";
  for(;;) {
    LOG_INFO("Command processing started!\n");
    usbcon_send_response_await_query(reply, cmd);
    execute_cmd(cmd, reply);
  }
}
