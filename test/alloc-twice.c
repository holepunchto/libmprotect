#include <assert.h>
#include <errno.h>

#include "../include/mprotect.h"

int
main() {
  int e;

  mprotect_t m = {0};

  e = mprotect_alloc(&m, 32);
  assert(e == 0);

  uint8_t *data = m.data;

  e = mprotect_alloc(&m, 64);
  assert(e == -EINVAL);
  assert(m.data == data);
  assert(m.len == 32);
  assert(m.state == MPROTECT_NOACCESS);

  mprotect_free(&m);

  e = mprotect_alloc(&m, 64);
  assert(e == 0);
  assert(m.len == 64);

  mprotect_free(&m);
}
