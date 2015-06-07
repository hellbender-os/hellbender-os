#include <setjmp.h>
#include <hellbender/threadlocal.h>

void _hellbender_setjmpval(int val) {
  THREADLOCAL->jmpval = val;
}

int _hellbender_getjmpval() {
  return THREADLOCAL->jmpval;
}
