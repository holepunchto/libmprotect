#include <assert.h>

#include "../include/mprotect.h"
#include "fault.h"

static mprotect_t m;

static void
write_past_end(void) {
  ((volatile uint8_t *) m.data)[m.len] = 1;
}

int
main() {
  int e;

  e = mprotect_alloc(&m, 32);
  assert(e == 0);
  e = mprotect_readwrite(&m);
  assert(e == 0);

  m.data[31] = 1;

  expect_fault(write_past_end);

  mprotect_free(&m);
}
