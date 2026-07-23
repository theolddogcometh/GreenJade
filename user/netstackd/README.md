# netstackd (product)

Userspace network stack host for GreenJade — freestanding live daemon + host
UDP-echo smoke.

## License

Product code: **MIT OR Apache-2.0** (SPDX dual). Pure C freestanding product
path; **no GPL**.

## Status

| Binary | Build | Role |
|--------|-------|------|
| `build/user/netstackd.elf` | `make netstackd-gj` | Freestanding product daemon (net door, TCP multi-seg, virtio ring + soft door) |
| `build/netstackd` | `make netstackd` | Host POSIX UDP-echo smoke |

- Kernel (interim doors): `net_lo` (UDP/dgram), **`net_tcp`** (IPv4 TCP over virtio-net + loopback pairs), `net_eth` (ARP/ICMP/UDP echo).
- Freestanding `netstackd-gj`: door CLAIM, dgram echo, **TCP listen/accept/send/recv**, **multi-segment TX/RX**, virtio ring path, **soft door deepen**.
- Product direction: own more of the stack in this process; kernel retains IRQ/DMA windows.
- **Live product path:** embedded ELF spawned at boot (`netstackd: live spawn PASS` + freestanding `netstackd-gj: live path PASS`).

## Layout

| Path | Role |
|------|------|
| `src/netstackd_gj.c` | Freestanding live path (`_start`) — pure C11, dual SPDX |
| `src/netstackd.c` | Host A1 software loopback UDP-echo smoke (`main`) |
| `README.md` | This file — markers, TCP multi-seg, door bounce, soft door |

Source of truth under this tree: `src/netstackd_gj.c` + `src/netstackd.c`.

Door message layouts (`vq_export`, `vq_dma`, virtq desc/avail, `TCP_STATS`
`u32[4]` packing) are **kernel ABI mirrors** — field order must not change here.

## TCP path

`SOCK_STREAM` via net door → `net_tcp` (FDs 96+):

- Loopback `connect` ↔ local `LISTEN` + `ACCEPT`
- Virtio-net SYN demux (sshd :22)
- **Multi-segment** send (`> MSS`, MSS = 1024): kernel chunks TX; freestanding drains RX in a loop and checks payload integrity
- Peer **window**, **retransmit** of last segment on `net_tcp_poll`
- Stats: accepts, segs, rx/tx bytes, retransmits (`GJ_NET_OP_TCP_STATS`)

### Door bounce: `NET_XFER_MAX` 4096

Kernel net door (`kernel/net/net_door.c`) uses a stack bounce buffer for
`GJ_NET_OP_SEND` / `GJ_NET_OP_RECV` (and virtio frame TX/RX ops):

```c
#define NET_XFER_MAX 4096u   /* SEND/RECV door bounce */
```

Userspace freestanding mirrors this as `GJ_NET_XFER_MAX` (defensive only;
kernel still clamps):

```c
#define GJ_NET_XFER_MAX  4096u
#define GJ_TCP_MSS       1024u
#define GJ_MULTI_CB      3000u   /* > MSS, ≤ XFER_MAX; ≥3 segs */
```

- User payload is `copy_from_user` / `copy_to_user` through `u8 aBuf[NET_XFER_MAX]`.
- Lengths above 4096 are clamped to `NET_XFER_MAX` (one door call ≤ 4 KiB).
- **Must cover multi-seg TCP smokes**: freestanding bulk is **3000 B** (`GJ_MULTI_CB`) in a single `SEND`, which is `> MSS` (1024) but `< NET_XFER_MAX`, so the full bulk fits one bounce and `net_tcp_send` chunks by MSS internally into **≥3** payload segments.
- Eth/virtio frame path still clamps TX to 1514 inside the driver; the 4 KiB ceiling is for the door transfer, not L2 MTU.
- Top50 note: net door **4K xfer** = `NET_XFER_MAX` 4096 SEND/RECV bounce enabling multi-seg TCP bulk via door.

### Multi-segment messaging (freestanding client path)

`netstackd_gj` exercises product-shaped bulk transfer for sshd / Top50:

1. Bind/listen **:7777**, loopback connect + accept
2. Small single-segment smoke (`"gj-tcp-ip"`) → `TCP path PASS`
3. Fill **3000** bytes (pattern `i & 0xff`) and `SEND` once through the **4 KiB door bounce** — kernel emits **≥3** payload segments (`ceil(3000/1024)`)
4. Peer `RECV` loop until full length (short reads OK; remaining cap + iteration ceiling); byte-compare TX vs RX
5. `GJ_NET_OP_TCP_STATS` reports segs/rtx/rx/tx (`[1]=(rtx<<16)|segs`); multi-seg PASS is payload integrity of one `SEND` > MSS

Userspace defensive bounds (do not replace kernel clamps):

- Compile-time: `GJ_MULTI_CB > MSS` and `GJ_MULTI_CB ≤ GJ_NET_XFER_MAX`
- RECV request = remaining `aRx` capacity, also capped to `GJ_NET_XFER_MAX`
- RECV return `> remaining` → fail drain (never write past buffer)
- Drain iteration ceiling (`GJ_MULTI_MAX_RECV`) — no unbounded loop
- Short/failed multi `SEND` skips integrity PASS

## Soft door path (freestanding deepen)

Soft helpers deepen door surface **beyond** hard CLAIM / DGRAM / TCP / virtio
markers. Pattern matches `sessiond` soft health and `vfsd` soft door:

- **Never** aborts `live path PASS` (hard path still keyed on DGRAM `RECV n > 0`)
- **Never** renames hard PASS substrings grepped by `scripts/smoke-all.sh`
- Each sub-step is `… PASS` or `… soft-skip` (empty RX is `VIRTIO_RX soft-empty`, still a green probe)
- Aggregate `soft door PASS` if any sub-step greened; else `soft door soft-skip`

Soft suite (while CLAIM held, after hard virtio markers):

| Step | Op / check | Soft green when |
|------|------------|-----------------|
| reclaim | same-token `CLAIM` | door returns 0 (idempotent) |
| POLL | `GJ_NET_OP_POLL` | door returns 0 |
| STATS | eth `u32[4]` arp/udp/icmp/calls | door 0 and `calls != 0` |
| QUEUE_INFO | `u32[5]` tx/rx/ready/owned/vq | door 0 and `owned != 0` |
| dgram echo | compare TX vs RX payload | lengths match + byte-eq |
| TCP_STATS | door success from multi-seg smoke | door returned 0 |
| VIRTIO_RX | drain cap 8 frames | op accepted (`PASS` if bytes, `soft-empty` if 0); NODEV → skip |
| re-MAP | `MAP_RING` same `RING_VA` | hard path mapped and re-MAP 0 |
| KICK | `GJ_NET_OP_KICK` which=TX | door 0; NODEV → skip |
| RING_STATE | always-fill soft | door 0 (zeros OK without virtio) |

Post-RELEASE soft free:

- Second `RELEASE` while unowned → kernel soft no-op 0 → `RELEASE free soft PASS`

## Smoke markers

Greppable product lines for **netstackd live**. Prefix-stable — do not rename
PASS lines. `scripts/smoke-all.sh` and `scripts/gj-product-summary.sh` key off
these substrings.

### Freestanding hard path (`netstackd_gj.c`)

`scripts/smoke-all.sh` requires these **substrings** in the Multiboot log:

```text
netstackd-gj: avail push PASS
netstackd-gj: user ring PASS
netstackd-gj: live path PASS
netstackd: live spawn PASS
```

Typical freestanding success sequence (order fixed where noted; soft lines optional):

```text
netstackd-gj: start
netstackd-gj: TCP path PASS
netstackd-gj: TCP multi-segment PASS
netstackd-gj: virtio queue PASS
netstackd-gj: avail push PASS
netstackd-gj: user ring PASS
netstackd-gj: soft door start
netstackd-gj: reclaim soft PASS
netstackd-gj: POLL soft PASS
netstackd-gj: STATS soft PASS
netstackd-gj: QUEUE_INFO soft PASS
netstackd-gj: dgram echo soft PASS
netstackd-gj: TCP_STATS soft PASS
netstackd-gj: VIRTIO_RX soft-empty          (or soft PASS / soft-skip)
netstackd-gj: re-MAP soft PASS              (or soft-skip if ring not mapped)
netstackd-gj: KICK soft PASS                (or soft-skip)
netstackd-gj: RING_STATE soft PASS
netstackd-gj: soft door PASS
netstackd-gj: RELEASE free soft PASS
netstackd-gj: live path PASS
```

| Marker | Meaning |
|--------|---------|
| `start` | `_start` banner |
| `TCP path PASS` | Single-segment loopback STREAM smoke (`"gj-tcp-ip"`) |
| `TCP multi-segment PASS` | 3000 B bulk > MSS integrity (sshd / Top50 path) |
| `virtio queue PASS` | Door `VIRTIO_TX` accepted a frame |
| `avail push PASS` | `AVAIL_PUSH` + ring state soft green (**hard**; smoke-all) |
| `user ring PASS` | UDX user desc/avail programming path (**hard**; smoke-all) |
| `soft door PASS` | Aggregate soft suite (≥1 sub-step greened) |
| `soft door soft-skip` | No soft sub-step greened (live path still green) |
| `RELEASE free soft PASS` | Second RELEASE while free (kernel soft no-op) |
| `live path PASS` | Aggregate freestanding success (required for live) |

Failure / soft-miss lines (same prefix):

```text
netstackd-gj: CLAIM fail
netstackd-gj: socket fail
netstackd-gj: TCP multi-segment FAIL
netstackd-gj: recv fail
netstackd-gj: reclaim soft-skip
netstackd-gj: POLL soft-skip
netstackd-gj: STATS soft-skip
netstackd-gj: QUEUE_INFO soft-skip
netstackd-gj: dgram echo soft-skip
netstackd-gj: TCP_STATS soft-skip
netstackd-gj: VIRTIO_RX soft-skip
netstackd-gj: re-MAP soft-skip
netstackd-gj: KICK soft-skip
netstackd-gj: RING_STATE soft-skip
netstackd-gj: soft door soft-skip
netstackd-gj: RELEASE free soft-skip
```

Product summary soft grep (`gj-product-summary.sh`):

```text
TCP multi-segment PASS
```

### Kernel live spawn (not from this directory)

Embedded via `kernel/proc/netstackd_embed.S` and spawned from `kernel/main.c`:

```text
netstackd: live elf thr=…
netstackd: live spawn PASS
```

| Marker | Meaning |
|--------|---------|
| `live elf thr=…` | ELF load + thread created |
| `live spawn PASS` | Boot-time live spawn green (**netstackd live**) |

### Host path (`netstackd.c`)

```text
make netstackd && ./build/netstackd
```

```text
netstackd: start (software lo until virtio door)
netstackd: udp echo ok tx=… rx=… echo=…
netstackd: door-shaped sockets ok
netstackd: PASS
```

`scripts/smoke-all.sh` greps host: `netstackd: PASS`.

## Virtio / UDX ring

After TCP smokes, freestanding path programs the TX queue:

- `VIRTIO_TX` / `QUEUE_INFO`
- `AVAIL_PUSH` + `USED_REAP` + `RING_STATE`
- UDX user ring: `EXPORT_RING` → `MAP_RING` / `MAP_DMA` → `BOUNCE_FILL` → `DESC_ALLOC` → userspace desc/avail write → `USER_AVAIL`

Soft deepen (after hard markers, still claimed):

- `VIRTIO_RX` drain (empty OK)
- same-VA `MAP_RING` re-MAP (map reclaim soft)
- `KICK` + `RING_STATE` soft probes

Ring export structs in `netstackd_gj.c` mirror kernel layouts (ABI-stable).

## Build

```
make netstackd-gj    # → build/user/netstackd.elf
make netstackd       # → build/netstackd (host)
```

Kernel live spawn embeds `netstackd.elf` via `kernel/proc/netstackd_embed.S`
(not part of this directory).
