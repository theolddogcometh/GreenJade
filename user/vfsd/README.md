# vfsd (product)

Userspace VFS door host for GreenJade — multi-client mini-FS ownership,
named page-cache, and freestanding bring-up smoke (hard + soft door surface).

## License

Product code: **MIT OR Apache-2.0**.

## Status

| Binary | Build | Role |
|--------|-------|------|
| `build/user/vfsd.elf` | `make vfsd-gj` | Freestanding product daemon (`vfsd_gj.c`) |

- Kernel interim: `vfs_door` mini-FS over RAM/LBA, multi-client ops after CLAIM.
- Product: this process owns FORMAT/MOUNT + named cache; other clients may use
  file/openfd ops while the door is claimed.
- Kernel retains the door implementation; this ELF is ownership + smoke.
- This directory owns only `user/vfsd/**` — parent Makefile / kernel embed /
  boot spawn live outside this tree.

## Layout

| Path | Role |
|------|------|
| `vfsd_gj.c` | `_start` freestanding VFS door bring-up + soft deepen |
| `README.md` | This file |

Source of truth under this tree: `vfsd_gj.c` only.

## Freestanding path (`vfsd_gj.c`)

1. **Named memobj** `vfsd-cache` — create/map shareable page; write `GJ_VFS_MAGIC`
2. **VFS door CLAIM** — token `VFSD` (`0x56465344`); multi-client owner
3. **FORMAT + MOUNT** — on-disk layout bring-up; verify super magic + version 3
4. **File path** — CREATE / READ / WRITE / STAT / LIST on door names
5. **OPENFD** — `/mnt/…` OPEN + READFD + CLOSE
6. **STATS** (soft if door rejects) — while claimed: owned=1, mounted=1
7. **Soft door deepen** (never hard-fails live path):
   - same-token **re-CLAIM** (idempotent reclaim)
   - bare-name OPEN + READFD on `hello.txt`
   - **WRITEFD** via `OPEN(CREAT|RDWR)` on `soft.tmp`
   - **SEEKFD** END → WRITEFD patch → SEEK SET → READFD verify
   - **UNLINK** `soft.tmp` + LIST absence check
   - aggregate `soft door PASS` if any sub-step greened
8. Optional **store** LBA0 super-block mirror + soft READ verify
9. **RELEASE** door; soft free RELEASE no-op; re-check named cache → `live path PASS`

Boot embed (parent tree): `kernel/proc/vfsd_embed.S` (`.incbin` of the ELF).

## Smoke markers

`scripts/smoke-all.sh` requires these **substrings** in the boot log (prefix-stable;
parentheticals after `PASS` are allowed):

```text
vfsd-gj: block mount PASS
vfsd-gj: file path PASS
vfsd-gj: multi-client door PASS
vfsd-gj: live path PASS
```

### Freestanding lines (`vfsd_gj.c`)

Success path (order fixed; optional / soft lines noted):

```text
vfsd-gj: start
vfsd-gj: named create miss/reuse          (optional if name already present)
vfsd-gj: named cache PASS (memobj map+magic)
vfsd-gj: CLAIM PASS
vfsd-gj: block mount PASS (format+mount)
vfsd-gj: file path PASS (create/read/write/stat/list)
vfsd-gj: multi-client door PASS (CLAIM owned)
vfsd-gj: openfd PASS (/mnt + READFD)
vfsd-gj: STATS PASS (owned+mounted)       (or STATS soft)
vfsd-gj: reclaim soft PASS                (or reclaim soft-skip)
vfsd-gj: bare OPEN soft PASS              (or bare OPEN soft-skip)
vfsd-gj: WRITEFD soft PASS                (or WRITEFD soft-skip)
vfsd-gj: SEEKFD soft PASS                 (or SEEKFD soft-skip)
vfsd-gj: UNLINK soft PASS                 (or UNLINK soft-skip)
vfsd-gj: soft door PASS                   (or soft door soft-skip)
vfsd-gj: store LBA0 super mirror PASS     (optional; store door free)
vfsd-gj: store LBA0 super mirror soft-skip (optional; claim busy / short write)
vfsd-gj: store LBA0 read soft PASS        (optional; after successful write)
vfsd-gj: store LBA0 read soft-skip        (optional; short read / magic miss)
vfsd-gj: RELEASE PASS
vfsd-gj: RELEASE free soft PASS           (or RELEASE free soft-skip)
vfsd-gj: live path PASS (door+mini-FS+named cache)
```

| Marker | Meaning |
|--------|---------|
| `start` | `_start` entered |
| `named create miss/reuse` | Named memobj already present (non-fatal) |
| `named cache PASS` | Named memobj mapped; magic written and verified |
| `CLAIM PASS` | VFS door ownership token held |
| `block mount PASS` | FORMAT + MOUNT; super magic/version green |
| `file path PASS` | CREATE/READ/WRITE/STAT/LIST (+ LIST contains `hello.txt`) |
| `multi-client door PASS` | Door remains claimed for multi-client clients |
| `openfd PASS` | `/mnt/…` OPEN + READFD + CLOSE |
| `STATS PASS` | Door stats: owned=1, mounted=1 while claimed |
| `STATS soft` | STATS op rejected; live path still green |
| `reclaim soft PASS` | Same-token re-CLAIM idempotent soft |
| `bare OPEN soft PASS` | Bare-name OPEN + READFD on hard-path file |
| `WRITEFD soft PASS` | CREAT\|RDWR OPEN + WRITEFD + name READ verify |
| `SEEKFD soft PASS` | SEEK END/SET + WRITEFD patch + READFD integrity |
| `UNLINK soft PASS` | Unlink soft.tmp; LIST no longer contains name |
| `soft door PASS` | Aggregate: ≥1 soft sub-step greened |
| `soft door soft-skip` | No soft sub-step greened (live path still green) |
| `store LBA0 super mirror PASS` | Optional store WRITE of super magic at LBA0 |
| `store LBA0 super mirror soft-skip` | Store claim busy or short WRITE (non-fatal) |
| `store LBA0 read soft PASS` | Soft READ-back of LBA0 magic + version nibble |
| `store LBA0 read soft-skip` | Short READ or magic mismatch (non-fatal) |
| `RELEASE PASS` | Door released cleanly |
| `RELEASE free soft PASS` | Second RELEASE while free (kernel soft no-op) |
| `live path PASS` | Aggregate: door + mini-FS + named cache sticky |

Failure lines use the same `vfsd-gj:` prefix with **fail** (e.g.
`CLAIM fail`, `file read fail`, `MOUNT super verify fail`,
`named cache sticky fail`) and `exit(1)`. Hard fail after CLAIM always
releases the door token. Soft steps never call `fail_exit`.

### Kernel markers (not from this directory)

```text
vfsd: live elf thr=…
vfsd: live spawn PASS
vfs_door: multi-client PASS
vfs_door: openfd PASS
```

## VFS door ops (subset used here)

Via `gj_vfs(op, a1, a2, a3)` / `GJ_SYS_VFS` — opcodes must match
`kernel/include/gj/vfs_door.h` / `user/libgj/include/gj/syscalls.h`
(**do not renumber**).

| op | name | args | hard / soft |
|----|------|------|-------------|
| 1  | CLAIM | token | hard + soft reclaim |
| 2  | RELEASE | token | hard + soft free no-op |
| 3  | FORMAT | — | hard |
| 4  | MOUNT | optional `u32[4]` → magic, ver, files, data0 | hard |
| 5  | CREATE | name, data, bytes → inode index ≥0 | hard |
| 6  | READ | name, buf, max → bytes | hard (+ soft WRITEFD verify) |
| 7  | WRITE | name, data, bytes → bytes written | hard |
| 8  | UNLINK | name | soft (`soft.tmp`) |
| 9  | STAT | name, `u32[2]` → size, data_lba | hard |
| 10 | LIST | buf, 0, buf size → bytes (names + `\n`) | hard (+ soft UNLINK) |
| 11 | STATS | `u32[4]` → calls, files, owned, mounted | soft-tolerant |
| 12 | OPEN | path (`/mnt/foo` or `foo`), flags | hard `/mnt` + soft bare |
| 13 | CLOSE | door fd | hard + soft |
| 14 | READFD | door fd, buf, max | hard + soft |
| 15 | WRITEFD | door fd, buf, len | soft |
| 16 | SEEKFD | door fd, off, whence (SET/CUR/END) | soft |

Also uses named memobj (`gj_memobj_create_named` / `gj_memobj_map_named`) and
optional store door (`GJ_STORE_OP_CLAIM` / `WRITE` / `READ` / `RELEASE`) for
LBA0 mirror + soft read-back.

Constants in `vfsd_gj.c`:

| Symbol | Value | Role |
|--------|-------|------|
| `GJ_VFS_MAGIC` | `0x31444a47` | Super / cache magic ("GJD1") |
| `GJ_VFS_VERSION` | `3` | Must match kernel `VFS_VERSION` |
| `VFSD_TOKEN` | `0x56465344` | CLAIM / RELEASE / store claim ("VFSD") |
| `VFSD_CACHE_VA` | `0x34000000` | Preferred named-map VA |
| `VFSD_CACHE_NAME` | `vfsd-cache` | Named memobj key |
| `VFSD_SOFT_NAME` | `soft.tmp` | Soft WRITEFD/SEEKFD/UNLINK scratch |

Named-cache soft slots (after hard `pCache[0..7]`): reclaim / WRITEFD cb /
SEEKFD total / UNLINK flag / soft bit-mask.

## Build

```sh
make vfsd-gj
# → build/user/vfsd.elf
```

Manual (same `USER_CFLAGS` / link as other freestanding ELFs):

```sh
cc -std=c11 -ffreestanding -fno-builtin -fno-stack-protector \
   -fno-pic -mno-red-zone -msse2 -Wall -Wextra -Werror \
   -Iuser/libgj/include -g -O2 \
   -c -o build/user/vfsd/vfsd_gj.o user/vfsd/vfsd_gj.c
ld -nostdlib -static -T user/init/user.ld -z max-page-size=0x1000 \
   -o build/user/vfsd.elf build/user/vfsd/vfsd_gj.o build/user/libgj.a
```
