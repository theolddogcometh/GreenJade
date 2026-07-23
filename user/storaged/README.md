# storaged (product)

Block/FS host for GreenJade — freestanding store-door ELF + host software-image
smoke.

## License

Product code under this tree: **MIT OR Apache-2.0** (dual license; **no GPL**).
Pure C11. See the repository root `LICENSE` for full text.

## Status

| Binary | Build | Role |
|--------|-------|------|
| `build/user/storaged.elf` | `make storaged-gj` | Freestanding product daemon (store door + UDX ring) |
| `build/storaged` | `make storaged` | Host POSIX smoke (software image) |

- Kernel interim: virtio-blk `/dev/vda`, virtio-scsi `/dev/sda`, `scsi_door` /
  `store_door`.
- Product: this process owns block aggregation (+ FS later); kernel retains
  DMA/IRQ windows only.
- This directory owns only `user/storaged/**` — parent Makefile, kernel embed,
  and boot spawn live outside this tree.

## Layout

| Path | Role |
|------|------|
| `src/storaged_gj.c` | Freestanding live path (`_start`) |
| `src/storaged_stub.c` | Host A1 software-image smoke (`main`) |
| `README.md` | This file |

Source of truth under this tree: `src/storaged_gj.c` + `src/storaged_stub.c`.

## Freestanding path (`storaged_gj.c`)

Live smoke via `gj_store` / `GJ_SYS_STORE`:

1. **CLAIM** store door (token `0x510e0002`)
2. **Soft door surface** (never hard-fails) — reclaim, CAP, STATS, QUEUE_INFO,
   multi-sector R/W at LBA 3 (2 × 512 B), FLUSH, RING_STATE peek
3. **WRITE** / **READ** sector smoke at LBA 2 (512 B), full-sector verify
4. **UDX ring** — `EXPORT_RING` → `MAP_RING` (VA `0x32000000`, page-aligned) →
   `RING_STATE` → `KICK`
5. **RELEASE** + soft free path (unowned RELEASE + QUEUE owned=0) +
   `live path PASS`

### Soft vs hard outcomes

| Condition | Result |
|-----------|--------|
| CLAIM / WRITE / READ-verify / RELEASE fail | Hard fail — exit 1 (best-effort RELEASE) |
| Soft door sub-step fails (CAP/STATS/QUEUE/multi/FLUSH/RING_STATE) | Soft-skip line; still `live path PASS` |
| EXPORT fails, export not ready, or MAP_RING fails | Soft — log soft line; still `live path PASS` |
| virtio-blk absent | Soft-skip ring; still green on non-hard markers |

Boot Multiboot smoke with a ready ring expects the greppable substring
`ring map PASS`. Ring soft-skip alone is **not** enough for Multiboot ring
checks in `scripts/smoke-all.sh` (those assume virtio-blk present).

Boot embed (parent tree): `kernel/proc/storaged_embed.S` (`.incbin` of the ELF).

### Compatibility (ring-map / store door)

Keep these stable unless coordinated with the kernel door and smoke scripts:

| Contract | Value / rule |
|----------|----------------|
| Store ops | STATS=1 CAP=2 READ=3 WRITE=4 CLAIM=5 RELEASE=6 QUEUE_INFO=7 FLUSH=8 EXPORT_RING=9 KICK=10 RING_STATE=11 MAP_RING=12 |
| `STORE_TOKEN` | `0x510e0002` (non-zero claim token) |
| Smoke LBA | LBA **2**, 512 B (avoids LBA0 super/mirror) |
| Soft multi LBA | LBA **3**, 2 × 512 B (≤ `GJ_STORE_XFER_MAX`) |
| `RING_VA` | `0x32000000` (page-aligned user map base) |
| `struct vq_export` | Field layout mirrors `gj_virtq_export` / UDX |
| Marker prefixes | `storaged-gj: …` freestanding; `storaged: …` host + kernel spawn |

Do **not** rename required smoke substrings below.

## Smoke markers

`scripts/smoke-all.sh` requires these **substrings** in the Multiboot log:

```
storaged-gj: ring map PASS
storaged-gj: live path PASS
storaged: live spawn PASS
```

Host path (`make storaged && ./build/storaged`):

```
storaged: PASS
```

### Userspace freestanding lines (`storaged_gj.c`)

Typical product path (order fixed for hard markers; soft lines interleave):

```
storaged-gj: start
storaged-gj: CLAIM PASS
storaged-gj: reclaim soft PASS
storaged-gj: CAP soft sectors=…
storaged-gj: STATS soft blk=… scsi=… calls=…
storaged-gj: QUEUE soft blk=… scsi=… rw=… own=…
storaged-gj: multi-sector soft PASS
storaged-gj: FLUSH soft PASS
storaged-gj: RING_STATE soft free=… ready=…
storaged-gj: soft door PASS
storaged-gj: WRITE PASS
storaged-gj: READ PASS
storaged-gj: ring map PASS
storaged-gj: RELEASE PASS
storaged-gj: free RELEASE soft PASS
storaged-gj: free own soft PASS
storaged-gj: free soft PASS
storaged-gj: live path PASS
```

| Marker | Meaning |
|--------|---------|
| `start` | `_start` banner |
| `CLAIM PASS` | Store door owned (`STORE_TOKEN`) |
| `WRITE PASS` | Sector write at LBA 2 (512 B) |
| `READ PASS` | Full-sector read-back matches pattern |
| `ring map PASS` | EXPORT + MAP_RING (+ RING_STATE + KICK) |
| `RELEASE PASS` | Door released |
| `live path PASS` | Aggregate freestanding success |

Soft / non-hard lines (not required by `smoke-all` alone):

```
storaged-gj: reclaim soft PASS | reclaim soft-skip
storaged-gj: CAP soft sectors=… | CAP soft-skip
storaged-gj: STATS soft blk=… scsi=… calls=… | STATS soft-skip
storaged-gj: QUEUE soft blk=… scsi=… rw=… own=… | QUEUE soft-skip
storaged-gj: multi-sector soft PASS | multi-sector soft-skip
storaged-gj: FLUSH soft PASS | FLUSH soft-skip
storaged-gj: RING_STATE soft free=… ready=… | RING_STATE soft-skip
storaged-gj: soft door PASS | soft door soft-skip
storaged-gj: ring EXPORT soft-skip
storaged-gj: ring MAP fail (soft)
storaged-gj: ring soft-skip (no virtio-blk)
storaged-gj: free RELEASE soft PASS | free RELEASE soft-skip
storaged-gj: free own soft PASS | free own soft-skip
storaged-gj: free soft PASS | free soft-skip
```

Kernel companion (not from this directory): `storaged: live spawn PASS` after
embedded spawn of `build/user/storaged.elf`.

### Host lines (`storaged_stub.c`)

```
storaged: start (software image until scsi door)
storaged: reclaim soft ok token=0x510e0002
storaged: CAP soft sectors=64
storaged: sector0 ok io=… magic="GreenJade…"
storaged: multi-sector soft ok lba=3 bytes=1024
storaged: FLUSH soft ok
storaged: STATS soft blk=… scsi=0 calls=…
storaged: QUEUE soft blk=… scsi=0 rw=… own=1
storaged: free soft ok soft_steps=…
storaged: door-shaped multi-lba ok sectors=64 io=…
storaged: PASS
```

Host path also exercises defensive rejects (null, zero length, non-sector
multiple, LBA past end, range overflow, over XFER_MAX, null CAP/STATS/QUEUE,
zero-token CLAIM) before successful I/O. Ring-map is freestanding-only (no
host virtq).

## Build

```sh
make storaged-gj   # → build/user/storaged.elf
make storaged      # → build/storaged (host smoke)
```

Manual freestanding (same `USER_CFLAGS` / link as other user ELFs; parent tree):

```sh
cc -std=c11 -ffreestanding -fno-builtin -fno-stack-protector \
   -fno-pic -mno-red-zone -msse2 -Wall -Wextra -Werror \
   -Iuser/libgj/include -g -O2 \
   -c -o build/user/storaged/storaged_gj.o user/storaged/src/storaged_gj.c
ld -nostdlib -static -T user/init/user.ld -z max-page-size=0x1000 \
   -o build/user/storaged.elf build/user/storaged/storaged_gj.o \
   build/user/libgj.a
```

Host smoke (hosted C; parent Makefile `storaged` target):

```sh
cc -std=c11 -Wall -Wextra -O2 -o build/storaged user/storaged/src/storaged_stub.c
```

## Store door ops (subset used here)

| op | name | args |
|----|------|------|
| 1  | STATS | `u32[3]` → blk_io, scsi_io, door_calls |
| 2  | CAP | `u64*` capacity sectors |
| 3  | READ | lba, buf, bytes (sector multiple) |
| 4  | WRITE | lba, buf, bytes (sector multiple) |
| 5  | CLAIM | token (non-zero; same-token reclaim soft) |
| 6  | RELEASE | token (unowned → soft free 0) |
| 7  | QUEUE_INFO | `u32[4]` → blk_io, scsi_io, door_rw, owned |
| 8  | FLUSH | fsync-shaped when transport ready |
| 9  | EXPORT_RING | `vq_export *` |
| 10 | KICK | — |
| 11 | RING_STATE | `u32[2]` → free, ready |
| 12 | MAP_RING | user VA base (page-aligned), export out |

Full surface (STATS/CAP/QUEUE_INFO/FLUSH, etc.): parent
`kernel/include/gj/store_door.h` / `user/libgj/include/gj/syscalls.h`.

## Plan

1. Door to `scsi_door_submit` / HBA host.
2. Simple FS (ustar or fat32) for `/`.
3. fsync barrier on writeback (`GJ_STORE_OP_FLUSH`).
