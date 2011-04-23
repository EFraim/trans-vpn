#include <stdint.h>
#include <stdbool.h>
#include "iap.h"
#include "../lpc2000/interrupt.h"


static unsigned int command[5];
static unsigned int result[2];
typedef void (*IAP)(unsigned int [],unsigned int[]);
static IAP iap_entry =(IAP)0x7ffffff1;
static const int CCLK_KHz=60000;

enum Commands { PREPARE_SECTOR_FOR_WRITE=5010, WRITE_SECTOR=5110, ERASE_SECTOR=5210 };
enum ReturnCodes { CMD_SUCCESS=0 };

static bool prepareForWrite(int sectorNum) {
  command[0] = PREPARE_SECTOR_FOR_WRITE;
  command[1] = command[2] = sectorNum;
  iap_entry(command, result);
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
  
  CLEANUP
  interruptsRestore(oldState);
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
  return success;
}

