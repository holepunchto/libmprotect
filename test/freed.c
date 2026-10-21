#include <assert.h>
#include <errno.h>

#include "../include/mprotect.h"

int
main() {
  mprotect_t m = {0};

  assert(mprotect_noaccess(&m) == -EINVAL);
  assert(mprotect_readonly(&m) == -EINVAL);
  assert(mprotect_readwrite(&m) == -EINVAL);

  assert(mprotect_alloc(&m, 32) == 0);

  mprotect_free(&m);
  mprotect_free(&m);

  assert(m.state == MPROTECT_FREED);
  assert(mprotect_readwrite(&m) == -EINVAL);
}
