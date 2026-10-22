#include <errno.h>
#include <sodium.h>
#include <stddef.h>

#include "../include/mprotect.h"

static int
mprotect__error(void) {
  // Not every failure path in libsodium sets errno, notably VirtualProtect()
  return errno ? -errno : -EIO;
}

static int
mprotect__set(mprotect_t *m, mprotect_state_t state, int (*cb)(void *ptr)) {
  if (m->state == MPROTECT_FREED) return -EINVAL;
  if (m->state == state) return 0;

  if (m->len > 0) {
    errno = 0;
    if (cb(m->data) != 0) return mprotect__error();
  }

  m->state = state;

  return 0;
}

int
mprotect_alloc(mprotect_t *m, size_t len) {
  if (m->state != MPROTECT_FREED) return -EINVAL;

  if (sodium_init() == -1) return -ENOSYS;

  uint8_t *data = NULL;
  bool locked = true;

  // An empty allocation is bookkeeping only: sodium_malloc(0) would hand out
  // a pointer to the start of its guard page, which is a confusing thing to
  // give out as a zero-length buffer
  if (len > 0) {
    errno = 0;
    data = sodium_malloc(len);
    if (data == NULL) return mprotect__error();

    // sodium_malloc() ignores sodium_mlock() failing, so lock again to find out
    locked = sodium_mlock(data, len) == 0;

    sodium_memzero(data, len);

    errno = 0;
    if (sodium_mprotect_noaccess(data) != 0) {
      int err = mprotect__error();
      sodium_free(data);
      return err;
    }
  }

  m->data = data;
  m->len = len;
  m->locked = locked;
  m->state = MPROTECT_NOACCESS;

  return 0;
}

void
mprotect_free(mprotect_t *m) {
  if (m->state == MPROTECT_FREED) return;

  // sodium_free() makes the region writable and zeroes it before unmapping
  if (m->data) sodium_free(m->data);

  m->data = NULL;
  m->len = 0;
  m->state = MPROTECT_FREED;
  m->locked = false;
}

int
mprotect_noaccess(mprotect_t *m) {
  return mprotect__set(m, MPROTECT_NOACCESS, sodium_mprotect_noaccess);
}

int
mprotect_readonly(mprotect_t *m) {
  return mprotect__set(m, MPROTECT_READONLY, sodium_mprotect_readonly);
}

int
mprotect_readwrite(mprotect_t *m) {
  return mprotect__set(m, MPROTECT_READWRITE, sodium_mprotect_readwrite);
}
