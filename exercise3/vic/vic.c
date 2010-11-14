typedef void (*interrupt_handler_t)(void);

#if    defined(LPC2119)
typedef enum {
  INT_SOURCE_WDT    = 0,
  INT_SOURCE_SWI    = 1,
  INT_SOURCE_CORE1  = 2,
  INT_SOURCE_CORE2  = 3,
  INT_SOURCE_TIMER0 = 4,
  INT_SOURCE_TIMER1 = 5,
  INT_SOURCE_UART0  = 6,
  INT_SOURCE_UART1  = 7,
  INT_SOURCE_PWM0   = 8,
  INT_SOURCE_I2C    = 9,
  INT_SOURCE_SPI0   = 10,
  INT_SOURCE_SPI1   = 11,
  INT_SOURCE_PLL    = 12,
  INT_SOURCE_RTC    = 13,
  INT_SOURCE_EINT0  = 14,
  INT_SOURCE_EINT1  = 15,
  INT_SOURCE_EINT2  = 16,
  INT_SOURCE_EINT3  = 17,
  INT_SOURCE_ADC    = 18,
  INT_SOURCE_CANACC = 19,
  INT_SOURCE_CAN1TX = 20,
  INT_SOURCE_CAN2TX = 21,
  INT_SOURCE_CAN3TX = 22,
  INT_SOURCE_CAN4TX = 23,
  INT_SOURCE_CAN1RX = 26,
  INT_SOURCE_CAN2RX = 27,
  INT_SOURCE_CAN3RX = 28,
  INT_SOURCE_CAN4RX = 29
} vic_source_t;
#endif

#if    defined(LPC2148)
typedef enum {
  INT_SOURCE_WDT    = 0,
  INT_SOURCE_SWI    = 1,
  INT_SOURCE_CORE1  = 2,
  INT_SOURCE_CORE2  = 3,
  INT_SOURCE_TIMER0 = 4,
  INT_SOURCE_TIMER1 = 5,
  INT_SOURCE_UART0  = 6,
  INT_SOURCE_UART1  = 7,
  INT_SOURCE_PWM0   = 8,
  INT_SOURCE_I2C0   = 9,
  INT_SOURCE_SPI0   = 10,
  INT_SOURCE_SPI1   = 11,
  INT_SOURCE_PLL    = 12,
  INT_SOURCE_RTC    = 13,
  INT_SOURCE_EINT0  = 14,
  INT_SOURCE_EINT1  = 15,
  INT_SOURCE_EINT2  = 16,
  INT_SOURCE_EINT3  = 17,
  INT_SOURCE_ADC    = 18,
  INT_SOURCE_I2C1   = 19,
  INT_SOURCE_BOD    = 20,
  INT_SOURCE_ADC1   = 21,
  INT_SOURCE_USB    = 22
} vic_source_t;
#endif

#define vicUpdatePriority() (VICVectAddr=0xff)

typedef uint32_t interrupt_source_state_t;

static void __attribute__ ((interrupt("IRQ")))
  _vicDefaultIsr(void) {
  vicUpdatePriority();
}

static void vicInit(void) {
  VICDefVectAddr = (uint32_t) _vicDefaultIsr;
}

static volatile uint32_t* address_register = &VICVectAddr0;
static volatile uint32_t* control_register = &VICVectCntl0;

static void vicEnableFIQ(vic_source_t c) {
  VICIntSelect |= (1 << c); /* generate FIQ interrupt */
  VICIntEnable |= (1 << c);
}

static void vicEnableIRQ(vic_source_t c,
                  uint32_t            priority,
                  interrupt_handler_t h) {
  address_register[priority] = (uint32_t) h;
  control_register[priority] = c | (1<<5);
  VICIntSelect &= ~(1 << c); /* generate IRQ interrupt */
  VICIntEnable = (1 << c);
}

static void vicEnableDefaultIRQ(interrupt_handler_t h) {
  VICDefVectAddr = (uint32_t) h;
}

static inline void vicEnable(vic_source_t c) {
  VICIntEnable = (1 << c);
}

static inline void vicDisable(vic_source_t c) {
  VICIntEnClr = (1 << c);
}

static inline interrupt_source_state_t vicSaveAndDisable(vic_source_t c) {
  interrupts_state_t is = interruptsSaveAndDisable();
  interrupt_source_state_t vs = VICIntEnable;
  VICIntEnClr = 0; // to make sure the next write has an effect; see manual
  VICIntEnClr = (1 << c);
  interruptsRestore(is);
  return vs;
}

static inline void vicRestore(interrupt_source_state_t s) {
  VICIntEnable = s;
}

static inline void vicSoftSignal(vic_source_t c) {
  VICSoftInt = (1 << c);
}

static inline void vicSoftClear(vic_source_t c) {
  VICSoftIntClr = (1 << c);
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
