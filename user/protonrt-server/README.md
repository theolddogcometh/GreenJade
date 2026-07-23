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
| `server.S` | Freestanding door loop + boot DEBUG_LOG marker |
| `README.md` | This file — markers, ABI, build |

Symbols exported for the kernel map path:

| Symbol | Meaning |
|--------|---------|
| `gj_protonrt_user_blob` | Start of RX code blob |
| `gj_protonrt_user_blob_end` | One past end (size = end − start, ≤ page) |

Map window (kernel `user_task.c`, outside this tree): code `@ 0x1200000`,
stack top `@ 0x1300000`.

## Door loop

```text
DEBUG_LOG "protonrt-server: door server up"
loop:
  IPC_RECV     → 128-byte gj_linux_regs buffer on stack
  PERSONALITY_SERVE(regs)   # interim cold policy in kernel
  IPC_REPLY(i64Ret from regs+56)
```

| Syscall | nr | Notes |
|---------|----|--------|
| `GJ_SYS_DEBUG_LOG` | 0 | Boot banner only |
| `GJ_SYS_IPC_RECV` | 11 | Block for next cold call |
| `GJ_SYS_PERSONALITY_SERVE` | 82 | Cold service (vfs_ram / net_lo / stubs) |
| `GJ_SYS_IPC_REPLY` | 12 | Return `i64Ret` to caller |

`gj_linux_regs.i64Ret` is at **offset 56**. On `IPC_RECV` failure the loop
`pause`s and retries (no tear-down).

## Smoke markers

### From this tree (`server.S`)

Emitted once on entry via `GJ_SYS_DEBUG_LOG`:

```
protonrt-server: door server up
```

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

The door-server banner and `pers:` / `user: personality` lines are bring-up
context; they are not currently hard-required by `smoke-all.sh`.

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
| Cold policy in userspace | **Interim** — `PERSONALITY_SERVE` still kernel policy |
| Wine-server A0 shape | **Kernel smoke** (`winesrv:` markers above) |
| Deck Top 50 / libprotonrt growth | See `docs/PROTON_PERSONALITY.md` |

Cap surface for production personality: cold door, vfs client, optional net
client — **no** IOMMU/IRQ (G-PERS-2).
