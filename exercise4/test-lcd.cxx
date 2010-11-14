#include <stdint.h>
#include <lpc214x.c>
#include <interrupt.c>

static void wait_at_least(int interval_ns) {
  int nsPerClock = 1000/60;
  for(volatile int left_ns = interval_ns; left_ns >= 0; left_ns -= nsPerClock)
    ;
}

class MSBusHAL {
public:
  
  inline static void init() {
    IODIR0 |= BIT30 | BIT22;
    IODIR1 |= BIT24 | BIT25;
    IOPIN0 = IOPIN0 & ~(BIT30 | BIT22);
    IOPIN1 = IOPIN1 & ~(BIT24 | BIT25);
  }
  inline static void backlight_state(bool state) {	(state ? IOSET0 : IOCLR0) = BIT30;	}
  inline static void rw_set_read() {			IOSET0 = BIT22;	}
  inline static void rw_set_write() {			IOCLR0 = BIT22;	}
  inline static void rs_set_memory() {			IOSET1 = BIT24;	}
  inline static void rs_set_instruction() {		IOCLR1 = BIT24; }
  inline static void en_set() {				IOSET1 = BIT25; }
  inline static void en_clear() {			IOCLR1 = BIT25; }
  inline static void data8_set(uint8_t data) {		
    IODIR1 |= 0xFF0000;
    IOPIN1 = (IOPIN1 & ~(0xFF0000)) | ((uint32_t)data << 16);
  }
  inline static uint8_t data8_get() {			
    IODIR1 &= ~0xFF0000;
    return IOPIN1 >> 16;
  }
};

class ParametricCmd {
public:
  operator uint8_t&() { return reinterpret_cast<uint8_t&>(*this); }
  operator const uint8_t&() const { return reinterpret_cast<const uint8_t&>(*this); }
};

class FUNCTION_CMD : public ParametricCmd {
  uint8_t Reserved:2;
  uint8_t CharacterFont:1;
  uint8_t DisplayLines:1;
  uint8_t WideBus:1;
  uint8_t cmd:3;
public:
  FUNCTION_CMD(bool WideBus, bool DisplayLines, bool CharacterFont) : cmd(1), WideBus(WideBus), DisplayLines(DisplayLines), CharacterFont(CharacterFont), Reserved(0) {}
};

class DISPLAY_CTRL_CMD : public ParametricCmd {
  uint8_t Blinking:1, Cursor:1, Display:1;
  uint8_t cmd:5;
public:
  DISPLAY_CTRL_CMD(bool Display, bool Cursor, bool Blinking) : cmd(1), Display(Display), Cursor(Cursor), Blinking(Blinking) {}
};

class DDR_ADDR_CMD : public ParametricCmd {
  uint8_t addr:7;
  uint8_t cmd:1;
public:
  DDR_ADDR_CMD(uint8_t addr) : cmd(1), addr(addr) {}
};

const uint8_t CLEAR_CMD=0x1;
const uint8_t ENTRY_CMD=0x6;

template<class HAL, bool halfByte=false> class CommandLayer
{
  enum AccessType { INSTRUCTION, MEMORY };
  static const uint8_t BUSY_FLAG = 1 << 7;
  static const int t_CYCLE = 500, t_AH = 10, t_AS = 40, t_PW = 230, t_DDR = 160;

  inline static void send_byte(AccessType access, uint8_t val, bool half_byte) {
    if(access == INSTRUCTION)
      HAL::rs_set_instruction();
    else
      HAL::rs_set_memory();
    HAL::rw_set_write();
    HAL::data8_set(val);
    wait_at_least(t_AS);
    HAL::en_set();
    wait_at_least(t_PW);
    HAL::en_clear();
    wait_at_least(t_AH);
    wait_at_least(t_CYCLE-t_AH-t_PW-t_AS);
    if(half_byte) {
      HAL::data8_set(val << 4);
      wait_at_least(t_AS);
      HAL::en_set();
      wait_at_least(t_PW);
      HAL::en_clear();
      wait_at_least(t_AH);
      wait_at_least(t_CYCLE-t_AH-t_PW-t_AS);
    }
  }

  inline static uint8_t read_byte(AccessType access, bool half_byte)
  {
    if(access == INSTRUCTION)
      HAL::rs_set_instruction();
    else
      HAL::rs_set_memory();
    HAL::rw_set_read();
    HAL::data8_get(); //ignore return, switch direction
    wait_at_least(t_AS);
    HAL::en_set();
    wait_at_least(t_DDR);
    uint8_t val = HAL::data8_get();
    wait_at_least(t_PW-t_DDR);
    HAL::en_clear();
    wait_at_least(t_AH);
    wait_at_least(t_CYCLE-t_AH-t_PW-t_AS);
    if(half_byte) {
      wait_at_least(t_AS);
      HAL::en_set();
      wait_at_least(t_DDR);
      val = (val & ~0xFF) | (HAL::data8_get() >> 4);
      wait_at_least(t_PW-t_DDR);
      HAL::en_clear();
      wait_at_least(t_AH);
      wait_at_least(t_CYCLE-t_AH-t_PW-t_AS);
    }
    return val;
  }
public:
  inline static void init() {
    HAL::init();
    wait_at_least(10000000);
    HAL::backlight_state(true);
    if(halfByte)
      {
	send_byte(INSTRUCTION, FUNCTION_CMD(!halfByte, true, false), false); //not half send byte
	wait_at_least(37000);
      }
    write_opcode(FUNCTION_CMD(!halfByte, true, false));
    write_opcode(CLEAR_CMD);
    write_opcode(DISPLAY_CTRL_CMD(true, true, true));
    write_opcode(ENTRY_CMD);
    write_opcode(DDR_ADDR_CMD(0x40));

  }
  inline static void write_opcode(uint8_t opcode) {
    send_byte(INSTRUCTION, opcode, halfByte);
    while(is_busy())
      ;
  }
  inline static void write_mem(uint8_t value) {
    send_byte(MEMORY, value, halfByte);
  }
  inline static uint8_t read_mem() {
    read_byte(MEMORY, halfByte);
  }
  inline static bool is_busy() {
    return read_byte(INSTRUCTION, halfByte) & BUSY_FLAG;
  }
};

typedef CommandLayer<MSBusHAL> LCD;

int main() {
  IODIR0 |= BIT10 | BIT11;
  IOPIN0 &= ~(BIT10 | BIT11);
  LCD::init();
  IOPIN0 |= BIT10;
  LCD::write_mem('A');
  LCD::write_mem('B');
  LCD::write_mem('C');
  IOPIN0 |= BIT11;
  return 0;
}
