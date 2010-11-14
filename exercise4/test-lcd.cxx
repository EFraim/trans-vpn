#include <stdint.h>
#include <lpc214x.c>
#include <interrupt.c>
#include "lcd.cxx"
typedef CommandLayer<MSBusHAL, true> LCD;
void print_char(char c) {
  static int cur_line=0;
  if(c != '\n')
    LCD::write_mem(c);
  else
    LCD::set_line(cur_line ^= 1);
}

#define printf_char(c) print_char(c)
#include "print.c"
#include "printf.c"


int main() {
  IODIR0 |= BIT10 | BIT11;
  IOPIN0 &= ~(BIT10 | BIT11);
  LCD::init();
  IOPIN0 |= BIT10;
  printf("Sp code: %d\nHello world!", 777);
  IOPIN0 |= BIT11;
  return 0;
}
