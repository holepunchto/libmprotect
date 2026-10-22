#include <assert.h>
#include <errno.h>
#include <stdint.h>

#include "../include/mprotect.h"

int
main() {
  int e;

  mprotect_t m = {0};

  e = mprotect_alloc(&m, SIZE_MAX - 1);
  assert(e == -ENOMEM);
  assert(m.state == MPROTECT_FREED);
  assert(m.data == NULL);
  assert(m.len == 0);
  assert(m.locked == false);

  mprotect_free(&m);
  assert(m.len == 0);

  e = mprotect_alloc(&m, 32);
  assert(e == 0);
  assert(m.len == 32);

  mprotect_free(&m);
}
