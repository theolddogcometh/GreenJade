# libgj-gnu (product SO)

Product **DT_NEEDED** shared object with **DT_GNU_HASH** for GreenJade.

Pair of `libgj-so.so.1` (SysV `DT_HASH`): this SO is the GNU-hash half of the
multi-SO resolve smokes used by the kernel ELF loader and `ld-gj`. Clean-room
freestanding C; staged into install rootfs/ESP as a real ELF, not a text stub.

## License

**MIT OR Apache-2.0** (dual). Pure C11 freestanding; no libc, no `libcgj`.

```text
SPDX-License-Identifier: MIT OR Apache-2.0
Copyright (c) 2026 Project GreenJade contributors
```

## Status

| Artifact | Build | Role |
|----------|-------|------|
| `build/user/libgj-gnu.so.1` | `make libgj-gnu` | GNU-hash product SO (`libgj_gnu.c`) |

Install paths (via parent `stage-rootfs` / `stage-esp`):

- rootfs: `lib/libgj-gnu.so.1`, `usr/lib/libgj-gnu.so.1`
- ESP: under `EFI/GREENJADE/lib/` when staged

Soname: `libgj-gnu.so.1` (`-Wl,-soname,libgj-gnu.so.1 -Wl,--hash-style=gnu`).

## Exports

### Stable product contracts

| Symbol | Kind | Notes |
|--------|------|-------|
| `gj_gnu_export` | `volatile uint64_t` (data) | Value `0x43`; GLOB_DAT / JUMP_SLOT target |
| `gj_gnu_init` | function | Restores export + soft stamp + soft caps |

### Soft deepen surface (product SO)

Extra dynsyms enrich `.gnu.hash` bloom/chains. Soft-only: never hard-fail;
smoke contracts above stay stable.

| Symbol | Kind | Notes |
|--------|------|-------|
| `gj_gnu_soft_stamp` | data (`0x474E5531` / `'GNU1'`) | Soft identity stamp |
| `gj_gnu_soft_caps` | data (`0xf`) | Soft capability bitmask |
| `gj_gnu_soft_get` | function | Returns current `gj_gnu_export` |
| `gj_gnu_soft_id` | function | Returns soft stamp |
| `gj_gnu_soft_probe` | function | 1 if export canonical; careful restore on miss |
| `gj_gnu_soft_touch` | function | Re-assert export/stamp/caps; returns `0x43` |

Greppable markers: `GJ_GNU_SOFT_EXPORT_CANON`, `GJ_GNU_SOFT_INIT_RESTORE`,
`GJ_GNU_SOFT_STAMP`, `GJ_GNU_SOFT_GET`, `GJ_GNU_SOFT_ID`, `GJ_GNU_SOFT_PROBE`,
`GJ_GNU_SOFT_TOUCH`.

Kernel / ld-gj markers that probe this SO typically look for:

```text
elf: gnu-hash resolve gj_gnu_export …
ld-gj: hash/sym gj_gnu_export PASS
```

## Layout

| Path | Role |
|------|------|
| `libgj_gnu.c` | Product SO entry TU — linked by parent Makefile |
| `include/gj_gnu.h` | Public decls for consumers / smokes |
| `src/graph_gnu_batch1.c` | Optional GNU extension batch scaffold (not wired) |
| `README.md` | This file |

Source of truth for the shipped SO: **`libgj_gnu.c` only**.

### `src/graph_gnu_batch1.c` (unwired)

Freestanding marker TU for a future GNU extension surface on this SO.
Ownership policy: only **missing** symbols are defined in this tree; symbols
that already exist in `user/libcgj` stay there (`strverscmp`, `memrchr`,
`rawmemchr`, `strchrnul` are PRESENT — not redefined here).

Soft-deepened batch markers (parent must wire TU into `libgj-gnu` when ready):

| Symbol | Kind | Notes |
|--------|------|-------|
| `gj_gnu_batch1_export` | data `0x471` | Batch presence marker |
| `gj_gnu_batch1_soft_stamp` | data `'B1s1'` | Soft companion stamp |
| `gj_gnu_batch1_init` | function | Careful restore of marker + stamp |
| `gj_gnu_batch1_id` | function | Returns marker |
| `gj_gnu_batch1_soft_get` | function | Soft read of marker |
| `gj_gnu_batch1_soft_probe` | function | Soft check + careful restore |

Parent Makefile must list the TU when symbols land; do not redefine libcgj
exports here. This agent tree does **not** edit top-level Makefile.

## Build

```sh
make libgj-gnu
# → build/user/libgj-gnu.so.1
```

Manual (same freestanding shared flags as parent Makefile):

```sh
cc -shared -nostdlib -fPIC -std=c11 -ffreestanding -fno-builtin \
   -fno-stack-protector -Wall -Wextra -Werror -O2 \
   -Wl,-soname,libgj-gnu.so.1 -Wl,--hash-style=gnu \
   -o build/user/libgj-gnu.so.1 user/libgj-gnu/libgj_gnu.c
objcopy --strip-debug build/user/libgj-gnu.so.1
```

Optional (parent-only) wire of batch1 into the product SO:

```sh
cc -c -std=c11 -ffreestanding -fno-builtin -fno-stack-protector \
   -fPIC -Wall -Wextra -Werror -O2 \
   -o /tmp/graph_gnu_batch1.o user/libgj-gnu/src/graph_gnu_batch1.c
# link /tmp/graph_gnu_batch1.o with libgj_gnu.c under --hash-style=gnu
```

## Related (outside this tree)

| Component | Relation |
|-----------|----------|
| `user/libgj/` | Static freestanding archive (string + syscalls) |
| `user/libgj-so/` | SysV-hash product SO counterpart (`gj_so_export`) |
| `user/ld-gj/` | Userspace dynlinker; resolves `gj_gnu_export` via GNU hash |
| `kernel/proc/elf_load.c` | Kernel multi-SO GNU-hash lookup / smoke |
| `kernel/fs/vfs_ram.c` | Placeholder seed paths until real ELF is staged |
