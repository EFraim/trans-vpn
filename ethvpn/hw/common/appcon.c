#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "appcon.h"

#include "log.h"
#include "drivers/usb.h"
#include "drivers/iap.h"
#include "usbcon.h"
#include "vpnConfig.h"

extern int CONFIG_LOCATION;
extern int CONFIG_SECTOR;

char configArea[512] __attribute__((aligned(64)));

static vpn_config_t* liveCopy;

static char* rawmemchr(char* str, char x) { while(*str != x) str++; return str; }

static void cmd_help(char* reply, const char* param);
static void cmd_save(char* reply, const char* param);
static void cmd_recall(char* reply, const char* param);

static bool recall() {
  memcpy(&configArea, &CONFIG_SECTOR, sizeof(configArea));
  return true;
}

static bool save() {
  const int SECTOR = (int)&CONFIG_SECTOR;
  return eraseSector(SECTOR)
    && writeSector(&CONFIG_LOCATION, SECTOR, configArea, sizeof(configArea));
}

typedef struct cmd_t {
  const char* name;
  const char* descr;
  enum type_t { NOUN, GETSET } type;
  union {
    struct getset_t {
      void (*exec)(void* val, char* reply, const char* param, const char* cmdName);
      void *val;
    } getset;
    struct noun_t {
      void (*exec)(char* reply, const char* param);
    } noun;
  } act;
} cmd_t;

static void ip_edit(void* val, char* reply, const char* param, const char* cmdName) {
  u16_t *conf = (u16_t*)val;
  u16_t newVal[4];
  LOG_INFO("IP address is %x", val);
  LOG_INFO("Conf on stack addr is %x", &conf);
  for(int i=0; i<4; i++) {
    char *endPtr;
    newVal[i] = strtoul(param, &endPtr, 10);
    LOG_INFO("IP component %d\n", newVal[i]);
    if(endPtr == param || *endPtr != (i < 3 ? '.' : 0)) {
      sprintf(reply, "%s %d.%d.%d.%d\n", cmdName, uip_ipaddr1(conf), uip_ipaddr2(conf), uip_ipaddr3(conf), uip_ipaddr4(conf));
      return;
    }
    param = endPtr+1;
  }
  LOG_INFO("%u.%u.%u.%u", newVal[0], newVal[1], newVal[2], newVal[3]);
  uip_ipaddr(conf, newVal[0], newVal[1], newVal[2], newVal[3]);
  //LOG_INFO("%u.%u.%u.%u", uip_ipaddr1(conf), uip_ipaddr2(conf), uip_ipaddr3(conf), uip_ipaddr4(conf));
  //sprintf(reply, "%s %d.%d.%d.%d\n", cmdName, uip_ipaddr1(conf), uip_ipaddr2(conf), uip_ipaddr3(conf), uip_ipaddr4(conf));
  strcpy(reply, "OK\n");
}

static void str_edit(void* val, char* reply, const char* param, const char* cmdName) {
  LOG_INFO("str_edit %X", val);
  char *conf = val;
  conf[MAX_DNS_NAME] = 0;
  if(*param) {
    strcpy(conf, param);
    strcpy(reply, "OK\n");
  } else
    sprintf(reply, "%s %s\n", cmdName, conf);
}

static uint8_t readHexByte(const char* str) {
  return (tolower(str[0] >= 'a' ? str[0]-'a'+0xA : str[0]-'0'+0) << 4) |
    (tolower(str[1] >= 'a' ? str[1]-'a'+0xA : str[1]-'0'+0));
}

static void writeHexByte(char* str, uint8_t x) {
  str[0] = (x >> 4) >= 0xA ? 'a' + (x >> 4) - 0xA : '0' + (x >> 4);
  str[1] = (x & 0xF) >= 0xA ? 'a' + (x & 0xF) - 0xA : '0' + (x & 0xF);
}

static void key_edit(void* val, char* reply, const char* param, const char* cmdName) {
  uint8_t *conf=val;
  while(strlen(param) == KEY_BYTE_LEN*2) {
    int i;
    for(i=0; i<KEY_BYTE_LEN*2; i++)
      if(!isxdigit(param[i]))
	break;
    if(i < KEY_BYTE_LEN*2)
      break;
    for(int i=0; i<KEY_BYTE_LEN; i++)
      conf[i] = readHexByte(param+2*i);
    strcpy(reply, "OK\n");
    return;
  }
  int chars = sprintf(reply, "%s ", cmdName);
  reply+=chars;
  for(int i=0; i<KEY_BYTE_LEN; i++, reply+=2)
    writeHexByte(reply, conf[i]);
  *(reply++) = '\n';
  *(reply++) = 0;
}


static void mac_edit(void* val, char* reply, const char* param, const char* cmdName) {
  uint8_t *conf=val;
  if(strlen(param) != 17 || !isxdigit(param[0]) || !isxdigit(param[1]) || param[2] != ':'
     || !isxdigit(param[3]) || !isxdigit(param[4]) || param[5] != ':'
     || !isxdigit(param[6]) || !isxdigit(param[7]) || param[8] != ':'
     || !isxdigit(param[9]) || !isxdigit(param[10]) || param[11] != ':'
     || !isxdigit(param[12]) || !isxdigit(param[13]) || param[14] != ':'
     || !isxdigit(param[15]) || !isxdigit(param[16]))
    sprintf(reply, "%s %02x:%02x:%02x:%02x:%02x:%02x\n", cmdName, conf[0], conf[1], conf[2], conf[3], conf[4], conf[5]);
  else {
    for(int i=0; i<6; i++)
      conf[i] = readHexByte(param+3*i);
    strcpy(reply, "OK\n");
  }
}

static void ipconfenum_edit(void* val, char* reply, const char* param, const char* cmdName) {
  enum ipconf_t *conf=val;
  if(strcmp(param, "dhcp") == 0) {
    *conf = ADDR_DHCP;
    strcpy(reply, "OK\n");
  } else if(strcmp(param, "static") == 0) {
    *conf = ADDR_STATIC;
    strcpy(reply, "OK\n");
  } else
    sprintf(reply, "%s %s\n", cmdName, *conf == ADDR_DHCP ? "dhcp" : (*conf == ADDR_STATIC ? "static" : "invalid"));
}

static void uint_edit(void* val, char* reply, const char* param, const char* cmdName) {
  uint16_t *conf=val;
  char *endPtr;
  unsigned long res = strtoul(param, &endPtr, 10);
  LOG_INFO("Val %d\n", res);
  if(endPtr == param || *endPtr != 0)
    sprintf(reply, "%s %u\n", cmdName, (unsigned)*conf);
  else {
    *conf = res;
    strcpy(reply, "OK\n");
  }
}

const cmd_t commands[] = {
  { .name = "help", .descr = "Describe available commands", .type=NOUN, .act = { .noun = { .exec = cmd_help } } },
  { .name = "recall", .descr = "Restore saved settings", .type=NOUN, .act = { .noun = { .exec = cmd_recall } } },
  { .name = "save", .descr = "Save current settings", .type=NOUN, .act = { .noun = { .exec = cmd_save } } },
  { .name = "guestip", .descr = "The way to configure IP networking on guest network (static/dhcp)", .type=GETSET, .act={.getset={.exec=ipconfenum_edit, .val=&CONFIG.HostileNetAddrConfWay }}},
  { .name = "staddr", .descr = "Static IP address on guest network", .type=GETSET, .act={.getset={.exec=ip_edit, .val=CONFIG.hostileNetStaticConfig.addr }}},
  { .name = "stmask", .descr = "Static IP mask on guest network", .type=GETSET, .act={.getset={.exec=ip_edit, .val=CONFIG.hostileNetStaticConfig.mask }}},
  { .name = "stgw", .descr = "Static gateway on guest network", .type=GETSET, .act={.getset={.exec=ip_edit, .val=CONFIG.hostileNetStaticConfig.defGateway }}},
  { .name = "stdns", .descr = "DNS static config on guest network", .type=GETSET, .act={.getset={.exec=ip_edit, .val=CONFIG.hostileNetStaticConfig.dns }}},
  { .name = "vpnhost", .descr = "VPN gateway hostname or IP address", .type=GETSET, .act={.getset={.exec=str_edit, .val=CONFIG.vpnHostOrIp }}},
  { .name = "vpnport", .descr = "VPN gateway port", .type=GETSET, .act={.getset={.exec=uint_edit, .val=&CONFIG.vpnPort }}},
  { .name = "servpubkey", .descr = "Server public key", .type=GETSET, .act={.getset={.exec=key_edit, .val=CONFIG.serverPublicKey }}},
  { .name = "clientpubkey", .descr = "Client public key", .type=GETSET, .act={.getset={.exec=key_edit, .val=CONFIG.clientPublicKey }}},
  { .name = "clientprivkey", .descr = "Client private key", .type=GETSET, .act={.getset={.exec=key_edit, .val=CONFIG.clientPrivateKey }}},
  { .name = "vpnmac", .descr = "MAC address on virtualized network", .type=GETSET, .act={.getset={.exec=mac_edit, .val=CONFIG.vpnMAC }}},
  { .name = "physmac", .descr = "MAC address on guest network", .type=GETSET, .act={.getset={.exec=mac_edit, .val=CONFIG.physMAC }}}
};

#define _countof(a) (sizeof(a)/sizeof(*(a)))
static void cmd_help(char* reply, const char* param) {
  if(!*param) {
    strcpy(reply, "Commands: ");
    reply = rawmemchr(reply, 0);
    for(unsigned i=0; i<_countof(commands); i++) {
      strcpy(reply, commands[i].name);
      reply = rawmemchr(reply, 0);
      strcpy(reply, i < _countof(commands)-1 ? ", " : "\n");
      reply = rawmemchr(reply, 0);
    }
  }
  else
    {
      for(unsigned i=0; i<_countof(commands); i++)
	if(strcmp(param, commands[i].name) == 0) {
	  strcpy(reply, commands[i].descr);
	  reply = rawmemchr(reply, 0);
	  strcpy(reply, "\n");
	  return;
	}
      cmd_help(reply, "");
    }
}

static void cmd_recall(char* reply, const char* param) {
  recall();
  strcpy(reply, "OK\n");
}

static void cmd_save(char* reply, const char* param) {
  save();
  strcpy(reply, "OK\n");
}

static void execute_cmd(char* cmd, char* reply) {
  int nameEnd=0;
  //printf(cmd);
  while(isalpha(cmd[nameEnd]))
    nameEnd++;
  int paramStart=nameEnd;
  while(isspace(cmd[paramStart]))
    paramStart++;
  int paramEnd=paramStart;
  while(isprint(cmd[paramEnd]) && !isspace(cmd[paramEnd]))
    paramEnd++;
  cmd[paramEnd] = 0;
  for(int i=0; i<_countof(commands); i++)
    if(strncmp(cmd, commands[i].name, nameEnd) == 0 && strlen(commands[i].name) == nameEnd) {
      switch(commands[i].type) {
      case NOUN: commands[i].act.noun.exec(reply, cmd+paramStart); return;
      case GETSET: LOG_INFO("%x %x %x %x", (char*)commands[i].act.getset.val, (char*)&CONFIG, ((char*)commands[i].act.getset.val-(char*)&CONFIG), (char*)liveCopy+((char*)commands[i].act.getset.val-(char*)&CONFIG));
	commands[i].act.getset.exec((char*)liveCopy+((char*)commands[i].act.getset.val-(char*)&CONFIG),
					       reply, cmd+paramStart, commands[i].name);
	return;
      }
    }
  strcpy(reply, "BAD COMMAND OR FILE NAME\n");
}

void appcon_loop() {
  const int SECTOR = (int)&CONFIG_SECTOR;
  liveCopy = (vpn_config_t*)(configArea + ((int)&CONFIG-(int)&CONFIG_LOCATION));
  
  recall();
  LOG_INFO("Config address is %X, configuration starts at %X, sector %d, liveCopy %X\n", (int)&CONFIG, (int)&CONFIG_LOCATION, SECTOR, (int)liveCopy);
  char cmd[MAX_CMD_LEN], reply[MAX_CMD_LEN] = "TransVPN configuration console 1.0.0; Enter help<RET> for syntax\n";
  for(;;) {
    LOG_INFO("Command processing started!\n");
    usbcon_send_response_await_query(reply, cmd);
    LOG_INFO("Command execution started!\n");
    execute_cmd(cmd, reply);
  }
}
