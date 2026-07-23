# hda_client (smoke)

Freestanding ring-3 HDA stream client for GreenJade. Exercises
`GJ_SYS_HDA_STREAM` (nr **94**) via `gj_hda_stream`:

**open → write PCM → start → tick → stats → underrun tick → close**, then a
**soft multi-op** second cycle that prints `q`/`p`/`u` stats (soft-skip on
OPEN/WRITE/START fail; never fails the live path).

This is kernel-door smoke — not Steam audio, PipeWire, or game PCM.

## License

Product code: **MIT OR Apache-2.0** (SPDX dual-license; same as libgj).

## Status

| Binary | Build | Role |
|--------|-------|------|
| `build/user/hda_client.elf` | `make hda_client-gj` | Freestanding live path (`hda_gj.c`) |

- Kernel interim: software HDA stream + optional ICH-class MMIO (`hda_stub`);
  syscall door `GJ_SYS_HDA_STREAM` (nr **94** — do not renumber).
- Matches the kernel `hda_stream_smoke` path from userland.
- This directory owns only `user/hda_client/**` — parent Makefile / kernel
  embed / boot spawn live outside this tree (scsi_mid-style).

## Layout

| Path | Role |
|------|------|
| `hda_gj.c` | Freestanding live path (`_start`) |
| `README.md` | This file |

Source of truth under this tree: `hda_gj.c` only.

## Freestanding path (`hda_gj.c`)

Live smoke via `gj_hda_stream` / `GJ_SYS_HDA_STREAM` (stereo 48 kHz 16-bit LE;
4 bytes/frame):

1. **OPEN** — ch=2, rate=48000, bits=16
2. **WRITE** — 256 B square-ish PCM (`HDA_SMOKE_BYTES`)
3. **START** — arm stream (software and/or SD0 DMA when MMIO present)
4. **TICK** — 64 frames; expect 256 B played
5. **STATS** — `q=0 p=256 u=0` after full consume
6. Empty-ring **TICK** → **STATS** with `u > 0` (underrun)
7. **CLOSE**
8. **Soft multi-op** — second open/write/start/tick/stats/close (128 B);
   soft-skip on OPEN/WRITE/START fail; STATS soft-miss still soft-PASS

### Hard vs soft

| Path | Failure | Exit |
|------|---------|------|
| Hard (steps 1–7) | OPEN/WRITE/START/TICK/STATS/underrun | `gj_exit(1)` after best-effort CLOSE |
| Soft multi-op | OPEN/WRITE/START | log + `soft multi-op soft-skip`; live path still PASS |
| Soft multi-op | STATS miss | log + CLOSE + `soft multi-op PASS` |

Boot embed (parent tree): `kernel/proc/hda_client_embed.S` (`.incbin` of
`build/user/hda_client.elf`). Kernel spawn markers include
`hda_client: live spawn PASS`.

## Smoke markers

Prefix-stable lines from `hda_gj.c` (do **not** rename PASS lines):

### Hard path (must PASS for green live path)

```text
hda_client-gj: start
hda_client-gj: OPEN PASS
hda_client-gj: WRITE PASS
hda_client-gj: START PASS
hda_client-gj: TICK played=256
hda_client-gj: TICK PASS
hda_client-gj: stats post-tick q=0 p=256 u=0
hda_client-gj: STATS PASS
hda_client-gj: stats underrun q=0 p=256 u=…   (u > 0)
hda_client-gj: underrun PASS
hda_client-gj: CLOSE PASS
hda_client-gj: live path PASS
```

| Marker | Meaning |
|--------|---------|
| `start` | `_start` entered |
| `OPEN PASS` | Stream opened (ch=2 rate=48000 bits=16) |
| `WRITE PASS` | Full 256 B PCM accepted |
| `START PASS` | Stream armed |
| `TICK played=256` / `TICK PASS` | Software consume of full buffer |
| `stats post-tick q=0 p=256 u=0` | Queue empty, all bytes played |
| `STATS PASS` | Post-tick stats check green |
| `stats underrun … u=…` | Empty-ring tick raised underrun |
| `underrun PASS` | Underrun counter non-zero |
| `CLOSE PASS` | Stream closed |
| `live path PASS` | Aggregate freestanding success (required) |

### Soft multi-op (never fails live path)

```text
hda_client-gj: soft multi-op start
hda_client-gj: soft TICK played=128
hda_client-gj: stats soft q=… p=… u=…
hda_client-gj: soft multi-op PASS
```

— or on soft OPEN/WRITE/START failure:

```text
hda_client-gj: soft OPEN|WRITE|START fail ret=<n>
hda_client-gj: soft multi-op soft-skip
```

| Marker | Meaning |
|--------|---------|
| `soft multi-op start` | Second cycle begins |
| `soft TICK played=…` | Soft tick result (diagnostic) |
| `stats soft q= p= u=` | Soft STATS snapshot |
| `soft multi-op PASS` | Soft cycle completed (or STATS soft-miss after CLOSE) |
| `soft multi-op soft-skip` | OPEN/WRITE/START failed; live path still green |

Hard failure lines: `hda_client-gj: <tag> fail ret=<n>` (then best-effort
CLOSE + `gj_exit(1)` on hard path only).

Kernel companion (not from this directory):

```text
hda_client: live elf thr=… entry=… cb=…
hda_client: live spawn PASS
```

## Ops (`gj_hda_stream(op, a1, a2, a3)`)

Door: `GJ_SYS_HDA_STREAM` = **94** (libgj / kernel native; keep stable).

| op | name | args / return |
|----|------|----------------|
| 0 | OPEN | ch, rate_hz, bits → 0 or -errno |
| 1 | WRITE | buf, nbytes, 0 → bytes written |
| 2 | START | — → 0 or -errno |
| 3 | TICK | frames, 0, 0 → bytes consumed |
| 4 | CLOSE | — → 0 |
| 5 | STATS | `u32[3]` → queued, played, underruns → 0 |

Defined in `user/libgj/include/gj/syscalls.h` (`GJ_HDA_OP_*` / `gj_hda_stream`).
Local `#ifndef` aliases in `hda_gj.c` mirror those values for self-description.

## Build

```sh
make hda_client-gj
# → build/user/hda_client.elf  (libgj + user/init/user.ld)
```

Manual freestanding (same `USER_CFLAGS` / link as `scsi_mid-gj`):

```sh
cc -std=c11 -ffreestanding -fno-builtin -fno-stack-protector \
   -fno-pic -mno-red-zone -msse2 -Wall -Wextra -Werror \
   -Iuser/libgj/include -g -O2 \
   -c -o build/user/hda_client/hda_gj.o user/hda_client/hda_gj.c
ld -nostdlib -static -T user/init/user.ld -z max-page-size=0x1000 \
   -o build/user/hda_client.elf build/user/hda_client/hda_gj.o \
   build/user/libgj.a
```

## Live embed (parent — docs only)

Parent tree owns:

- Makefile target `hda_client-gj` → `build/user/hda_client.elf`
- `kernel/proc/hda_client_embed.S` (`.incbin` + `gj_hda_client_elf_blob`)
- Boot spawn of the ELF (scsi_mid-style stack VA, e.g. `0x1190000`)

Do not treat this directory as owning Makefile or kernel edits.

### Expected boot log (client)

```text
hda_client-gj: start
…
hda_client-gj: live path PASS
hda_client: live spawn PASS
```
