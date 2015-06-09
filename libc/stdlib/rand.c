#include <stdint.h>

static struct rand_state {
  unsigned s;
} _rand_state;

static uint32_t _rand_next_xorshift32(unsigned *s) {
  *s ^= *s << 5;
  *s ^= *s >> 9;
  *s ^= *s << 28;
  return (uint32_t) *s;
}

int rand_r(unsigned *seed) {
  uint32_t x = _rand_next_xorshift32(seed);
  return (int)(x>>1);
}

int rand(void) {
  return rand_r(&_rand_state.s);
}

void srand(unsigned seed) {
  _rand_state.s = seed;
}
