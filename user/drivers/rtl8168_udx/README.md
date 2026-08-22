# rtl8168_udx — clean-room UDX NIC host (RTL8168/8111)

| | |
|--|--|
| **PCI** | **`10ec:8168`** (Realtek RTL8168 / RTL8111 class) |
| **Target class** | ASUS G752VT laptop wired NIC (and similar) |
| **License** | **MIT OR Apache-2.0** — **no GPL**, no Linux source |
| **Role** | Userspace **Dual DoD B** product residual host vs [libudx](../../udx/) |
| **QEMU T0 net** | **virtio-net** on QEMU/CI. Laptop wire is this UDX host |
| **Dual DoD B** | **OPEN** until host **interactive SSH login**. Banner / :22 / ARP / ping / PK_OK / SUCCESS ≠ close. **L3 ARP + ping proven** (2026-08-14) on lab **10.200.125.50** |
| **Fly bar** | Match kernel `GJ_IMAGE_VERSION` (**STATUS (static) v0.1.184**; packed, not host-probed). Dual DoD **A/B OPEN**. **0.2.0** reserved |

## Honesty (read first)

This tree is the **Linux-shaped userspace NIC host** for Dual DoD **B** (hot+cold ABI + DDI/UDX). Soft residual catalogs remain; **option-3 product residual** can write TNPDS/RDSAR/TE\|RE/Own on real DDI + program_gate. Freestanding kernel `rtl8168` is **SKIP** default — not product. **Soft!=product.** Agent≠close.

| Rule | Meaning |
|------|---------|
| **product=UDX+ABI** | Product NIC path = userspace Linux-shaped host over UDX/DDI (MMIO/IRQ/DMA **caps**), not freestanding `.c` in kernel |
| **freestanding SKIP** | `abandoned/kernel/drv/rtl8168.c` default **SKIP** (`GJ_RTL8168_PROBE=0`) — not linked, **not** product |
| **Soft!=product** | Soft residual lamps / catalogs / soft MAP-IRQ-DMA != product wire ownership |
| **G-AC-1** | No Linux `.ko` runs in kernel as product AC |

- Exercises Linux-shaped `probe` / `remove` / `quiesce`, BAR MMIO, soft IRQ, soft DMA ring residual.
- Soft residual stages: **open → MAP → cfg/regs → ring layout/verify → IRQ/work → product residual catalog → C2 deepen → caps residual → wire handoff residual → functional residual**.
- Soft ring residual lays Own=0 + EOR last only; **never** programs TNPDS/RDSAR/THPDS/TE/RE/TPPOLL.
- Soft **product residual catalog** (6 steps) **names** next OPEN program with public RTL registers (TNPDS/RDSAR/ChipCmd/IntrMask/TxConfig/RxConfig/RMS/CPlusCmd/PHYStatus/PHYAR/TPPOLL) — != product TX/RX.
- **Functional residual** (6 steps): denser **open → MAP → ring → IRQ → work → wire-handoff** path catalog toward product when DDI caps live.
- **C2 deepen residual catalogs** (program / PHY / NAPI / TXRX) expand those steps with public register *names* only — still **never** program silicon.
- **Wire handoff residual** (9 steps): ordered Dual DoD B next-program *names* (caps → ring → TNPDS → RDSAR → TE\|RE → PHY → NAPI → TXRX/TPPOLL → owner flip) — **never** executes.
- **Soft IRQ residual** / **soft work residual**: once-lamps (IntrMask honesty; work↔reclaim catalog) — not per-hit stamp storms.
- **Caps residual** (MMIO / IRQ / DMA via UDX/DDI): catalog only; **product_mint=0** (no CNode MMIO_FRAME / IRQ Notification / DMA window mint).
- **bind != wire**: host inject / freestanding `bind_by_id` residual is not wire handoff. Userspace probe residual **never claims freestanding wire**.
- **Product direction:** this UDX host (plus ABI/DDI caps) is the laptop NIC path. Freestanding rtl is **SKIP by default** (`GJ_RTL8168_PROBE=0`) — not product.
- Lean residual: **no version stamp**, no multi-KiB kprintf floods, no stamp storms.
- Soft residual stages still dominate host inject demos. **Laptop L3 ARP/ping is proven**; Dual DoD B remains **OPEN** until host **interactive SSH login** (agent≠close).
- Is **not** in-kernel `r8169.ko` product (**G-AC-1**).
- Greppable soft lamps are **not** HCL product close and **not** bar3.

### Product residual path (real writes; Soft≠product; Dual DoD B OPEN)

When bind is **real DDI** + program_gate (MAP+DMA+ring+align), option-3 **product** residual is **not** soft `never_program` only:

| Symbol / lamp | What it does (public RTL only) |
|---------------|--------------------------------|
| `rtl8168_product_program_try` | RST → C+CR → **PHY BMCR ANE** → `own_handoff` → TNPDS/RDSAR → TE\|RE → **IntrMask** → lock 9346CR |
| `rtl8168_product_phy_link_bringup` | Public PHYAR: clear PWD/ISOLATE, ANE+Restart_AN, poll LinkOk (glass .93 LINKOK=0 dig) |
| `rtl8168_product_own_handoff` | FORCE32 RX slots (4 KiB page = Buffer_Size) + TX bounce, IOMMU grant, Own=1, opts2=0, EOR last only, RCR AAP\|APM\|AB\|AM, MAR all1, RMS, MTPS, **IDR MAC** under 9346CR cfgwrite |
| `rtl8168_product_l2_poll` | thr-only: IntrStatus ack → RX reclaim (Frame_Length−FCS) → ETH_INJECT → TX pull + TPPOLL; densified `own_stuck` dig |

Greppable product lamps (≠ Dual DoD B close):

| Marker | Meaning |
|--------|---------|
| `rtl8168_udx: product phy bringup` | PASS/WAIT after BMCR ANE; physt/linkok/bmsr |
| `rtl8168_udx: product phy link UP` / `DOWN` | thr-poll once-lamps; re-AN when down |
| `rtl8168_udx: product program PASS` | TNPDS/RDSAR/TE\|RE wrote; `product_mint=1` `wire_owner=udx` |
| `rtl8168_udx: product own_handoff PASS` | RX Own + RCR/MAR residual armed |
| `rtl8168_udx: product dma_pa dig` | Once: page_align of rx0/ring/txb + ring↔RX span_ok (v0.1.97 FOVW overlap dig) |
| `rtl8168_udx: product rx_desc dig` | Once after Own: opts0, buf_mask, eor_last, page_bytes, soft_rx_buf |
| `rtl8168_udx: product idr` | IDR MAC keep-EEPROM or lab fallback `02:00:00:47:4a:50` |
| `rtl8168_udx: product imr` | Public IntrMask ROK\|RER\|TOK\|TER\|… (not MSI-X) |
| `rtl8168_udx: product l2 poll own_stuck` | Densified dig @ ~0x10000 polls (rdsar/tnpds/desc vs slot/inject/imr + live RCR/TCR/RMS) |
| `rtl8168_udx: product l2 poll own_stuck_isr` | Once: ISR raw + decoded ROK/RER/TOK/TER/RDU/FOVW/LinkChg |
| `rtl8168_udx: product l2 poll fovw_rer` | Once: FOVW\|RER sticky RX FIFO dig (RCR RXFTH/MXDMA re-read) |
| `rtl8168_udx: product l2 poll fovw_reappear` | Once: FOVW still set after public W1C IntrStatus clear |
| `rtl8168_udx: product program densify_obs` | Once after program PASS: RCR/TCR/RMS/ChipCmd/PHY/IMR/ISR observe-back |
| `rtl8168_udx: product l2 poll rekick` | Once: RE off → re-Own RX + clflush + RDSAR + RE on (FOVW recovery) |
| `rtl8168_udx: product l2 poll inject_fail` | ETH_INJECT n≤0 once-lamp (still re-arms) |
| `rtl8168_udx: product l2 poll tx_own_stuck` | TX frame ready but desc Own stuck |

**Soft≠product. Dual DoD B L3 ARP/ping proven on DUT (2026-08-14). Dual DoD B stays OPEN until host interactive SSH login. Banner ≠ close. Never claim Dual DoD closed from lamps alone.**

**RX desc residual (public 8168/8111B-class):** Own fill advertises `Buffer_Size` = page (4096, ×8) matching FORCE32 DMA (glass FOVW had `opts0=0x80000800` = OWN+2 KiB while page was 4 KiB); EOR only on last slot; opts2 zeroed; store-barrier+clflush before TE\|RE. When Own clears, bits 13:0 are `Frame_Length` (includes FCS) — reclaim strips 4 B before inject; RES skips inject. Soft≠product; Dual DoD B OPEN.

**Glass .92 FOVW/RER dig residual (Dual DoD B OPEN):** Under TE with `own_handoff=1` / `rx_all_own=16` / `inject=0`, glass ISR residual showed **RER+TOK+TER+FOVW** and **no ROK** while desc PA and RDSAR matched. Product thr-poll densifies observe-back of live **RCR/TCR/RMS/ChipCmd/PHYStatus**, **IMR**, decoded **ISR** bits, residual-compat **MPC@0x4C** (Table-1 reserved — not public RxMissed; public MissPkt is DTCCR tally dump, not armed here), and **TX desc opts** (no public **TxStatus/TSD** on 8168B). On FOVW\|RER sticky once: re-read RCR and confirm public **RXFTH/MXDMA** already unlimited; do **not** invent silicon FIFO magic — next dig names only are DTCCR MissPkt tally + C+CR RxChkSum residual. ISR ack remains public **write-1-to-clear**; once-lamp if FOVW reappears after clear. **≠ Dual DoD B close.**

| Marker | Meaning |
|--------|---------|
| `rtl8168_udx: soft open path` / `soft map path` | OPEN vs MAP residual path lamps |
| `rtl8168_udx: soft open/map honesty` | open!=map!=ring!=TNPDS!=wire rollup |
| `rtl8168_udx: soft ring residual` / `soft ring path` | CPU DMA desc layout residual |
| `rtl8168_udx: soft ring verify` | Own=0 / EOR-last residual check |
| `rtl8168_udx: soft ring bases` | TNPDS/RDSAR re-observe (never program) |
| `rtl8168_udx: soft ring reclaim` | Reclaim catalog only (product OPEN) |
| `rtl8168_udx: soft product residual` / `soft product step=` | Next OPEN program catalog (6 steps; public RTL reg names) |
| `rtl8168_udx: soft product model` | Userspace UDX direction · not `r8169.ko` · wire=FS |
| `rtl8168_udx: soft product catalog PASS` | Catalog walked — **!=** product TX/RX |
| `rtl8168_udx: soft functional residual` / `soft functional step=` | open/map/ring/IRQ/work/wire-handoff denser catalog |
| `rtl8168_udx: soft functional catalog PASS` | Functional path walked — **!=** product wire |
| `rtl8168_udx: soft program residual` / `soft program step=` | 64-bit TNPDS/RDSAR write-order catalog (never write) |
| `rtl8168_udx: soft phy residual` / `soft phy step=` | PHYStatus / PHYAR / link residual (never PHYAR R/W) |
| `rtl8168_udx: soft napi residual` / `soft napi step=` | Budget / TX done / RX fill catalog (never poll) |
| `rtl8168_udx: soft txrx residual` / `soft txrx step=` | FS/LS / TPPOLL / net door catalog (never wire I/O) |
| `rtl8168_udx: soft caps residual` / `soft caps step=` | MMIO/IRQ/DMA via UDX/DDI catalog (**product_mint=0**) |
| `rtl8168_udx: soft caps catalog PASS` | Caps residual walked — != product cap mint |
| `rtl8168_udx: soft irq residual` | IntrMask/line once-lamp; TE/RE/MSI-X OPEN |
| `rtl8168_udx: soft work residual` | Work path once-lamp + reclaim catalog (never NAPI) |
| `rtl8168_udx: soft wire handoff` / `soft wire step=` | Dual DoD B next-program *names* (9 steps; never write) |
| `rtl8168_udx: soft wire handoff catalog PASS` | Wire handoff walked — **!=** product handoff |
| `rtl8168_udx: soft c2 product path` | C2 claim-class honesty — soft scaffold != product AC |
| `rtl8168_udx: soft freestanding product=SKIP` | Freestanding rtl not product (`GJ_RTL8168_PROBE=0`) |
| `rtl8168_udx: soft bind_ne_wire` | Bind residual != freestanding wire |
| `rtl8168_udx: soft probe residual` | Stage flag rollup (userspace host; never claim FS wire) |
| `rtl8168_udx: soft probe PASS` | Host soft path completed (probe + ISR + work) |
| `rtl8168_udx: soft SKIP` | Soft path not completed (no inject, bind missing, etc.) |

### Product residual catalog (OPEN only)

| Step | Name | Public RTL names (soft observe) | Soft does |
|------|------|----------------------------------|-----------|
| 1 | TNPDS program | TNPDS · TNPDS_HI · TxConfig | Lamp only — **never** writes desc base |
| 2 | RDSAR program | RDSAR · RDSAR_HI · RxConfig · RMS | Lamp only — **never** writes desc base |
| 3 | ChipCmd TE\|RE | ChipCmd · IntrMask · IntrStatus | Observe; **never** arm TE/RE |
| 4 | PHY / link | PHYStatus · PHYAR | PHYStatus RO snap; PHYAR OPEN; **never_phyar_rw=1** |
| 5 | NAPI / reclaim | CPlusCmd | Catalog only |
| 6 | TX/RX datapath | TPPOLL · THPDS | Catalog only — **never_tx_rx=1** / **never_tppoll=1** |

### Functional residual catalog (OPEN only)

| Step | Name | Soft does |
|------|------|-----------|
| 1 | open | enable+regions residual; **product_mint=0** |
| 2 | MAP | BAR ioremap; MMIO_FRAME OPEN |
| 3 | ring | Own=0 EOR last; TNPDS/RDSAR/THPDS names only |
| 4 | IRQ | IntrMask ROK\|TOK soft; ChipCmd TE\|RE OPEN |
| 5 | work | ISR→work→reclaim catalog; never NAPI product |
| 6 | wire handoff | Dual DoD B order residual; freestanding owns wire |

### C2 deepen residual catalogs (OPEN only)

| Catalog | Steps | Soft does |
|---------|-------|-----------|
| **program** | TNPDS lo/hi · RDSAR lo/hi | Names 64-bit write *order*; **never_program=1** |
| **phy** | PHYStatus · PHYAR · link | RO snap + name residual; **never_phyar_rw=1** |
| **napi** | budget · TX done · RX fill | Catalog only; **never_napi_poll=1** |
| **txrx** | FS/LS · TPPOLL · net door | Catalog only; **never_tx_rx=1** / **never_tppoll=1** |
| **caps** | MMIO · IRQ · DMA | Catalog only; **product_mint=0** (UDX/DDI cap mint OPEN) |
| **wire handoff** | caps · ring · TNPDS · RDSAR · TE\|RE · PHY · NAPI · TXRX/TPPOLL · owner | Dual DoD B order *names* only; **never_program=1** / **product_handoff=0** |
| **functional** | open · MAP · ring · IRQ · work · wire_ho | Denser live soft path; **product_mint=0** |

T0 product NIC on GreenJade remains **virtio-net**. Real NIC product path is **userspace UDX+ABI+DDI** (MMIO/IRQ/DMA caps) — not this soft residual alone, and **not** freestanding lab wire forever (`GJ_RTL8168_PROBE=0` product SKIP).

**Related (kernel lab, not this tree):** soft load of host `r8169.ko` via
`linux_module` + 40-byte `pci_device_id` match for `10ec:8168` — see
[docs/LINUX_MODULE_PATH.md](../../../docs/LINUX_MODULE_PATH.md). Soft!=product;
**G-AC-1** forbids treating Linux `.ko` as product AC.

## Clean-room rules

- IDs from **public PCI** (`vendor 0x10ec`, `device 0x8168`).
- Register **names** only from **public** Realtek documentation / widely published maps (e.g. `IntrStatus`, `IntrMask`, `ChipCmd`, `TxConfig`, `RxConfig`, `TNPDS`, `RDSAR`).
- **Do not** paste Linux `r8169.c`, Realtek GPL out-of-tree blobs, or GreenJade `abandoned/kernel/drv/rtl8168.c`.
- Caps / IOMMU / hard IRQ stay inside UDX — never minted in this driver.

## BAR note (soft)

RTL8168/8111-class devices **often** place the primary MMIO window on **BAR2**. This skeleton:

1. Prefers **BAR2** for `ioremap`.
2. Soft-falls back to the first non-empty **memory** BAR if BAR2 is empty (host inject mistakes / odd revs).
3. Logs a warning when falling back — **re-check real silicon BARs** from Linux inventory / config space before any hard programming.

Host inject for the lab demo maps **only BAR2** (`0x1000` soft window).

## Soft lifecycle

```text
udx_init
  → udx_pci_register_driver(rtl8168_udx)
  → [host] udx_host_inject_pci(10ec, 8168, BAR2=0x1000)
  → probe: enable → master → regions (soft open) →
           prefer BAR2 / fallback BAR0 (soft MAP) →
           cfg/reg snap → dma_alloc + soft ring residual/verify →
           request_irq → IntrMask soft
  → [host] bar writew IntrStatus(ROK|TOK) → fire_irq
  → ISR: read IntrStatus → ack → schedule_work
  → udx_run → work → stop
  → unregister (quiesce → remove) → udx_exit
  → "rtl8168_udx: soft probe PASS"
```

**Freestanding:** call `rtl8168_udx_register()` (or `rtl8168_udx_freestanding_start()`).  
If the platform links **`udx_host_bind_by_id`**, freestanding start will bind `10ec:8168` and require open+MAP+ring residual/verify before **soft probe PASS**.  
**bind != wire** — freestanding lab still owns live wire interim.  
If that symbol is **absent**, freestanding logs **soft SKIP** and documents host inject (below).

## Build instructions

Prerequisites: host toolchain with `gcc`/`ar`, and **libudx** built for host.

### 1. Build libudx (repo root preferred)

```sh
cd /path/to/knano
make udx
# → build/libudx.a
```

Or local UDX tree:

```sh
make -C user/udx lib
# → user/udx/build/libudx.a
```

### 2. Build this driver (local Makefile)

```sh
make -C user/drivers/rtl8168_udx
# → user/drivers/rtl8168_udx/build/rtl8168_udx
```

Override lib path if you used the top-level UDX build:

```sh
make -C user/drivers/rtl8168_udx UDX_LIB=../../../../build/libudx.a \
  UDX_INC=../../udx/include
```

(Paths are relative to `user/drivers/rtl8168_udx/`; the local Makefile defaults to `user/udx`.)

### 3. Run host soft demo

```sh
./user/drivers/rtl8168_udx/build/rtl8168_udx | tee /tmp/rtl8168_udx.log
grep -E 'rtl8168_udx: soft (probe PASS|SKIP)' /tmp/rtl8168_udx.log
```

Expected greppable success lines (subset):

```text
rtl8168_udx: soft open path enable=1 regions=1 map=0
rtl8168_udx: soft map path open=1 map=1
rtl8168_udx: soft ring residual PASS
rtl8168_udx: soft ring verify PASS
rtl8168_udx: soft product residual catalog …
rtl8168_udx: soft product catalog PASS steps=6/6 …
rtl8168_udx: soft program residual catalog …
rtl8168_udx: soft program catalog PASS steps=4/4 …
rtl8168_udx: soft phy catalog PASS steps=3/3 …
rtl8168_udx: soft napi catalog PASS steps=3/3 …
rtl8168_udx: soft txrx catalog PASS steps=3/3 …
rtl8168_udx: soft caps residual catalog …
rtl8168_udx: soft caps catalog PASS steps=3/3 …
rtl8168_udx: soft irq residual … mask_bits=ROK|TOK … chipcmd_te_re=OPEN …
rtl8168_udx: soft wire handoff residual catalog …
rtl8168_udx: soft wire step=… tnpds=OPEN / rdsar=OPEN / te_re=OPEN …
rtl8168_udx: soft wire handoff catalog PASS steps=9/9 …
rtl8168_udx: soft functional residual catalog … path=open,map,ring,irq,work,wire_handoff
rtl8168_udx: soft functional step=… open= / map= / ring= / irq= / work= / wire_ho=
rtl8168_udx: soft functional catalog PASS steps=6/6 …
rtl8168_udx: soft c2 product path claim_class=C2 product=UDX+ABI …
rtl8168_udx: soft product model product=UDX+ABI … freestanding_product=SKIP …
rtl8168_udx: soft freestanding product=SKIP GJ_RTL8168_PROBE_default=0 …
rtl8168_udx: soft open/map honesty open=1 map=1 …
rtl8168_udx: soft bind_ne_wire … freestanding_owns_wire=1
rtl8168_udx: soft work residual … reclaim_catalog=1 …
rtl8168_udx: soft probe residual id=10ec:8168 userspace_host=1 …
rtl8168_udx: soft probe PASS … caps=1 … wire_ho=1 … func=1 func_steps=6/6 … product_mint=0 never_tx_rx=1
```

### One-liner from repo root (after `make udx` or `make -C user/udx lib`)

```sh
make -C user/drivers/rtl8168_udx && ./user/drivers/rtl8168_udx/build/rtl8168_udx
```

## Host inject IDs (lab)

When freestanding bind is unavailable, host test inject:

| Field | Value |
|-------|--------|
| Vendor | `0x10ec` |
| Device | `0x8168` |
| BAR2 | memory, length `0x1000` (soft) |
| IRQ | any unused soft line (demo uses `12`) |

API: `udx_host_inject_pci(0x10ec, 0x8168, bus, devfn, irq, aBarLen, aBarMem, &pPdev)` with `aBarLen[2] = 0x1000`.

## Files

| File | Role |
|------|------|
| `rtl8168_udx.c` | Driver skeleton + host `main` / freestanding stubs |
| `rtl8168_udx_regs.h` | Clean-room public reg / desc / residual-step names |
| `Makefile` | Local host build against libudx |
| `README.md` | This note |

## What is intentionally missing / still OPEN

- Soft inject / incomplete gate: still **never** programs TNPDS/RDSAR/TE\|RE
- Product DDI path programs public bases + TE\|RE + IMR + IDR residual — **not** full MSI-X / NAPI / ethtool
- No MSI-X product path
- No claim of G752VT LAN product support / Dual DoD B **closed**
- No in-kernel `r8169.ko` product AC (**G-AC-1**)
- Lab L3 ARP/ping **proven** (2026-08-14). Dual DoD B **OPEN** until host **interactive SSH login**. Banner / :22 / PK_OK / SUCCESS ≠ close.

**License:** MIT OR Apache-2.0 only — **no GPL**, no Linux source paste.

*Userspace UDX host for laptop NIC class IDs — product=UDX+ABI; freestanding product=SKIP; Soft!=product; G-AC-1; product_program/own_handoff/l2_poll write on real_ddi+gate; laptop ARP/ping L3 proven; Dual DoD B OPEN until interactive SSH login; QEMU T0 remains virtio.*

---

**Project:** GreenJade · Soft≠product · Dual DoD A **OPEN** until host USB path · Dual DoD B **OPEN** until interactive SSH login. [root README](../../../README.md). Support: [Patreon — TheOldDog](https://www.patreon.com/cw/TheOldDog).
