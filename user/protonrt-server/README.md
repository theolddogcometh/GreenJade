# protonrt-server (G-PERS door)

Freestanding ring-3 **personality door server** for GreenJade — the product
shape of G-PERS. The blob is assembled from `server.S`, linked into the
kernel image, and mapped into a **NATIVE** process at boot.

Cold Linux syscalls that leave the hot path become door calls; this process
`IPC_RECV`s, runs cold policy via `PERSONALITY_SERVE` (interim kernel policy
until full userspace cold service), and `IPC_REPLY`s.

License: **MIT OR Apache-2.0**. Clean-room; no GPL source.

## Layout

| Path | Role |
|------|------|
| `server.S` | Freestanding door loop + hard/soft DEBUG_LOG markers |
| `README.md` | This file — markers, ABI, build |

Symbols exported for the kernel map path:

| Symbol | Meaning |
|--------|---------|
| `gj_protonrt_user_blob` | Start of RX code blob |
| `gj_protonrt_user_blob_end` | One past end (size = end − start, ≤ 4 pages) |

Map window (kernel `user_task.c`, outside this tree): code `@ 0x1200000`,
stack top `@ 0x1300000`.

## Door loop

```text
DEBUG_LOG "protonrt-user: door server up"
fixed 128-byte regs frame once
loop:
  zero wire-size (64) regs frame          # soft hygiene
  IPC_RECV     → frame
  on soft fail → one-shot soft recv-miss, YIELD, retry
  PERSONALITY_SERVE(frame)                # interim cold policy in kernel
  on copy/arg soft fault (frame i64Ret==0, rax<0):
                 one-shot soft serve-miss (still reply)
  IPC_REPLY(rax from serve)               # match cold_personality_server
  on soft fail → one-shot soft reply-miss, YIELD, retry
  first full flight → one-shot soft serve ready
```

| Syscall | nr | Notes |
|---------|----|--------|
| `GJ_SYS_DEBUG_LOG` | 0 | Banner + soft one-shot markers |
| `GJ_SYS_YIELD` | 1 | Soft miss path (matches kernel yield) |
| `GJ_SYS_IPC_RECV` | 11 | Block for next cold call |
| `GJ_SYS_PERSONALITY_SERVE` | 82 | Cold service (vfs_ram / net_lo / stubs) |
| `GJ_SYS_IPC_REPLY` | 12 | Return serve `rax` to caller |

`gj_linux_regs` wire size is **64** bytes (`i64Ret` at **offset 56**). The
loop keeps a **128-byte** fixed frame for alignment/headroom. Soft state
(flags) lives in `r15`; the fixed frame pointer is `r14`; serve return is
`rbx` across the reply path (SYSCALL preserves those GPRs).

On soft `IPC_RECV` / `IPC_REPLY` failure the loop **never** exits: one-shot
marker (if any), `pause` + `YIELD`, then retry (no tear-down).

## Smoke markers

### Hard (from this tree — prefix-stable)

Emitted once on entry via `GJ_SYS_DEBUG_LOG`:

```
protonrt-user: door server up
```

### Soft (from this tree — optional; never hard-fail)

One-shot only; safe to ignore in `smoke-all.sh`:

| Marker | Meaning |
|--------|---------|
| `protonrt-user: soft serve ready` | First full `recv → serve → reply` completed |
| `protonrt-user: soft recv-miss` | First soft `IPC_RECV` failure (door not ready / peer / …) |
| `protonrt-user: soft reply-miss` | First soft `IPC_REPLY` failure (rare after live recv) |
| `protonrt-user: soft serve-miss` | First `PERSONALITY_SERVE` copy/arg soft fault (frame still 0, `rax < 0`); reply still sent |

### Soft inventory (Wave 14 exclusive deepen)

Asm-safe soft deepen: greppable comments + one-shot markers above; no new
hard exits. Source greppable stamps:

```text
protonrt-user: soft deepen wave=14
protonrt-user: soft inventory areas=6 wave=14
```

| Area | Meaning |
|------|---------|
| 1 up-banner | Hard `door server up` once |
| 2 fixed-frame | 128-byte regs frame allocated once (`r14`) |
| 3 wire-zero | Zero 64-byte wire frame before each `IPC_RECV` |
| 4 yield-miss | Soft miss → `pause` + `YIELD` (no busy-spin alone) |
| 5 one-shot-soft | Ready / recv-miss / reply-miss / serve-miss (bits in `r15`) |
| 6 never-exit-soft | Never `EXIT` / tear-down on soft error |

Soft deepenings stay product-safe:

- Fixed frame (no per-iter `sub`/`add` thrash)
- Zero before each recv (no stale `i64Ret` / NR)
- Reply uses serve `rax` (same value as kernel `cold_personality_server`)
- Soft miss → `YIELD` (not busy-spin alone)
- Never `EXIT` on soft error
- Wave 14 inventory is comment + soft-flag documentation only (asm-safe)

Kernel map/schedule companions (not from this directory):

```
user: personality server mapped @0x… (…) native
pers: userspace door server scheduled (G-PERS)
```

Fallback if ring-3 map/create fails:

```
pers: user map/create failed — kernel fallback
pers: kernel door server parked (fallback)
```

### Wineserver-shaped A0 (related Proton smoke)

A0 wine-server shape is exercised in-kernel (`proton_a0_wineserver_demo` in
`kernel/main.c`) after the personality is attached. Same product story:
**two processes**, **named shm**, **PA-shared futex**, **socketpair**
request/reply, **exception port**. Greppable `winesrv:` lines:

| Marker | Meaning |
|--------|---------|
| `winesrv: A0 two-process demo start` | Demo begin |
| `winesrv: maps srv=… cli=… (shared frames)` | Named memobj mapped into both AS |
| `winesrv: thr srv=… cli=… sv=…` | Server/client threads + socketpair FDs |
| `winesrv: server futex_wait => …` | Cross-proc futex wait (PA key) |
| `winesrv: client futex_wake => … (shared PA)` | Client wake on shared word |
| `winesrv: server got req n=…` | socketpair request received |
| `winesrv: client reply 'OK'` | socketpair reply seen |
| `winesrv: A0 demo PASS` | Success (`done=1`) |
| `winesrv: A0 demo TIMEOUT` | Failure (yield budget exhausted) |

Failure / soft-error forms:

```
winesrv: shm create failed
winesrv: map failed srv=… cli=…
winesrv: socketpair failed
winesrv: server key fail
```

Exception-port companion (same demo path):

```
except: port smoke PASS
```

`scripts/smoke-all.sh` **requires** (Multiboot log):

```
winesrv: A0 demo PASS
except: port smoke PASS
```

The door-server hard banner, soft one-shots, and `pers:` / `user: personality`
lines are bring-up context; they are not currently hard-required by
`smoke-all.sh`.

## Build

No separate `make` target — `server.S` is in the kernel `S_SRCS` list and
links into `build/greenjade.elf`.

```text
make                              # full kernel (includes this blob)
make build/user/protonrt-server/server.o   # object only
```

Object path: `build/user/protonrt-server/server.o`.

Cold Linux helpers used by `PERSONALITY_SERVE` / cold IPC live in
`user/libprotonrt/` (compiled into the kernel for smoke); they are **not**
part of this directory.

## Product notes

| Topic | Status |
|-------|--------|
| G-PERS door loop in ring-3 | **Landed** (`server.S`) |
| Door loop soft deepen | **Landed** (fixed frame, yield miss, one-shot soft markers) |
| Wave 14 soft inventory | **Landed** (greppable comments + `SOFT_INV_WAVE=14` / areas=6) |
| Cold policy in userspace | **Interim** — `PERSONALITY_SERVE` still kernel policy |
| Wine-server A0 shape | **Kernel smoke** (`winesrv:` markers above) |
| Deck Top 50 / libprotonrt growth | See `docs/PROTON_PERSONALITY.md` |

Cap surface for production personality: cold door, vfs client, optional net
client — **no** IOMMU/IRQ (G-PERS-2).
