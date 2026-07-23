# scsi_mid (product)

Userspace SCSI mid-layer for GreenJade — pure C11 CDB builders,
`scsi_mid_submit`, host skeleton, and a freestanding live smoke that talks to
the kernel `scsi_door` over `GJ_SYS_SCSI`.

## License

| Field | Value |
|-------|--------|
| **License** | **MIT OR Apache-2.0** (dual; choose either) |
| **Language** | Pure C11 only — clean-room, **no Linux kernel source** |
| **SPDX** | `MIT OR Apache-2.0` on every `.c` / `.h` in this tree |
| **Tree** | See repo root `LICENSE` for full dual-license text |

Product code under `user/scsi_mid/**` is dual-licensed pure C. No GPL-only
headers, no Linux mid-layer paste.

## Status

| Binary | Build | Role |
|--------|-------|------|
| `build/user/scsi_mid.elf` | `make scsi_mid-gj` | Freestanding product path (`scsi_mid_gj.c`) |
| host smoke | link `server.c` + `src/cdb.c` | Host skeleton (soft PASS without kernel door) |

- Kernel interim: `scsi_door` → virtio-scsi HBA; CDB helpers may also live in
  `kernel/fs/scsi_mid.c` until full mid ownership moves here.
- Product direction: this process owns CDB build, LUN routing, sense, and
  timeouts; kernel retains DMA/IRQ windows only.
- This directory owns only `user/scsi_mid/**` — parent Makefile / kernel embed /
  boot spawn live outside this tree.

## Layout

| Path | Role |
|------|------|
| `include/scsi_mid.h` | Public CDB / `scsi_io` / sense API (pure C + `extern "C"`) |
| `src/cdb.c` | CDB builders + `scsi_mid_submit` (GJ freestanding → door; host → `-1`) |
| `src/scsi_mid_gj.c` | Freestanding live path (`_start`, direct `gj_scsi`) |
| `server.c` | Host skeleton server (soft PASS when submit is ENOSYS-shaped) |
| `README.md` | This file |

Source of truth under this tree: pure C sources above (no assembly in-tree).

## CDB / server path

```text
Host skeleton (server.c)
  scsi_cdb_inquiry(...)     → pack struct scsi_io
  scsi_mid_submit(&io)      → cdb.c host stub → -1 (ENOSYS-shaped)
  printf soft PASS

Product library (cdb.c, freestanding)
  scsi_cdb_*                → fill scsi_cdb (SPC/SBC big-endian layout)
  scsi_mid_submit           → decode opcode → gj_scsi(GJ_SCSI_OP_*)
                            → GJ_SYS_SCSI → scsi_door → virtio-scsi

Live smoke (scsi_mid_gj.c)
  gj_scsi(READY/INQUIRY/…)  → same door, tight freestanding loop
```

| Builder | Opcode | CDB length | Notes |
|---------|--------|------------|--------|
| `scsi_cdb_inquiry` | `0x12` | 6 | EVPD, page, alloc length (BE) |
| `scsi_cdb_read_capacity10` | `0x25` | 10 | Simple capacity probe |
| `scsi_cdb_read10` | `0x28` | 10 | LBA + block count (BE) |
| `scsi_cdb_write10` | `0x2A` | 10 | Same layout as READ10 |

`scsi_mid_submit` maps those CDBs to door ops when `SCSI_HAS_SYS` is on
(GJ freestanding). Host CI keeps `SCSI_HAS_SYS=0` so submit never needs a
kernel.

## Freestanding path (`scsi_mid_gj.c`)

Live smoke via `gj_scsi` / `GJ_SYS_SCSI` → `scsi_door` → virtio-scsi:

1. **READY** — probe transport (`GJ_SCSI_OP_READY` → `1` if HBA ready)
2. **INQUIRY** — 36-byte standard inquiry; log vendor id (SPC bytes 8..15) when present
3. **READ_CAP** — optional capacity (`last_lba`, `block_len`) when transport allows
4. **READ10** — optional single-block read (LBA 0, 512 B) after successful READ_CAP
5. Emit **live path PASS** and `gj_exit(0)`

### Soft vs hard outcomes

| Condition | Result |
|-----------|--------|
| READY ≠ 1 (no virtio-scsi in this QEMU config) | Soft PASS — exit 0, no hard fail |
| READY ok, INQUIRY fails | Hard fail — exit 1 |
| INQUIRY ok; READ_CAP / READ10 skip or soft | Still live path PASS (door exercised) |

Boot embed (parent tree): `kernel/proc/scsi_mid_embed.S` (`.incbin` of the ELF).
Kernel spawn markers (outside this directory) include `scsi_mid: live spawn PASS`.

## Smoke markers

Freestanding success / soft paths emit greppable lines (prefix `scsi_mid-gj:`):

```
scsi_mid-gj: start
scsi_mid-gj: READY soft-skip (no virtio-scsi)
scsi_mid-gj: live path PASS (no-HBA soft)
```

— or, with HBA ready:

```
scsi_mid-gj: start
scsi_mid-gj: READY PASS
scsi_mid-gj: INQUIRY PASS
scsi_mid-gj: vendor="XXXXXXXX"    # only when vendor field non-blank
scsi_mid-gj: READ_CAP PASS        # optional
scsi_mid-gj: READ10 PASS          # optional after READ_CAP
scsi_mid-gj: live path PASS (scsi door)
```

| Marker | Meaning |
|--------|---------|
| `start` | `_start` entered |
| `READY soft-skip (no virtio-scsi)` | Transport not ready; soft green |
| `live path PASS (no-HBA soft)` | Soft exit without HBA |
| `READY PASS` | `GJ_SCSI_OP_READY` returned 1 |
| `INQUIRY PASS` / `INQUIRY FAIL` | Standard inquiry result |
| `vendor="…"` | Printable 8-byte SPC vendor id |
| `READ_CAP PASS` | Capacity probe succeeded |
| `READ10 PASS` | Single-block read returned 512 |
| `live path PASS (scsi door)` | Full door path green |

Host skeleton markers:

```
scsi_mid-server: host soft PASS (ENOSYS-shaped; no kernel door)
scsi_mid-server: submit ok (unexpected on host-only link)
```

## SCSI door ops (subset used here)

| op | name | args / return |
|----|------|----------------|
| 0 | INQUIRY | arg1=buf, arg2=cb → ret cb / status |
| 1 | READ_CAP | arg1=`u32[2]` last_lba, block_len |
| 2 | READ10 | arg1=lba, arg2=buf, arg3=blocks |
| 3 | WRITE10 | arg1=lba, arg2=buf, arg3=blocks |
| 5 | READY | → 1 if transport ready |
| 6 | STATS | arg1=`u32[2]` door_io, ready |

Defined in `user/libgj/include/gj/syscalls.h` (`GJ_SCSI_OP_*` / `gj_scsi`).
CDB opcodes in `include/scsi_mid.h` (`SCSI_OP_*`) mirror SPC/SBC mid builders.

## Host smoke

Link `server.c` + `src/cdb.c` with host libc for a compile/link smoke.
On host, `scsi_mid_submit` returns `-1` (ENOSYS-shaped); `server.c` treats that
as a soft PASS so CI stays green without a kernel door.

## Build

```sh
make scsi_mid-gj
# → build/user/scsi_mid.elf  (libgj + user/init/user.ld)
```

Source of truth for freestanding live code under this tree: `src/scsi_mid_gj.c`.
CDB / submit library: `src/cdb.c` + `include/scsi_mid.h`.
Host skeleton: `server.c`.

## Plan

1. Own more mid-layer policy here (sense decode, LUN map, timeouts) over door.
2. Route block I/O to storaged / store door after HBA claim.
3. WRITE10 live smoke once durable test media is default in QEMU configs.
