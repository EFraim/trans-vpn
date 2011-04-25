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

const cmd_t commands[] = {
  { .name = "help", .descr = "Describe available commands", .type=NOUN, .act = { .noun = { .exec = cmd_help } } },
  { .name = "recall", .descr = "Restore saved settings", .type=NOUN, .act = { .noun = { .exec = cmd_recall } } },
  { .name = "save", .descr = "Save current settings", .type=NOUN, .act = { .noun = { .exec = cmd_save } } }
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
    for(unsigned i=0; i<_countof(commands); i++)
      if(strcmp(param, commands[i].name) == 0) {
	strcpy(reply, commands[i].descr);
	reply = rawmemchr(reply, 0);
	strcpy(reply, "\n");
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
