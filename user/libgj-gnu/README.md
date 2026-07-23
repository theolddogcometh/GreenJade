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

Defined symbols consumed by multi-SO / GNU-hash resolve smokes:

| Symbol | Kind | Notes |
|--------|------|-------|
| `gj_gnu_export` | `volatile uint64_t` (data) | Value `0x43`; GLOB_DAT / JUMP_SLOT target |
| `gj_gnu_init` | function | Re-stores `gj_gnu_export = 0x43` |

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
that already exist in `user/libcgj` stay there. This file therefore exports
only batch markers (`gj_gnu_batch1_export` / `gj_gnu_batch1_init` /
`gj_gnu_batch1_id`) until a missing GNU symbol is assigned here.

Parent Makefile must list the TU when symbols land; do not redefine libcgj
exports here.

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

## Related (outside this tree)

| Component | Relation |
|-----------|----------|
| `user/libgj/` | Static freestanding archive (string + syscalls) |
| `user/libgj-so/` | SysV-hash product SO counterpart (`gj_so_export`) |
| `user/ld-gj/` | Userspace dynlinker; resolves `gj_gnu_export` via GNU hash |
| `kernel/proc/elf_load.c` | Kernel multi-SO GNU-hash lookup / smoke |
| `kernel/fs/vfs_ram.c` | Placeholder seed paths until real ELF is staged |
