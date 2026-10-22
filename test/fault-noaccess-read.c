#include <assert.h>

#include "../include/mprotect.h"
#include "fault.h"

static mprotect_t m;

static void
read_byte(void) {
  volatile uint8_t b = m.data[0];
  (void) b;
}

int
main() {
  int e;

  e = mprotect_alloc(&m, 32);
  assert(e == 0);

  expect_fault(read_byte);

  mprotect_free(&m);
}
