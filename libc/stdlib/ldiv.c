#include <stdlib.h>

ldiv_t ldiv (long int numer, long int denom) {
  return (ldiv_t){ .quot = numer / denom, .rem = numer % denom };
}
