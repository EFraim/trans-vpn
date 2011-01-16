#include "vic.h"

#include <lpc2000/io.h>

static void __attribute__ ((interrupt("IRQ")))
  _vicDefaultIsr(void) {
  vicUpdatePriority();
}

void vicInit(void) {
  VICDefVectAddr = (uint32_t) _vicDefaultIsr;
}

static volatile uint32_t* address_register = &VICVectAddr0;
static volatile uint32_t* control_register = &VICVectCntl0;

void vicEnableFIQ(interrupt_channel_t c) {
  VICIntSelect |= (1 << c); /* generate FIQ interrupt */
  VICIntEnable |= (1 << c);
}

void vicEnableIRQ(interrupt_channel_t c, 
                  uint32_t            priority,
                  interrupt_handler_t h) {
  address_register[priority] = (uint32_t) h;
  control_register[priority] = c | (1<<5);
  VICIntSelect &= ~(1 << c); /* generate IRQ interrupt */
  VICIntEnable = (1 << c);
}

void vicEnableDefaultIRQ(interrupt_handler_t h) {
  VICDefVectAddr = (uint32_t) h;
}

static void ramvectorsInit(void) {
  uint32_t i;
  uint32_t* src = (uint32_t*) 0x40000200;
  uint32_t* dst = (uint32_t*) 0x40000000;
  
  for (i=0; i<16; i++) {
    *dst = *src;
    src  ++;
    dst++;
  }
  
  MEMMAP = BIT1;
}
