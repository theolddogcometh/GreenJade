# Freestanding → soft r8169 MMIO ownership handoff

| Field | Value |
|-------|--------|
| **Status** | **4a hybrid eng PROVEN** lab evidence: `HYBRID WIRE=FS SOFT=R8169` · `L2 BR RX=2`; Phase **0** live gate **0**; Phases **1–3** stubs; **4b** OPEN |
| **Gate** | `GJ_SOFT_R8169_MMIO_HANDOFF` — **default 0** (hybrid is the default proven lab path) |
| **Soft load gate** | `GJ_SOFT_R8169_LOAD` — **default 1** (boot load/init of embed/media `r8169.ko`; set **0** for freestanding-only net prove) |
| **Option B gate** | `GJ_SOFT_R8169_KO_NDO_OPEN` — **default 0** (needs handoff gate 1 + layout confidence) |
| **Law** | Dual **MIT OR Apache-2.0**; Soft ≠ product; **G-AC-1** |
| **Companion** | [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) **D7**; [TODO.md](TODO.md) Current track |

**Honesty:** Soft-loaded host `.ko` is an engineering path on lab media. It is **not** product AC and **not** bar3. Enabling the handoff gate on a laptop can drop freestanding net until soft open is proven. **Phase-4a hybrid** (gate0) is the pragmatic eng claim: Linux driver **hosted**, freestanding **datapath** — still Soft ≠ product / **G-AC-1**.

### Hybrid reclaim (2026-08-04)

REAL `r8169.ko` probe maps the **same BAR** and reprograms the NIC. That **orphans** freestanding TX/RX rings → STATUS `B###` busy storm, ARP/ICMP no reply, **pings not returned**. After REAL probe (gate0), boot calls **`rtl8168_reclaim_wire()`** — soft-reset, re-arm freestanding rings, TX selftest. Grep: `rtl8168: soft reclaim wire PASS`.

### Soft CF8 / iomap zero-touch (gate0 hybrid)

Boot order: **`net_l2_init` → freestanding `rtl8168_probe`** programs rings first; later **soft load `r8169` INIT=0** → `__pci_register_driver` → gate0 **EMU** bind (no `.ko` probe — `soft hostish probe SKIP hybrid`). EMU itself does not call `pci_enable` / config write / `pci_iomap`. Soft ksym **bodies** would still perform **real CF8 writes** and **BAR iomap** if invoked with a soft/hostish `pci_dev` for the live BDF — that can yield **EMPTY poll + R0** while freestanding owns wire. Policy blocks that.

| Rule | Detail |
|------|--------|
| **Gate0 REAL** | `GJ_SOFT_R8169_MMIO_HANDOFF==0` → **never** call `.ko` probe for `10ec:8168` (EMU only) |
| **Gate0 CF8/iomap** | soft/hostish `10ec:8168` → **always** NOOP (`linux_pci_soft_hw_touch_ok` returns 0); **not** gated on `rtl8168_ready()` (no race window) |
| **CF8 writes** | `pci_enable_device` / `pci_set_master` / `pci_clear_master` / `pci_write_config_byte` / status clear → **soft bookkeeping only**, no `outl` CF8 data write |
| **iomap** | `pci_iomap` / `pcim_iomap_region` → **NULL** (no second VA on freestanding BAR) |
| **Gate1** | handoff=1: NOOP only while `rtl8168_ready()!=0`; after quiesce soft CF8/iomap allowed toward REAL |
| **Load skip** | `-DGJ_SOFT_R8169_LOAD=0` skips boot soft r8169 entirely (freestanding-only net isolation) |
| **Grep** | `linux_pci_soft: soft hostish probe SKIP hybrid` · `linux_pci_soft: soft cf8 write NOOP hybrid` · `linux_dma_soft: soft pci_iomap NOOP hybrid` · `linux_pci_soft: soft hybrid zero-touch` · `main: soft linux_module path SKIP name=r8169 reason=GJ_SOFT_R8169_LOAD=0` |

---

## Problem

Today on G752VT (`10ec:8168`):

| Owner | Owns |
|-------|------|
| **Freestanding `rtl8168`** | PCI BAR MMIO + poll TE/RE + TX/RX descriptor rings (product-ish lab ssh path) |
| **Soft hostish `pci_dev` 0xb40** | BAR phys/len + soft IRQ in blob; REAL probe `ST=0` + `NETDEV SOFT 1` |
| **Soft L2 bridge** | **Copies** freestanding frames into soft skbs; freestanding still does wire TX/RX |

If soft `r8169` (or a future soft-open) also programs the same BAR while freestanding still owns rings, that is **dual-drive** — undefined HW, dual DMA, likely hang or corruption.

---

## Ownership map (current code)

| Piece | Where | Notes |
|-------|--------|--------|
| BAR map | `kernel/drv/rtl8168.c` — `vmm_map_device_uc`, `g_pMmio` | Prefer larger MEM BAR; soft-reset + MAC check |
| TE/RE start | `rtl_program_hw()` — `RTL_CHIPCMD` RE\|TE | Poll mode; `INTRMASK=0` |
| Keepalive | `rtl8168_poll_hw()` | Re-asserts TE\|RE only if `g_fReady` **and** not handoff-prepared |
| TX/RX | `rtl8168_tx` / `rtl8168_rx` | Gate on `g_fReady` **and** not `g_fSoftHandoffPrepared` |
| L2 mux | `kernel/drv/net_l2.c` | Backend virtio → rtl8168; soft note does **not** switch TX/RX |
| Hostish BAR/IRQ | `linux_pci_soft.c` `lpcis_hostish_fill*` | `resource[N].start/end/flags`, soft IRQ line |
| Soft ioremap | `linux_dma_soft.c` `pci_iomap` / `ioremap` | Optional soft map from hostish `resource[]` only; phase 2 does **not** dual-map for freestanding reclaim |
| Soft bridge | `linux_netdev_soft_l2_bridge_*` | Copy-only; freestanding owns wire until handoff |
| NAPI .ko poll allow | `g_fMmioHandoff` | **Phase 3+** after sole-owner try_open; phase 1/2 leave **0** |

### Phase 2 BAR claim (sole-owner deepen)

| Rule | Detail |
|------|--------|
| **Sole documented soft owner** | Hostish `pci_dev` blob `resource[]` + IRQ filled by `lpcis_hostish_fill*` |
| **Freestanding `g_pMmio`** | After prepare: **leave mapped**, **stop using** (no TE/RE, no tx/rx). **Do not unmap** mid-handoff (dangerous) |
| **No dual-map** | Soft path does not ioremap the same BAR for freestanding reclaim while soft also maps; soft `pci_iomap` is for `.ko` open path only (phase 3+) |
| **No `g_fMmioHandoff=1` until phase 3** | Phase 2 never allows soft NAPI `.ko` poll |

### Phase 3 soft-open (this wave)

| Rule | Detail |
|------|--------|
| **Entry** | `linux_netdev_soft_mmio_try_open()` after sole-owner on REAL path only |
| **Option A (default)** | Soft open only: carrier/queue/attach via `linux_netdev_soft_open_primary()`; **no** `.ko` ndo_open |
| **`g_fMmioHandoff=1`** | Set carefully after sole-owner + freestanding not ready — allows soft NAPI `.ko` poll path |
| **Option B** | Separate gate `GJ_SOFT_R8169_KO_NDO_OPEN` **default 0** — call `.ko` ndo_open if ops layout proven (RISKY; **not this wave**). Hostish dual blob + ops recovery **landed** (no call): [R8169_KO_NDO_OPEN.md](R8169_KO_NDO_OPEN.md) · `linux_netdev_hostish_off.h` · fill PASS grep |
| **L2** | Still fail-closed while handoff pending (wire TX/RX not soft yet — phase 4) |
| **FAULT** | Freestanding ready at try_open → dual-drive FAULT |

---

## Phases

```text
  (0) bridge copy-only          [LIVE — gate off; default boot]
       freestanding TE/RE live; soft L2 copies; reverse TX soft→freestanding OK

  (1) freestanding quiesce MMIO [STUB — gate1 only]
       stop TE/RE; mask IntrMask; g_fReady=0; net_l2 handoff-pending
       do NOT call .ko / ndo_open

  (2) hand hostish resources    [STUB — sole-owner ready codes]
       ready() codes: 0 skip | 1 wait | 2 ready-for-open | -1 fault
       still NO .ko / ndo_open; g_fMmioHandoff stays 0 until phase 3 try_open

  (3) soft-open                 [STUB — Option A when gate1 + sole]
       try_open: g_fMmioHandoff=1 + soft open only (no .ko ndo_open)
       Option B: GJ_SOFT_R8169_KO_NDO_OPEN (next lab)
       FAULT if freestanding still live

  (4a) hybrid (default lab)     [LIVE — gate0; recommended eng claim]
       freestanding wire owner; soft netdev = Linux-shaped control;
       L2 bridge + reverse dev_queue_xmit; lamp HYBRID hold15

  (4b) full .ko wire            [OPEN — after Option B + DMA/IRQ]
       soft/.ko owns rings + TE/RE; freestanding fully quiesced
```

| Phase | Goal | State | Default |
|-------|------|-------|---------|
| **0** | REAL+SOFT1 + L2 bridge ON | **LIVE** (= 4a operationally) | **ON** |
| **1** | Quiesce freestanding MMIO | **Stub** | gate0 OFF |
| **2** | Sole hostish owner | **Stub** | gate0 OFF |
| **3** | Soft open Option A | **Stub** (gate1) | gate0 OFF |
| **4a** | Hybrid: freestanding wire + soft control | **LIVE** (named) | **ON** (gate0) |
| **4b** | Full `.ko` wire TX/RX | **OPEN** | OFF (gate1 + Option B) |

---

## Safety rules

1. **Never dual-drive.** At most one of {freestanding rings, soft `.ko`/open path} may enable TE/RE or program rings.
2. **Default OFF.** `GJ_SOFT_R8169_MMIO_HANDOFF` defaults to **0**. Build with gate 0 must behave exactly as today (freestanding datapath).
3. **FAULT flag.** If code detects freestanding `g_fReady` and soft-open/MMIO claim both live → set soft handoff **FAULT**, refuse further soft program, log greppable fault; do not “try both.”
4. **Fail closed.** While handoff-pending and soft open not complete, `net_l2_tx`/`rx` must not use freestanding rings (return fail). Prefer lost packets over dual DMA.
5. **No GPL.** Handoff is knano dual-license glue only; staged `.ko` remains host binary oracle.
6. **Soft ≠ product / G-AC-1.** Soft TX/RX PASS is **not** bar3 or shipping product AC.
7. **Phase 2 ≠ NAPI allow.** Do **not** set `g_fMmioHandoff=1` until phase 3 try_open after freestanding quiesce + sole-owner.
8. **Phase 3 ≠ `.ko` ndo_open by default.** Option A only unless `GJ_SOFT_R8169_KO_NDO_OPEN=1` (next lab; requires handoff gate 1).

### Greppable markers

| Marker | Meaning |
|--------|---------|
| `rtl8168: soft mmio handoff SKIP (gate off)` | Gate 0; no MMIO change |
| `rtl8168: soft mmio handoff PREPARE …` | Phase 1: TE/RE off, IRQ masked; `g_pMmio` idle mapped |
| `rtl8168: soft mmio handoff FAULT …` | Dual-drive or bad state |
| `linux_netdev_soft: soft mmio handoff SKIP (gate off)` | Gate 0 ready stub |
| `linux_netdev_soft: soft mmio handoff READY\|WAIT\|FAULT …` | Soft-side readiness (no `.ko` open in stub) |
| `linux_netdev_soft: soft mmio handoff sole-owner …` | Phase 2: hostish BAR/IRQ sole documented owner; ready-for-open |
| `linux_netdev_soft: soft mmio handoff phase3 SKIP (gate off)` | Gate 0 try_open |
| `linux_netdev_soft: soft mmio handoff phase3 WAIT …` | Gate 1, sole-owner not ready (soft EBUSY) |
| `linux_netdev_soft: soft mmio handoff phase3 soft-open only …` | Phase 3 Option A done; `mmio_handoff=1`; no `.ko` ndo_open |
| `linux_netdev_soft: soft mmio handoff phase3 Option B …` | Option B gate on: deferred / next lab |
| `linux_netdev_soft: soft ops diagnostic …` | Read-only soft vs hostish ops / ndo_open + r8169 load range (no `ndo_open` call) |
| `linux_netdev_soft: soft hostish net_device fill PASS …` | Strategy A dual hostish net_device blob (≥0xaf0) filled from soft primary (no call) |
| `linux_netdev_soft: soft napi mmio_handoff=1` | NAPI `.ko` poll path allowed (phase 3+) |
| `net_l2: soft mmio handoff pending …` | Backend marked pending |
| `main: linux path HYBRID wire=freestanding soft=r8169` | Phase **4a** hybrid lamp (serial once) |
| STATUS hold **15** `HYBRID wire=fs soft=r8169` | Same; never clobber 7–14 |

---

## Phase 4 — hybrid vs full `.ko` wire

Phase 0 and phase **4a** are the **same operational path** under gate **0**. Naming **4a hybrid** is the eng claim for “run GreenJade using Linux drivers” **without** taking full `.ko` ring ownership.

### 4a hybrid — default recommended (gate 0)

| Piece | Owner |
|-------|--------|
| **Wire / MMIO / rings / TE\|RE** | Freestanding `rtl8168` |
| **Linux-shaped control object** | Soft `net_device` after REAL probe (`NETDEV SOFT 1`) |
| **RX into soft stack** | Soft L2 bridge **copy** (`linux_netdev_soft_l2_feed_rx`) |
| **Soft → wire TX** | Reverse path: soft skb + `dev_queue_xmit` → freestanding `net_l2_tx` |
| **Soft open** | Option A soft open primary only (**no** `.ko` `ndo_open`) |
| **NAPI `.ko` poll** | **Blocked** (`g_fMmioHandoff=0`) |

**Slogan:** *Linux driver hosted, freestanding datapath.*

**When hybrid is active** (all of):

1. Last probe mode **REAL** (`linux_pci_soft_last_probe_mode()==1`)
2. Soft L2 bridge **ON**
3. Soft primary netdev registered (and soft-open edge after bridge)
4. Freestanding `rtl8168_ready()` (wire owner live)
5. MMIO handoff **not** claimed (`g_fMmioHandoff==0`) and not pending/fault

**Lamps (once / STATUS):**

| Channel | Marker |
|---------|--------|
| Serial | `main: linux path HYBRID wire=freestanding soft=r8169` |
| hold **15** | `HYBRID wire=fs soft=r8169` |
| API | `linux_netdev_soft_hybrid_active()` → non-zero |

**Not dual-drive:** freestanding alone programs CHIPCMD/rings; soft never enables TE\|RE in 4a.

### 4b full `.ko` wire — only after Option B + DMA/IRQ proven

| Prerequisite | Why |
|--------------|-----|
| Gate `GJ_SOFT_R8169_MMIO_HANDOFF=1` | Phase 1 prepare + sole-owner |
| Phase 3 Option A (or proven path) | Soft open after freestanding quiesce |
| **Option B** `GJ_SOFT_R8169_KO_NDO_OPEN=1` | Real `.ko` `ndo_open` |
| Hostish DMA map + IRQ path proven | Else hang / dual DMA |
| Firmware load as required by host `.ko` | Still often soft ENOENT |

Until those are proven, **do not** claim 4b. Prefer **4a hybrid** for lab eng.

```text
  gate0 (default) ──► 4a HYBRID ──► freestanding wire + soft control   [LIVE]
  gate1 + Option B ─► 4b .ko wire ──► soft owns rings after prepare    [OPEN]
  never ──────────── dual freestanding + .ko TE/RE
```

---

## Stub API (phase 1–3)

| Symbol | Role |
|--------|------|
| `rtl8168_soft_handoff_prepare()` | Gate off → log SKIP once. Gate on → stop CHIPCMD, mask IntrMask, clear ready, mark net_l2 pending; leave `g_pMmio` idle. **No** `.ko` open. |
| `linux_netdev_soft_mmio_handoff_ready()` | Gate off → log SKIP once, return **0**. Gate on → **1** wait, **2** ready-for-open (prepare + primary + freestanding not ready), **-1** fault. Still **no** ndo_open until try_open. |
| `linux_netdev_soft_mmio_sole_owner()` | Non-zero only when `ready()==2`. Grep sole-owner lamp. |
| `linux_netdev_soft_mmio_try_open()` | **Phase 3.** Gate off → SKIP 0. Gate on + !sole → WAIT **-16** (soft EBUSY). Gate on + sole → set `g_fMmioHandoff=1` + Option A soft open; return **0**. FAULT **-1** if freestanding live. **No** `.ko` ndo_open unless Option B gate. |
| `net_l2_soft_handoff_pending()` | Non-zero while phase 1 done and soft wire open not complete (phase 4b). |
| `net_l2_soft_handoff_fault()` | Non-zero after dual-drive FAULT. |
| `linux_netdev_soft_hybrid_active()` | Phase **4a**: non-zero when REAL + bridge + primary + freestanding ready + not handoff. |
| `linux_netdev_soft_hybrid_lamp_once()` | Serial once: `linux path HYBRID wire=freestanding soft=r8169` (also paints hold15 when called from main). |

Config:

```c
/* kernel/include/gj/config.h — default keeps freestanding path */
#ifndef GJ_SOFT_R8169_MMIO_HANDOFF
#define GJ_SOFT_R8169_MMIO_HANDOFF  0
#endif
/* Option B: .ko ndo_open after sole-owner — next lab only; default 0 */
#ifndef GJ_SOFT_R8169_KO_NDO_OPEN
#define GJ_SOFT_R8169_KO_NDO_OPEN  0
#endif
```

Enable lab only: rebuild with `-DGJ_SOFT_R8169_MMIO_HANDOFF=1` or flip the default locally (do not commit 1 as tree default).

Option B (RISKY): also `-DGJ_SOFT_R8169_KO_NDO_OPEN=1` **and** handoff gate 1. This wave only **defers** Option B (logs DEFER); does not invoke `.ko` ndo_open.

### Freestanding TE/RE freeze (phase 1+)

After prepare, these paths **must not** re-assert TE\|RE:

| Path | Guard |
|------|--------|
| `rtl8168_poll_hw` | `g_fSoftHandoffPrepared \|\| !g_fReady` → return |
| `rtl8168_tx` / `rtl_tx_kick` | prepared or !ready → fail / return |
| `rtl8168_rx` | prepared or !ready → fail |
| `net_l2_tx` / `rx` / `ready` | handoff pending or fault → fail closed |

---

## DoD — “Linux-driver datapath soft PASS”

Not product. Engineering soft PASS for **D7** class:

| # | Criterion | Evidence |
|---|-----------|----------|
| H1 | Gate default 0 preserves freestanding TX/RX + ssh lab path | Boot with default; no PREPARE; net works |
| H2 | Gate 1 runs prepare once after REAL+bridge | `rtl8168: soft mmio handoff PREPARE` |
| H3 | After prepare, freestanding does not re-assert TE/RE | No dual CHIPCMD; `g_fReady==0`; prepared guards |
| H4 | Soft open gated; not implicit from prepare | try_open only after sole-owner; no automatic ndo_open in phase 1/2 |
| H5 | Soft TX or RX produces wire frame **or** explicit FAIL without freestanding dual-drive | Capture / counter honesty |
| H6 | Dual-drive attempt → FAULT lamp, fail closed | Grep FAULT |
| H7 | Soft ≠ product documented on PASS | Logs + this doc |
| H8 | Phase 2 sole-owner when prepare+primary | `soft mmio handoff sole-owner` + ready()==2 before try_open |
| H9 | Phase 3 Option A after sole-owner | `phase3 soft-open only` + `mmio_handoff=1`; **no** `.ko` ndo_open (unless Option B gate) |

**Soft PASS** = H1–H6 on G752 with staged/embed `r8169` path. Still **not** G-AC-1 / bar3. H9 is phase-3 stub evidence, not wire PASS.

---

## DoD — eng “can run with Linux drivers (hybrid)” (phase **4a**)

**Distinct from product AC.** This is the engineering claim for laptop Linux-driver path **without** full `.ko` ring ownership. Soft ≠ product; **G-AC-1** still applies (staged `.ko` ≠ shipping product / bar3).

| # | Criterion | Evidence |
|---|-----------|----------|
| **HY1** | **D5** module load | `main: soft linux_module path PASS` · `init=0` · hold8 |
| **HY2** | **D6** probe binds | `PROBE 10ec:8168 REAL` `ST=0` · `NETDEV SOFT 1` · hold10/11 |
| **HY3** | Soft L2 bridge ON | `soft l2 bridge ON` · `net_l2: soft linux datapath bridge ON mode=REAL` |
| **HY4** | Soft open primary | `soft open primary` (bridge path; no `.ko` ndo_open) |
| **HY5** | Freestanding NET up | `rtl8168: ready` / `net_l2: backend=rtl8168` · STATUS net line with link |
| **HY6** | Hybrid lamp | Serial `main: linux path HYBRID wire=freestanding soft=r8169` · hold **15** |
| **HY7** | hold14 counters can move | `l2 br rx=N tx=M` refreshes with freestanding traffic (code done; panel/serial optional) |
| **HY8** | Reverse TX soft path present | `soft dev_queue_xmit` / `soft l2 tx from_skb` (lab optional smoke) |
| **HY9** | **D4** media optional | `source=media` when ESP/UEFI stages `.ko`; embed fallback OK for eng |
| **HY10** | Gate handoff remains **0** | No PREPARE; freestanding still owns wire; `hybrid_active()==1` |
| **HY11** | Honesty | Logs + this doc: Soft ≠ product; **not** product AC / **not** bar3 / **not** 4b |

**Hybrid eng PASS** = HY1–HY6 + HY10–HY11 on G752 (HY7–HY9 strengthen; not all required for first lamp). **Still not** G-AC-1 product close.

| Claim | Means | Does **not** mean |
|-------|--------|-------------------|
| Hybrid eng | Hosted Linux `r8169` control + freestanding wire | Product NIC AC |
| Product AC | T0 virtio / UDX path; bar3 separate | Soft REAL+SOFT1 |
| 4b `.ko` wire | Soft owns rings after handoff | Available today |

---

## Lab test — gate 1 (phase 1–3; Option A)

Rebuild with `-DGJ_SOFT_R8169_MMIO_HANDOFF=1` (do not ship default 1). Boot G752 with REAL probe path:

| Expect serial / grep | Meaning |
|----------------------|---------|
| `rtl8168: soft mmio handoff PREPARE …` | TE/RE stopped; IRQ masked; `g_pMmio` idle |
| `net_l2: soft mmio handoff pending …` | L2 fail-closed |
| `linux_netdev_soft: soft mmio handoff READY …` **or** `WAIT …` | READY when primary registered |
| `linux_netdev_soft: soft mmio handoff sole-owner …` | Phase 2 when READY (code 2) |
| `linux_netdev_soft: soft mmio handoff phase3 soft-open only …` | Phase 3 Option A after sole-owner |
| `linux_netdev_soft: soft napi mmio_handoff=1` | Soft NAPI `.ko` poll allowed (post sole-owner) |
| **No** `.ko` ndo_open (Option B off) | `GJ_SOFT_R8169_KO_NDO_OPEN=0` |
| Freestanding ssh **drops** (expected) | Fail closed until soft wire proven (phase 4) |

Gate **0** (default image): only `… SKIP (gate off)` lamps (ready + phase3); freestanding ssh path unchanged. main does **not** call try_open when handoff gate is 0 (`#if`).

### How lab enables phase 3

```text
# Default product/lab image — freestanding path 100% intact
# (no -D flags; GJ_SOFT_R8169_MMIO_HANDOFF=0)

# Phase 1–3 lab (Option A soft-open after sole-owner):
make ... CFLAGS+='-DGJ_SOFT_R8169_MMIO_HANDOFF=1'
# Expect: PREPARE → sole-owner → phase3 soft-open only + mmio_handoff=1
# Still no .ko ndo_open; net may stay fail-closed (phase 4 OPEN)

# Option B next lab (RISKY — not implemented this wave; logs DEFER only):
make ... CFLAGS+='-DGJ_SOFT_R8169_MMIO_HANDOFF=1 -DGJ_SOFT_R8169_KO_NDO_OPEN=1'
```

---

## Soft reverse TX (phase 0 / 4a; code done)

While freestanding owns MMIO (gate 0 / default hybrid): soft skb helpers + `dev_queue_xmit` may reverse TX into freestanding L2 when safe. Grep: `linux_netdev_soft: soft dev_queue_xmit`. Part of **4a hybrid**, **not** phase-4b soft wire ownership. Soft ≠ product.

## Lab test — hybrid 4a (default image, gate 0)

Boot G752 with default build (no handoff flags). After r8169 REAL+SOFT1:

| Expect | Meaning |
|--------|---------|
| `PROBE … REAL` `ST=0` `NETDEV SOFT 1` | D6 soft bind |
| `linux_netdev_soft: soft l2 bridge ON` | Bridge copies freestanding frames |
| `linux_netdev_soft: soft open primary …` | Soft control object open |
| `main: linux path HYBRID wire=freestanding soft=r8169` | **4a** serial lamp once |
| hold **15** `HYBRID wire=fs soft=r8169` | STATUS pane |
| hold **14** `l2 br rx=… tx=…` | Counters (may start 0; live refresh in net_eth) |
| Freestanding ssh / net still works | Wire owner unchanged |
| Only handoff `… SKIP (gate off)` | Gate stays 0; no PREPARE |

## Next blockers (safe order)

1. **G752 hybrid lab panel/serial** — hold15 HYBRID + hold14 live + REAL+SOFT1 (eng hybrid DoD evidence)  
2. **Gate1 lab** — enable `GJ_SOFT_R8169_MMIO_HANDOFF=1`; prove PREPARE + **sole-owner** without dual-drive (path toward 4b only)  
3. **Option B** — real `.ko` `ndo_open` only after hostish **net_device** layout confidence ([R8169_KO_NDO_OPEN.md](R8169_KO_NDO_OPEN.md); both gates 1; **default still DEFER / no call**)  
4. **Phase 4b** — soft wire TX/RX after Option B + DMA/IRQ; never dual  
5. Firmware real load (still ENOENT — module-path; not this file’s sole owner)  
6. Lab panel/serial: hold14 live + (elsewhere) `source=media`

Leave gate **0** on all default images — **hybrid is the default proven lab path**. Enable handoff only for deliberate 4b lab.

---

*Dual MIT OR Apache-2.0. Soft ≠ product. G-AC-1: no `.ko` as product AC.*
