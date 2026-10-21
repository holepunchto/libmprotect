#include <assert.h>

#include "../include/mprotect.h"

int
main() {
  int e;

  mprotect_t m = {0};

  e = mprotect_alloc(&m, 0);
  assert(e == 0);
  assert(m.state == MPROTECT_NOACCESS);
  assert(m.len == 0);

  e = mprotect_readwrite(&m);
  assert(e == 0);
  assert(m.state == MPROTECT_READWRITE);

  e = mprotect_readonly(&m);
  assert(e == 0);

  e = mprotect_noaccess(&m);
  assert(e == 0);

  mprotect_free(&m);
  assert(m.state == MPROTECT_FREED);
}
