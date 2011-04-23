#ifndef VPNCONFIG_H
#define VPNCONFIG_H
#include <uip/uip.h>

#define MAX_DNS_NAME 255
#define KEY_BYTE_LEN 64
typedef struct {
  enum { DHCP, STATIC } HostileNetAddrConfWay;
  struct {
    uip_ipaddr_t addr, mask, defGateway, dns1, dns2;
  } hostileNetStaticConfig;

  enum { DNS, HARDWIRED } VpnServerWay;
  char vpnHostName[MAX_DNS_NAME+1];
  uip_ipaddr_t vpnIpAddr;

  unsigned char serverPublicKey[KEY_BYTE_LEN];
  unsigned char clientPrivateKey[KEY_BYTE_LEN];
  unsigned char clientPublicKey[KEY_BYTE_LEN];
} vpnConfig;

extern vpnConfig CONFIG;

#endif

