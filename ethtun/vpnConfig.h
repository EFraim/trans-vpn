#ifndef VPNCONFIG_H
#define VPNCONFIG_H
#include <uip/uip.h>

#define MAX_DNS_NAME 127
#define KEY_BYTE_LEN 64
typedef struct vpn_config_t {
  enum ipconf_t { DHCP, STATIC } HostileNetAddrConfWay;
  struct {
    uip_ipaddr_t addr, mask, defGateway, dns1, dns2;
  } hostileNetStaticConfig;

  char vpnHostOrIp[MAX_DNS_NAME+1];
  uint16_t vpnPort;

  unsigned char serverPublicKey[KEY_BYTE_LEN];
  unsigned char clientPrivateKey[KEY_BYTE_LEN];
  unsigned char clientPublicKey[KEY_BYTE_LEN];
  unsigned char vpnMAC[6], physMAC[6];
} vpn_config_t;

extern vpn_config_t CONFIG;

#endif

