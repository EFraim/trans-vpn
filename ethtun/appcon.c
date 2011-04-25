#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
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

}

static void mac_edit(void* val, char* reply, const char* param, const char* cmdName) {

}

static void str_edit(void* val, char* reply, const char* param, const char* cmdName) {

}

static void key_edit(void* val, char* reply, const char* param, const char* cmdName) {

}

static void ipconfenum_edit(void* val, char* reply, const char* param, const char* cmdName) {

}

static void uint_edit(void* val, char* reply, const char* param, const char* cmdName) {

}

const cmd_t commands[] = {
  { .name = "help", .descr = "Describe available commands", .type=NOUN, .act = { .noun = { .exec = cmd_help } } },
  { .name = "recall", .descr = "Restore saved settings", .type=NOUN, .act = { .noun = { .exec = cmd_recall } } },
  { .name = "save", .descr = "Save current settings", .type=NOUN, .act = { .noun = { .exec = cmd_save } } },
  { .name = "guestip", .descr = "The way to configure IP networking on guest network", .type=GETSET, .act={.getset={.exec=ipconfenum_edit, .val=&CONFIG.HostileNetAddrConfWay }}},
  { .name = "staddr", .descr = "Static IP address on guest network", .type=GETSET, .act={.getset={.exec=ip_edit, .val=&CONFIG.hostileNetStaticConfig.addr }}},
  { .name = "stmask", .descr = "Static IP mask on guest network", .type=GETSET, .act={.getset={.exec=ip_edit, .val=&CONFIG.hostileNetStaticConfig.mask }}},
  { .name = "stgw", .descr = "Static gateway on guest network", .type=GETSET, .act={.getset={.exec=ip_edit, .val=&CONFIG.hostileNetStaticConfig.defGateway }}},
  { .name = "stdns1", .descr = "DNS1 static config on guest network", .type=GETSET, .act={.getset={.exec=ip_edit, .val=&CONFIG.hostileNetStaticConfig.dns1 }}},
  { .name = "stdns2", .descr = "DNS2 static config on guest network", .type=GETSET, .act={.getset={.exec=ip_edit, .val=&CONFIG.hostileNetStaticConfig.dns2 }}},
  { .name = "vpnhost", .descr = "VPN gateway hostname or IP address", .type=GETSET, .act={.getset={.exec=str_edit, .val=&CONFIG.vpnHostOrIp }}},
  { .name = "vpnport", .descr = "VPN gateway port", .type=GETSET, .act={.getset={.exec=uint_edit, .val=&CONFIG.vpnPort }}},
  { .name = "servpubkey", .descr = "Server public key", .type=GETSET, .act={.getset={.exec=key_edit, .val=&CONFIG.serverPublicKey }}},
  { .name = "clientpubkey", .descr = "Client public key", .type=GETSET, .act={.getset={.exec=key_edit, .val=&CONFIG.clientPublicKey }}},
  { .name = "clientprivkey", .descr = "Client private key", .type=GETSET, .act={.getset={.exec=key_edit, .val=&CONFIG.clientPrivateKey }}},
  { .name = "vpnmac", .descr = "MAC address on virtualized network", .type=GETSET, .act={.getset={.exec=mac_edit, .val=&CONFIG.vpnMAC }}},
  { .name = "physmac", .descr = "MAC address on guest network", .type=GETSET, .act={.getset={.exec=mac_edit, .val=&CONFIG.physMAC }}}
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
  printf(cmd);
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
      case GETSET: commands[i].act.getset.exec((char*)liveCopy+((char*)commands[i].act.getset.val-(char*)&CONFIG),
					       reply, cmd+paramStart, commands[i].name);
	return;
      }
    }
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
