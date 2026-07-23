# hda_client (smoke)

Freestanding ring-3 HDA stream client for GreenJade. Exercises
`GJ_SYS_HDA_STREAM` (nr **94**) via `gj_hda_stream`:

**open → write PCM → start → tick → stats → underrun tick → close**, then a
**deepened soft suite** that exercises multi-op, partial-tick, multi-write,
soft underrun, mono format, reclaim re-OPEN, and reject probes (soft-skip on
fail; never fails the live path).

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
8. **Soft suite** — multi-op + partial-tick + multi-write + soft underrun +
   mono + reclaim + reject (each soft-skip independent; never hard-fails)

### Hard vs soft

| Path | Failure | Exit |
|------|---------|------|
| Hard (steps 1–7) | OPEN/WRITE/START/TICK/STATS/underrun | `gj_exit(1)` after best-effort CLOSE |
| Soft suite sub-step | any sub-step fail | log + `… soft-skip`; live path still PASS |
| Soft suite aggregate | all sub-steps soft-skip | `soft suite soft-skip`; live path still PASS |
| Soft suite aggregate | ≥1 sub-step PASS | `soft suite PASS` |

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

### Soft suite (never fails live path)

```text
hda_client-gj: soft suite start
hda_client-gj: soft multi-op start
hda_client-gj: soft TICK played=128
hda_client-gj: stats soft q=… p=… u=…
hda_client-gj: soft multi-op PASS
hda_client-gj: soft partial TICK1 played=64
hda_client-gj: stats soft partial mid q=128 p=… u=…
hda_client-gj: soft partial TICK2 played=128
hda_client-gj: soft partial-tick PASS
hda_client-gj: stats soft multi-write pre q=128 …
hda_client-gj: soft multi-write PASS
hda_client-gj: stats soft underrun q=0 p=… u=…   (u > 0)
hda_client-gj: soft underrun PASS
hda_client-gj: soft mono TICK played=64
hda_client-gj: soft mono PASS
hda_client-gj: stats soft reclaim q=0 p=0 u=…
hda_client-gj: soft reclaim PASS
hda_client-gj: soft reject PASS
hda_client-gj: soft suite PASS
```

— or on soft sub-step failure (example):

```text
hda_client-gj: soft OPEN|WRITE|START fail ret=<n>
hda_client-gj: soft multi-op soft-skip
…
hda_client-gj: soft suite PASS          (if any other sub-step greened)
hda_client-gj: soft suite soft-skip     (if every sub-step soft-skipped)
```

| Marker | Meaning |
|--------|---------|
| `soft suite start` | Soft orchestrator begins |
| `soft multi-op start` | Second full open/write/start/tick/stats cycle |
| `soft TICK played=…` | Soft multi-op tick result |
| `stats soft q= p= u=` | Soft multi-op STATS snapshot |
| `soft multi-op PASS` | Multi-op cycle completed (STATS soft-miss still PASS) |
| `soft multi-op soft-skip` | OPEN/WRITE/START failed in multi-op |
| `soft partial-tick PASS` | Mid-buffer TICK left residual queue, then drain |
| `soft multi-write PASS` | Two WRITE chunks then single TICK |
| `soft underrun PASS` | Soft empty-ring underrun counter non-zero |
| `soft mono PASS` | Mono 16-bit 48 kHz short path greened |
| `soft reclaim PASS` | Re-OPEN while open reset queue/played |
| `soft reject PASS` | Invalid OPEN (ch=0 / bits=24) rejected by door |
| `… soft-skip` | That sub-step failed; live path still green |
| `soft suite PASS` | ≥1 soft sub-step greened |
| `soft suite soft-skip` | All soft sub-steps soft-skipped |

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

### Soft suite sub-paths (door surface deepen)

| Soft step | Door ops exercised | Expect |
|-----------|--------------------|--------|
| multi-op | OPEN→WRITE 128 B→START→TICK 32→STATS→CLOSE | full soft cycle |
| partial-tick | WRITE 192 B, TICK 16 then remainder | mid `q=128`, end `q=0` |
| multi-write | WRITE 64 + WRITE 64, pre-STATS `q=128`, TICK 32 | both chunks play |
| underrun | full soft consume + empty TICK | `u > 0` |
| mono | OPEN ch=1 bits=16, WRITE 64 B, TICK 32 | mono frame size |
| reclaim | OPEN→WRITE→re-OPEN→STATS | `q=0 p=0` after re-OPEN |
| reject | OPEN ch=0; OPEN bits=24 | both non-zero (INVAL) |

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
hda_client-gj: CLOSE PASS
hda_client-gj: soft suite start
…
hda_client-gj: soft suite PASS
hda_client-gj: live path PASS
hda_client: live spawn PASS
```
