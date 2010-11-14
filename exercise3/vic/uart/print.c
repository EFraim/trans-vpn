static uint8_t _printBuffer[12];

static void print(uint8_t* s) {
  for (; *s; s++)
    print_char(*s);
}

static void printHex(uint32_t v, uint8_t digits) {
  int8_t i;
  uint8_t d;

  for (i=0; i<digits; i++) {
    d = v & 0x0000000F;
    _printBuffer[i] = d < 10 ? d + '0' : d - 10 + 'A';
    v >>= 4;
  }

  for (i=digits-1; i>=0; i--)
    print_char(_printBuffer[i]);
}

static void printNum(int32_t v) {
  int8_t i;
  uint8_t digits;
  int16_t negative;

  if (v==0) {
    print_char('0');
    return;
  }

  if (v < 0) {
    negative = 1;
    v = -v;
  } else negative = 0;

  digits = 0;
  for (i=0; v != 0; i++) {
    _printBuffer[i] = (v % 10) + '0';
    v /= 10;
    digits++;
  }

  if (negative) print_char('-');
  for (i=digits-1; i>=0; i--)
    print_char(_printBuffer[i]);
}
