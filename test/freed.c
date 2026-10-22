#include <assert.h>
#include <errno.h>

#include "../include/mprotect.h"

int
main() {
  int e;

  mprotect_t m = {0};

  e = mprotect_noaccess(&m);
  assert(e == -EINVAL);
  e = mprotect_readonly(&m);
  assert(e == -EINVAL);
  e = mprotect_readwrite(&m);
  assert(e == -EINVAL);

  e = mprotect_alloc(&m, 32);
  assert(e == 0);

  mprotect_free(&m);
  mprotect_free(&m);

  assert(m.state == MPROTECT_FREED);
  e = mprotect_readwrite(&m);
  assert(e == -EINVAL);
}
