#include "vpnConfig.h"

//This should NOT be initialized. It is a configuration which is not overwritten by firmware update. Thus it should be kept in bss section;

#ifndef ETHVPN_HARD_CONFIG
const vpn_config_t CONFIG;
#endif
