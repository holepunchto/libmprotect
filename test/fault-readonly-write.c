#include <assert.h>

#include "../include/mprotect.h"
#include "fault.h"

static mprotect_t m;

static void
write_byte(void) {
  ((volatile uint8_t *) m.data)[0] = 1;
}

int
main() {
  int e;

  e = mprotect_alloc(&m, 32);
  assert(e == 0);
  e = mprotect_readonly(&m);
  assert(e == 0);

  expect_fault(write_byte);

  mprotect_free(&m);
}
