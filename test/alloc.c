#include <assert.h>

#include "../include/mprotect.h"

int
main() {
  int e;

  mprotect_t m = {0};

  e = mprotect_alloc(&m, 32);
  assert(e == 0);
  assert(m.state == MPROTECT_NOACCESS);
  assert(m.len == 32);
  assert(m.data != NULL);

  e = mprotect_readwrite(&m);
  assert(e == 0);
  assert(m.state == MPROTECT_READWRITE);

  for (size_t i = 0; i < m.len; i++) assert(m.data[i] == 0);

  m.data[0] = 0xaa;
  m.data[31] = 0xbb;

  e = mprotect_readonly(&m);
  assert(e == 0);
  assert(m.state == MPROTECT_READONLY);
  assert(m.data[0] == 0xaa);
  assert(m.data[31] == 0xbb);

  e = mprotect_readonly(&m);
  assert(e == 0);
  assert(m.state == MPROTECT_READONLY);

  e = mprotect_noaccess(&m);
  assert(e == 0);
  assert(m.state == MPROTECT_NOACCESS);

  mprotect_free(&m);
  assert(m.state == MPROTECT_FREED);
  assert(m.data == NULL);
  assert(m.len == 0);
}
