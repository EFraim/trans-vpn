static const int MAX_DNS_NAME 255
static const int KEY_BYTE_LEN 64
typedef struct {
  enum { DHCP, STATIC } HostileNetAddrConfWay;
  struct {
    uip_ipaddr_t addr, mask, defGateway, dns1, dns2;
  };

  enum { DNS, HARDWIRED } VpnServerWay;
  union {
    char hostName[MAX_DNS_NAME+1];
    uip_ipaddr_t ipAddr;
  };

  unsigned char serverPublicKey[KEY_BYTE_LEN];
  unsigned char clientPrivateKey[KEY_BYTE_LEN];
  unsigned char clientPublicKey[KEY_BYTE_LEN];
} vpnConfig;

extern vpnConfig CONFIG;
