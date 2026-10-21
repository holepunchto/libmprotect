# libmprotect

Guarded, access controlled memory for secrets, built on the libsodium secure allocator.

Each allocation sits between guard pages, is locked out of swap and core dumps where the platform allows it, and tracks its own length and access state so there is no confusion about whether a pointer is accessible or still allocated.

## API

See [`include/mprotect.h`](include/mprotect.h) for the public API.

```c
mprotect_t m = {0};

int err = mprotect_alloc(&m, 32); // zeroed, MPROTECT_NOACCESS

mprotect_readwrite(&m);
memcpy(m.data, secret, 32);
mprotect_noaccess(&m);

mprotect_readonly(&m);
use(m.data, m.len);
mprotect_noaccess(&m);

mprotect_free(&m); // zeroed and released, MPROTECT_FREED
```

Functions return 0 on success or a negative errno value. `mprotect_alloc()` fails with `-ENOSYS` if the platform build has no page protection, rather than handing out unprotected memory.

`m.locked` is `false` if the memory could not be locked, for example because `RLIMIT_MEMLOCK` is exhausted. The memory is still guarded and access controlled, but may be swapped to disk.

A handle is not thread safe; synchronise access to it externally.

## Building

```console
npm install
bare-make generate --debug
bare-make build
bare-make test
```

## License

Apache-2.0
