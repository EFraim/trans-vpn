#include <stdint.h>
#include <stdbool.h>
#include "iap.h"
#include "../lpc2000/interrupt.h"
#include "../log.h"


static unsigned int command[5];
static unsigned int result[2];
typedef void (*IAP)(unsigned int [],unsigned int[]);
static IAP iap_entry =(IAP)0x7ffffff1;
static const int CCLK_KHz=60000;

enum Commands { PREPARE_SECTOR_FOR_WRITE=50, WRITE_SECTOR=51, ERASE_SECTOR=52 };
enum ReturnCodes { CMD_SUCCESS=0 };

static bool prepareForWrite(int sectorNum) {
  interrupts_state_t oldState = interruptsSaveAndDisable();
  command[0] = PREPARE_SECTOR_FOR_WRITE;
  command[1] = command[2] = sectorNum;
  iap_entry(command, result);
  interruptsRestore(oldState);
  LOG_INFO("Prepare for write %d", result[0]);
  return result[0] == CMD_SUCCESS;
}

#define PROC_START bool success=true;
#define CLEANUP err:
#define FAIL do { success = false; goto err; } while(0)

bool writeSector(void *targetAddress, int sectorNum, const void *source, unsigned byteCount) {
  interrupts_state_t oldState = interruptsSaveAndDisable();
  PROC_START
  if(!prepareForWrite(sectorNum))
    FAIL;
  command[0] = WRITE_SECTOR;
  command[1] = (int)targetAddress;
  command[2] = (int)source;
  command[3] = byteCount;
  command[4] = CCLK_KHz;
  iap_entry(command,result);
  if(result[0] != CMD_SUCCESS)
    FAIL;
  
  CLEANUP
  interruptsRestore(oldState);
  LOG_INFO("Write %d", result[0]);
  return success;
}

bool eraseSector(int sectorNum) {
  interrupts_state_t oldState = interruptsSaveAndDisable();
  PROC_START
  if(!prepareForWrite(sectorNum))
    FAIL;
  command[0] = ERASE_SECTOR;
  command[1] = command[2] = sectorNum;
  command[3] = CCLK_KHz;
  iap_entry(command,result);
  if(result[0] != CMD_SUCCESS)
    FAIL;

  CLEANUP
  interruptsRestore(oldState);
  LOG_INFO("Erase %d", result[0]);
  return success;
}

