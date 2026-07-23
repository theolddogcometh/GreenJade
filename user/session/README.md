# sessiond (product)

Userspace session / compositor owner for GreenJade — freestanding ELF + host A1 smoke.

## License

Product code: **MIT OR Apache-2.0** (`SPDX-License-Identifier: MIT OR Apache-2.0`).
No GPL in this tree.

## Status

| Binary | Build | Role |
|--------|-------|------|
| `build/user/sessiond.elf` | `make sessiond-gj` | Freestanding product daemon (session door ownership) |
| `build/sessiond` | `make sessiond` | Host POSIX A1 smoke (scanout + input fanout + soft health) |

- Kernel interim: `session_door` + compositor / input hub (`kernel/session/`).
- Product: this process **claims** the session door, presents a user FB, runs soft health helpers, then releases.
- Host CI simulates present + input fanout + door protocol shapes without the kernel door.

## Layout

| Path | Role |
|------|------|
| `sessiond_gj.c` | Freestanding live path (`_start`) — CLAIM → present → soft health → RELEASE |
| `sessiond.c` | Host A1 smoke (libc): scanout + input ring + protocol shapes + soft health |
| `compositor_stub.c` | Thin pointer — full path lives in `sessiond` / `sessiond_gj` |
| `README.md` | This file |

Source of truth under this tree: `sessiond_gj.c` (live) and `sessiond.c` (host).
Do not edit kernel/session, Makefile, or other user trees from this workstream.

## Freestanding path (`sessiond_gj.c`)

Live smoke via `gj_session` / `GJ_SYS_SESSION`:

1. **CLAIM** session door (token `0xc0ffe1`) — kernel defers interim auto-scanout
2. **DISPLAY_INFO** — session door first; GPU fallback if needed (**soft**)
3. **PRESENT_FB** — 64×64 BGRA jade tile through the door (**hard**)
4. **STATS** — optional soft read of door counters (`p/in/c/f/own`); early **user-fb** bit
5. **Soft health** (deepened, all non-fatal):
   - DISPLAY_INFO recheck with dimensions
   - MAP_SCANOUT geometry (`WxH stride=S`)
   - INPUT_POLL + INPUT_POP drain (`drained=` / `pend=`)
   - same-token **reclaim** CLAIM (door STATS bit19)
   - STATS flags: ready / input-ready / ownership / user-fb / multi-frame / reclaim
   - interim PRESENT of kernel scanout
   - second + third PRESENT_FB tints (multi-frame bookkeeping)
   - post multi-frame STATS recheck
6. **RELEASE** + soft free recheck (unowned STATS + free RELEASE) + `ownership path PASS`

Boot embed: `kernel/proc/sessiond_embed.S` (`.incbin` of the ELF; outside this directory).

## Smoke markers

### Freestanding (`sessiond_gj.c`)

`scripts/smoke-all.sh` requires this substring in the boot log:

```text
sessiond-gj: ownership path PASS
```

Typical success sequence (prefix-stable; do not rename **hard** PASS lines):

```text
sessiond-gj: start
sessiond-gj: CLAIM PASS
sessiond-gj: DISPLAY_INFO PASS
sessiond-gj: PRESENT_FB PASS
sessiond-gj: STATS soft p=… in=… c=… f=… own=0x…
sessiond-gj: user-fb soft
sessiond-gj: soft health start
sessiond-gj: DISPLAY_INFO soft WxH
sessiond-gj: MAP_SCANOUT soft WxH stride=S
sessiond-gj: INPUT soft drained=N pend=P
sessiond-gj: reclaim soft
sessiond-gj: STATS soft …
sessiond-gj: ready soft
sessiond-gj: ownership soft
sessiond-gj: user-fb soft
sessiond-gj: reclaim flag soft
sessiond-gj: soft PRESENT PASS
sessiond-gj: soft PRESENT_FB PASS
sessiond-gj: soft PRESENT_FB2 PASS
sessiond-gj: multi-frame soft
sessiond-gj: soft health PASS
sessiond-gj: RELEASE PASS
sessiond-gj: free soft
sessiond-gj: free RELEASE soft
sessiond-gj: ownership path PASS
```

| Marker | Kind | Meaning |
|--------|------|---------|
| `CLAIM PASS` | hard | Door ownership acquired (token held) |
| `DISPLAY_INFO PASS` | soft-ok | Session door returned width/height |
| `DISPLAY_INFO soft` | soft | Door miss; GPU query / skip (non-fatal) |
| `DISPLAY_INFO soft WxH` | soft | Soft-health recheck with dimensions |
| `DISPLAY_INFO soft-skip` | soft | Soft-health display recheck unavailable |
| `PRESENT_FB PASS` | hard | User BGRA FB presented via door |
| `STATS soft p=…` | soft | Door stats snapshot (presents/in/calls/flags/owner) |
| `MAP_SCANOUT soft WxH stride=S` | soft | Scanout VA hint + geometry OK |
| `MAP_SCANOUT soft-skip` | soft | Map unavailable (NODEV etc.) |
| `INPUT soft drained=N pend=P` | soft | INPUT_POLL + INPUT_POP drain OK (empty fine) |
| `INPUT soft-skip` | soft | Poll failed; non-fatal |
| `reclaim soft` | soft | Same-token CLAIM idempotent (door reclaim) |
| `reclaim soft-skip` | soft | Reclaim call failed / no token |
| `reclaim flag soft` | soft | STATS bit19 set after reclaim |
| `ready soft` | soft | STATS bit0 compositor ready |
| `input-ready soft` | soft | STATS bit1 input hub ready |
| `ownership soft` | soft | STATS owned bit + token match while claimed |
| `user-fb soft` | soft | STATS bit17 any user PRESENT_FB success |
| `multi-frame soft` | soft | STATS bit18 user presents ≥ 2 |
| `multi-frame soft-skip` | soft | Multi-frame bit not yet set |
| `drop sticky soft` | soft | STATS bit16 input drop observed (info only) |
| `soft PRESENT PASS` | soft | Interim PRESENT of kernel scanout |
| `soft PRESENT soft-skip` | soft | Interim present unavailable |
| `soft PRESENT_FB PASS` | soft | Second tinted PRESENT_FB OK |
| `soft PRESENT_FB2 PASS` | soft | Third tinted PRESENT_FB (multi-frame deepen) |
| `soft health PASS` | soft | At least one soft helper succeeded |
| `soft health soft-skip` | soft | No soft helper succeeded (live path still green) |
| `RELEASE PASS` | hard | Ownership released with matching token |
| `free soft` | soft | Post-RELEASE STATS unowned |
| `free RELEASE soft` | soft | RELEASE when already free returns 0 |
| `ownership path PASS` | hard | Full claim→present→release green (**required**) |

Failure lines use the same prefix with **fail** / **fail ret=** (e.g. `CLAIM fail`,
`PRESENT_FB fail ret=…`, `RELEASE fail`). Hard fail after claim releases the token.

Kernel spawn markers (not from this directory):

```text
sessiond: live spawn PASS
session_door: PRESENT_FB PASS
session_door: ownership PASS
session_door: RELEASE PASS
```

### Host (`sessiond.c`)

```text
make sessiond && ./build/sessiond
```

Host success markers:

```text
sessiond: PRESENT_FB path PASS
sessiond: ownership PASS
sessiond: soft health PASS
sessiond: RELEASE PASS
sessiond: compositor path PASS
sessiond: PASS
```

`scripts/smoke-all.sh` greps `sessiond: PASS` for the host binary.

Soft host lines (never fail `sessiond: PASS`):

```text
sessiond: soft health start
sessiond: DISPLAY_INFO soft …
sessiond: MAP_SCANOUT soft …
sessiond: INPUT_POLL soft …
sessiond: INPUT_POP soft drained=…
sessiond: STATS soft p=… in=… c=… f=… own=0x…
sessiond: ownership soft
sessiond: soft PRESENT_FB PASS
sessiond: soft health PASS
```

### Compositor stub (`compositor_stub.c`)

Prints product pointers only; not grepped by smoke-all.

```text
session-compositor: use sessiond (make sessiond)
session-compositor: freestanding path sessiond_gj (make sessiond-gj)
session-compositor: door ops PRESENT..MAP_SCANOUT = 1..9 (stable)
session-compositor: soft reclaim + multi-frame + free path
```

## Session door ops (subset used here)

Numbers are **stable** — extend carefully; do not renumber existing ops.
See `kernel/include/gj/session_door.h` and `user/libgj/include/gj/syscalls.h`.

| op | name | args | freestanding | host sim |
|----|------|------|--------------|----------|
| 1  | PRESENT | — | soft | (via present helper) |
| 2  | DISPLAY_INFO | `u32[2]` → w, h | soft (+ soft recheck) | hard shape |
| 3  | INPUT_POLL | — | soft | soft |
| 4  | INPUT_POP | → event; ret 1/0 | soft | hard fanout |
| 5  | STATS | `u32[5]` → presents, in_push, calls, flags, owner | soft (flags deep) | hard ownership check |
| 6  | PRESENT_FB | w, h, user BGRA ptr (stride = w×4) | **hard** + soft multi | hard |
| 7  | CLAIM | token (non-zero) | **hard** + soft reclaim | hard |
| 8  | RELEASE | matching token | **hard** + free soft | hard |
| 9  | MAP_SCANOUT | va hint + `u32[3]` w,h,stride | soft | soft |

STATS flags: bit0 compositor ready, bit1 input ready, bit2 owned,
bits 8..15 pending input (0..255), bit16 drop sticky, bit17 user PRESENT_FB,
bit18 multi-frame (≥2 user presents), bit19 reclaim soft observed.
Token used by product smokes: `0xc0ffe1`.

## Soft health helpers

Freestanding soft health **deepens** door surface beyond the hard
claim/present/release path without failing the required smoke marker:

- DISPLAY_INFO recheck (dimensions)
- MAP_SCANOUT (scanout geometry with WxH/stride)
- INPUT_POLL / INPUT_POP drain + pending peek
- Same-token CLAIM reclaim + STATS bit19
- STATS ownership / ready / input-ready / user-fb / multi-frame flags
- Optional interim PRESENT
- Second + third PRESENT_FB tints (multi-frame shape)
- Post-RELEASE free soft (unowned + free RELEASE)

Soft failures print `*-skip` / `fail ret=` and continue.

Host soft health remains a lighter protocol-shape suite (map / input / stats /
second present).

## Build

```text
make sessiond-gj   # → build/user/sessiond.elf
make sessiond      # → build/sessiond (host A1 smoke)
```

Source of truth for freestanding code under this tree: `sessiond_gj.c` only.
Host smoke: `sessiond.c`. Compositor pointer: `compositor_stub.c`.
