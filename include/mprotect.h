#ifndef MPROTECT_H
#define MPROTECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct mprotect_s mprotect_t;

typedef enum {
  MPROTECT_FREED = 0,
  MPROTECT_NOACCESS,
  MPROTECT_READONLY,
  MPROTECT_READWRITE,
} mprotect_state_t;

struct mprotect_s {
  uint8_t *data;
  size_t len;
  mprotect_state_t state;
  bool locked;
};

/**
 * Allocate `len` bytes of guarded memory into `m`, which must be zeroed or
 * freed. The memory is zeroed and left in the `MPROTECT_NOACCESS` state.
 *
 * `m->locked` reports whether the memory could be locked out of swap and core
 * dumps; allocation still succeeds if it could not, for example when
 * `RLIMIT_MEMLOCK` is exhausted.
 *
 * Returns 0 on success or a negative errno value.
 */
int
mprotect_alloc(mprotect_t *m, size_t len);

/**
 * Zero and release the memory, leaving `m` in the `MPROTECT_FREED` state.
 * Valid in any state, including already freed.
 */
void
mprotect_free(mprotect_t *m);

/**
 * Change the access to the memory. Returns 0 on success or a negative errno
 * value, and leaves `m->state` unchanged on failure.
 */
int
mprotect_noaccess(mprotect_t *m);

int
mprotect_readonly(mprotect_t *m);

int
mprotect_readwrite(mprotect_t *m);

#ifdef __cplusplus
}
#endif

#endif // MPROTECT_H
