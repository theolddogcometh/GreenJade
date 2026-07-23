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
| `libgj_so.c` | Sole linked product source — export + SysV hash + get |
| `include/gj_so.h` | Public decls for consumers / smokes / soft batches |
| `src/graph_so_batch1.c` | Soft SysV-hash helpers (unwired) |
| `src/graph_so_batch2.c` | Soft multi-marker + chain walk (unwired) |
| `README.md` | This file |

Source of truth for the shipped SO: **`libgj_so.c` only**. Soft batches under
`src/` are CREATE-ONLY scaffolds until the parent Makefile lists them.

## Exports (product SO)

| Symbol | Type | Notes |
|--------|------|-------|
| `gj_so_export` | `volatile uint64_t` | Defined value `0x42`; primary resolve target |
| `gj_so_init` | `void(void)` | Resets `gj_so_export` to `0x42` |
| `gj_so_get_export` | `uint64_t(void)` | JUMP_SLOT-friendly read of the export |
| `gj_so_sysv_hash` | `uint32_t(const char *)` | Classic ELF SysV name hash (`DT_HASH`) |

`scripts/smoke-all.sh` checks `nm -D build/user/libgj-so.so.1` for
`gj_so_export`. Kernel / ld-gj multi-SO paths also look for
`hash/sym gj_so_export PASS` in the boot log.

## Soft batches (unwired)

Ownership: only **missing** SysV/product-SO symbols land here; do not
redefine `user/libcgj` or the product TU. Parent Makefile must list a TU
when symbols ship; until then these are freestanding soft scaffolds.

### `src/graph_so_batch1.c`

| Symbol | Role |
|--------|------|
| `gj_so_batch1_export` | Presence marker `0x421` |
| `gj_so_batch1_init` / `gj_so_batch1_id` | Marker restore / read |
| `gj_so_batch1_sysv_hash` | Soft copy of classic ELF SysV hash |
| `gj_so_batch1_streq` | Freestanding string equality (chain match) |
| `gj_so_batch1_bucket` | `hash % nbucket` helper |

### `src/graph_so_batch2.c`

| Symbol | Role |
|--------|------|
| `gj_so_batch2_export_a` / `_b` | Distinct markers `0x422` / `0x423` |
| `gj_so_batch2_init` / `gj_so_batch2_id` | Restore / XOR id |
| `gj_so_batch2_chain_next` | One SysV chain step (bounds-checked) |
| `gj_so_batch2_chain_find` | Soft bucket→chain probe with match callback |

Soft only — **not** a product resolve path and **not** bar3 / Top-50 evidence.

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

Host compile check for soft batches (not linked into the product SO):

```sh
cc -c -std=c11 -ffreestanding -fno-builtin -fno-stack-protector \
   -Wall -Wextra -Werror -O2 -Iuser/libgj-so/include \
   -o /tmp/graph_so_batch1.o user/libgj-so/src/graph_so_batch1.c
cc -c -std=c11 -ffreestanding -fno-builtin -fno-stack-protector \
   -Wall -Wextra -Werror -O2 -Iuser/libgj-so/include \
   -o /tmp/graph_so_batch2.o user/libgj-so/src/graph_so_batch2.c
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
