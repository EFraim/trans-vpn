static uint32_t uint32FromLittleEndian(uint8_t *buffer) {
  uint32_t tmp;
  uint32_t result;

  tmp = buffer[0];
  result = tmp;

  tmp = buffer[1];
  tmp <<= 8;
  result += tmp;

  tmp = buffer[2];
  tmp <<= 16;
  result += tmp;

  tmp = buffer[3];
  tmp <<= 24;
  result += tmp;

  return result;
}

static void uint32ToLittleEndian(uint32_t x, uint8_t *buffer) {
  buffer[0] = ( x        & 0xFF);
  buffer[1] = ((x >>  8) & 0xFF);
  buffer[2] = ((x >> 16) & 0xFF);
  buffer[3] = ((x >> 24) & 0xFF);
}

static uint16_t uint16FromLittleEndian(uint8_t *buffer) {
  uint16_t tmp;
  uint16_t result;

  tmp = buffer[0];
  result = tmp;

  tmp = buffer[1];
  tmp <<= 8;
  result += tmp;

  return result;
}

static void uint16ToLittleEndian(uint16_t x, uint8_t *buffer) {
  buffer[0] = ( x        & 0xFF);
  buffer[1] = ((x >>  8) & 0xFF);
}




