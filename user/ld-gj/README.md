# ld-gj (freestanding dynamic linker)

Freestanding **PT_INTERP** target for GreenJade — pure C11, libgj only.
Userspace product path that consumes the kernel handoff page, loads
DT_NEEDED SOs, applies RELA/JMPREL, and transfers to `AT_ENTRY`.

## License

Product code: **MIT OR Apache-2.0** (dual license; no GPL source).

## Status

| Binary | Build | Role |
|--------|-------|------|
| `build/user/ld-gj.so.1` | `make ld-gj` / `make userland` | Dynlinker scaffold (`ld_gj.c`) |

Linked freestanding with `libgj` and `ld-gj.ld` (preferred base `0x20000000`).
Debug stripped so INTERP embed / VFS stage stay under bring-up size caps.

This directory owns only `user/ld-gj/**`. Parent Makefile / kernel embed live
outside this tree.

Install paths (via `stage-rootfs` / `stage-esp`):

- rootfs: `lib/ld-gj.so.1` (also staged as `ld-linux-x86-64.so.2`)
- ESP: `EFI/GREENJADE/lib/ld-gj.so.1`

Kernel embeds the packaged ELF via `kernel/proc/ld_gj_embed.S` (outside this
directory) when the VFS INTERP stub is tiny/non-ELF.

### Freestanding path (`ld_gj.c`)

1. **Banner** — `start product path`
2. **Handoff** — magic at `0x6ff00000`, stack auxv at `0x6ff01000`, SO list
3. **Self + libc** — open `/lib/ld-gj.so.1` and `/lib/libc.so.6`
4. **Main + multi-SO** — EXECFN/path open, RELA/SYM, soft SysV+GNU hash resolve
5. **Entry** — `AT_ENTRY ready` → transfer → optional return
6. **Exit** — `live path PASS` (when handoff+entry) + `scaffold PASS` + `exit(0)`

Soft resolve/hash depth (userspace freestanding):

- Cache `PT_DYNAMIC` per SO (`DT_HASH` / `DT_GNU_HASH` / `SYMTAB` / `STRTAB` / `SONAME`)
- Lookup order: **GNU hash (+64-bit bloom) → SysV DT_HASH → SYMTAB scan**
- Prefer `STB_GLOBAL` over `STB_WEAK` across the multi-SO registry
- Soft probes: `gj_so_export` / `gj_gnu_export` / `gj_so_init` / `gj_gnu_init` + method tags

Reloc depth (userspace): `R_X86_64_{RELATIVE,GLOB_DAT,JUMP_SLOT,64,COPY}`,
TLS (`DTPMOD64`/`DTPOFF64`/`TPOFF64`), `IRELATIVE`, `DT_JMPREL`, `DT_INIT`.

## Smoke markers

`scripts/smoke-all.sh` requires these **substrings** in the boot log (kernel
and/or this ELF):

```text
ld-gj: handoff PASS
ld-gj: AT_ENTRY ready
ld-gj: multi-SO
ld-gj: hash/sym
```

### Userspace freestanding lines (`ld_gj.c`)

Typical INTERP-first product path (order fixed; optional lines depend on
handoff contents):

```text
ld-gj: start product path
ld-gj: handoff magic PASS
ld-gj: stack auxv PASS
ld-gj: auxv handoff PASS
ld-gj: handoff SYM ready
ld-gj: handoff SO list ready
ld-gj: multi-SO load PASS n=N
ld-gj: hash/sym sysv table PASS
ld-gj: hash/sym gnu table PASS
ld-gj: self-ELF PASS
ld-gj: libc path PASS
ld-gj: main ELF PASS
ld-gj: RELA userspace PASS
ld-gj: SYM userspace PASS
ld-gj: multi-SO resolve PASS
ld-gj: TLS reloc PASS
ld-gj: COPY reloc PASS
ld-gj: IRELATIVE PASS
ld-gj: DT_INIT PASS
ld-gj: hash/sym algo soft PASS
ld-gj: hash/sym gj_so_export PASS
ld-gj: hash/sym gj_gnu_export PASS
ld-gj: hash/sym sysv path PASS
ld-gj: hash/sym gnu path PASS
ld-gj: hash/sym soft resolve PASS
ld-gj: AT_ENTRY ready
ld-gj: transfer AT_ENTRY
ld-gj: AT_ENTRY returned
ld-gj: live path PASS
ld-gj: scaffold PASS
```

| Marker | Meaning |
|--------|---------|
| `start product path` | `_start` banner (write(1)) |
| `handoff magic PASS` | Page at `GJ_LD_HANDOFF_VA` has `GJ_LD_HANDOFF_MAGIC` |
| `stack auxv PASS` | Handoff stack pointer has argc ≥ 1 |
| `auxv handoff PASS` | Handoff `cAuxv` / AT_ENTRY / AT_BASE used |
| `handoff SO list ready` | `cSo` > 0; load path runs |
| `multi-SO load PASS n=` | N SO images loaded (mapped bias and/or `/lib`) |
| `hash/sym sysv table PASS` | Loaded SO(s) expose cached `DT_HASH` |
| `hash/sym gnu table PASS` | Loaded SO(s) expose cached `DT_GNU_HASH` |
| `self-ELF PASS` | `/lib/ld-gj.so.1` opens as ELF |
| `libc path PASS` | `/lib/libc.so.6` openable |
| `main ELF PASS` | Main path/EXECFN is ELF |
| `RELA userspace PASS` | At least one reloc applied on main image |
| `SYM userspace PASS` | GLOB_DAT / JUMP_SLOT / 64 / COPY applied |
| `multi-SO resolve PASS` | Cross-SO resolve (reloc and/or hash probe) |
| `hash/sym algo soft PASS` | SysV/GNU hash self-check (empty/name) |
| `hash/sym gj_so_export PASS` | Soft resolve found product SysV export |
| `hash/sym gj_gnu_export PASS` | Soft resolve found product GNU export |
| `hash/sym sysv/gnu/scan path PASS` | Method tag for successful soft bind |
| `hash/sym soft resolve PASS` | ≥2 product SO symbols soft-bound |
| `AT_ENTRY ready` | Entry VA known; transfer may follow |
| `live path PASS` | Handoff seen + AT_ENTRY known (aggregate) |
| `scaffold PASS` | Product path finished; exit(0) |

Deferred / soft-fail lines (not hard smoke fails):

```text
ld-gj: libc path deferred
ld-gj: multi-SO resolve miss
ld-gj: hash/sym scan path PASS
ld-gj: hash/sym builtin soft PASS
ld-gj: auxv file PASS
ld-gj: self-path open PASS
ld-gj: main path open PASS
```

### Kernel markers (not from this directory)

Printed by `kernel/proc/elf_load.c` during INTERP handoff setup:

```text
ld-gj: handoff PASS
ld-gj: AT_ENTRY ready 0x…
ld-gj: stack auxv ready 0x…
ld-gj: INTERP entry …
```

## Build

```sh
make ld-gj
# → build/user/ld-gj.so.1

# or:
make build/user/ld-gj.so.1
make userland
```

Manual (same freestanding flags as Makefile `USER_CFLAGS`):

```sh
cc -std=c11 -ffreestanding -fno-builtin -fno-stack-protector \
   -fno-pic -mno-red-zone -msse2 -Wall -Wextra -Werror \
   -Iuser/libgj/include -g -O2 \
   -c -o build/user/ld-gj/ld_gj.o user/ld-gj/ld_gj.c
ld -nostdlib -static -T user/ld-gj/ld-gj.ld -z max-page-size=0x1000 \
   -o build/user/ld-gj.so.1 build/user/ld-gj/ld_gj.o build/user/libgj.a
objcopy --strip-debug build/user/ld-gj.so.1
```

## Layout

| Path | Role |
|------|------|
| `ld_gj.c` | `_start` freestanding dynlinker product path |
| `ld-gj.ld` | ET_EXEC scaffold linker script (base `0x20000000`) |
| `README.md` | This file |

Source of truth under this tree: `ld_gj.c` + `ld-gj.ld`.

## Live embed (parent — docs only)

Parent tree owns:

- Makefile target `ld-gj` → `build/user/ld-gj.so.1`
- `kernel/proc/ld_gj_embed.S` (`.incbin` when INTERP stub needs it)
- Rootfs/ESP stage of `/lib/ld-gj.so.1`

Do not treat this directory as owning Makefile or kernel edits.
