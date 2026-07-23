# libgj-so (product SysV-hash SO)

Clean-room freestanding **shared object** for GreenJade multi-SO bring-up.

Provides a real `ET_DYN` ELF with **SysV `DT_HASH`** (not GNU hash) and a
defined dynamic symbol so `ld-gj` can resolve cross-object `GLOB_DAT` /
`JUMP_SLOT` relocations. Companion GNU-hash SO: `user/libgj-gnu/`.

## License

**MIT OR Apache-2.0** (dual). Pure C11 freestanding; no libc, no `libcgj`.

```text
SPDX-License-Identifier: MIT OR Apache-2.0
Copyright (c) 2026 Project GreenJade contributors
```

## Status

| Artifact | Build | Role |
|----------|-------|------|
| `build/user/libgj-so.so.1` | `make libgj-so` / `make userland` | Product DT_NEEDED SO (`libgj_so.c`) |

SONAME: `libgj-so.so.1`. Built freestanding (`-nostdlib -fPIC`, stripped).

Install paths (via `stage-rootfs` / `stage-esp`):

- rootfs: `lib/libgj-so.so.1`, `usr/lib/libgj-so.so.1`
- ESP: `EFI/GREENJADE/lib/libgj-so.so.1`

## Layout

| Path | Role |
|------|------|
| `libgj_so.c` | Sole linked source — export + optional init |
| `include/gj_so.h` | Public decls for consumers / smokes |
| `README.md` | This file |

Source of truth for the shipped SO: **`libgj_so.c` only**.

## Exports

| Symbol | Type | Notes |
|--------|------|-------|
| `gj_so_export` | `volatile uint64_t` | Defined value `0x42`; primary resolve target |
| `gj_so_init` | `void(void)` | Resets `gj_so_export` to `0x42` |

`scripts/smoke-all.sh` checks `nm -D build/user/libgj-so.so.1` for
`gj_so_export`. Kernel / ld-gj multi-SO paths also look for
`hash/sym gj_so_export PASS` in the boot log.

## Build

```sh
make libgj-so
# → build/user/libgj-so.so.1
```

Equivalent flags (parent Makefile; do not edit that file from this tree):

```sh
cc -shared -nostdlib -fPIC -std=c11 -ffreestanding -fno-builtin \
   -fno-stack-protector -Wall -Wextra -Werror -O2 \
   -Wl,-soname,libgj-so.so.1 -Wl,--hash-style=sysv \
   -o build/user/libgj-so.so.1 user/libgj-so/libgj_so.c
objcopy --strip-debug build/user/libgj-so.so.1 2>/dev/null || true
```

## Role in product path

1. Staged under `/lib` for INTERP-first / rootfs install images.
2. Appears as `DT_NEEDED` (`libgj-so.so.1`) in multi-SO exec smokes.
3. ld-gj opens the SO, walks **SysV hash**, binds `gj_so_export`.
4. Paired with `libgj-gnu.so.1` so both classic and GNU hash chains are live.

## Related (outside this tree)

| Component | Relation |
|-----------|----------|
| `user/libgj/` | Static freestanding archive (string + syscalls) |
| `user/libgj-gnu/` | GNU-hash product SO counterpart (`gj_gnu_export`) |
| `user/ld-gj/` | Userspace dynlinker; resolves `gj_so_export` via SysV hash |
| `kernel/proc/elf_load.c` | Kernel multi-SO SysV-hash lookup / smoke |
