#include <stdint.h>

static struct rand_state {
  unsigned short s[3];
  unsigned short old[3];
  uint64_t a, c;
} _rand_state;

static uint64_t _rand_next(unsigned short *state) {
  uint64_t Xn =
    ((uint64_t)state[0] & 0xffffl)
    + (((uint64_t)state[1] & 0xffffl)<<16)
    + (((uint64_t)state[2] & 0xffffl)<<32);
  uint64_t Xn1 = (_rand_state.a * Xn + _rand_state.c) % (1ll<<48);
  state[0] = (unsigned short)(Xn1 & 0xffffl);
  state[1] = (unsigned short)((Xn1>>16) & 0xffffl);
  state[2] = (unsigned short)((Xn1>>32) & 0xffffl);
  return Xn1;
}

unsigned short int *seed48(unsigned short int seed16v[3]) {
  unsigned short *old = _rand_state.old;
  old[0] = _rand_state.s[0];
  old[1] = _rand_state.s[1];
  old[2] = _rand_state.s[2];
  
  _rand_state.s[0] = seed16v[0];
  _rand_state.s[1] = seed16v[1];
  _rand_state.s[2] = seed16v[2];
  _rand_state.a = 0x5DEECE66Dll;
  _rand_state.c = 0xB;
  return old;
}

void srand48(long x) {
  unsigned short seed[3];
  seed[0] = (unsigned short)(x & 0xffff);
  seed[1] = (unsigned short)((x & 0xffff0000) >> 16);
  seed[2] = (unsigned short)(0xf0b4);
  seed48(seed);
}

void lcong48(unsigned short int param[7]) {
  _rand_state.s[0] = (unsigned short)param[0];
  _rand_state.s[1] = (unsigned short)param[1];
  _rand_state.s[2] = (unsigned short)param[2];
  _rand_state.a = (((uint64_t)param[3] & 0xffff)
                   | (((uint64_t)param[4] & 0xffff) << 16)
                   | (((uint64_t)param[5] & 0xffff) << 32));
  _rand_state.c = (uint64_t)param[6];
}

long nrand48(unsigned short state[3]) {
  uint64_t x = _rand_next(state);
  uint32_t y = (uint32_t)(x>>16);
  return (long)(y & 0x7fffffff);
}

long jrand48(unsigned short state[3]) {
  uint64_t x = _rand_next(state);
  uint32_t y = (uint32_t)(x>>16);
  if (y & 0x80000000) {
    return (long)(y & 0x7fffffff);
  } else {
    return -(long)(y & 0x7fffffff);
  }
}

long lrand48(void) {
  return nrand48(_rand_state.s);
}

long mrand48(void) {
  return jrand48(_rand_state.s);
}
