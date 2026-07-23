# scsi_mid (product)

Userspace SCSI mid-layer for GreenJade — pure C11 CDB builders, soft LUN,
`scsi_mid_submit`, host skeleton, and a freestanding live smoke that talks to
the kernel `scsi_door` over `GJ_SYS_SCSI` (or a full soft mid when no HBA).

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
| host smoke | link `server.c` + `src/cdb.c` | Host soft LUN mid (no kernel door) |

- Kernel interim: `scsi_door` → virtio-scsi HBA; CDB helpers may also live in
  `kernel/fs/scsi_mid.c` until full mid ownership moves here.
- Product direction: this process owns CDB build, LUN routing, sense, timeouts,
  and soft LUN policy; kernel retains DMA/IRQ windows only.
- This directory owns only `user/scsi_mid/**` — parent Makefile / kernel embed /
  boot spawn live outside this tree.

## Layout

| Path | Role |
|------|------|
| `include/scsi_mid.h` | Public CDB / `scsi_io` / sense / soft / stats API |
| `src/cdb.c` | CDB builders + soft LUN + sense decode + `scsi_mid_submit` |
| `src/scsi_mid_gj.c` | Freestanding live path (door + self-contained soft mid) |
| `server.c` | Host soft LUN skeleton |
| `README.md` | This file |

Source of truth under this tree: pure C sources above (no assembly in-tree).

## Soft LUN (userspace mid policy)

Geometry: `SCSI_MID_SOFT_SECTORS` (64) × `SCSI_MID_SOFT_SEC_SIZE` (512).

| Command | Behavior |
|---------|----------|
| TEST UNIT READY | GOOD on LUN 0 |
| REQUEST SENSE | Fixed-format sense; auto-clear unit sense after success |
| INQUIRY | Standard inquiry; vendor `GreenJad`, product `SOFTLUN` |
| MODE SENSE(6) | Minimal 4-byte mode parameter header |
| READ CAPACITY(10) | last_lba=63, block_len=512 |
| READ(10) / WRITE(10) | Soft disk R/W; LBA range check → CHECK + ASC 0x21 |
| SYNCHRONIZE CACHE(10) | Always GOOD (coherent soft store) |
| LUN ≠ 0 | CHECK + ASC 0x25 (LUN not supported) |
| Unknown opcode | CHECK + ASC 0x20 |

Sense decode: `scsi_sense_decode` reads fixed format 0x70/0x71 → KEY/ASC/ASCQ.

## CDB / server path

```text
Host skeleton (server.c)
  scsi_mid_soft_init()
  scsi_cdb_* + scsi_mid_submit → soft LUN (SCSI_HAS_SYS=0)
  sense decode + stats → host soft PASS

Product library (cdb.c)
  scsi_cdb_*                → fill scsi_cdb (SPC/SBC big-endian layout)
  scsi_mid_submit           → door when READY (GJ freestanding)
                            → else soft LUN (auto-armed)
  scsi_mid_soft_submit      → force soft path
  scsi_sense_decode         → fixed KEY/ASC/ASCQ

Live smoke (scsi_mid_gj.c)
  door READY?
    yes → gj_scsi INQUIRY/READ_CAP/READ10/STATS + soft side INQUIRY
    no  → full soft mid sequence (TUR…SYNC) → no-HBA soft PASS
```

| Builder | Opcode | CDB length | Notes |
|---------|--------|------------|--------|
| `scsi_cdb_test_unit_ready` | `0x00` | 6 | TUR |
| `scsi_cdb_request_sense` | `0x03` | 6 | alloc length |
| `scsi_cdb_inquiry` | `0x12` | 6 | EVPD, page, alloc length (BE) |
| `scsi_cdb_mode_sense6` | `0x1A` | 6 | page + alloc |
| `scsi_cdb_read_capacity10` | `0x25` | 10 | Simple capacity probe |
| `scsi_cdb_read10` | `0x28` | 10 | LBA + block count (BE) |
| `scsi_cdb_write10` | `0x2A` | 10 | Same layout as READ10 |
| `scsi_cdb_synchronize_cache10` | `0x35` | 10 | LBA + blocks (BE) |

## Freestanding path (`scsi_mid_gj.c`)

### Door path (HBA ready)

1. **READY** — probe transport (`GJ_SCSI_OP_READY` → `1`)
2. **INQUIRY** — 36-byte standard inquiry; log vendor when present
3. **READ_CAP** — optional capacity when transport allows
4. **READ10** — optional single-block read (LBA 0, 512 B)
5. **STATS** — optional door stats (`door_io`, `ready`)
6. **soft side INQUIRY** — userspace soft mid still armed beside door
7. Emit **live path PASS (scsi door)** and `gj_exit(0)`

### Soft path (no HBA)

When READY ≠ 1, freestanding runs a self-contained soft mid (same policy as
`cdb.c`, inlined so this TU links alone):

1. soft mid start / init
2. TUR → INQUIRY → MODE SENSE → READ_CAP
3. WRITE10 + READ10 verify (LBA 1, pattern `0xA5^i`)
4. illegal LUN map → REQUEST SENSE harvest
5. SYNCHRONIZE CACHE + soft stats
6. **soft mid PASS** + **live path PASS (no-HBA soft)**

### Soft vs hard outcomes

| Condition | Result |
|-----------|--------|
| READY ≠ 1 | Soft mid sequence; soft PASS — exit 0 |
| READY ok, INQUIRY fails | Hard fail — exit 1 |
| INQUIRY ok; optional steps soft-skip | Still live path PASS (door exercised) |

Boot embed (parent tree): `kernel/proc/scsi_mid_embed.S` (`.incbin` of the ELF).
Kernel spawn markers (outside this directory) include `scsi_mid: live spawn PASS`.

## Smoke markers

Freestanding (prefix `scsi_mid-gj:`):

**No-HBA soft (deepened):**

```
scsi_mid-gj: start
scsi_mid-gj: READY soft-skip (no virtio-scsi)
scsi_mid-gj: soft mid start
scsi_mid-gj: soft TUR PASS
scsi_mid-gj: soft INQUIRY PASS
scsi_mid-gj: vendor="GreenJad"
scsi_mid-gj: soft MODE_SENSE PASS
scsi_mid-gj: soft READ_CAP PASS
scsi_mid-gj: soft WRITE10 PASS
scsi_mid-gj: soft READ10 PASS
scsi_mid-gj: soft R/W verify PASS
scsi_mid-gj: soft LUN map PASS
scsi_mid-gj: soft REQ_SENSE PASS
scsi_mid-gj: soft SYNC PASS
scsi_mid-gj: soft stats ok=… fail=…
scsi_mid-gj: soft mid PASS
scsi_mid-gj: live path PASS (no-HBA soft)
```

**Door path:**

```
scsi_mid-gj: start
scsi_mid-gj: READY PASS
scsi_mid-gj: INQUIRY PASS
scsi_mid-gj: vendor="…"            # when vendor field non-blank
scsi_mid-gj: READ_CAP PASS         # optional
scsi_mid-gj: READ10 PASS           # optional
scsi_mid-gj: STATS PASS io=… ready=…  # optional
scsi_mid-gj: soft side INQUIRY PASS   # soft mid beside door
scsi_mid-gj: live path PASS (scsi door)
```

| Marker | Meaning |
|--------|---------|
| `start` | `_start` entered |
| `READY soft-skip (no virtio-scsi)` | Transport not ready; enter soft mid |
| `soft mid start` / `soft mid PASS` | Soft sequence bookends |
| `soft TUR/INQUIRY/MODE_SENSE/READ_CAP/…` | Soft mid step results |
| `soft R/W verify PASS` | WRITE+READ pattern match |
| `soft LUN map PASS` | LUN≠0 → illegal-request sense |
| `soft stats ok= fail=` | Soft I/O counters |
| `live path PASS (no-HBA soft)` | Soft exit without HBA |
| `READY PASS` | `GJ_SCSI_OP_READY` returned 1 |
| `INQUIRY PASS` / `INQUIRY FAIL` | Door inquiry result |
| `STATS PASS` | Door stats snapshot |
| `soft side INQUIRY PASS` | Soft mid beside door |
| `live path PASS (scsi door)` | Full door path green |

Host skeleton markers:

```
scsi_mid-server: host soft PASS (soft LUN; no kernel door)
scsi_mid-server: soft stats ok=… fail=…
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

```sh
cc -std=c11 -Wall -Wextra -Iuser/scsi_mid/include \
  -o /tmp/scsi_mid_server user/scsi_mid/server.c user/scsi_mid/src/cdb.c
/tmp/scsi_mid_server
```

Expect `host soft PASS` and non-zero soft `ok` stats.

## Build

```sh
make scsi_mid-gj
# → build/user/scsi_mid.elf  (libgj + user/init/user.ld)
```

Source of truth for freestanding live code under this tree: `src/scsi_mid_gj.c`.
CDB / soft / submit library: `src/cdb.c` + `include/scsi_mid.h`.
Host skeleton: `server.c`.

## Plan

1. ~~Own more mid-layer policy here (sense decode, LUN map, soft LUN)~~ (this tree)
2. Route block I/O to storaged / store door after HBA claim.
3. WRITE10 live door smoke once durable test media is default in QEMU configs.
4. Collapse freestanding soft into linked `cdb.o` when parent Makefile allows multi-TU.
