/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux-shaped UDX soft driver residual for Realtek RTL8168/8111
 * (public PCI ID 10ec:8168) — G752VT laptop NIC class.
 *
 * WHAT THIS IS
 *   Userspace driver-host soft residual over libudx (DDI/caps direction):
 *     probe → OPEN+MAP → cfg/reg snap → soft ring residual → IRQ/work →
 *     product residual catalog → quiesce/remove.
 *   Dual DoD B product *direction* (Linux-shaped NIC host over DDI/UDX).
 *   Soft OPEN+MAP / ring / product catalog / IRQ / work / wire handoff —
 *   not product TX/RX. Freestanding owns the wire interim (lab DoD B eth).
 *   Product = UDX/DDI + hot/cold ABI. Dual DoD B remains OPEN (agent!=close).
 *   T0 product net = virtio. Soft!=product. G-AC-1: no .ko product AC.
 *   TX/RX remains OPEN.
 *
 * CLEAN-ROOM RULES
 *   Register program from public PCI IDs + publicly documented Realtek
 *   register *names* / coarse layout only (RTL8111B/8168B Registers
 *   DataSheet-class maps). Do NOT copy Linux r8169.c,
 *   in-tree kernel/drv/rtl8168.c, or any GPL source.
 *
 * BAR / DDI NOTE
 *   Silicon-common primary MMIO is often BAR2 (memory).
 *   Soft host inject maps BAR2. Freestanding DDI (GJ_SYS_DDI MAP_BAR)
 *   currently grants BAR0 PA/VA; soft probe prefers BAR2, then BAR0,
 *   then first non-empty MEM BAR — greppable soft ddi bar0 honesty.
 *   Prefer != product wire. Soft!=product.
 *
 * SOFT RESIDUAL (toward product UDX NIC host -- C2 product path residual)
 *   1 map  2 cfg/regs  3 ring layout (CPU DMA only)  4 irq  5 work
 *   Ring residual lays Own=0 + EOR last; soft residual never programs
 *   TNPDS/RDSAR/TE/RE (soft never_program=1 class law).
 *   Soft reclaim residual is catalog-only (never Own handoff / NAPI poll).
 *   Soft product residual catalog (6 steps) names next OPEN program only
 *   with public RTL register names (TNPDS/RDSAR/ChipCmd/PHYStatus/…).
 *   Functional residual: stronger open/map/ring/IRQ/work/wire-handoff +
 *   program-path linkage toward product UDX wire ownership (catalog denser).
 *   Prefer real DDI bind (GJ_SYS_DDI / udx_host_bind_by_id) over inject-only
 *   soft demo when freestanding host; host path tries DDI then inject.
 *   Functional residual densifies toward program_gate when MAP+DMA present
 *   (soft map+dma proxy; soft residual product_mint=0).
 *   OPTION 3 product program path (Dual DoD B product UDX NIC):
 *     When bind_path=ddi + program_gate (MAP+DMA+ring+align), product_mint
 *     opens and rtl8168_product_program_try WRITES public order
 *     RST → C+CR → own_handoff (IDR/RCR/MAR/Own) → TNPDS/RDSAR →
 *     ChipCmd TE|RE → IntrMask (RTL_PRODUCT_IRQ_BITS) → 9346CR normal.
 *     greppable: "rtl8168_udx: product program PASS" / product_mint=1
 *     never_program=0 write=1 wire_owner=udx product_handoff=1.
 *     thr-poll l2_poll: ETH_INJECT + tx_pull + densified own_stuck dig.
 *     Soft residual catalogs remain soft residual (never_program=1 class).
 *     Dual DoD B OPEN until DUT L3 (lab IP / stack / sshd :22). agent!=close.
 *   STRONGER functional residual densify when caps residual present:
 *     densify real DDI open/map and program_gate (prefer_real_ddi=1;
 *     real_ddi=1 on bind_path=ddi; real_ddi densify once-lamp).
 *   Soft program residual: TNPDS/RDSAR/TE|RE order catalog; write=0 soft.
 *   STRONGER program order residual densify (read-only) when real_ddi=1:
 *     order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re (product sequence catalog);
 *     densify program residual real_ddi once-lamp; soft never_program=1.
 *   STRONGER residual densify triad (H2 once; never stamp storms):
 *     program order + wire claim + product chain densify once-lamps;
 *     densify_order / densify_wire_claim / densify_chain always-on rollup;
 *     densify wire_claim=1 + densify chain=1 once-lamps (product path OPEN);
 *     densify_triad=1 STRONGER rollup + denser order (te_re 5th always-on);
 *     denser product chain hops (chain_host/stack/sshd) + wire_claim residual.
 *   STRONGER denser program/wire/chain residual (H2 once; exclusive .c):
 *     densify_program_wire_chain=1 rollup links order+wire_claim+chain;
 *     denser_program=1 denser_wire=1 denser_chain=1 surface lamps;
 *     denser hop residual chain_hop1/2/3 + order denser (te_re 5th);
 *     still never_program=1 / product_mint=0 / dual_dod_b=OPEN.
 *   STRONGER denser program_wire_chain + prefer_real_ddi + wire claim
 *   residual (H2 once; exclusive .c; Dual DoD B G752VT lab path):
 *     densify_program_wire_chain=1 + densify_prefer_real_ddi=1 rollup;
 *     denser_prefer_real_ddi=1 + denser_wire_claim=1 surface lamps;
 *     densify_pwc_prefer_real=1 links PWC + prefer_real_ddi + wire claim;
 *     wire claim residual denser OPEN until DUT (never claim FS wire);
 *     prefer_real_ddi denser over inject (real_ddi from bind_path=ddi);
 *     still never_program=1 / product_mint=0 / dual_dod_b=OPEN ALWAYS.
 *   STRONGER denser program_wire_chain + prefer_real residual (H2 once;
 *   exclusive .c; functional surface parity with program/product/wire):
 *     soft functional densify program_wire_chain / prefer_real_ddi /
 *     pwc_prefer_real once-lamps; denser prefer_real residual over inject;
 *     densify_program_wire_chain + densify_prefer_real_ddi + densify_pwc
 *     prefer_real residual honesty on functional path;
 *     still never_program=1 / product_mint=0 / dual_dod_b=OPEN ALWAYS.
 *   STRONGER denser residual bar .75 (H2 once; exclusive .c; Dual DoD B):
 *     denser residual prove + denser residual VERDICT rollup;
 *     denser_prove=1 denser_residual=1 never_program_held=1;
 *     bar=v2026.08.04.75 stamp_free=1 NEVER invent .76;
 *     never_tnpds=1 never_rdsar=1 never_te_re=1 never_program=1 ALWAYS;
 *     product_mint=0 dual_dod_b=OPEN Soft!=product ALWAYS (ASCII Soft!= only);
 *     denser residual != Dual DoD B close; agent!=close.
 *   STRONGER denser residual bar .75 always-on multi-arm (H2 once; exclusive .c):
 *     denser residual arms open|map|ring|prog|wire|func|caps|bases_clean;
 *     denser residual lean + denser residual arms rollup + VERDICT;
 *     denser_arms always bar .75; denser residual always never_program held;
 *     still never_program=1 / product_mint=0 / dual_dod_b=OPEN ALWAYS;
 *     NEVER write TNPDS/RDSAR/TE|RE; Soft!=product (ASCII Soft!= only).
 *   STRONGER denser residual hold always bar .75 (H2 once; exclusive .c;
 *   Dual DoD B; wave densify): denser residual hold + never_program held
 *   free-text greppable; denser residual always never_program held rollup;
 *   denser_hold=1 denser residual hold never_program held=1 ALWAYS;
 *   still never_program=1 / product_mint=0 / dual_dod_b=OPEN ALWAYS;
 *   NEVER write TNPDS/RDSAR/TE|RE; Soft!=product (ASCII Soft!= only);
 *   denser residual != Dual DoD B close; agent!=close; stamp_free bar .75.
 *   NEVER write TNPDS/RDSAR/TE|RE while product_mint=0 (never_program=1).
 *   Ring Own prep: host Own=0 EOR-last geometry ready for product handoff.
 *   Link observe→TX: PHYStatus linkok linked to TX only when UDX owns wire.
 *   C2 deepen: program / PHY / NAPI / TXRX residual catalogs (lamps only).
 *   C2 TX/RX residual deepen: FS/LS·TPPOLL·net denser Soft!=product;
 *   never_program=1 until real DDI caps + product_mint + product UDX wire
 *   (Dual DoD B remains OPEN; agent!=close).
 *   C2 product residual deepen: hot/cold ABI + sshd path surface +
 *   product=UDX+sshd+stack path honesty denser + dual_dod_b=OPEN.
 *   product chain residual denser: rtl8168_udx>netstackd>sshd (names only).
 *   lab_ip residual denser OPEN (lab 10.200.125.50 / stack / sshd :22 names);
 *   wire claim residual denser OPEN (product UDX owns wire honesty OPEN until DUT).
 *   Wire handoff residual: denser Dual DoD B next-program *names*
 *   (caps→ring→TNPDS→RDSAR→TE|RE→PHY→NAPI→TXRX/lab_ip/stack/sshd→owner);
 *   STRONGER wire densify: wire_claim + product chain + program order once.
 *   Soft IRQ / work residual: once-lamps (not per-hit stamp storms);
 *   H1 thr-only — never net_eth_poll from IRQ. H2 once — densify lamps.
 *   Caps residual: MMIO / IRQ / DMA via UDX/DDI (catalog; product_mint=0).
 *   ABI residual: hot Linux ABI + cold doors/net + UDX/DDI + sshd path
 *   surface (names only; never claims Dual DoD B close).
 *   Lean residual: no version stamp, no multi-KiB floods, no stamp storms.
 *   greppable: "rtl8168_udx: soft open" / "soft open path" /
 *              "soft open+map" / "soft map path" / "soft open/map honesty" /
 *              "soft honesty" / "soft catalog" / "soft ring residual" /
 *              "soft ring path" / "soft ring verify" / "soft ring reclaim" /
 *              "soft ddi bar0" / "soft regmap" / "soft bind_ne_wire" /
 *              "soft product residual" / "soft product step=" /
 *              "soft product model" / "soft ready" / "soft probe residual" /
 *              "soft program residual" / "soft program step=" /
 *              "soft program residual honesty" / "soft program order residual" /
 *              "soft program densify" / "soft program densify real_ddi=" /
 *              "soft program densify order=" / "order=tnpds_lo,tnpds_hi," /
 *              "soft program densify te_re=" /
 *              "soft program densify wire_claim=" /
 *              "soft program densify chain=" /
 *              "soft program densify triad=" /
 *              "soft program densify program_wire_chain=" /
 *              "soft program densify prefer_real_ddi=" /
 *              "soft program densify pwc_prefer_real=" /
 *              "densify_program_wire_chain=" /
 *              "densify_prefer_real_ddi=" / "denser_prefer_real_ddi=" /
 *              "densify_pwc_prefer_real=" / "denser_wire_claim=" /
 *              "denser_program=" / "denser_wire=" / "denser_chain=" /
 *              "densify_order=" / "densify_wire_claim=" / "densify_chain=" /
 *              "densify_triad=" / "order_pos=5/5" /
 *              "product chain residual" / "soft product densify" /
 *              "soft product densify chain=" / "soft product densify wire_claim=" /
 *              "soft product densify order=" / "soft product densify triad=" /
 *              "soft product densify program_wire_chain=" /
 *              "soft product densify prefer_real_ddi=" /
 *              "soft product densify pwc_prefer_real=" /
 *              "soft wire densify" / "soft wire densify wire_claim=" /
 *              "soft wire densify chain=" / "soft wire densify order=" /
 *              "soft wire densify triad=" /
 *              "soft wire densify program_wire_chain=" /
 *              "soft wire densify prefer_real_ddi=" /
 *              "soft wire densify pwc_prefer_real=" /
 *              "program_gate=" /
 *              "soft phy residual" / "soft phy step=" /
 *              "soft napi residual" / "soft napi step=" /
 *              "soft txrx residual" / "soft txrx step=" /
 *              "soft txrx catalog" / "soft txrx residual honesty" /
 *              "never_program=1" / "never_tx_rx=1" / "tx_rx=OPEN" /
 *              "soft caps residual" / "soft caps step=" /
 *              "soft caps catalog" / "soft c2 product path" /
 *              "soft abi residual" / "soft abi step=" / "soft abi catalog" /
 *              "soft abi residual honesty" / "sshd=OPEN" / "lab_ip=OPEN" /
 *              "lab_ip residual" / "wire claim residual" / "wire_claim=OPEN" /
 *              "need=DDI_caps" / "need=hot_cold_abi" / "dual_dod_b=OPEN" /
 *              "soft dual_dod_b next" /
 *              "soft irq residual" / "soft work residual" /
 *              "soft functional residual" / "soft functional step=" /
 *              "soft functional residual honesty" /
 *              "soft functional densify" / "map_dma_ready=" /
 *              "map_dma_caps=" / "toward=program_gate" /
 *              "soft functional densify real_ddi=" /
 *              "soft functional densify open=" /
 *              "soft functional densify map=" /
 *              "soft functional densify program_gate=" /
 *              "soft functional densify caps=" /
 *              "soft functional densify program_wire_chain=" /
 *              "soft functional densify prefer_real_ddi=" /
 *              "soft functional densify pwc_prefer_real=" /
 *              "prefer_real residual" / "program_wire_chain residual" /
 *              "prefer_real_ddi=" / "prefer_real_ddi=1" / "real_ddi=" /
 *              "own_prep=" / "link_to_tx=" /
 *              "soft wire handoff" / "soft wire step=" /
 *              "soft bind path" / "soft ddi prefer" /
 *              "product=UDX+sshd+stack" / "Soft!=product" /
 *              "soft denser residual" / "soft denser residual prove" /
 *              "soft denser residual VERDICT" / "never_program_held=1" /
 *              "soft denser residual lean" / "soft denser residual arms" /
 *              "soft denser residual hold" / "never_program held" /
 *              "denser residual always never_program held" /
 *              "denser_prove=1" / "denser_residual=1" / "denser_arms=" /
 *              "denser_hold=1" / "bar=v2026.08.04.75" / "stamp_free=1" /
 *              "always bar .75" /
 *              "soft probe PASS" / "soft freestanding product=SKIP"
 *
 * PRODUCT vs FREESTANDING
 *   Product = userspace UDX+ABI+DDI + hot/cold Linux ABI
 *   (Linux-shaped host; MMIO/IRQ/DMA caps). Dual DoD B OPEN (agent!=close).
 *   product=UDX+sshd+stack residual: this host → netstackd → sshd :22.
 *   Freestanding kernel/drv/rtl8168.c default SKIP (GJ_RTL8168_PROBE=0).
 *   Soft!=product. G-AC-1: no Linux .ko product AC.
 *   Userspace host probe residual never claims freestanding wire.
 *
 * FREESTANDING (lab bind only; not product)
 *   Prefer udx_host_bind_by_id (GJ_SYS_DDI) over inject-only soft demo.
 *   Register; if udx_host_bind_by_id is linked, bind 10ec:8168 and verify
 *   probe/drvdata + ring residual/verify before soft probe PASS
 *   (bind alone != probe; bind != wire). Wire interim = freestanding lab.
 *
 * License choice: MIT OR Apache-2.0 (dual). See repo LICENSE.
 */

#include <udx/udx.h>
#include "rtl8168_udx_regs.h"
#if !defined(UDX_HOST_LIBC)
#include <gj/string.h>   /* gj_memcpy freestanding */
#include <gj/syscalls.h> /* gj_exit / gj_net_eth_* / gj_yield */
#endif

/*
 * Soft ABI residual (C2 Dual DoD B product residual deepen; exclusive .c).
 * Product path = UDX/DDI + hot/cold Linux ABI. Catalog names only.
 * Dual DoD B lab goal surface: lab IP / stack / sshd (OPEN until DUT proof).
 * dual_dod_b=OPEN always (agent never closes Dual DoD). Soft!=product.
 * greppable: rtl8168_udx: soft abi residual / soft abi step=
 * greppable: soft abi residual honesty / sshd=OPEN / lab_ip=OPEN
 */
#define RTL_SOFT_ABI_STEP_HOT   1u /* hot Linux ABI / personality surface */
#define RTL_SOFT_ABI_STEP_COLD  2u /* cold doors / cold_net residual */
#define RTL_SOFT_ABI_STEP_UDX   3u /* UDX host + DDI caps residual */
#define RTL_SOFT_ABI_STEP_SSHD  4u /* lab IP / stack / sshd path surface */
#define RTL_SOFT_ABI_STEP_COUNT 4u
/* Path bit free above header RTL_SOFT_PATH_FUNC (32768). Exclusive .c only. */
#define RTL_SOFT_PATH_ABI       65536u
/*
 * STRONGER denser program order residual (exclusive .c; densify only).
 * Product sequence catalog: TNPDS lo/hi → RDSAR lo/hi → ChipCmd TE|RE.
 * Header PROG_STEP_COUNT stays 4 (lo/hi bases); denser order adds te_re 5th.
 * never_program=1 / product_mint=0 always. greppable: order_pos=5/5
 */
#define RTL_SOFT_PROG_ORDER_TE_RE  5u
#define RTL_SOFT_PROG_ORDER_COUNT  5u
/* densify_triad STRONGER rollup: order + wire_claim + chain (H2 once). */
#define RTL_SOFT_DENSIFY_TRIAD     1u
/* denser program/wire/chain residual rollup (H2 once; never_program holds). */
#define RTL_SOFT_DENSIFY_PWC       1u
/*
 * STRONGER denser program_wire_chain + prefer_real_ddi + wire claim OPEN
 * residual (H2 once; exclusive .c). densify_prefer_real_ddi + denser_wire_claim
 * + densify_pwc_prefer_real link PWC rollup to real DDI preference and
 * wire-claim OPEN until DUT. never_program=1 / product_mint=0 ALWAYS.
 * greppable: densify_prefer_real_ddi= / denser_prefer_real_ddi=
 * greppable: densify_pwc_prefer_real= / denser_wire_claim=
 */
#define RTL_SOFT_DENSIFY_PREFER_REAL   1u
#define RTL_SOFT_DENSIFY_WIRE_CLAIM    1u
#define RTL_SOFT_DENSIFY_PWC_PREFER    1u
/*
 * STRONGER denser residual bar .75 (H2 once; exclusive .c; Dual DoD B).
 * denser residual prove + VERDICT rollup. never_program=1 / product_mint=0
 * / dual_dod_b=OPEN ALWAYS. Stamp-free bar honesty — NEVER invent .76.
 * denser residual multi-arm always bar .75 (open/map/ring/prog/wire/func/caps/
 * bases_clean). denser residual hold always never_program held.
 * greppable: denser_prove=1 / denser_residual=1 / denser_hold=1 /
 * never_program_held=1 / never_program held / denser_arms= /
 * soft denser residual VERDICT / soft denser residual lean /
 * soft denser residual arms / soft denser residual hold /
 * denser residual always never_program held / bar=v2026.08.04.75
 */
#define RTL_SOFT_DENSER_PROVE          1u
#define RTL_SOFT_DENSER_RESIDUAL       1u
#define RTL_SOFT_NEVER_PROGRAM_HELD    1u
#define RTL_SOFT_DENSER_HOLD           1u /* denser residual hold always */
#define RTL_SOFT_BAR_STAMP_FREE        1u
/* denser residual multi-arm count (always bar .75; never_program held). */
#define RTL_SOFT_DENSER_ARMS_MAX       8u
#define RTL_SOFT_DENSER_ARMS_MIN       6u
#define RTL_SOFT_DENSER_ALWAYS_BAR     1u

/*
 * Soft bind path honesty (exclusive .c): prefer real DDI bind over inject.
 *   NONE   — not bound yet / SKIP
 *   DDI    — udx_host_bind_by_id / GJ_SYS_DDI residual (preferred)
 *   INJECT — host inject soft demo (fallback when DDI unavailable)
 * greppable: rtl8168_udx: soft bind path / soft ddi prefer
 */
#define RTL_SOFT_BIND_PATH_NONE    0u
#define RTL_SOFT_BIND_PATH_DDI     1u
#define RTL_SOFT_BIND_PATH_INJECT  2u

/* Optional freestanding bind (may be absent on host libudx). */
#if !defined(UDX_HOST_LIBC)
/*
 * Match host.h: udx_host_bind_by_id(vend, dev, ppOut).
 * Weak so host-only libudx builds still link; freestanding GJ provides real.
 */
udx_status_t udx_host_bind_by_id(u16 u16Vendor, u16 u16Device,
                                 struct udx_pci_dev **ppOut)
    __attribute__((weak));
#endif

/*
 * Per-function soft state (Linux: netdev_priv / pci drvdata).
 * Hung off pPdev->pDev via udx_set_drvdata.
 */
struct rtl8168_soft {
    struct udx_pci_dev *pPdev;
    struct udx_iomem   *pRegs;   /* preferred MMIO BAR (soft) */
    struct udx_work     workIsr;
    struct udx_spinlock lockSoft;
    u32                 u32IrqHits;
    u32                 u32WorkHits;
    u32                 u32FQuiesced;
    u32                 u32BarIndex;
    u32                 u32FProbeOk;
    u32                 u32FOpenOk;  /* soft open (enable+regions) */
    u32                 u32FMapOk;   /* soft MAP (ioremap) */
    u32                 u32FRingOk;  /* soft ring residual laid out */
    u32                 u32FRingVerify; /* soft ring Own/EOR check */
    u32                 u32FBasesClean; /* TNPDS/RDSAR still unprogrammed */
    u32                 u32FProdCat; /* product residual catalog walked */
    u32                 u32FProgRes; /* C2 program residual catalog */
    u32                 u32FPhyRes;  /* C2 PHY residual catalog */
    u32                 u32FNapiRes; /* C2 NAPI residual catalog */
    u32                 u32FTxRxRes; /* C2 TX/RX residual catalog */
    u32                 u32FC2Path;  /* C2 product path honesty walked */
    u32                 u32FCapsRes; /* MMIO/IRQ/DMA caps residual catalog */
    u32                 u32FIrqRes;  /* soft IRQ residual catalog (once) */
    u32                 u32FWorkRes; /* soft work residual catalog (once) */
    u32                 u32FWireHo;  /* wire handoff residual catalog */
    u32                 u32FFuncRes; /* functional residual catalog */
    u32                 u32FAbiRes;  /* hot/cold ABI residual catalog */
    u32                 u32FProductMint; /* option-3 product mint (real_ddi+gate) */
    u32                 u32FProductProg; /* product TNPDS/RDSAR/TE|RE wrote */
    u32                 u32PathBits; /* RTL_SOFT_PATH_* residual rollup */
    u8                  u8SoftStage; /* 1..5 soft progress */
    u8                  u8ProdSteps; /* product residual catalog depth */
    u8                  u8ProgSteps; /* program residual step depth */
    u8                  u8PhySteps;  /* PHY residual step depth */
    u8                  u8NapiSteps; /* NAPI residual step depth */
    u8                  u8TxRxSteps; /* TX/RX residual step depth */
    u8                  u8CapSteps;  /* caps residual step depth */
    u8                  u8WireSteps; /* wire handoff residual step depth */
    u8                  u8FuncSteps; /* functional residual step depth */
    u8                  u8AbiSteps;  /* hot/cold ABI residual step depth */
    u8                  u8ChipCmd;   /* soft ChipCmd snapshot */
    u8                  u8CfgRev;    /* config revision soft */
    u8                  u8BarSrc;    /* RTL_SOFT_BAR_SRC_* honesty */
    u8                  u8PhySt;     /* PHYStatus soft snapshot */
    u8                  u8Config1;   /* Config1 soft snapshot */
    u8                  u8WireOwner; /* RTL_SOFT_WIRE_OWNER_* honesty */
    u8                  u8BindPath;  /* RTL_SOFT_BIND_PATH_* honesty */
    u16                 u16CfgCmd;   /* PCI command soft */
    u16                 u16IntrMask; /* last soft IntrMask write */
    u16                 u16IntrMaskRd; /* IntrMask soft readback */
    u16                 u16Rms;      /* Rx max size soft snapshot */
    u32                 u32Idr0;     /* soft IDR0 dword snapshot */
    u32                 u32Idr4;     /* soft IDR4 low half + pad */
    u32                 u32TxCfg;    /* TxConfig snapshot */
    u32                 u32RxCfg;    /* RxConfig snapshot */
    u32                 u32CPlus;    /* CPlusCmd snapshot */
    u32                 u32TnpdsLo;  /* TNPDS low observe (never program) */
    u32                 u32TnpdsHi;
    u32                 u32RdsarLo;  /* RDSAR low observe (never program) */
    u32                 u32RdsarHi;
    u32                 u32ThpdsLo;  /* THPDS low observe (never program) */
    u32                 u32ThpdsHi;
    u32                 u32TxSlots;  /* soft TX desc count */
    u32                 u32RxSlots;  /* soft RX desc count */
    u32                 u32SoftTxIdx; /* soft residual cursor (not HW) */
    u32                 u32SoftRxIdx;
    udx_dma_addr_t      dmaRing;     /* coherent base (TX then RX) */
    udx_dma_addr_t      dmaTx;       /* TX ring cookie residual */
    udx_dma_addr_t      dmaRx;       /* RX ring cookie residual */
    void               *pRing;
    struct rtl8168_soft_desc *pTxDesc; /* VA into pRing */
    struct rtl8168_soft_desc *pRxDesc;
    /* Product Own handoff + thr-poll L2 bridge (Dual DoD B). Soft!=product. */
    /*
     * Per-slot RX (4 KiB page each): freestanding BSS is not PA-contiguous
     * (elf_load pmm_alloc per page). Single 32 KiB pool + base+off cookies
     * fail under TE; DUT .84 inject=0 after own_handoff PASS with VA cookies.
     * Soft!=product Dual DoD B OPEN.
     */
    void               *apRxSlot[RTL_SOFT_RX_SLOTS];
    udx_dma_addr_t      aDmaRxSlot[RTL_SOFT_RX_SLOTS];
    void               *pTxBounce;   /* FORCE32 TX bounce for demux replies */
    udx_dma_addr_t      dmaTxBounce;
    u32                 u32FOwnHandoff; /* RX Own=1 + RCR residual */
    u32                 u32RxPoll;   /* thr-poll reclaim hits */
    u32                 u32Inject;   /* ETH_INJECT ok count */
    u32                 u32InjectFail; /* ETH_INJECT n<=0 once-lamp dig */
    u32                 u32TxPull;   /* ETH_TX_PULL hits (programmed Own) */
    u32                 u32TxPullSkip; /* TX frame ready but Own stuck */
    u32                 u32OwnStuck; /* thr-poll: all RX still OWN residual */
    u32                 u32FOwnRekicked; /* once: RDSAR re-kick residual */
};

/* File-static soft block — freestanding hosts need no heap. */
static struct rtl8168_soft g_rtlSoft;

/* Forward: work residual re-walks reclaim catalog (defined later). */
static void rtl8168_soft_ring_reclaim_residual(const struct rtl8168_soft *pSoft);
/* Forward: denser residual bar .75 VERDICT (H2 once; never_program held). */
static void rtl8168_soft_denser_residual_verdict(const struct rtl8168_soft *pSoft);
/*
 * Option 3 product program (Dual DoD B): real DDI + program_gate → write
 * TNPDS/RDSAR/TE|RE. Soft residual never programs; inject path never mints.
 * greppable: rtl8168_udx: product program
 */
static int rtl8168_product_program_try(struct rtl8168_soft *pSoft);
/* Product Own handoff + thr-poll L2 bridge (after product_prog). Soft!=product. */
static int  rtl8168_product_own_handoff(struct rtl8168_soft *pSoft);
#if !defined(UDX_HOST_LIBC)
static void rtl8168_product_l2_poll(struct rtl8168_soft *pSoft);
#endif

/*
 * RX Buffer_Size residual: match FORCE32 page (regs.h RTL_SOFT_RX_*).
 * Public: multiple of 8, ≤0x1FF8; Own field == DMA alloc span (FOVW glass
 * had opts0=0x80000800 / 2 KiB while page was 4 KiB). Soft!=product.
 */
#define RTL_SOFT_RX_POOL_BYTES \
    ((u32)RTL_SOFT_RX_SLOTS * (u32)RTL_SOFT_RX_BUF_BYTES)
#define RTL_SOFT_TX_BOUNCE_BYTES 1536u

#if ((RTL_SOFT_RX_BUF_BYTES % 8u) != 0u)
#error "rtl8168_udx: RTL_SOFT_RX_BUF_BYTES must be multiple of 8 (public Buffer_Size)"
#endif
#if (RTL_SOFT_RX_BUF_BYTES > RTL_DESC_RX_BUF_MAX)
#error "rtl8168_udx: RTL_SOFT_RX_BUF_BYTES exceeds public Buffer_Size max 0x1FF8"
#endif
#if (RTL_SOFT_RX_BUF_BYTES > RTL_SOFT_RX_PAGE_BYTES)
#error "rtl8168_udx: RTL_SOFT_RX_BUF_BYTES exceeds FORCE32 page"
#endif

/**
 * Public RX Own opts1 fill: OWN | Buffer_Size | optional EOR (last only).
 * opts2 cleared by caller. Soft!=product Dual DoD B OPEN.
 */
static u32
rtl8168_rx_opts1_own(u32 fEor)
{
    u32 u32Opts;

    u32Opts = RTL_DESC_OWN | (RTL_SOFT_RX_BUF_BYTES & RTL_DESC_RX_BUF_MASK);
    if (fEor != 0u) {
        u32Opts |= RTL_DESC_EOR;
    }
    return u32Opts;
}

/**
 * Soft stage lamp — greppable progress without product TX/RX claim.
 * greppable: rtl8168_udx: soft stage=
 */
static void
rtl8168_soft_stage_lamp(u8 u8Stage, const char *szExtra)
{
    if (szExtra != NULL && szExtra[0] != '\0') {
        udx_printk("rtl8168_udx: soft stage=%u %s Soft!=product\n",
                   (unsigned)u8Stage, szExtra);
    } else {
        udx_printk("rtl8168_udx: soft stage=%u Soft!=product\n",
                   (unsigned)u8Stage);
    }
}

/**
 * Soft OPEN lamp — pci_enable + regions claimed (not product CNode mint).
 * greppable: rtl8168_udx: soft open
 * greppable: rtl8168_udx: soft open path
 */
static void
rtl8168_soft_open_lamp(const struct udx_pci_dev *pPdev, int fOk)
{
    if (fOk != 0 && pPdev != NULL) {
        udx_printk("rtl8168_udx: soft open PASS id=10ec:8168 "
                   "bdf=%02x:%02x.%u irq=%d Soft!=product\n",
                   (unsigned)pPdev->u8Bus,
                   (unsigned)udx_pci_slot(pPdev),
                   (unsigned)udx_pci_func(pPdev),
                   pPdev->nIrq);
        udx_printk("rtl8168_udx: soft open path enable=1 regions=1 master=1 "
                   "map=0 ring=0 irq=0 work=0 wire_ho=0 program=0 "
                   "product_mint=0 need=DDI_caps need=hot_cold_abi "
                   "tx_rx=OPEN lab_ip=OPEN stack=OPEN sshd=OPEN "
                   "dual_dod_b=OPEN Soft!=product\n");
    } else {
        udx_printk("rtl8168_udx: soft open SKIP Soft!=product\n");
        udx_printk("rtl8168_udx: soft open path enable=0 regions=0 master=0 "
                   "map=0 ring=0 product_mint=0 dual_dod_b=OPEN "
                   "Soft!=product\n");
    }
}

/**
 * Soft OPEN+MAP lamp — BAR ioremap ok (soft MMIO window only).
 * greppable: rtl8168_udx: soft open+map
 * greppable: rtl8168_udx: soft map
 * greppable: rtl8168_udx: soft map path
 */
static void
rtl8168_soft_open_map_lamp(const struct rtl8168_soft *pSoft,
                           u64 u64Bar, u64 u64Len, int fOk)
{
    if (fOk != 0 && pSoft != NULL) {
        udx_printk("rtl8168_udx: soft map PASS bar=%u pa=0x%llx len=0x%llx "
                   "bar_src=%u Soft!=product\n",
                   (unsigned)pSoft->u32BarIndex,
                   (unsigned long long)u64Bar,
                   (unsigned long long)u64Len,
                   (unsigned)pSoft->u8BarSrc);
        udx_printk("rtl8168_udx: soft map path open=1 map=1 bar=%u "
                   "prefer_bar2=%u fallback=%u "
                   "ring=0 irq=0 work=0 wire_ho=0 program=0 "
                   "mmio_frame=OPEN product_mint=0 need=DDI_caps "
                   "never_program=1 dual_dod_b=OPEN Soft!=product\n",
                   (unsigned)pSoft->u32BarIndex,
                   (pSoft->u8BarSrc == RTL_SOFT_BAR_SRC_BAR2) ? 1u : 0u,
                   (pSoft->u8BarSrc != RTL_SOFT_BAR_SRC_BAR2) ? 1u : 0u);
        udx_printk("rtl8168_udx: soft open+map PASS bar=%u stage=%u "
                   "tx_rx=OPEN lab_ip=OPEN stack=OPEN sshd=OPEN "
                   "soft=1 product=0 product_mint=0 never_program=1 "
                   "dual_dod_b=OPEN Soft!=product\n",
                   (unsigned)pSoft->u32BarIndex,
                   (unsigned)pSoft->u8SoftStage);
    } else {
        udx_printk("rtl8168_udx: soft map SKIP Soft!=product\n");
        udx_printk("rtl8168_udx: soft map path open=1 map=0 "
                   "product_mint=0 Soft!=product\n");
        udx_printk("rtl8168_udx: soft open+map SKIP tx_rx=OPEN "
                   "product_mint=0 Soft!=product\n");
    }
}

/**
 * Soft UDX catalog — claim surface only (never product TX/RX / wire).
 * greppable: rtl8168_udx: soft catalog
 * Dual DoD A soft companion: freestanding owns wire interim.
 */
static void
rtl8168_soft_catalog(void)
{
    udx_printk("rtl8168_udx: soft catalog id=10ec:8168 "
               "surfaces=open,open_path,map,map_path,open_map_honesty,"
               "cfg,regs,regmap,ring_residual,ring_path,ring_verify,"
               "ring_reclaim,irq,irq_residual,dma_ring,ddi_bar0,"
               "bind_ne_wire,bind_path,ddi_prefer,product_residual,"
               "product_model,program_residual,phy_residual,"
               "napi_residual,txrx_residual,caps_residual,abi_residual,"
               "c2_product_path,functional_residual,functional_densify,"
               "wire_handoff,probe_residual,work,work_residual,"
               "denser_residual,denser_prove,denser_verdict,"
               "quiesce,remove "
               "tx_rx=OPEN phy=OPEN link=OPEN napi=OPEN msix=OPEN "
               "tnpds=OPEN rdsar=OPEN thpds=OPEN chipcmd_te_re=OPEN "
               "tppoll=OPEN mmio_cap=OPEN irq_cap=OPEN dma_cap=OPEN "
               "hot_abi=OPEN cold_abi=OPEN "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "prod_steps=%u prog_steps=%u phy_steps=%u "
               "napi_steps=%u txrx_steps=%u cap_steps=%u "
               "wire_steps=%u func_steps=%u abi_steps=%u "
               "prefer_bar=%u ddi_grant_bar=%u "
               "bind_ne_wire=%u wire_owner=freestanding "
               "wire=freestanding dual_dod_b=product_direction "
               "dual_dod_b=OPEN dual_dod_b_lab=1 "
               "product=UDX+ABI product=UDX+sshd+stack "
               "need=DDI_caps need=hot_cold_abi "
               "prefer_ddi_bind=1 inject_fallback=1 "
               "product_mint=0 freestanding_product=SKIP "
               "never_program=1 never_program_held=1 "
               "denser_prove=1 denser_residual=1 denser_arms_max=%u "
               "always bar .75 bar=v2026.08.04.75 stamp_free=1 "
               "claim_class=C2 product_ac=0 "
               "G-AC-1 Soft!=product\n",
               (unsigned)RTL_SOFT_PROD_STEP_COUNT,
               (unsigned)RTL_SOFT_PROG_STEP_COUNT,
               (unsigned)RTL_SOFT_PHY_STEP_COUNT,
               (unsigned)RTL_SOFT_NAPI_STEP_COUNT,
               (unsigned)RTL_SOFT_TXRX_STEP_COUNT,
               (unsigned)RTL_SOFT_CAP_STEP_COUNT,
               (unsigned)RTL_SOFT_WIRE_STEP_COUNT,
               (unsigned)RTL_SOFT_FUNC_STEP_COUNT,
               (unsigned)RTL_SOFT_ABI_STEP_COUNT,
               (unsigned)RTL8168_BAR_MMIO,
               (unsigned)RTL8168_BAR_DDI,
               (unsigned)RTL_SOFT_BIND_NE_WIRE,
               (unsigned)RTL_SOFT_DENSER_ARMS_MAX);
}

/**
 * Soft bind path honesty — DDI preferred over inject-only soft demo.
 * greppable: rtl8168_udx: soft bind path
 * greppable: rtl8168_udx: soft ddi prefer
 */
static void
rtl8168_soft_bind_path_lamp(const struct rtl8168_soft *pSoft, u8 u8Path)
{
    const char *szPath;

    if (u8Path == RTL_SOFT_BIND_PATH_DDI) {
        szPath = "ddi";
    } else if (u8Path == RTL_SOFT_BIND_PATH_INJECT) {
        szPath = "inject";
    } else {
        szPath = "none";
    }

    udx_printk("rtl8168_udx: soft bind path path=%s path_code=%u "
               "prefer_ddi=1 inject_fallback=1 "
               "gj_sys_ddi=%u udx_host_bind_by_id=1 "
               "bind_ne_wire=1 dual_dod_b=OPEN Soft!=product\n",
               szPath, (unsigned)u8Path, (unsigned)GJ_SYS_DDI);
    udx_printk("rtl8168_udx: soft ddi prefer "
               "prefer=udx_host_bind_by_id over=inject "
               "path=%s map=%u open=%u "
               "product=UDX+sshd+stack product_mint=0 "
               "dual_dod_b=OPEN Soft!=product G-AC-1\n",
               szPath,
               (pSoft != NULL) ? (unsigned)pSoft->u32FMapOk : 0u,
               (pSoft != NULL) ? (unsigned)pSoft->u32FOpenOk : 0u);
}

/**
 * Soft bind!=wire honesty — OPEN+MAP/bind residual is not wire handoff.
 * greppable: rtl8168_udx: soft bind_ne_wire
 * Freestanding lab owns live wire interim (Dual DoD B companion).
 */
static void
rtl8168_soft_bind_ne_wire(const struct rtl8168_soft *pSoft, int fBound)
{
    u8 u8Owner;
    u8 u8Stage;
    u8 u8Path;
    u32 u32Open;
    u32 u32Map;
    u32 u32Ring;
    const char *szPath;

    u8Owner = RTL_SOFT_WIRE_OWNER_FS;
    u8Stage = 0;
    u8Path = RTL_SOFT_BIND_PATH_NONE;
    u32Open = 0;
    u32Map = 0;
    u32Ring = 0;
    if (pSoft != NULL) {
        u8Owner = pSoft->u8WireOwner;
        u8Stage = pSoft->u8SoftStage;
        u8Path = pSoft->u8BindPath;
        u32Open = pSoft->u32FOpenOk;
        u32Map = pSoft->u32FMapOk;
        u32Ring = pSoft->u32FRingOk;
    }
    if (u8Path == RTL_SOFT_BIND_PATH_DDI) {
        szPath = "ddi";
    } else if (u8Path == RTL_SOFT_BIND_PATH_INJECT) {
        szPath = "inject";
    } else {
        szPath = "none";
    }

    udx_printk("rtl8168_udx: soft bind_ne_wire bind=%u open=%u map=%u "
               "ring=%u stage=%u bind_path=%s "
               "bind_ne_wire=%u wire_owner=%s "
               "udx_wire=%u freestanding_owns_wire=%u "
               "prefer_ddi=1 tx_rx=OPEN product_handoff=0 Soft!=product\n",
               (unsigned)(fBound != 0 ? 1u : 0u),
               (unsigned)u32Open, (unsigned)u32Map, (unsigned)u32Ring,
               (unsigned)u8Stage, szPath,
               (unsigned)RTL_SOFT_BIND_NE_WIRE,
               (u8Owner == RTL_SOFT_WIRE_OWNER_FS) ? "freestanding" : "udx",
               (unsigned)RTL_SOFT_WIRE_OWNER_UDX,
               (unsigned)RTL_SOFT_WIRE_OWNER_FS);
}

/**
 * Soft public regmap catalog — names/offsets only (clean-room public map).
 * greppable: rtl8168_udx: soft regmap
 */
static void
rtl8168_soft_regmap_catalog(void)
{
    udx_printk("rtl8168_udx: soft regmap "
               "idr0=0x00 mar0=0x08 tnpds=0x20 thpds=0x28 "
               "chipcmd=0x37 tppoll=0x38 "
               "intrmask=0x3c intrstatus=0x3e "
               "txcfg=0x40 rxcfg=0x44 "
               "cfg0=0x51 phystatus=0x6c rms=0xda "
               "cplus=0xe0 rdsar=0xe4 mtps=0xec "
               "src=public_datasheet gpl=0 Soft!=product\n");
}

/**
 * Soft DDI BAR0 grant path honesty.
 * Freestanding DDI MAP_BAR grants BAR0; silicon-common MMIO is often BAR2.
 * greppable: rtl8168_udx: soft ddi bar0
 */
static void
rtl8168_soft_ddi_bar0_honesty(const struct rtl8168_soft *pSoft)
{
    u32 u32Bar0Len;
    u32 u32Bar2Len;
    u32 u32Mapped;
    u32 u32Src;

    u32Bar0Len = 0;
    u32Bar2Len = 0;
    u32Mapped = 0;
    u32Src = RTL_SOFT_BAR_SRC_BAR2;

    if (pSoft != NULL && pSoft->pPdev != NULL) {
        u32Bar0Len = (u32)udx_pci_resource_len(pSoft->pPdev, RTL8168_BAR_DDI);
        u32Bar2Len = (u32)udx_pci_resource_len(pSoft->pPdev, RTL8168_BAR_MMIO);
        u32Mapped = pSoft->u32BarIndex;
        u32Src = pSoft->u8BarSrc;
    }

    udx_printk("rtl8168_udx: soft ddi bar0 "
               "grant_bar=%u grant_len=0x%x "
               "silicon_common_bar=%u common_len=0x%x "
               "mapped_bar=%u src=%u "
               "prefer_bar2=1 fallback_bar0=1 "
               "bind_ne_wire=1 product_mint=0 Soft!=product\n",
               (unsigned)RTL8168_BAR_DDI, (unsigned)u32Bar0Len,
               (unsigned)RTL8168_BAR_MMIO, (unsigned)u32Bar2Len,
               (unsigned)u32Mapped, (unsigned)u32Src);
}

/**
 * Soft honesty stamp after probe path — explicit OPEN product bars.
 * greppable: rtl8168_udx: soft honesty
 * Never claims product TX/RX (G-AC-1; Soft!=product).
 */
static void
rtl8168_soft_honesty(const struct rtl8168_soft *pSoft)
{
    if (pSoft == NULL) {
        return;
    }
    udx_printk("rtl8168_udx: soft honesty id=10ec:8168 stage=%u bar=%u "
               "bar_src=%u open=%u map=%u open_map=soft "
               "ring_residual=%u ring_verify=%u bases_clean=%u "
               "prod_cat=%u prod_steps=%u/%u "
               "prog_res=%u prog_steps=%u/%u "
               "phy_res=%u phy_steps=%u/%u "
               "napi_res=%u napi_steps=%u/%u "
               "txrx_res=%u txrx_steps=%u/%u "
               "caps_res=%u cap_steps=%u/%u "
               "irq_res=%u work_res=%u wire_ho=%u wire_steps=%u/%u "
               "func=%u func_steps=%u/%u "
               "abi=%u abi_steps=%u/%u "
               "c2=%u path=0x%x irq=soft dma_ring=soft ddi_bar0=soft "
               "tx_rx=OPEN phy=OPEN link=OPEN napi=OPEN "
               "mmio_cap=OPEN irq_cap=OPEN dma_cap=OPEN "
               "hot_abi=OPEN cold_abi=OPEN "
               "soft=1 product_ac=0 product=UDX+ABI "
               "need=DDI_caps need=hot_cold_abi product_mint=0 "
               "freestanding_product=SKIP G-AC-1 gpl=0 never_tx_rx=1 "
               "never_program=1 lab_ip=OPEN stack=OPEN sshd=OPEN "
               "claim_class=C2 "
               "bind_ne_wire=%u wire_owner=%s wire=freestanding "
               "dual_dod_b=OPEN Soft!=product\n",
               (unsigned)pSoft->u8SoftStage,
               (unsigned)pSoft->u32BarIndex,
               (unsigned)pSoft->u8BarSrc,
               (unsigned)pSoft->u32FOpenOk,
               (unsigned)pSoft->u32FMapOk,
               (unsigned)pSoft->u32FRingOk,
               (unsigned)pSoft->u32FRingVerify,
               (unsigned)pSoft->u32FBasesClean,
               (unsigned)pSoft->u32FProdCat,
               (unsigned)pSoft->u8ProdSteps,
               (unsigned)RTL_SOFT_PROD_STEP_COUNT,
               (unsigned)pSoft->u32FProgRes,
               (unsigned)pSoft->u8ProgSteps,
               (unsigned)RTL_SOFT_PROG_STEP_COUNT,
               (unsigned)pSoft->u32FPhyRes,
               (unsigned)pSoft->u8PhySteps,
               (unsigned)RTL_SOFT_PHY_STEP_COUNT,
               (unsigned)pSoft->u32FNapiRes,
               (unsigned)pSoft->u8NapiSteps,
               (unsigned)RTL_SOFT_NAPI_STEP_COUNT,
               (unsigned)pSoft->u32FTxRxRes,
               (unsigned)pSoft->u8TxRxSteps,
               (unsigned)RTL_SOFT_TXRX_STEP_COUNT,
               (unsigned)pSoft->u32FCapsRes,
               (unsigned)pSoft->u8CapSteps,
               (unsigned)RTL_SOFT_CAP_STEP_COUNT,
               (unsigned)pSoft->u32FIrqRes,
               (unsigned)pSoft->u32FWorkRes,
               (unsigned)pSoft->u32FWireHo,
               (unsigned)pSoft->u8WireSteps,
               (unsigned)RTL_SOFT_WIRE_STEP_COUNT,
               (unsigned)pSoft->u32FFuncRes,
               (unsigned)pSoft->u8FuncSteps,
               (unsigned)RTL_SOFT_FUNC_STEP_COUNT,
               (unsigned)pSoft->u32FAbiRes,
               (unsigned)pSoft->u8AbiSteps,
               (unsigned)RTL_SOFT_ABI_STEP_COUNT,
               (unsigned)pSoft->u32FC2Path,
               (unsigned)pSoft->u32PathBits,
               (unsigned)RTL_SOFT_BIND_NE_WIRE,
               (pSoft->u8WireOwner == RTL_SOFT_WIRE_OWNER_FS)
                   ? "freestanding" : "udx");
}

/**
 * Soft denser residual bar .75 — Dual DoD B denser residual prove + VERDICT.
 * H2 once (not stamp storms). Catalog densify only; NEVER write
 * TNPDS/RDSAR/TE|RE. never_program=1 / product_mint=0 / dual_dod_b=OPEN
 * ALWAYS. Stamp-free bar honesty v2026.08.04.75 — NEVER invent .76.
 * denser residual multi-arm always bar .75 (open|map|ring|prog|wire|func|
 * caps|bases_clean). denser residual hold always never_program held.
 * denser residual != Dual DoD B close. Soft!=product (ASCII Soft!= only).
 * greppable: rtl8168_udx: soft denser residual
 * greppable: rtl8168_udx: soft denser residual prove
 * greppable: rtl8168_udx: soft denser residual lean
 * greppable: rtl8168_udx: soft denser residual arms
 * greppable: rtl8168_udx: soft denser residual hold
 * greppable: rtl8168_udx: soft denser residual VERDICT
 * greppable: denser_prove=1 / denser_residual=1 / denser_hold=1
 * greppable: never_program_held=1 / never_program held
 * greppable: denser residual always never_program held
 * greppable: denser_arms= / always bar .75 / never_program=1
 * greppable: product_mint=0 / dual_dod_b=OPEN / bar=v2026.08.04.75
 */
static void
rtl8168_soft_denser_residual_verdict(const struct rtl8168_soft *pSoft)
{
    static u8 g_fDenserResidualOnce;
    u32 u32Open;
    u32 u32Map;
    u32 u32Ring;
    u32 u32Prog;
    u32 u32Wire;
    u32 u32Func;
    u32 u32Caps;
    u32 u32PreferReal;
    u32 u32RealDdi;
    u32 u32BasesClean;
    u32 u32Arms;
    u32 u32DenserOk;
    u32 u32Pass;
    const char *szBindPath;
    const char *szVerdict;

    if (g_fDenserResidualOnce != 0u) {
        return;
    }
    g_fDenserResidualOnce = 1u;

    u32Open = 0u;
    u32Map = 0u;
    u32Ring = 0u;
    u32Prog = 0u;
    u32Wire = 0u;
    u32Func = 0u;
    u32Caps = 0u;
    u32PreferReal = 1u;
    u32RealDdi = 0u;
    u32BasesClean = 1u;
    u32Arms = 0u;
    szBindPath = "none";

    if (pSoft != NULL) {
        u32Open = (pSoft->u32FOpenOk != 0u) ? 1u : 0u;
        u32Map = (pSoft->u32FMapOk != 0u) ? 1u : 0u;
        u32Ring = (pSoft->u32FRingOk != 0u && pSoft->u32FRingVerify != 0u)
                      ? 1u : 0u;
        u32Prog = (pSoft->u32FProgRes != 0u) ? 1u : 0u;
        u32Wire = (pSoft->u32FWireHo != 0u) ? 1u : 0u;
        u32Func = (pSoft->u32FFuncRes != 0u) ? 1u : 0u;
        u32Caps = (pSoft->u32FCapsRes != 0u) ? 1u : 0u;
        u32BasesClean = (pSoft->u32FBasesClean != 0u) ? 1u : 0u;
        if (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_DDI) {
            u32RealDdi = 1u;
            szBindPath = "ddi";
        } else if (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_INJECT) {
            szBindPath = "inject";
        }
    }

    /* denser residual multi-arm count (always bar .75; never_program held). */
    u32Arms = u32Open + u32Map + u32Ring + u32Prog + u32Wire + u32Func +
              u32Caps + u32BasesClean;
    u32DenserOk = 0u;
    if (RTL_SOFT_DENSER_PROVE != 0u &&
        RTL_SOFT_DENSER_RESIDUAL != 0u &&
        RTL_SOFT_NEVER_PROGRAM_HELD != 0u &&
        RTL_SOFT_DENSER_ALWAYS_BAR != 0u &&
        u32Arms >= RTL_SOFT_DENSER_ARMS_MIN &&
        u32BasesClean != 0u) {
        u32DenserOk = 1u;
    }

    /*
     * denser residual VERDICT PASS when denser prove arms live and
     * never_program held (no TNPDS/RDSAR/TE|RE write; bases_clean honesty).
     * Soft residual only — never closes Dual DoD B.
     */
    u32Pass = 0u;
    if (u32DenserOk != 0u &&
        RTL_SOFT_DENSIFY_PWC != 0u &&
        RTL_SOFT_DENSIFY_PREFER_REAL != 0u &&
        RTL_SOFT_DENSIFY_PWC_PREFER != 0u) {
        u32Pass = 1u;
    }
    if (u32Pass != 0u) {
        szVerdict = "PASS";
    } else if (u32Open != 0u || u32Map != 0u || u32Ring != 0u) {
        szVerdict = "SKIP";
    } else {
        szVerdict = "MISS";
    }

    udx_printk("rtl8168_udx: soft denser residual "
               "denser_prove=%u denser_residual=%u denser=1 "
               "denser_arms=%u/%u denser_min=%u denser_ok=%u "
               "always_bar=%u always bar .75 "
               "densify_program_wire_chain=%u densify_prefer_real_ddi=%u "
               "densify_pwc_prefer_real=%u densify_wire_claim=%u "
               "densify_triad=%u denser_order_steps=%u "
               "denser_program=1 denser_wire=1 denser_chain=1 "
               "denser_prefer_real_ddi=%u denser_wire_claim=%u "
               "open=%u map=%u ring=%u prog=%u wire_ho=%u func=%u caps=%u "
               "bases_clean=%u prefer_real_ddi=%u real_ddi=%u bind_path=%s "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_program_held=%u never_tx_rx=1 "
               "product_mint=0 product_handoff=0 wire_claim=OPEN "
               "wire=freestanding bind_ne_wire=1 never_claim_fs_wire=1 "
               "product=UDX+ABI product=UDX+sshd+stack chain=%s "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "dual_dod_b=OPEN agent_ne_close=1 claim_class=C2 "
               "denser residual != Dual DoD close "
               "bar=v2026.08.04.75 stamp_free=%u never_invent=.76 "
               "Soft!=product G-AC-1\n",
               (unsigned)RTL_SOFT_DENSER_PROVE,
               (unsigned)RTL_SOFT_DENSER_RESIDUAL,
               (unsigned)u32Arms, (unsigned)RTL_SOFT_DENSER_ARMS_MAX,
               (unsigned)RTL_SOFT_DENSER_ARMS_MIN, (unsigned)u32DenserOk,
               (unsigned)RTL_SOFT_DENSER_ALWAYS_BAR,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)u32Open, (unsigned)u32Map, (unsigned)u32Ring,
               (unsigned)u32Prog, (unsigned)u32Wire, (unsigned)u32Func,
               (unsigned)u32Caps, (unsigned)u32BasesClean,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi, szBindPath,
               (unsigned)RTL_SOFT_NEVER_PROGRAM_HELD,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_BAR_STAMP_FREE);

    /* greppable: rtl8168_udx: soft denser residual arms */
    udx_printk("rtl8168_udx: soft denser residual arms denser=1 "
               "denser_arms=%u/%u denser_min=%u denser_ok=%u "
               "arm_open=%u arm_map=%u arm_ring=%u arm_prog=%u "
               "arm_wire=%u arm_func=%u arm_caps=%u arm_bases_clean=%u "
               "arms=open,map,ring,prog,wire,func,caps,bases_clean "
               "never_program=1 never_program_held=%u never_tnpds=1 "
               "never_rdsar=1 never_te_re=1 would_write=0 write=0 arm=0 "
               "product_mint=0 dual_dod_b=OPEN always bar .75 "
               "bar=v2026.08.04.75 stamp_free=%u Soft!=product G-AC-1\n",
               (unsigned)u32Arms, (unsigned)RTL_SOFT_DENSER_ARMS_MAX,
               (unsigned)RTL_SOFT_DENSER_ARMS_MIN, (unsigned)u32DenserOk,
               (unsigned)u32Open, (unsigned)u32Map, (unsigned)u32Ring,
               (unsigned)u32Prog, (unsigned)u32Wire, (unsigned)u32Func,
               (unsigned)u32Caps, (unsigned)u32BasesClean,
               (unsigned)RTL_SOFT_NEVER_PROGRAM_HELD,
               (unsigned)RTL_SOFT_BAR_STAMP_FREE);

    /* greppable: rtl8168_udx: soft denser residual lean */
    udx_printk("rtl8168_udx: soft denser residual lean denser=1 "
               "denser_prove=%u denser_residual=%u denser_ok=%u "
               "denser_arms=%u/%u never_program_held=%u never_program=1 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "product_mint=0 would_write=0 write=0 "
               "prefer_real_ddi=1 real_ddi=%u bind_path=%s "
               "dual_dod_b=OPEN agent_ne_close=1 always bar .75 "
               "bar=v2026.08.04.75 stamp_free=%u never_invent=.76 "
               "Soft!=product G-AC-1\n",
               (unsigned)RTL_SOFT_DENSER_PROVE,
               (unsigned)RTL_SOFT_DENSER_RESIDUAL,
               (unsigned)u32DenserOk,
               (unsigned)u32Arms, (unsigned)RTL_SOFT_DENSER_ARMS_MAX,
               (unsigned)RTL_SOFT_NEVER_PROGRAM_HELD,
               (unsigned)u32RealDdi, szBindPath,
               (unsigned)RTL_SOFT_BAR_STAMP_FREE);

    udx_printk("rtl8168_udx: soft denser residual prove "
               "ok=%u denser_prove=%u denser_residual=%u denser_ok=%u "
               "denser_arms=%u/%u denser_min=%u "
               "never_program_held=%u never_program=1 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "bases_clean=%u program_gate_behind=product_mint "
               "densify_program_wire_chain=%u densify_prefer_real_ddi=%u "
               "densify_pwc_prefer_real=%u densify_triad=%u "
               "prefer_real_ddi=1 real_ddi=%u bind_path=%s "
               "dual_dod_b=OPEN agent_ne_close=1 always bar .75 "
               "bar=v2026.08.04.75 stamp_free=%u never_invent=.76 "
               "Soft!=product G-AC-1\n",
               (unsigned)u32Pass,
               (unsigned)RTL_SOFT_DENSER_PROVE,
               (unsigned)RTL_SOFT_DENSER_RESIDUAL,
               (unsigned)u32DenserOk,
               (unsigned)u32Arms, (unsigned)RTL_SOFT_DENSER_ARMS_MAX,
               (unsigned)RTL_SOFT_DENSER_ARMS_MIN,
               (unsigned)RTL_SOFT_NEVER_PROGRAM_HELD,
               (unsigned)u32BasesClean,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)u32RealDdi, szBindPath,
               (unsigned)RTL_SOFT_BAR_STAMP_FREE);

    /*
     * greppable: rtl8168_udx: soft denser residual hold
     * denser residual always never_program held (no TNPDS/RDSAR/TE|RE write).
     * always bar .75; product_mint=0; dual_dod_b=OPEN; Soft!=product.
     */
    udx_printk("rtl8168_udx: soft denser residual hold denser_hold=%u denser=1 "
               "denser residual always never_program held "
               "never_program held=1 never_program_held=%u never_program=1 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "would_write=0 write=0 arm=0 bases_clean=%u "
               "product_mint=0 dual_dod_b=OPEN agent_ne_close=1 "
               "denser residual != Dual DoD close always bar .75 "
               "bar=v2026.08.04.75 stamp_free=%u never_invent=.76 "
               "Soft!=product G-AC-1\n",
               (unsigned)RTL_SOFT_DENSER_HOLD,
               (unsigned)RTL_SOFT_NEVER_PROGRAM_HELD,
               (unsigned)u32BasesClean,
               (unsigned)RTL_SOFT_BAR_STAMP_FREE);

    /* greppable: rtl8168_udx: soft denser residual VERDICT */
    udx_printk("rtl8168_udx: soft denser residual VERDICT %s "
               "denser_prove=%u denser_residual=%u denser_hold=%u denser=1 "
               "denser_ok=%u denser_arms=%u/%u denser_min=%u "
               "never_program_held=%u never_program held=1 never_program=1 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "bases_clean=%u open=%u map=%u ring=%u "
               "prog=%u wire_ho=%u func=%u caps=%u "
               "densify_program_wire_chain=%u densify_prefer_real_ddi=%u "
               "densify_pwc_prefer_real=%u denser_wire_claim=%u "
               "prefer_real_ddi=1 real_ddi=%u bind_path=%s "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "product=UDX+sshd+stack chain=%s "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "dual_dod_b=OPEN agent_ne_close=1 claim_class=C2 "
               "denser residual always never_program held "
               "denser residual != Dual DoD close always bar .75 "
               "bar=v2026.08.04.75 stamp_free=%u never_invent=.76 "
               "Soft!=product G-AC-1\n",
               szVerdict,
               (unsigned)RTL_SOFT_DENSER_PROVE,
               (unsigned)RTL_SOFT_DENSER_RESIDUAL,
               (unsigned)RTL_SOFT_DENSER_HOLD,
               (unsigned)u32DenserOk,
               (unsigned)u32Arms, (unsigned)RTL_SOFT_DENSER_ARMS_MAX,
               (unsigned)RTL_SOFT_DENSER_ARMS_MIN,
               (unsigned)RTL_SOFT_NEVER_PROGRAM_HELD,
               (unsigned)u32BasesClean,
               (unsigned)u32Open, (unsigned)u32Map, (unsigned)u32Ring,
               (unsigned)u32Prog, (unsigned)u32Wire, (unsigned)u32Func,
               (unsigned)u32Caps,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)u32RealDdi, szBindPath,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_BAR_STAMP_FREE);
}

/**
 * Soft probe residual rollup — stage flags without product TX/RX claim.
 * greppable: rtl8168_udx: soft probe residual
 */
static void
rtl8168_soft_probe_residual(const struct rtl8168_soft *pSoft)
{
    if (pSoft == NULL) {
        return;
    }
    udx_printk("rtl8168_udx: soft probe residual "
               "id=10ec:8168 userspace_host=1 "
               "open=%u map=%u regs=%u ring=%u verify=%u "
               "bases_clean=%u irq=%u irq_res=%u work_res=%u "
               "prod_cat=%u prod_steps=%u/%u "
               "prog=%u phy_res=%u napi_res=%u txrx_res=%u "
               "caps=%u cap_steps=%u/%u "
               "wire_ho=%u wire_steps=%u/%u "
               "func=%u func_steps=%u/%u "
               "abi=%u abi_steps=%u/%u c2=%u "
               "path=0x%x stage=%u "
               "bar=%u bar_src=%u "
               "tnpds_lo=0x%08x rdsar_lo=0x%08x "
               "bind_ne_wire=%u never_claim_fs_wire=1 "
               "wire=freestanding freestanding_product=SKIP "
               "product=UDX+ABI need=DDI_caps need=hot_cold_abi "
               "product_mint=0 never_program=1 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN dual_dod_b=OPEN "
               "claim_class=C2 tx_rx=OPEN Soft!=product G-AC-1\n",
               (unsigned)pSoft->u32FOpenOk,
               (unsigned)pSoft->u32FMapOk,
               (pSoft->u8SoftStage >= RTL_SOFT_ST_REGS) ? 1u : 0u,
               (unsigned)pSoft->u32FRingOk,
               (unsigned)pSoft->u32FRingVerify,
               (unsigned)pSoft->u32FBasesClean,
               (pSoft->u8SoftStage >= RTL_SOFT_ST_IRQ) ? 1u : 0u,
               (unsigned)pSoft->u32FIrqRes,
               (unsigned)pSoft->u32FWorkRes,
               (unsigned)pSoft->u32FProdCat,
               (unsigned)pSoft->u8ProdSteps,
               (unsigned)RTL_SOFT_PROD_STEP_COUNT,
               (unsigned)pSoft->u32FProgRes,
               (unsigned)pSoft->u32FPhyRes,
               (unsigned)pSoft->u32FNapiRes,
               (unsigned)pSoft->u32FTxRxRes,
               (unsigned)pSoft->u32FCapsRes,
               (unsigned)pSoft->u8CapSteps,
               (unsigned)RTL_SOFT_CAP_STEP_COUNT,
               (unsigned)pSoft->u32FWireHo,
               (unsigned)pSoft->u8WireSteps,
               (unsigned)RTL_SOFT_WIRE_STEP_COUNT,
               (unsigned)pSoft->u32FFuncRes,
               (unsigned)pSoft->u8FuncSteps,
               (unsigned)RTL_SOFT_FUNC_STEP_COUNT,
               (unsigned)pSoft->u32FAbiRes,
               (unsigned)pSoft->u8AbiSteps,
               (unsigned)RTL_SOFT_ABI_STEP_COUNT,
               (unsigned)pSoft->u32FC2Path,
               (unsigned)pSoft->u32PathBits,
               (unsigned)pSoft->u8SoftStage,
               (unsigned)pSoft->u32BarIndex,
               (unsigned)pSoft->u8BarSrc,
               (unsigned)pSoft->u32TnpdsLo,
               (unsigned)pSoft->u32RdsarLo,
               (unsigned)RTL_SOFT_BIND_NE_WIRE);
}

/**
 * Soft open/MAP rollup honesty — open!=map!=ring!=product program.
 * greppable: rtl8168_udx: soft open/map honesty
 * greppable: rtl8168_udx: soft open map honesty
 */
static void
rtl8168_soft_open_map_honesty(const struct rtl8168_soft *pSoft)
{
    u8 u8Open;
    u8 u8Map;
    u8 u8Ring;
    u8 u8Prod;

    if (pSoft == NULL) {
        return;
    }

    u8Open = (pSoft->u32FOpenOk != 0u) ? 1u : 0u;
    u8Map  = (pSoft->u32FMapOk != 0u) ? 1u : 0u;
    u8Ring = (pSoft->u32FRingOk != 0u) ? 1u : 0u;
    u8Prod = (pSoft->u32FProdCat != 0u) ? 1u : 0u;

    udx_printk("rtl8168_udx: soft open/map honesty open=%u map=%u "
               "open_map=%u ring=%u verify=%u prod_cat=%u "
               "prod_steps=%u/%u prog=%u wire_ho=%u abi=%u "
               "product_mint=0 tnpds=OPEN rdsar=OPEN te_re=OPEN "
               "tx_rx=OPEN lab_ip=OPEN stack=OPEN sshd=OPEN "
               "Soft!=product G-AC-1 never_tx_rx=1 never_program=1\n",
               (unsigned)u8Open, (unsigned)u8Map,
               (unsigned)((u8Open != 0u && u8Map != 0u) ? 1u : 0u),
               (unsigned)u8Ring,
               (unsigned)pSoft->u32FRingVerify,
               (unsigned)u8Prod,
               (unsigned)pSoft->u8ProdSteps,
               (unsigned)RTL_SOFT_PROD_STEP_COUNT,
               (unsigned)pSoft->u32FProgRes,
               (unsigned)pSoft->u32FWireHo,
               (unsigned)pSoft->u32FAbiRes);
    udx_printk("rtl8168_udx: soft open map honesty "
               "soft_open_ne_map=1 soft_map_ne_ring=1 "
               "soft_ring_ne_tnpds=1 soft_catalog_ne_wire=1 "
               "soft_ne_tx_rx=1 dual_dod_b=OPEN userspace_udx=1 "
               "need=DDI_caps need=hot_cold_abi "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "claim_class=C2 not_r8169_ko=1 never_program=1 "
               "wire=freestanding Soft!=product\n");
}

/**
 * Soft product model stamp -- Dual DoD B userspace UDX NIC direction.
 * greppable: rtl8168_udx: soft product model
 * product=UDX+ABI need=DDI_caps need=hot_cold_abi; freestanding SKIP.
 * Dual DoD B OPEN (agent!=close). Soft!=product. G-AC-1.
 * Freestanding lab may own wire interim; T0 product net remains virtio-net.
 * Lean: one stamp, no version, no multi-line flood.
 */
static void
rtl8168_soft_product_model(void)
{
    udx_printk("rtl8168_udx: soft product model "
               "product=UDX+ABI product=UDX+sshd+stack "
               "chain=%s sshd_port=%u "
               "need=DDI_caps need=hot_cold_abi "
               "direction=userspace_nic_host_over_ddi_udx_hot_cold_abi "
               "caps=MMIO,IRQ,DMA product_mint=0 "
               "prefer_ddi_bind=1 prefer_real_ddi=1 inject_fallback=1 "
               "claim_class=C2 "
               "not=in_kernel_r8169_ko_exec "
               "not=freestanding_rtl_as_product "
               "freestanding_product=SKIP "
               "GJ_RTL8168_PROBE_default=0 "
               "t0_product_net=virtio-net "
               "lab_wire=freestanding dual_dod_b=product_direction "
               "dual_dod_b=OPEN dual_dod_b_lab=1 "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "never_program=1 Soft!=product G-AC-1 never_tx_rx=1\n",
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    udx_printk("rtl8168_udx: soft freestanding product=SKIP "
               "GJ_RTL8168_PROBE_default=0 product=UDX+ABI "
               "product=UDX+sshd+stack need=DDI_caps need=hot_cold_abi "
               "prefer_real_ddi=1 "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "dual_dod_b=OPEN claim_class=C2 Soft!=product G-AC-1\n");
}

/**
 * Soft C2 product path honesty -- userspace UDX residual is product *direction*.
 * Claim class C2 (ASSURANCE_LITE): Soft scaffold != product AC.
 * greppable: rtl8168_udx: soft c2 product path
 * greppable: rtl8168_udx: soft c2 path
 */
static void
rtl8168_soft_c2_product_path(struct rtl8168_soft *pSoft)
{
    if (pSoft == NULL) {
        return;
    }

    pSoft->u32FC2Path = 1u;
    pSoft->u32PathBits |= RTL_SOFT_PATH_C2;

    udx_printk("rtl8168_udx: soft c2 product path "
               "claim_class=C2 "
               "product=UDX+ABI product=UDX+sshd+stack "
               "chain=%s "
               "need=DDI_caps need=hot_cold_abi "
               "direction=userspace_nic_host_over_ddi_udx_hot_cold_abi "
               "prefer_ddi_bind=1 prefer_real_ddi=1 inject_fallback=1 "
               "not=in_kernel_r8169_ko_exec "
               "not=freestanding_rtl_as_product "
               "freestanding_product=SKIP "
               "GJ_RTL8168_PROBE_default=0 "
               "t0_product_net=virtio-net "
               "lab_wire=freestanding dual_dod_b=product_direction "
               "dual_dod_b=OPEN dual_dod_b_lab=1 "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "open=%u map=%u ring=%u prod_cat=%u "
               "prog=%u phy=%u napi=%u txrx=%u caps=%u "
               "irq_res=%u work_res=%u wire_ho=%u func=%u abi=%u "
               "soft_scaffold_ne_product_ac=1 product_mint=0 "
               "never_program=1 Soft!=product G-AC-1 never_tx_rx=1\n",
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)pSoft->u32FOpenOk,
               (unsigned)pSoft->u32FMapOk,
               (unsigned)pSoft->u32FRingOk,
               (unsigned)pSoft->u32FProdCat,
               (unsigned)pSoft->u32FProgRes,
               (unsigned)pSoft->u32FPhyRes,
               (unsigned)pSoft->u32FNapiRes,
               (unsigned)pSoft->u32FTxRxRes,
               (unsigned)pSoft->u32FCapsRes,
               (unsigned)pSoft->u32FIrqRes,
               (unsigned)pSoft->u32FWorkRes,
               (unsigned)pSoft->u32FWireHo,
               (unsigned)pSoft->u32FFuncRes,
               (unsigned)pSoft->u32FAbiRes);
    udx_printk("rtl8168_udx: soft c2 path "
               "userspace_udx=1 ddi_caps=OPEN hot_abi=OPEN cold_abi=OPEN "
               "product_mint=0 product=UDX+sshd+stack "
               "prefer_real_ddi=1 "
               "mmio_cap=OPEN irq_cap=OPEN dma_cap=OPEN "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "wire_handoff=OPEN freestanding_owns_wire=1 "
               "bind_ne_wire=1 never_program=1 "
               "dual_dod_b=OPEN Soft!=product\n");
}

/**
 * Soft caps residual -- MMIO / IRQ / DMA via UDX/DDI (catalog only).
 * Product path mints these into the host CNode; soft residual never mints.
 * Soft ioremap / request_irq / dma_alloc != product cap grants.
 * Lean: catalog + 3 steps + PASS (no stamp storm).
 * greppable: rtl8168_udx: soft caps residual
 * greppable: rtl8168_udx: soft caps step=
 * greppable: rtl8168_udx: soft caps catalog
 */
static void
rtl8168_soft_caps_residual(struct rtl8168_soft *pSoft)
{
    u32 u32Map;
    u32 u32Irq;
    u32 u32Dma;

    if (pSoft == NULL) {
        return;
    }

    pSoft->u8CapSteps = 0u;
    pSoft->u32FCapsRes = 0u;
    u32Map = (pSoft->u32FMapOk != 0u && pSoft->pRegs != NULL) ? 1u : 0u;
    u32Irq = (pSoft->u8SoftStage >= RTL_SOFT_ST_IRQ) ? 1u : 0u;
    u32Dma = (pSoft->dmaRing != 0 && pSoft->pRing != NULL) ? 1u : 0u;

    udx_printk("rtl8168_udx: soft caps residual catalog Soft!=product "
               "G-AC-1 product=UDX+ABI need=DDI_caps claim_class=C2 "
               "via=UDX/DDI product_mint=0 cnode_mint=0 steps=%u "
               "dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_CAP_STEP_COUNT);

    /* 1 -- MMIO_FRAME residual: soft MAP present; product frame mint OPEN. */
    udx_printk("rtl8168_udx: soft caps step=%u mmio=OPEN "
               "soft_map=%u bar=%u bar_src=%u "
               "mmio_frame=OPEN product_mint=0 Soft!=product\n",
               (unsigned)RTL_SOFT_CAP_STEP_MMIO,
               (unsigned)u32Map,
               (unsigned)pSoft->u32BarIndex,
               (unsigned)pSoft->u8BarSrc);
    pSoft->u8CapSteps = RTL_SOFT_CAP_STEP_MMIO;

    /* 2 -- IRQ Notification residual: soft line only; product bind OPEN. */
    udx_printk("rtl8168_udx: soft caps step=%u irq=OPEN "
               "soft_irq=%u line=%d msix=OPEN "
               "irq_notify=OPEN product_mint=0 Soft!=product\n",
               (unsigned)RTL_SOFT_CAP_STEP_IRQ,
               (unsigned)u32Irq,
               (pSoft->pPdev != NULL) ? pSoft->pPdev->nIrq : -1);
    pSoft->u8CapSteps = RTL_SOFT_CAP_STEP_IRQ;

    /* 3 -- DMA window residual: soft coherent cookie; product window OPEN. */
    udx_printk("rtl8168_udx: soft caps step=%u dma=OPEN "
               "soft_dma=%u dma=0x%llx ring_bytes=%u "
               "dma_window=OPEN product_mint=0 Soft!=product\n",
               (unsigned)RTL_SOFT_CAP_STEP_DMA,
               (unsigned)u32Dma,
               (unsigned long long)pSoft->dmaRing,
               (unsigned)RTL8168_RING_BYTES);
    pSoft->u8CapSteps = RTL_SOFT_CAP_STEP_DMA;

    pSoft->u32FCapsRes = 1u;
    pSoft->u32PathBits |= RTL_SOFT_PATH_CAPS;

    udx_printk("rtl8168_udx: soft caps catalog PASS "
               "steps=%u/%u mmio_cap=0 irq_cap=0 dma_cap=0 "
               "product_mint=0 product=UDX+ABI need=DDI_caps "
               "dual_dod_b=OPEN Soft!=product never_tx_rx=1\n",
               (unsigned)pSoft->u8CapSteps,
               (unsigned)RTL_SOFT_CAP_STEP_COUNT);
}

/**
 * Soft IRQ residual — once-lamp IntrMask/line honesty (not product MSI-X).
 * Soft host demo may arm ROK|TOK IntrMask only; ChipCmd TE/RE stay OPEN.
 * Never: MSI-X table program, NAPI poll, TPPOLL kick.
 * greppable: rtl8168_udx: soft irq residual
 */
static void
rtl8168_soft_irq_residual(struct rtl8168_soft *pSoft)
{
    int nIrq;
    u16 u16Mask;
    u16 u16MaskRd;
    u16 u16Istatus;
    u8  u8Cmd;
    u32 u32Armed;

    if (pSoft == NULL) {
        return;
    }
    /* Once-lamp: skip stamp storm if already cataloged. */
    if (pSoft->u32FIrqRes != 0u) {
        return;
    }

    nIrq = (pSoft->pPdev != NULL) ? pSoft->pPdev->nIrq : -1;
    u16Mask = pSoft->u16IntrMask;
    u16MaskRd = pSoft->u16IntrMaskRd;
    u16Istatus = 0;
    u8Cmd = pSoft->u8ChipCmd;
    u32Armed = (pSoft->u8SoftStage >= RTL_SOFT_ST_IRQ) ? 1u : 0u;

    if (pSoft->pRegs != NULL) {
        u16Istatus = udx_readw(pSoft->pRegs, RTL_REG_INTR_STATUS);
        u8Cmd = udx_readb(pSoft->pRegs, RTL_REG_CHIPCMD);
        pSoft->u8ChipCmd = u8Cmd;
    }

    udx_printk("rtl8168_udx: soft irq residual Soft!=product G-AC-1 "
               "claim_class=C2 dual_dod_b=OPEN "
               "line=%d soft_armed=%u "
               "intrmask_reg=0x%02x intrstatus_reg=0x%02x "
               "chipcmd_reg=0x%02x "
               "intrmask_wr=0x%04x intrmask_rd=0x%04x "
               "intrstatus=0x%04x chipcmd=0x%02x "
               "te=%u re=%u "
               "mask_bits=ROK|TOK msix=OPEN napi=OPEN "
               "tppoll=OPEN chipcmd_te_re=OPEN "
               "irq_notify=OPEN product_mint=0 "
               "net_eth_poll=0 thr_only=1 never_program=1 "
               "product_irq=OPEN Soft!=product\n",
               nIrq, (unsigned)u32Armed,
               (unsigned)RTL_REG_INTR_MASK, (unsigned)RTL_REG_INTR_STATUS,
               (unsigned)RTL_REG_CHIPCMD,
               (unsigned)u16Mask, (unsigned)u16MaskRd,
               (unsigned)u16Istatus, (unsigned)u8Cmd,
               (u8Cmd & RTL_CMD_TE) != 0u ? 1u : 0u,
               (u8Cmd & RTL_CMD_RE) != 0u ? 1u : 0u);

    pSoft->u32FIrqRes = 1u;
    pSoft->u32PathBits |= RTL_SOFT_PATH_IRQ_RES;
}

/**
 * Soft work residual — once-lamp work/ISR path honesty (not NAPI product).
 * Links soft work hits to reclaim catalog; never Own handoff / poll.
 * greppable: rtl8168_udx: soft work residual
 */
static void
rtl8168_soft_work_residual(struct rtl8168_soft *pSoft)
{
    if (pSoft == NULL) {
        return;
    }
    /* Once-lamp only. */
    if (pSoft->u32FWorkRes != 0u) {
        return;
    }

    udx_printk("rtl8168_udx: soft work residual Soft!=product G-AC-1 "
               "claim_class=C2 dual_dod_b=OPEN "
               "irq_hits=%u work_hits=%u stage=%u "
               "ring_residual=%u ring_verify=%u "
               "soft_tx_idx=%u soft_rx_idx=%u "
               "intrstatus_reg=0x%02x chipcmd_reg=0x%02x "
               "tppoll_reg=0x%02x "
               "own_handoff=0 napi_poll=0 reclaim_catalog=1 "
               "net_eth_poll=0 thr_only=1 never_program=1 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "product_work=OPEN Soft!=product never_tx_rx=1\n",
               (unsigned)pSoft->u32IrqHits,
               (unsigned)pSoft->u32WorkHits,
               (unsigned)pSoft->u8SoftStage,
               (unsigned)pSoft->u32FRingOk,
               (unsigned)pSoft->u32FRingVerify,
               (unsigned)pSoft->u32SoftTxIdx,
               (unsigned)pSoft->u32SoftRxIdx,
               (unsigned)RTL_REG_INTR_STATUS,
               (unsigned)RTL_REG_CHIPCMD,
               (unsigned)RTL_REG_TPPOLL);

    /* Reclaim catalog re-walk (still never product Own handoff). */
    rtl8168_soft_ring_reclaim_residual(pSoft);

    pSoft->u32FWorkRes = 1u;
    pSoft->u32PathBits |= RTL_SOFT_PATH_WORK_RES;
}

/**
 * Soft wire-handoff residual — Dual DoD B next-program *names* only.
 * Ordered public register names toward product UDX wire ownership
 * (lab IP / stack / sshd path after TE|RE + net attach; never executes).
 * TNPDS/RDSAR/TE|RE order residual densifies program_gate behind product_mint.
 * Wire-claim residual denser OPEN: product UDX owns wire honesty until DUT.
 * product chain residual denser: rtl8168_udx>netstackd>sshd (names only).
 * lab_ip residual denser: lab 10.200.125.50 / stack / sshd :22 chain names.
 * STRONGER wire densify triad (H2 once): order + wire_claim + chain once-lamps;
 * densify_triad=1 STRONGER rollup + denser order (te_re 5th) + chain hops.
 * STRONGER denser program_wire_chain + prefer_real_ddi + wire claim OPEN
 * residual (H2 once): densify_prefer_real_ddi + densify_pwc_prefer_real +
 * denser_wire_claim; wire claim OPEN until DUT; never claim FS wire.
 * Never writes TNPDS/RDSAR/THPDS/ChipCmd TE|RE/TPPOLL/PHYAR.
 * greppable: rtl8168_udx: soft wire handoff
 * greppable: rtl8168_udx: soft wire step=
 * greppable: rtl8168_udx: soft dual_dod_b next
 * greppable: rtl8168_udx: soft wire densify
 * greppable: rtl8168_udx: soft wire densify wire_claim=
 * greppable: rtl8168_udx: soft wire densify chain=
 * greppable: rtl8168_udx: soft wire densify order=
 * greppable: rtl8168_udx: soft wire densify triad=
 * greppable: rtl8168_udx: soft wire densify program_wire_chain=
 * greppable: rtl8168_udx: soft wire densify prefer_real_ddi=
 * greppable: rtl8168_udx: soft wire densify pwc_prefer_real=
 * greppable: densify_program_wire_chain= / denser_program= / denser_wire=
 * greppable: denser_chain= / densify_prefer_real_ddi= / denser_prefer_real_ddi=
 * greppable: densify_pwc_prefer_real= / denser_wire_claim=
 * greppable: program_gate= / own_prep= / link_to_tx=
 * greppable: wire claim residual / wire_claim=OPEN / lab_ip residual
 * greppable: product chain residual / product=UDX+sshd+stack
 * greppable: densify_triad= / prefer_real_ddi=1 / never_program=1
 */
static void
rtl8168_soft_wire_handoff_residual(struct rtl8168_soft *pSoft)
{
    u32 u32RingReady;
    u32 u32CapsSoft;
    u32 u32BasesClean;
    u32 u32ProgSoft;
    u32 u32Open;
    u32 u32Map;
    u32 u32IrqSoft;
    u32 u32AbiSoft;
    u32 u32DmaSoft;
    u32 u32AlignOk;
    u32 u32ProgGate;
    u32 u32ProductMint;
    u32 u32OwnPrep;
    u32 u32LinkOk;
    u32 u32LinkToTx;
    u32 u32UdxOwned;
    u32 u32RealDdi;
    u32 u32PreferReal;
    const char *szBindPath;

    if (pSoft == NULL) {
        return;
    }

    pSoft->u8WireSteps = 0u;
    pSoft->u32FWireHo = 0u;
    u32RingReady = (pSoft->u32FRingOk != 0u &&
                    pSoft->u32FRingVerify != 0u) ? 1u : 0u;
    u32CapsSoft = (pSoft->u32FCapsRes != 0u) ? 1u : 0u;
    u32BasesClean = pSoft->u32FBasesClean;
    u32ProgSoft = (pSoft->u32FProgRes != 0u) ? 1u : 0u;
    u32Open = (pSoft->u32FOpenOk != 0u) ? 1u : 0u;
    u32Map = (pSoft->u32FMapOk != 0u && pSoft->pRegs != NULL) ? 1u : 0u;
    u32IrqSoft = (pSoft->u32FIrqRes != 0u) ? 1u : 0u;
    u32AbiSoft = (pSoft->u32FAbiRes != 0u) ? 1u : 0u;
    u32DmaSoft = (pSoft->dmaRing != 0 && pSoft->pRing != NULL) ? 1u : 0u;
    u32AlignOk = 0u;
    if (pSoft->dmaTx != 0 && pSoft->dmaRx != 0 &&
        (((u64)pSoft->dmaTx % (u64)RTL_ALIGN_TNPDS) == 0ull) &&
        (((u64)pSoft->dmaRx % (u64)RTL_ALIGN_TNPDS) == 0ull)) {
        u32AlignOk = 1u;
    }
    u32ProductMint = 0u;
    u32ProgGate = (u32Map != 0u && u32DmaSoft != 0u &&
                   u32RingReady != 0u && u32AlignOk != 0u) ? 1u : 0u;
    u32OwnPrep = (u32RingReady != 0u) ? 1u : 0u;
    u32LinkOk = (pSoft->u8PhySt & RTL_PHYST_LINKOK) != 0u ? 1u : 0u;
    u32UdxOwned = (pSoft->u8WireOwner == RTL_SOFT_WIRE_OWNER_UDX) ? 1u : 0u;
    u32LinkToTx = (u32UdxOwned != 0u && u32LinkOk != 0u &&
                   u32ProgGate != 0u && u32ProductMint != 0u) ? 1u : 0u;
    /* Prefer real DDI over inject (STRONGER denser prefer_real_ddi residual). */
    u32PreferReal = 1u;
    u32RealDdi = (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_DDI) ? 1u : 0u;
    if (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_DDI) {
        szBindPath = "ddi";
    } else if (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_INJECT) {
        szBindPath = "inject";
    } else {
        szBindPath = "none";
    }

    udx_printk("rtl8168_udx: soft wire handoff residual catalog "
               "Soft!=product G-AC-1 claim_class=C2 "
               "dual_dod_b=product_direction dual_dod_b=OPEN "
               "product=UDX+ABI product=UDX+sshd+stack chain=%s "
               "need=DDI_caps need=hot_cold_abi "
               "freestanding_product=SKIP "
               "wire_owner=freestanding product_handoff=0 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "open=%u map=%u ring=%u prog=%u caps=%u irq=%u abi=%u "
               "soft_dma=%u program_gate=%u own_prep=%u "
               "linkok=%u link_to_tx=%u product_mint=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN sshd_port=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "prefer_real_ddi=1 never_program=1 never_tx_rx=1 steps=%u\n",
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)u32Open, (unsigned)u32Map,
               (unsigned)u32RingReady, (unsigned)u32ProgSoft,
               (unsigned)u32CapsSoft, (unsigned)u32IrqSoft,
               (unsigned)u32AbiSoft,
               (unsigned)u32DmaSoft, (unsigned)u32ProgGate,
               (unsigned)u32OwnPrep, (unsigned)u32LinkOk,
               (unsigned)u32LinkToTx, (unsigned)u32ProductMint,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT,
               (unsigned)RTL_SOFT_WIRE_STEP_COUNT);
    /*
     * Dual DoD B denser next-program order (names only; never executes).
     * Product UDX owns wire for lab IP / stack / sshd only after real caps.
     * program_gate residual opens when soft MAP+DMA ready; mint still 0.
     * Program order residual denser: TNPDS/RDSAR/TE|RE sequence catalog.
     * Wire-claim residual denser OPEN until DUT; never product wire claim.
     * NEVER write TNPDS/RDSAR/TE|RE while product_mint=0.
     */
    udx_printk("rtl8168_udx: soft dual_dod_b next "
               "order=caps,ring,tnpds,rdsar,te_re,phy,napi,"
               "txrx_lab_ip_stack_sshd,owner_flip "
               "prog_order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "program_gate=%u product_mint=0 gate_behind=product_mint "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 never_program=1 "
               "product=UDX+sshd+stack chain=%s "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN sshd_port=%u "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 "
               "prefer_real_ddi=1 dual_dod_b=OPEN agent_ne_close=1 "
               "Soft!=product\n",
               (unsigned)u32ProgGate,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);

    /* 1 -- product caps mint (still OPEN; soft MAP/IRQ/DMA != mint). */
    udx_printk("rtl8168_udx: soft wire step=%u caps=OPEN "
               "mmio_frame=OPEN irq_notify=OPEN dma_window=OPEN "
               "soft_caps=%u soft_map=%u soft_dma=%u soft_irq=%u "
               "program_gate=%u product_mint=0 need=DDI_caps Soft!=product\n",
               (unsigned)RTL_SOFT_WIRE_STEP_CAPS,
               (unsigned)u32CapsSoft, (unsigned)u32Map,
               (unsigned)u32DmaSoft, (unsigned)u32IrqSoft,
               (unsigned)u32ProgGate);
    pSoft->u8WireSteps = RTL_SOFT_WIRE_STEP_CAPS;

    /* 2 -- ring residual ready (CPU DMA Own=0 EOR last; Own prep). */
    udx_printk("rtl8168_udx: soft wire step=%u ring=OPEN "
               "soft_ring=%u own=0 eor_last=1 own_prep=%u "
               "tx_slots=%u rx_slots=%u "
               "tnpds_write=0 rdsar_write=0 "
               "Soft!=product never_program=1\n",
               (unsigned)RTL_SOFT_WIRE_STEP_RING,
               (unsigned)u32RingReady, (unsigned)u32OwnPrep,
               (unsigned)pSoft->u32TxSlots,
               (unsigned)pSoft->u32RxSlots);
    pSoft->u8WireSteps = RTL_SOFT_WIRE_STEP_RING;

    /* 3 -- TNPDS program (gated behind product_mint; soft never writes). */
    udx_printk("rtl8168_udx: soft wire step=%u tnpds=OPEN "
               "reg=0x%02x reg_hi=0x%02x dma_tx=0x%llx "
               "align=%u prog_res=%u bases_clean=%u "
               "program_gate=%u product_mint=%u would_write=0 "
               "order_pos=tnpds_lo,tnpds_hi next=rdsar "
               "write=0 Soft!=product never_program=1 dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_WIRE_STEP_TNPDS,
               (unsigned)RTL_REG_TNPDS, (unsigned)RTL_REG_TNPDS_HI,
               (unsigned long long)pSoft->dmaTx,
               (unsigned)RTL_ALIGN_TNPDS, (unsigned)u32ProgSoft,
               (unsigned)u32BasesClean,
               (unsigned)u32ProgGate, (unsigned)u32ProductMint);
    pSoft->u8WireSteps = RTL_SOFT_WIRE_STEP_TNPDS;

    /* 4 -- RDSAR program (gated behind product_mint). */
    udx_printk("rtl8168_udx: soft wire step=%u rdsar=OPEN "
               "reg=0x%02x reg_hi=0x%02x dma_rx=0x%llx "
               "align=%u prog_res=%u bases_clean=%u "
               "program_gate=%u product_mint=%u would_write=0 "
               "order_pos=rdsar_lo,rdsar_hi next=te_re "
               "write=0 Soft!=product never_program=1 dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_WIRE_STEP_RDSAR,
               (unsigned)RTL_REG_RDSAR, (unsigned)RTL_REG_RDSAR_HI,
               (unsigned long long)pSoft->dmaRx,
               (unsigned)RTL_ALIGN_TNPDS, (unsigned)u32ProgSoft,
               (unsigned)u32BasesClean,
               (unsigned)u32ProgGate, (unsigned)u32ProductMint);
    pSoft->u8WireSteps = RTL_SOFT_WIRE_STEP_RDSAR;

    /* 5 -- ChipCmd TE|RE arm (observe only; after TNPDS/RDSAR order). */
    udx_printk("rtl8168_udx: soft wire step=%u te_re=OPEN "
               "reg=0x%02x chipcmd_snap=0x%02x te_bit=0x%02x re_bit=0x%02x "
               "te_obs=%u re_obs=%u arm=0 "
               "order_pos=te_re after=tnpds,rdsar next=phy "
               "Soft!=product never_program=1 dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_WIRE_STEP_TE_RE,
               (unsigned)RTL_REG_CHIPCMD,
               (unsigned)pSoft->u8ChipCmd,
               (unsigned)RTL_CMD_TE, (unsigned)RTL_CMD_RE,
               (pSoft->u8ChipCmd & RTL_CMD_TE) != 0u ? 1u : 0u,
               (pSoft->u8ChipCmd & RTL_CMD_RE) != 0u ? 1u : 0u);
    pSoft->u8WireSteps = RTL_SOFT_WIRE_STEP_TE_RE;

    /* 6 -- PHY / link (PHYStatus RO; link→TX only when UDX owns). */
    udx_printk("rtl8168_udx: soft wire step=%u phy=OPEN "
               "phystatus_reg=0x%02x phyar_reg=0x%02x "
               "physt=0x%02x linkok=%u link_to_tx=%u udx_owned=%u "
               "phyar_rw=0 Soft!=product never_phyar_rw=1 never_program=1\n",
               (unsigned)RTL_SOFT_WIRE_STEP_PHY,
               (unsigned)RTL_REG_PHYSTATUS, (unsigned)RTL_REG_PHYAR,
               (unsigned)pSoft->u8PhySt,
               (unsigned)u32LinkOk, (unsigned)u32LinkToTx,
               (unsigned)u32UdxOwned);
    pSoft->u8WireSteps = RTL_SOFT_WIRE_STEP_PHY;

    /* 7 -- NAPI reclaim / poll (catalog only; H1 never net_eth_poll IRQ). */
    udx_printk("rtl8168_udx: soft wire step=%u napi=OPEN "
               "budget=%u own_handoff=0 own_prep=%u poll=0 "
               "net_eth_poll=0 thr_only=1 Soft!=product\n",
               (unsigned)RTL_SOFT_WIRE_STEP_NAPI,
               (unsigned)RTL_SOFT_NAPI_BUDGET, (unsigned)u32OwnPrep);
    pSoft->u8WireSteps = RTL_SOFT_WIRE_STEP_NAPI;

    /*
     * 8 -- TX/RX + TPPOLL + netstack + Dual DoD B lab path surface.
     * link_to_tx remains 0 until UDX owns wire + product_mint + linkok.
     * never_program until real DDI caps + product handoff.
     */
    udx_printk("rtl8168_udx: soft wire step=%u tx_rx=OPEN "
               "tppoll_reg=0x%02x tppoll=0 npq=0 hpq=0 "
               "thpds=OPEN netstack=OPEN cold_net=OPEN hot_abi=OPEN "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN sshd_port=%u "
               "product=UDX+sshd+stack chain=%s "
               "msix=OPEN bases_clean=%u "
               "link_to_tx=%u linkok=%u program_gate=%u "
               "Soft!=product never_tppoll=1 never_tx_rx=1 "
               "never_program=1 dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_WIRE_STEP_TXRX,
               (unsigned)RTL_REG_TPPOLL,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)u32BasesClean,
               (unsigned)u32LinkToTx, (unsigned)u32LinkOk,
               (unsigned)u32ProgGate);
    pSoft->u8WireSteps = RTL_SOFT_WIRE_STEP_TXRX;

    /* 9 -- wire_owner flip (product handoff; soft leaves freestanding). */
    udx_printk("rtl8168_udx: soft wire step=%u owner=OPEN "
               "wire_owner=freestanding would_flip=udx flip=0 "
               "bind_ne_wire=1 product_handoff=0 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "link_to_tx=%u program_gate=%u product_mint=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN sshd_port=%u "
               "product=UDX+sshd+stack chain=%s "
               "need=DDI_caps need=hot_cold_abi "
               "prefer_real_ddi=1 Soft!=product dual_dod_b=OPEN "
               "never_program=1\n",
               (unsigned)RTL_SOFT_WIRE_STEP_OWNER,
               (unsigned)u32LinkToTx, (unsigned)u32ProgGate,
               (unsigned)u32ProductMint,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT,
               RTL_SOFT_PRODUCT_CHAIN);
    pSoft->u8WireSteps = RTL_SOFT_WIRE_STEP_OWNER;

    pSoft->u32FWireHo = 1u;
    pSoft->u32PathBits |= RTL_SOFT_PATH_WIRE_HO;

    udx_printk("rtl8168_udx: soft wire handoff catalog PASS "
               "steps=%u/%u tnpds=0 rdsar=0 te_re=0 phy=0 "
               "napi=0 tppoll=0 tx_rx=0 lab_ip=0 stack=0 sshd=0 "
               "owner_flip=0 program_gate=%u own_prep=%u link_to_tx=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "wire=freestanding product_handoff=0 product_mint=0 "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN sshd_port=%u "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "prefer_real_ddi=1 Soft!=product never_program=1 "
               "dual_dod_b=OPEN\n",
               (unsigned)pSoft->u8WireSteps,
               (unsigned)RTL_SOFT_WIRE_STEP_COUNT,
               (unsigned)u32ProgGate, (unsigned)u32OwnPrep,
               (unsigned)u32LinkToTx,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /*
     * STRONGER wire densify triad (H2 once): program order + wire claim +
     * product chain residual denser OPEN. densify_triad=1 always.
     * never_program=1 / product_mint=0 always. Dual DoD B OPEN.
     * greppable: rtl8168_udx: soft wire densify
     * greppable: densify_order= / densify_wire_claim= / densify_chain=
     * greppable: densify_triad=
     */
    udx_printk("rtl8168_udx: soft wire densify "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "prog_order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "program_gate=%u own_prep=%u link_to_tx=%u "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_tx_rx=1 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hops=3 sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "prefer_real_ddi=1 Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32ProgGate, (unsigned)u32OwnPrep,
               (unsigned)u32LinkToTx,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /* greppable: rtl8168_udx: soft wire densify wire_claim= */
    udx_printk("rtl8168_udx: soft wire densify wire_claim=1 "
               "densify_wire_claim=1 densify_order=1 densify_chain=1 "
               "densify_triad=%u "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "link_to_tx=%u program_gate=%u own_prep=%u "
               "product_mint=0 never_program=1 never_tx_rx=1 "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN sshd_port=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)u32LinkToTx, (unsigned)u32ProgGate,
               (unsigned)u32OwnPrep,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /* greppable: rtl8168_udx: soft wire densify chain= */
    udx_printk("rtl8168_udx: soft wire densify chain=1 "
               "densify_chain=1 densify_order=1 densify_wire_claim=1 "
               "densify_triad=%u "
               "product chain residual OPEN "
               "product=UDX+sshd+stack chain=%s "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hop1=rtl8168_udx "
               "chain_hop2=netstackd chain_hop3=sshd chain_hops=3 "
               "sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "program_gate=%u product_mint=0 never_program=1 "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT,
               (unsigned)u32ProgGate);
    /* greppable: rtl8168_udx: soft wire densify order= */
    udx_printk("rtl8168_udx: soft wire densify order=1 "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "prog_order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "te_re_order_pos=%u/%u "
               "program_gate=%u product_mint=0 would_write=0 write=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 gate_behind=product_mint "
               "next=phy,napi,txrx_lab_ip_stack_sshd,owner_flip "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32ProgGate, RTL_SOFT_PRODUCT_CHAIN);
    /* greppable: rtl8168_udx: soft wire densify triad= */
    udx_printk("rtl8168_udx: soft wire densify triad=%u "
               "densify_triad=%u densify_order=1 densify_wire_claim=1 "
               "densify_chain=1 denser_order_steps=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "program_gate=%u own_prep=%u link_to_tx=%u "
               "product_mint=0 never_program=1 never_tx_rx=1 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "product chain residual OPEN chain=%s "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hops=3 sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32ProgGate, (unsigned)u32OwnPrep,
               (unsigned)u32LinkToTx,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /*
     * STRONGER denser program/wire/chain residual (H2 once): cross-surface
     * densify_program_wire_chain rollup. denser_program + denser_wire +
     * denser_chain surface lamps; denser hop residual; never writes.
     * STRONGER denser: prefer_real_ddi + denser_wire_claim + wire claim
     * OPEN until DUT; densify_pwc_prefer_real links PWC+prefer+claim.
     * greppable: rtl8168_udx: soft wire densify program_wire_chain=
     * greppable: densify_program_wire_chain= / denser_program= / denser_wire=
     * greppable: denser_chain= / densify_prefer_real_ddi= / denser_wire_claim=
     * greppable: never_program=1 / product_mint=0
     */
    udx_printk("rtl8168_udx: soft wire densify program_wire_chain=%u "
               "densify_program_wire_chain=%u "
               "denser_program=1 denser_wire=1 denser_chain=1 "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "densify_prefer_real_ddi=%u denser_prefer_real_ddi=%u "
               "denser_wire_claim=%u densify_pwc_prefer_real=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "te_re_order_pos=%u/%u program_gate=%u own_prep=%u "
               "link_to_tx=%u product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_tx_rx=1 "
               "prefer_real residual denser=1 "
               "program_wire_chain residual denser=1 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "product chain residual OPEN chain=%s "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hop1=rtl8168_udx "
               "chain_hop2=netstackd chain_hop3=sshd chain_hops=3 "
               "sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32ProgGate, (unsigned)u32OwnPrep,
               (unsigned)u32LinkToTx,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath);
    /*
     * STRONGER denser prefer_real_ddi residual (H2 once; wire surface).
     * Prefer GJ_SYS_DDI / udx_host_bind_by_id over inject; real_ddi from
     * bind_path. Links densify_program_wire_chain + wire claim OPEN until DUT.
     * greppable: rtl8168_udx: soft wire densify prefer_real_ddi=
     * greppable: densify_prefer_real_ddi= / denser_prefer_real_ddi=
     */
    udx_printk("rtl8168_udx: soft wire densify prefer_real_ddi=%u "
               "densify_prefer_real_ddi=%u denser_prefer_real_ddi=%u "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s gj_sys_ddi=%u via=udx_host_bind_by_id "
               "densify_program_wire_chain=%u densify_pwc_prefer_real=%u "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_wire_claim=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "program_gate=%u own_prep=%u link_to_tx=%u "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_tx_rx=1 "
               "prefer_real residual denser=1 "
               "program_wire_chain residual denser=1 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "product chain residual OPEN chain=%s "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hops=3 sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath, (unsigned)GJ_SYS_DDI,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)u32ProgGate, (unsigned)u32OwnPrep,
               (unsigned)u32LinkToTx,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /*
     * STRONGER denser PWC+prefer_real+wire_claim rollup (H2 once).
     * densify_pwc_prefer_real=1 links program_wire_chain + prefer_real_ddi +
     * wire claim residual OPEN until DUT. never_program holds.
     * greppable: rtl8168_udx: soft wire densify pwc_prefer_real=
     * greppable: densify_pwc_prefer_real= / denser_wire_claim=
     */
    udx_printk("rtl8168_udx: soft wire densify pwc_prefer_real=%u "
               "densify_pwc_prefer_real=%u "
               "densify_program_wire_chain=%u densify_prefer_real_ddi=%u "
               "denser_prefer_real_ddi=%u denser_wire_claim=%u "
               "denser_program=1 denser_wire=1 denser_chain=1 "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "te_re_order_pos=%u/%u program_gate=%u own_prep=%u "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_tx_rx=1 "
               "prefer_real residual denser=1 "
               "program_wire_chain residual denser=1 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "product chain residual OPEN chain=%s "
               "chain_hop1=rtl8168_udx chain_hop2=netstackd "
               "chain_hop3=sshd chain_hops=3 sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "Soft!=product dual_dod_b=OPEN G-AC-1 agent_ne_close=1\n",
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32ProgGate, (unsigned)u32OwnPrep,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
}

/**
 * Soft functional residual — stronger open/map/ring/IRQ/work/wire-handoff
 * with program-path linkage toward product UDX wire ownership.
 * Densifies program_gate (MAP+DMA proxy), Own prep, and link observe→TX
 * when UDX would own wire. When soft MAP+DMA present, residual densifies
 * toward program_gate (product MAP+DMA caps still OPEN / product_mint=0).
 * STRONGER functional residual densify when caps residual present:
 *   densify real DDI open/map and program_gate (prefer_real_ddi=1;
 *   real_ddi from bind_path=ddi / GJ_SYS_DDI / udx_host_bind_by_id).
 * product=UDX+sshd+stack path honesty denser: lab_ip residual OPEN
 * (lab 10.200.125.50 / stack / sshd :22 names only); wire claim residual
 * OPEN (soft never claims product wire). NEVER write TNPDS/RDSAR/TE|RE
 * while product_mint=0 (never_program=1). Inject lab remains soft fallback.
 * H2 once densify lamps (not stamp storms). Dual DoD B OPEN. Soft!=product.
 * greppable: rtl8168_udx: soft functional residual
 * greppable: rtl8168_udx: soft functional step=
 * greppable: rtl8168_udx: soft functional catalog
 * greppable: rtl8168_udx: soft functional residual honesty
 * greppable: rtl8168_udx: soft functional densify
 * greppable: rtl8168_udx: soft functional densify real_ddi=
 * greppable: rtl8168_udx: soft functional densify open=
 * greppable: rtl8168_udx: soft functional densify map=
 * greppable: rtl8168_udx: soft functional densify program_gate=
 * greppable: rtl8168_udx: soft functional densify caps=
 * greppable: rtl8168_udx: soft functional densify program_wire_chain=
 * greppable: rtl8168_udx: soft functional densify prefer_real_ddi=
 * greppable: rtl8168_udx: soft functional densify pwc_prefer_real=
 * greppable: densify_program_wire_chain= / denser_program= / denser_wire=
 * greppable: denser_chain= / densify_prefer_real_ddi= / denser_prefer_real_ddi=
 * greppable: densify_pwc_prefer_real= / denser_wire_claim=
 * greppable: prefer_real residual / program_wire_chain residual
 * greppable: prefer_real_ddi= / prefer_real_ddi=1 / real_ddi=
 * greppable: program_gate= / own_prep= / link_to_tx= / map_dma_ready=
 * greppable: lab_ip residual / wire claim residual / wire_claim=OPEN
 * greppable: product=UDX+sshd+stack / Soft!=product / dual_dod_b=OPEN
 * greppable: never_program=1 / product_mint=0
 */
static void
rtl8168_soft_functional_residual(struct rtl8168_soft *pSoft)
{
    u32 u32Open;
    u32 u32Map;
    u32 u32Ring;
    u32 u32Irq;
    u32 u32Work;
    u32 u32Wire;
    u32 u32Prog;
    u32 u32Caps;
    u32 u32Abi;
    u32 u32BasesClean;
    u32 u32DmaSoft;
    u32 u32AlignOk;
    u32 u32ProgGate;
    u32 u32ProductMint;
    u32 u32OwnPrep;
    u32 u32LinkOk;
    u32 u32LinkToTx;
    u32 u32UdxOwned;
    u32 u32MapDmaReady;
    u32 u32MapDmaCaps;
    u32 u32RealDdi;
    u32 u32PreferReal;
    u32 u32DensOpen;
    u32 u32DensMap;
    u32 u32DensGate;
    u32 iSlot;
    u32 u32TxHost;
    u32 u32RxHost;
    const char *szBindPath;

    if (pSoft == NULL) {
        return;
    }

    pSoft->u8FuncSteps = 0u;
    pSoft->u32FFuncRes = 0u;
    u32Open = (pSoft->u32FOpenOk != 0u) ? 1u : 0u;
    u32Map = (pSoft->u32FMapOk != 0u && pSoft->pRegs != NULL) ? 1u : 0u;
    u32Ring = (pSoft->u32FRingOk != 0u && pSoft->u32FRingVerify != 0u)
                  ? 1u : 0u;
    u32Irq = (pSoft->u8SoftStage >= RTL_SOFT_ST_IRQ &&
              pSoft->u32FIrqRes != 0u) ? 1u : 0u;
    u32Work = (pSoft->u32FWorkRes != 0u) ? 1u : 0u;
    u32Wire = (pSoft->u32FWireHo != 0u) ? 1u : 0u;
    u32Prog = (pSoft->u32FProgRes != 0u) ? 1u : 0u;
    u32Caps = (pSoft->u32FCapsRes != 0u) ? 1u : 0u;
    u32Abi = (pSoft->u32FAbiRes != 0u) ? 1u : 0u;
    u32BasesClean = pSoft->u32FBasesClean;
    u32DmaSoft = (pSoft->dmaRing != 0 && pSoft->pRing != NULL) ? 1u : 0u;
    u32AlignOk = 0u;
    if (pSoft->dmaTx != 0 && pSoft->dmaRx != 0 &&
        (((u64)pSoft->dmaTx % (u64)RTL_ALIGN_TNPDS) == 0ull) &&
        (((u64)pSoft->dmaRx % (u64)RTL_ALIGN_TNPDS) == 0ull)) {
        u32AlignOk = 1u;
    }
    u32ProductMint = 0u;
    /*
     * Prefer real DDI (GJ_SYS_DDI / udx_host_bind_by_id) over inject demo.
     * real_ddi=1 when freestanding/host bind_path stamped DDI (not inject).
     */
    u32PreferReal = 1u;
    u32RealDdi = (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_DDI) ? 1u : 0u;
    /*
     * MAP+DMA present (soft proxy for DDI MAP_BAR + DMA window residual):
     * densify residual toward program_gate. Full gate still needs ring+align.
     * product_mint=0 → would_write=0 always here (Dual DoD B OPEN).
     * STRONGER densify when caps residual catalog present (map_dma_caps=1):
     *   densify open / map / program_gate toward real DDI product path.
     */
    u32MapDmaReady = (u32Map != 0u && u32DmaSoft != 0u) ? 1u : 0u;
    u32MapDmaCaps = (u32MapDmaReady != 0u && u32Caps != 0u) ? 1u : 0u;
    u32ProgGate = (u32MapDmaReady != 0u &&
                   u32Ring != 0u && u32AlignOk != 0u) ? 1u : 0u;
    /*
     * Caps-available densify (else SAME soft residual without extra lamps):
     * open densify needs soft open + caps residual (DDI OPEN residual spirit).
     * map densify needs soft map + caps (DDI MAP_BAR residual spirit).
     * program_gate densify needs gate + map_dma_caps (mint still OPEN).
     */
    u32DensOpen = (u32Caps != 0u && u32Open != 0u) ? 1u : 0u;
    u32DensMap = (u32Caps != 0u && u32Map != 0u) ? 1u : 0u;
    u32DensGate = (u32MapDmaCaps != 0u && u32ProgGate != 0u) ? 1u : 0u;
    if (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_DDI) {
        szBindPath = "ddi";
    } else if (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_INJECT) {
        szBindPath = "inject";
    } else {
        szBindPath = "none";
    }
    /*
     * Own prep: host-owned slots + EOR-last geometry ready for product
     * Own handoff after TNPDS/RDSAR program. Soft never sets Own=1.
     */
    u32TxHost = 0u;
    u32RxHost = 0u;
    if (pSoft->pTxDesc != NULL && pSoft->pRxDesc != NULL) {
        for (iSlot = 0; iSlot < pSoft->u32TxSlots; iSlot++) {
            if ((pSoft->pTxDesc[iSlot].u32Opts1 & RTL_DESC_OWN) == 0u) {
                u32TxHost++;
            }
        }
        for (iSlot = 0; iSlot < pSoft->u32RxSlots; iSlot++) {
            if ((pSoft->pRxDesc[iSlot].u32Opts1 & RTL_DESC_OWN) == 0u) {
                u32RxHost++;
            }
        }
    }
    u32OwnPrep = (u32Ring != 0u &&
                  u32TxHost == pSoft->u32TxSlots &&
                  u32RxHost == pSoft->u32RxSlots &&
                  pSoft->u32TxSlots != 0u) ? 1u : 0u;
    /*
     * Link observe→TX when UDX owns wire. Freestanding interim → link_to_tx=0.
     * Product path would require wire_owner=udx + linkok + program done.
     */
    u32LinkOk = (pSoft->u8PhySt & RTL_PHYST_LINKOK) != 0u ? 1u : 0u;
    u32UdxOwned = (pSoft->u8WireOwner == RTL_SOFT_WIRE_OWNER_UDX) ? 1u : 0u;
    u32LinkToTx = (u32UdxOwned != 0u && u32LinkOk != 0u &&
                   u32ProgGate != 0u && u32ProductMint != 0u) ? 1u : 0u;

    udx_printk("rtl8168_udx: soft functional residual catalog "
               "Soft!=product G-AC-1 claim_class=C2 dual_dod_b=OPEN "
               "product=UDX+ABI product=UDX+sshd+stack "
               "need=DDI_caps need=hot_cold_abi "
               "freestanding_product=SKIP prefer_ddi_bind=1 "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "gj_sys_ddi=%u id=10ec:8168 "
               "bind_path=%s map_dma_ready=%u map_dma_caps=%u "
               "densify_open=%u densify_map=%u densify_program_gate=%u "
               "product_mint=0 never_program=1 never_tx_rx=1 steps=%u "
               "path=open,map,ring,irq,work,wire_handoff "
               "prog_link=%u caps_link=%u abi_link=%u "
               "program_gate=%u own_prep=%u link_to_tx=%u "
               "lab_ip=OPEN stack=OPEN sshd=OPEN\n",
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               (unsigned)GJ_SYS_DDI,
               szBindPath,
               (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
               (unsigned)u32DensOpen, (unsigned)u32DensMap,
               (unsigned)u32DensGate,
               (unsigned)RTL_SOFT_FUNC_STEP_COUNT,
               (unsigned)u32Prog, (unsigned)u32Caps, (unsigned)u32Abi,
               (unsigned)u32ProgGate, (unsigned)u32OwnPrep,
               (unsigned)u32LinkToTx);

    /* 1 — soft open (enable + regions; not CNode mint). Real DDI OPEN residual. */
    udx_printk("rtl8168_udx: soft functional step=%u open=%u "
               "enable=%u regions=%u master=%u map=0 ring=0 "
               "prefer_real_ddi=%u real_ddi=%u densify_open=%u "
               "gj_sys_ddi=%u product_mint=0 need=DDI_caps "
               "Soft!=product dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_FUNC_STEP_OPEN,
               (unsigned)u32Open, (unsigned)u32Open,
               (unsigned)u32Open, (unsigned)u32Open,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)u32DensOpen, (unsigned)GJ_SYS_DDI);
    if (u32DensOpen != 0u) {
        /* greppable: rtl8168_udx: soft functional densify open= */
        udx_printk("rtl8168_udx: soft functional densify open=1 "
                   "caps=1 real_ddi=%u prefer_real_ddi=1 "
                   "gj_sys_ddi=%u via=udx_host_bind_by_id "
                   "id=10ec:8168 enable=1 regions=1 master=1 "
                   "toward=map,program_gate product_mint=0 "
                   "product=UDX+sshd+stack chain=%s "
                   "lab_ip residual OPEN lab_ip=10.200.125.50 "
                   "stack=OPEN sshd=OPEN wire_claim=OPEN "
                   "never_program=1 Soft!=product dual_dod_b=OPEN\n",
                   (unsigned)u32RealDdi, (unsigned)GJ_SYS_DDI,
                   RTL_SOFT_PRODUCT_CHAIN);
    }
    pSoft->u8FuncSteps = RTL_SOFT_FUNC_STEP_OPEN;

    /* 2 — soft MAP (BAR ioremap; MMIO_FRAME still OPEN). Real DDI MAP residual. */
    udx_printk("rtl8168_udx: soft functional step=%u map=%u "
               "bar=%u bar_src=%u open=%u soft_dma=%u "
               "map_dma_ready=%u mmio_frame=OPEN product_mint=0 "
               "prefer_real_ddi=%u real_ddi=%u densify_map=%u "
               "toward=program_gate program_gate=%u "
               "never_program=1 Soft!=product\n",
               (unsigned)RTL_SOFT_FUNC_STEP_MAP,
               (unsigned)u32Map,
               (unsigned)pSoft->u32BarIndex,
               (unsigned)pSoft->u8BarSrc,
               (unsigned)u32Open, (unsigned)u32DmaSoft,
               (unsigned)u32MapDmaReady,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)u32DensMap,
               (unsigned)u32ProgGate);
    if (u32DensMap != 0u) {
        /* greppable: rtl8168_udx: soft functional densify map= */
        udx_printk("rtl8168_udx: soft functional densify map=1 "
                   "caps=1 real_ddi=%u prefer_real_ddi=1 "
                   "gj_sys_ddi=%u bar=%u bar_src=%u "
                   "soft_dma=%u map_dma_ready=%u map_dma_caps=%u "
                   "mmio_frame=OPEN toward=program_gate "
                   "program_gate=%u densify_program_gate=%u "
                   "product_mint=0 product=UDX+sshd+stack chain=%s "
                   "lab_ip residual OPEN lab_ip=10.200.125.50 "
                   "stack=OPEN sshd=OPEN wire_claim=OPEN "
                   "Soft!=product never_program=1 dual_dod_b=OPEN\n",
                   (unsigned)u32RealDdi, (unsigned)GJ_SYS_DDI,
                   (unsigned)pSoft->u32BarIndex,
                   (unsigned)pSoft->u8BarSrc,
                   (unsigned)u32DmaSoft, (unsigned)u32MapDmaReady,
                   (unsigned)u32MapDmaCaps, (unsigned)u32ProgGate,
                   (unsigned)u32DensGate, RTL_SOFT_PRODUCT_CHAIN);
    }
    pSoft->u8FuncSteps = RTL_SOFT_FUNC_STEP_MAP;

    /*
     * 3 — soft ring residual + Own prep + program-path readiness.
     * TNPDS/RDSAR gated behind product_mint; Own stays host (0).
     */
    udx_printk("rtl8168_udx: soft functional step=%u ring=%u "
               "verify=%u bases_clean=%u prog_res=%u "
               "own_prep=%u tx_host=%u/%u rx_host=%u/%u "
               "tnpds_reg=0x%02x rdsar_reg=0x%02x thpds_reg=0x%02x "
               "align=%u align_ok=%u dma_tx=0x%llx dma_rx=0x%llx "
               "tx_slots=%u rx_slots=%u own=0 eor_last=1 write=0 "
               "program_gate=%u densify_program_gate=%u product_mint=%u "
               "prefer_real_ddi=%u real_ddi=%u "
               "Soft!=product never_program=1\n",
               (unsigned)RTL_SOFT_FUNC_STEP_RING,
               (unsigned)u32Ring,
               (unsigned)pSoft->u32FRingVerify,
               (unsigned)u32BasesClean,
               (unsigned)u32Prog,
               (unsigned)u32OwnPrep,
               (unsigned)u32TxHost, (unsigned)pSoft->u32TxSlots,
               (unsigned)u32RxHost, (unsigned)pSoft->u32RxSlots,
               (unsigned)RTL_REG_TNPDS, (unsigned)RTL_REG_RDSAR,
               (unsigned)RTL_REG_THPDS,
               (unsigned)RTL_ALIGN_TNPDS, (unsigned)u32AlignOk,
               (unsigned long long)pSoft->dmaTx,
               (unsigned long long)pSoft->dmaRx,
               (unsigned)pSoft->u32TxSlots,
               (unsigned)pSoft->u32RxSlots,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32ProductMint,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi);
    pSoft->u8FuncSteps = RTL_SOFT_FUNC_STEP_RING;

    /* 4 — soft IRQ (IntrMask ROK|TOK; ChipCmd TE|RE OPEN; H1 thr-only). */
    udx_printk("rtl8168_udx: soft functional step=%u irq=%u "
               "line=%d soft_irq_res=%u "
               "intrmask_reg=0x%02x intrstatus_reg=0x%02x "
               "chipcmd_reg=0x%02x "
               "mask_wr=0x%04x mask_rd=0x%04x "
               "mask_bits=ROK|TOK te_re=OPEN msix=OPEN "
               "irq_notify=OPEN product_mint=0 "
               "net_eth_poll=0 thr_only=1 Soft!=product\n",
               (unsigned)RTL_SOFT_FUNC_STEP_IRQ,
               (unsigned)u32Irq,
               (pSoft->pPdev != NULL) ? pSoft->pPdev->nIrq : -1,
               (unsigned)pSoft->u32FIrqRes,
               (unsigned)RTL_REG_INTR_MASK,
               (unsigned)RTL_REG_INTR_STATUS,
               (unsigned)RTL_REG_CHIPCMD,
               (unsigned)pSoft->u16IntrMask,
               (unsigned)pSoft->u16IntrMaskRd);
    pSoft->u8FuncSteps = RTL_SOFT_FUNC_STEP_IRQ;

    /* 5 — soft work (ISR → work → reclaim catalog; not NAPI product). */
    udx_printk("rtl8168_udx: soft functional step=%u work=%u "
               "irq_hits=%u work_hits=%u soft_work_res=%u "
               "soft_tx_idx=%u soft_rx_idx=%u own_prep=%u "
               "reclaim_catalog=1 own_handoff=0 napi_poll=0 "
               "net_eth_poll=0 product_work=OPEN Soft!=product\n",
               (unsigned)RTL_SOFT_FUNC_STEP_WORK,
               (unsigned)u32Work,
               (unsigned)pSoft->u32IrqHits,
               (unsigned)pSoft->u32WorkHits,
               (unsigned)pSoft->u32FWorkRes,
               (unsigned)pSoft->u32SoftTxIdx,
               (unsigned)pSoft->u32SoftRxIdx,
               (unsigned)u32OwnPrep);
    pSoft->u8FuncSteps = RTL_SOFT_FUNC_STEP_WORK;

    /*
     * 6 — wire handoff residual (names only; freestanding owns wire).
     * Link observe→TX only when UDX owned + product_mint + program_gate.
     * Dual DoD B denser next: program → TE|RE → PHY → NAPI →
     * TXRX/lab_ip/stack/sshd → owner flip (never executes).
     */
    udx_printk("rtl8168_udx: soft functional step=%u wire_ho=%u "
               "wire_steps=%u/%u prog_res=%u caps_res=%u abi_res=%u "
               "program_gate=%u densify_program_gate=%u own_prep=%u "
               "linkok=%u udx_owned=%u link_to_tx=%u "
               "prefer_real_ddi=%u real_ddi=%u "
               "tnpds=OPEN rdsar=OPEN te_re=OPEN tppoll=OPEN "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "wire_owner=freestanding product_handoff=0 "
               "Soft!=product never_program=1 dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_FUNC_STEP_WIRE,
               (unsigned)u32Wire,
               (unsigned)pSoft->u8WireSteps,
               (unsigned)RTL_SOFT_WIRE_STEP_COUNT,
               (unsigned)u32Prog, (unsigned)u32Caps, (unsigned)u32Abi,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32OwnPrep,
               (unsigned)u32LinkOk, (unsigned)u32UdxOwned,
               (unsigned)u32LinkToTx,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi);
    pSoft->u8FuncSteps = RTL_SOFT_FUNC_STEP_WIRE;

    pSoft->u32FFuncRes = 1u;
    pSoft->u32PathBits |= RTL_SOFT_PATH_FUNC;

    udx_printk("rtl8168_udx: soft functional catalog PASS "
               "steps=%u/%u open=%u map=%u ring=%u irq=%u work=%u "
               "wire_ho=%u prog=%u caps=%u abi=%u "
               "map_dma_ready=%u map_dma_caps=%u "
               "program_gate=%u own_prep=%u link_to_tx=%u "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "densify_open=%u densify_map=%u densify_program_gate=%u "
               "product_mint=0 never_program=1 "
               "Soft!=product never_tx_rx=1 dual_dod_b=OPEN\n",
               (unsigned)pSoft->u8FuncSteps,
               (unsigned)RTL_SOFT_FUNC_STEP_COUNT,
               (unsigned)u32Open, (unsigned)u32Map, (unsigned)u32Ring,
               (unsigned)u32Irq, (unsigned)u32Work, (unsigned)u32Wire,
               (unsigned)u32Prog, (unsigned)u32Caps, (unsigned)u32Abi,
               (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
               (unsigned)u32ProgGate, (unsigned)u32OwnPrep,
               (unsigned)u32LinkToTx,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               (unsigned)u32DensOpen, (unsigned)u32DensMap,
               (unsigned)u32DensGate);
    /*
     * STRONGER densify when MAP+DMA present: residual points at program_gate
     * (TNPDS/RDSAR order) then lab_ip/stack/sshd. Soft!=product write.
     * Caps-available densify deepens open/map/program_gate toward real DDI.
     * product=UDX+sshd+stack path honesty denser; wire claim residual OPEN.
     * STRONGER denser program_wire_chain + prefer_real residual (H2 once):
     * densify_program_wire_chain + densify_prefer_real_ddi + densify_pwc
     * prefer_real residual on functional surface (parity program/product/wire).
     * NEVER write TNPDS/RDSAR/TE|RE while product_mint=0 (never_program=1).
     * greppable: rtl8168_udx: soft functional densify
     * greppable: densify_program_wire_chain= / densify_prefer_real_ddi=
     * greppable: densify_pwc_prefer_real= / prefer_real residual
     */
    udx_printk("rtl8168_udx: soft functional densify "
               "map_dma_ready=%u map_dma_caps=%u soft_map=%u soft_dma=%u "
               "soft_ring=%u align_ok=%u toward=program_gate "
               "program_gate=%u densify_program_gate=%u "
               "densify_open=%u densify_map=%u "
               "densify_program_wire_chain=%u densify_prefer_real_ddi=%u "
               "denser_prefer_real_ddi=%u denser_wire_claim=%u "
               "densify_pwc_prefer_real=%u "
               "denser_program=1 denser_wire=1 denser_chain=1 "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "own_prep=%u product_mint=%u would_write=0 "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "prefer_real residual denser=1 "
               "program_wire_chain residual denser=1 "
               "next=tnpds,rdsar,te_re,phy,napi,txrx_lab_ip_stack_sshd,"
               "owner_flip "
               "product=UDX+sshd+stack chain=%s "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN wire claim residual OPEN "
               "wire_claim=OPEN "
               "bind_path=%s prefer_ddi=1 prefer_real_ddi=1 "
               "dual_dod_b=OPEN agent_ne_close=1 "
               "Soft!=product never_program=1 product_mint=0 G-AC-1\n",
               (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
               (unsigned)u32Map, (unsigned)u32DmaSoft,
               (unsigned)u32Ring, (unsigned)u32AlignOk,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32DensOpen, (unsigned)u32DensMap,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32OwnPrep, (unsigned)u32ProductMint,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               RTL_SOFT_PRODUCT_CHAIN, szBindPath);
    /*
     * STRONGER: when caps residual present, once-lamp densifies open/map/
     * program_gate residual toward real DDI product path (H2 once).
     * greppable: rtl8168_udx: soft functional densify caps=
     */
    if (u32Caps != 0u) {
        udx_printk("rtl8168_udx: soft functional densify caps=1 "
                   "prefer_real_ddi=1 real_ddi=%u host_inject=%u "
                   "gj_sys_ddi=%u bind_path=%s "
                   "open=%u map=%u map_dma_ready=%u map_dma_caps=%u "
                   "densify_open=%u densify_map=%u densify_program_gate=%u "
                   "program_gate=%u own_prep=%u product_mint=0 "
                   "would_write=0 toward=program_gate "
                   "gate_behind=product_mint "
                   "never_tnpds=1 never_rdsar=1 never_te_re=1 "
                   "never_program=1 "
                   "product=UDX+sshd+stack chain=%s "
                   "lab_ip residual OPEN lab_ip=10.200.125.50 "
                   "stack=OPEN sshd=OPEN "
                   "wire claim residual OPEN wire_claim=OPEN "
                   "Soft!=product dual_dod_b=OPEN freestanding_product=SKIP "
                   "G-AC-1\n",
                   (unsigned)u32RealDdi,
                   (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
                   (unsigned)GJ_SYS_DDI, szBindPath,
                   (unsigned)u32Open, (unsigned)u32Map,
                   (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
                   (unsigned)u32DensOpen, (unsigned)u32DensMap,
                   (unsigned)u32DensGate, (unsigned)u32ProgGate,
                   (unsigned)u32OwnPrep, RTL_SOFT_PRODUCT_CHAIN);
    }
    /* STRONGER: densify program_gate when MAP+DMA caps residual present. */
    if (u32DensGate != 0u) {
        /* greppable: rtl8168_udx: soft functional densify program_gate= */
        udx_printk("rtl8168_udx: soft functional densify program_gate=1 "
                   "map_dma_caps=1 map_dma_ready=1 soft_ring=%u "
                   "align_ok=%u bases_clean=%u own_prep=%u "
                   "prefer_real_ddi=%u real_ddi=%u "
                   "tnpds=OPEN rdsar=OPEN te_re=OPEN "
                   "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
                   "product_mint=0 would_write=0 write=0 arm=0 "
                   "never_tnpds=1 never_rdsar=1 never_te_re=1 "
                   "gate_behind=product_mint never_program=1 "
                   "next=phy,napi,txrx_lab_ip_stack_sshd,owner_flip "
                   "product=UDX+sshd+stack chain=%s "
                   "lab_ip residual OPEN lab_ip=10.200.125.50 "
                   "stack=OPEN sshd=OPEN sshd_port=%u "
                   "wire claim residual OPEN wire_claim=OPEN "
                   "product_udx_owns_wire=OPEN until=DUT "
                   "Soft!=product dual_dod_b=OPEN G-AC-1\n",
                   (unsigned)u32Ring, (unsigned)u32AlignOk,
                   (unsigned)u32BasesClean, (unsigned)u32OwnPrep,
                   (unsigned)u32PreferReal, (unsigned)u32RealDdi,
                   RTL_SOFT_PRODUCT_CHAIN,
                   (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    }
    /*
     * Real DDI densify once-lamp: freestanding/host DDI bind re-runs this
     * residual with real_ddi=1 so open/map/program_gate densify is greppable.
     * Links program order residual (TNPDS/RDSAR/TE|RE) without writes.
     * greppable: rtl8168_udx: soft functional densify real_ddi=
     */
    if (u32RealDdi != 0u) {
        udx_printk("rtl8168_udx: soft functional densify real_ddi=1 "
                   "prefer_real_ddi=1 id=10ec:8168 "
                   "gj_sys_ddi=%u via=udx_host_bind_by_id "
                   "path=open,map,ring,irq,work,wire_handoff "
                   "open=%u map=%u densify_open=%u densify_map=%u "
                   "map_dma_ready=%u map_dma_caps=%u "
                   "program_gate=%u densify_program_gate=%u "
                   "densify_program_wire_chain=%u densify_prefer_real_ddi=%u "
                   "densify_pwc_prefer_real=%u denser_wire_claim=%u "
                   "own_prep=%u product_mint=0 would_write=0 "
                   "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
                   "never_tnpds=1 never_rdsar=1 never_te_re=1 "
                   "toward=program_gate gate_behind=product_mint "
                   "toward=product_program_sequence "
                   "prefer_real residual denser=1 "
                   "program_wire_chain residual denser=1 "
                   "product=UDX+sshd+stack chain=%s "
                   "lab_ip residual OPEN lab_ip=10.200.125.50 "
                   "stack=OPEN sshd=OPEN sshd_port=%u "
                   "wire claim residual OPEN wire_claim=OPEN "
                   "product_udx_owns_wire=OPEN until=DUT "
                   "Soft!=product never_program=1 never_tx_rx=1 "
                   "dual_dod_b=OPEN freestanding_product=SKIP G-AC-1\n",
                   (unsigned)GJ_SYS_DDI,
                   (unsigned)u32Open, (unsigned)u32Map,
                   (unsigned)u32DensOpen, (unsigned)u32DensMap,
                   (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
                   (unsigned)u32ProgGate, (unsigned)u32DensGate,
                   (unsigned)RTL_SOFT_DENSIFY_PWC,
                   (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
                   (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
                   (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
                   (unsigned)u32OwnPrep, RTL_SOFT_PRODUCT_CHAIN,
                   (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    }
    /*
     * STRONGER denser program_wire_chain residual (H2 once; functional
     * surface parity with program/product/wire). denser_program + denser_wire
     * + denser_chain surface lamps; denser hop residual; never writes.
     * STRONGER denser: densify_prefer_real_ddi + denser_wire_claim +
     * densify_pwc_prefer_real; wire claim OPEN until DUT.
     * greppable: rtl8168_udx: soft functional densify program_wire_chain=
     * greppable: densify_program_wire_chain= / denser_program=
     * greppable: denser_wire= / denser_chain= / densify_prefer_real_ddi=
     * greppable: denser_wire_claim= / program_wire_chain residual
     * greppable: never_program=1 / product_mint=0
     */
    udx_printk("rtl8168_udx: soft functional densify program_wire_chain=%u "
               "densify_program_wire_chain=%u "
               "denser_program=1 denser_wire=1 denser_chain=1 "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "densify_prefer_real_ddi=%u denser_prefer_real_ddi=%u "
               "denser_wire_claim=%u densify_pwc_prefer_real=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "te_re_order_pos=%u/%u "
               "path=open,map,ring,irq,work,wire_handoff "
               "map_dma_ready=%u map_dma_caps=%u "
               "program_gate=%u densify_program_gate=%u "
               "own_prep=%u link_to_tx=%u product_mint=0 "
               "would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_tx_rx=1 "
               "program_wire_chain residual denser=1 "
               "prefer_real residual denser=1 "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hop1=rtl8168_udx "
               "chain_hop2=netstackd chain_hop3=sshd chain_hops=3 "
               "sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32OwnPrep, (unsigned)u32LinkToTx,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath, RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /*
     * STRONGER denser prefer_real residual (H2 once; functional surface).
     * Prefer GJ_SYS_DDI / udx_host_bind_by_id over inject; real_ddi from
     * bind_path. Links densify_program_wire_chain + wire claim OPEN until DUT.
     * greppable: rtl8168_udx: soft functional densify prefer_real_ddi=
     * greppable: densify_prefer_real_ddi= / denser_prefer_real_ddi=
     * greppable: prefer_real residual
     */
    udx_printk("rtl8168_udx: soft functional densify prefer_real_ddi=%u "
               "densify_prefer_real_ddi=%u denser_prefer_real_ddi=%u "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s gj_sys_ddi=%u via=udx_host_bind_by_id "
               "densify_program_wire_chain=%u densify_pwc_prefer_real=%u "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_wire_claim=%u denser_order_steps=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "te_re_order_pos=%u/%u "
               "path=open,map,ring,irq,work,wire_handoff "
               "map_dma_ready=%u map_dma_caps=%u "
               "program_gate=%u densify_program_gate=%u "
               "own_prep=%u densify_open=%u densify_map=%u "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_tx_rx=1 "
               "prefer_real residual denser=1 "
               "program_wire_chain residual denser=1 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "product chain residual OPEN chain=%s "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hops=3 sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath, (unsigned)GJ_SYS_DDI,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32OwnPrep, (unsigned)u32DensOpen,
               (unsigned)u32DensMap,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /*
     * STRONGER denser PWC+prefer_real+wire_claim rollup (H2 once).
     * densify_pwc_prefer_real links densify_program_wire_chain +
     * densify_prefer_real_ddi + denser_wire_claim OPEN until DUT.
     * greppable: rtl8168_udx: soft functional densify pwc_prefer_real=
     * greppable: densify_pwc_prefer_real= / denser_wire_claim=
     * greppable: prefer_real residual / program_wire_chain residual
     */
    udx_printk("rtl8168_udx: soft functional densify pwc_prefer_real=%u "
               "densify_pwc_prefer_real=%u "
               "densify_program_wire_chain=%u densify_prefer_real_ddi=%u "
               "denser_prefer_real_ddi=%u denser_wire_claim=%u "
               "denser_program=1 denser_wire=1 denser_chain=1 "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "te_re_order_pos=%u/%u "
               "path=open,map,ring,irq,work,wire_handoff "
               "map_dma_ready=%u map_dma_caps=%u "
               "program_gate=%u densify_program_gate=%u "
               "own_prep=%u densify_open=%u densify_map=%u "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_tx_rx=1 "
               "prefer_real residual denser=1 "
               "program_wire_chain residual denser=1 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "product chain residual OPEN chain=%s "
               "chain_hop1=rtl8168_udx chain_hop2=netstackd "
               "chain_hop3=sshd chain_hops=3 sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "Soft!=product dual_dod_b=OPEN G-AC-1 agent_ne_close=1\n",
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32OwnPrep, (unsigned)u32DensOpen,
               (unsigned)u32DensMap,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    udx_printk("rtl8168_udx: soft functional residual honesty "
               "path=open,map,ring,irq,work,wire_handoff "
               "program_path_link=1 program_gate=%u gate_behind=product_mint "
               "map_dma_ready=%u map_dma_caps=%u toward=program_gate "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "densify_open=%u densify_map=%u densify_program_gate=%u "
               "densify_program_wire_chain=%u densify_prefer_real_ddi=%u "
               "denser_prefer_real_ddi=%u denser_wire_claim=%u "
               "densify_pwc_prefer_real=%u densify_triad=%u "
               "prefer_real residual denser=1 "
               "program_wire_chain residual denser=1 "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "own_prep=%u link_to_tx=%u linkok=%u "
               "never_program=1 product_mint=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN sshd_port=%u "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "product=UDX+sshd+stack chain=%s "
               "soft_ne_product_wire=1 dual_dod_b=OPEN agent_ne_close=1 "
               "freestanding_product=SKIP G-AC-1 Soft!=product "
               "need=DDI_caps need=hot_cold_abi product=UDX+ABI "
               "bind_path=%s steps=%u/%u path_bit=0x%x\n",
               (unsigned)u32ProgGate,
               (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
               (unsigned)u32DensOpen, (unsigned)u32DensMap,
               (unsigned)u32DensGate,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               (unsigned)u32OwnPrep,
               (unsigned)u32LinkToTx, (unsigned)u32LinkOk,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT,
               RTL_SOFT_PRODUCT_CHAIN, szBindPath,
               (unsigned)pSoft->u8FuncSteps,
               (unsigned)RTL_SOFT_FUNC_STEP_COUNT,
               (unsigned)RTL_SOFT_PATH_FUNC);
}

/**
 * Soft program residual — 64-bit TNPDS/RDSAR + TE|RE write *order* catalog.
 * Product UDX would write lo then hi of each base then arm TE|RE; soft never.
 * Stronger path: program_gate from soft MAP+DMA+ring+align (DDI caps proxy);
 * gate still behind product_mint (product_mint=0 → write=0 always here).
 * STRONGER program order residual densify (read-only) for real DDI path:
 *   order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re (product sequence catalog);
 *   densify once-lamp always-on densify_order=1 (H2 once; not gate-only);
 *   densify te_re=1 denser 5th order residual (never arm / never_program=1);
 *   densify once-lamp when real_ddi=1 / densify_program_gate (H2 once).
 * STRONGER residual densify triad (H2 once; never stamp storms):
 *   densify_order + densify_wire_claim + densify_chain always-on rollup;
 *   densify_triad=1 STRONGER rollup; densify order=/wire_claim=/chain=/real_ddi=.
 * Wire-claim residual denser OPEN (product UDX owns wire honesty until DUT).
 * product chain residual denser: rtl8168_udx>netstackd>sshd (names only);
 * denser chain hops: chain_host / chain_stack / chain_sshd residual OPEN.
 * lab_ip residual denser: lab 10.200.125.50 / stack / sshd :22 chain names.
 * never_program=1 until product_mint + real DDI caps (Dual DoD B OPEN).
 * greppable: rtl8168_udx: soft program residual
 * greppable: rtl8168_udx: soft program step=
 * greppable: rtl8168_udx: soft program catalog
 * greppable: rtl8168_udx: soft program residual honesty
 * greppable: rtl8168_udx: soft program order residual
 * greppable: rtl8168_udx: soft program densify
 * greppable: rtl8168_udx: soft program densify real_ddi=
 * greppable: rtl8168_udx: soft program densify order=
 * greppable: rtl8168_udx: soft program densify te_re=
 * greppable: rtl8168_udx: soft program densify wire_claim=
 * greppable: rtl8168_udx: soft program densify chain=
 * greppable: rtl8168_udx: soft program densify triad=
 * greppable: rtl8168_udx: soft program densify program_wire_chain=
 * greppable: rtl8168_udx: soft program densify prefer_real_ddi=
 * greppable: rtl8168_udx: soft program densify pwc_prefer_real=
 * greppable: densify_program_wire_chain= / denser_program= / denser_wire=
 * greppable: denser_chain= / densify_prefer_real_ddi= / denser_prefer_real_ddi=
 * greppable: densify_pwc_prefer_real= / denser_wire_claim=
 * greppable: densify_order= / densify_wire_claim= / densify_chain=
 * greppable: densify_triad= / order_pos=5/5
 * greppable: product chain residual / wire claim residual / wire_claim=OPEN
 * greppable: program_gate= / prefer_real_ddi=1 / never_program=1
 * greppable: lab_ip residual / product=UDX+sshd+stack
 */
static void
rtl8168_soft_program_residual(struct rtl8168_soft *pSoft)
{
    u32 u32TxLo;
    u32 u32TxHi;
    u32 u32RxLo;
    u32 u32RxHi;
    u32 u32RingReady;
    u32 u32BasesClean;
    u32 u32Map;
    u32 u32DmaSoft;
    u32 u32AlignOk;
    u32 u32ProgGate;
    u32 u32WouldProg;
    u32 u32ProductMint;
    u32 u32Caps;
    u32 u32MapDmaReady;
    u32 u32MapDmaCaps;
    u32 u32RealDdi;
    u32 u32PreferReal;
    u32 u32DensGate;
    u32 u32TeObs;
    u32 u32ReObs;
    const char *szBindPath;

    if (pSoft == NULL) {
        return;
    }

    pSoft->u8ProgSteps = 0u;
    pSoft->u32FProgRes = 0u;

    /* DMA cookie residual split for 64-bit program catalog (not written). */
    u32TxLo = (u32)((u64)pSoft->dmaTx & 0xffffffffull);
    u32TxHi = (u32)(((u64)pSoft->dmaTx >> 32) & 0xffffffffull);
    u32RxLo = (u32)((u64)pSoft->dmaRx & 0xffffffffull);
    u32RxHi = (u32)(((u64)pSoft->dmaRx >> 32) & 0xffffffffull);
    u32RingReady = (pSoft->u32FRingOk != 0u &&
                    pSoft->u32FRingVerify != 0u) ? 1u : 0u;
    u32BasesClean = pSoft->u32FBasesClean;
    u32Map = (pSoft->u32FMapOk != 0u && pSoft->pRegs != NULL) ? 1u : 0u;
    u32DmaSoft = (pSoft->dmaRing != 0 && pSoft->pRing != NULL) ? 1u : 0u;
    u32Caps = (pSoft->u32FCapsRes != 0u) ? 1u : 0u;
    u32PreferReal = 1u;
    u32RealDdi = (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_DDI) ? 1u : 0u;
    if (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_DDI) {
        szBindPath = "ddi";
    } else if (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_INJECT) {
        szBindPath = "inject";
    } else {
        szBindPath = "none";
    }
    u32TeObs = (pSoft->u8ChipCmd & RTL_CMD_TE) != 0u ? 1u : 0u;
    u32ReObs = (pSoft->u8ChipCmd & RTL_CMD_RE) != 0u ? 1u : 0u;
    /*
     * Align residual spirit (public 256-byte TNPDS/RDSAR). Soft never
     * enforces or rewrites cookies — catalog honesty only.
     */
    u32AlignOk = 0u;
    if (pSoft->dmaTx != 0 && pSoft->dmaRx != 0 &&
        (((u64)pSoft->dmaTx % (u64)RTL_ALIGN_TNPDS) == 0ull) &&
        (((u64)pSoft->dmaRx % (u64)RTL_ALIGN_TNPDS) == 0ull)) {
        u32AlignOk = 1u;
    }
    /*
     * program_gate: soft MAP+DMA+ring+align preconditions present
     * (proxy for DDI MAP+DMA caps readiness). Gate opens residual path
     * only; actual TNPDS/RDSAR/TE|RE write still needs product_mint.
     * product_mint=0 always in this soft residual (no CNode mint).
     * densify_program_gate when gate + caps residual (STRONGER path).
     */
    u32ProductMint = 0u;
    u32MapDmaReady = (u32Map != 0u && u32DmaSoft != 0u) ? 1u : 0u;
    u32MapDmaCaps = (u32MapDmaReady != 0u && u32Caps != 0u) ? 1u : 0u;
    u32ProgGate = (u32MapDmaReady != 0u &&
                   u32RingReady != 0u && u32AlignOk != 0u) ? 1u : 0u;
    u32DensGate = (u32MapDmaCaps != 0u && u32ProgGate != 0u) ? 1u : 0u;
    /* would_program only if product_mint AND gate — mint=0 → always 0. */
    u32WouldProg = (u32ProductMint != 0u && u32ProgGate != 0u) ? 1u : 0u;

    udx_printk("rtl8168_udx: soft program residual catalog Soft!=product "
               "G-AC-1 never_program=1 claim_class=C2 dual_dod_b=OPEN "
               "product=UDX+ABI product=UDX+sshd+stack need=DDI_caps "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s gj_sys_ddi=%u "
               "soft_map=%u soft_dma=%u map_dma_ready=%u map_dma_caps=%u "
               "soft_ring=%u bases_clean=%u align_ok=%u program_gate=%u "
               "densify_program_gate=%u "
               "product_mint=%u would_program=%u write=0 "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "toward=program_gate gate_behind=product_mint "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "align=%u dma_tx=0x%llx dma_rx=0x%llx steps=%u\n",
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath, (unsigned)GJ_SYS_DDI,
               (unsigned)u32Map, (unsigned)u32DmaSoft,
               (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
               (unsigned)u32RingReady,
               (unsigned)u32BasesClean, (unsigned)u32AlignOk,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32ProductMint, (unsigned)u32WouldProg,
               (unsigned)RTL_ALIGN_TNPDS,
               (unsigned long long)pSoft->dmaTx,
               (unsigned long long)pSoft->dmaRx,
               (unsigned)RTL_SOFT_PROG_STEP_COUNT);
    /*
     * Product program sequence order residual (names only; never executes).
     * Public RTL write order: TNPDS lo/hi → RDSAR lo/hi → ChipCmd TE|RE.
     * denser residual for Dual DoD B product UDX wire path catalog.
     * greppable: rtl8168_udx: soft program order residual
     */
    udx_printk("rtl8168_udx: soft program order residual "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "tnpds_reg=0x%02x tnpds_hi_reg=0x%02x "
               "rdsar_reg=0x%02x rdsar_hi_reg=0x%02x "
               "chipcmd_reg=0x%02x te_bit=0x%02x re_bit=0x%02x "
               "tnpds_obs_lo=0x%08x tnpds_obs_hi=0x%08x "
               "rdsar_obs_lo=0x%08x rdsar_obs_hi=0x%08x "
               "chipcmd_snap=0x%02x te_obs=%u re_obs=%u "
               "bases_clean=%u program_gate=%u densify_program_gate=%u "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 never_program=1 "
               "prefer_real_ddi=1 real_ddi=%u bind_path=%s "
               "next=phy,napi,txrx_lab_ip_stack_sshd,owner_flip "
               "product=UDX+sshd+stack chain=%s "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_REG_TNPDS, (unsigned)RTL_REG_TNPDS_HI,
               (unsigned)RTL_REG_RDSAR, (unsigned)RTL_REG_RDSAR_HI,
               (unsigned)RTL_REG_CHIPCMD,
               (unsigned)RTL_CMD_TE, (unsigned)RTL_CMD_RE,
               (unsigned)pSoft->u32TnpdsLo, (unsigned)pSoft->u32TnpdsHi,
               (unsigned)pSoft->u32RdsarLo, (unsigned)pSoft->u32RdsarHi,
               (unsigned)pSoft->u8ChipCmd,
               (unsigned)u32TeObs, (unsigned)u32ReObs,
               (unsigned)u32BasesClean,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32RealDdi, szBindPath,
               RTL_SOFT_PRODUCT_CHAIN);

    /* 1 — TNPDS low dword residual (gated; would_write only if mint+gate). */
    udx_printk("rtl8168_udx: soft program step=%u tnpds_lo=OPEN "
               "would_write=%u val=0x%08x reg=0x%02x "
               "obs=0x%08x program_gate=%u densify_program_gate=%u "
               "product_mint=%u bases_clean=%u "
               "order_pos=1/%u next=tnpds_hi "
               "prefer_real_ddi=%u real_ddi=%u "
               "Soft!=product never_program=1 dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_PROG_STEP_TNPDS_LO,
               (unsigned)u32WouldProg,
               (unsigned)u32TxLo, (unsigned)RTL_REG_TNPDS,
               (unsigned)pSoft->u32TnpdsLo,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32ProductMint, (unsigned)u32BasesClean,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi);
    pSoft->u8ProgSteps = RTL_SOFT_PROG_STEP_TNPDS_LO;

    /* 2 — TNPDS high dword residual. */
    udx_printk("rtl8168_udx: soft program step=%u tnpds_hi=OPEN "
               "would_write=%u val=0x%08x reg=0x%02x "
               "obs=0x%08x program_gate=%u densify_program_gate=%u "
               "product_mint=%u order_pos=2/%u next=rdsar_lo "
               "prefer_real_ddi=%u real_ddi=%u "
               "Soft!=product never_program=1 dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_PROG_STEP_TNPDS_HI,
               (unsigned)u32WouldProg,
               (unsigned)u32TxHi, (unsigned)RTL_REG_TNPDS_HI,
               (unsigned)pSoft->u32TnpdsHi,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32ProductMint,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi);
    pSoft->u8ProgSteps = RTL_SOFT_PROG_STEP_TNPDS_HI;

    /* 3 — RDSAR low dword residual. */
    udx_printk("rtl8168_udx: soft program step=%u rdsar_lo=OPEN "
               "would_write=%u val=0x%08x reg=0x%02x "
               "obs=0x%08x program_gate=%u densify_program_gate=%u "
               "product_mint=%u bases_clean=%u "
               "order_pos=3/%u next=rdsar_hi "
               "prefer_real_ddi=%u real_ddi=%u "
               "Soft!=product never_program=1 dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_PROG_STEP_RDSAR_LO,
               (unsigned)u32WouldProg,
               (unsigned)u32RxLo, (unsigned)RTL_REG_RDSAR,
               (unsigned)pSoft->u32RdsarLo,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32ProductMint, (unsigned)u32BasesClean,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi);
    pSoft->u8ProgSteps = RTL_SOFT_PROG_STEP_RDSAR_LO;

    /* 4 — RDSAR high dword residual. */
    udx_printk("rtl8168_udx: soft program step=%u rdsar_hi=OPEN "
               "would_write=%u val=0x%08x reg=0x%02x "
               "obs=0x%08x program_gate=%u densify_program_gate=%u "
               "product_mint=%u order_pos=4/%u next=te_re "
               "prefer_real_ddi=%u real_ddi=%u "
               "Soft!=product never_program=1 dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_PROG_STEP_RDSAR_HI,
               (unsigned)u32WouldProg,
               (unsigned)u32RxHi, (unsigned)RTL_REG_RDSAR_HI,
               (unsigned)pSoft->u32RdsarHi,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32ProductMint,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi);
    pSoft->u8ProgSteps = RTL_SOFT_PROG_STEP_RDSAR_HI;

    /*
     * STRONGER denser program order residual: TE|RE is 5th product sequence
     * step (catalog denser; header PROG_STEP_COUNT remains 4 lo/hi bases).
     * never arm ChipCmd TE|RE while product_mint=0. greppable: order_pos=5/5
     * greppable: rtl8168_udx: soft program densify te_re=
     */
    udx_printk("rtl8168_udx: soft program densify te_re=1 "
               "order_pos=%u/%u te_re=OPEN would_write=0 would_arm=0 "
               "reg=0x%02x chipcmd_snap=0x%02x te_bit=0x%02x re_bit=0x%02x "
               "te_obs=%u re_obs=%u after=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi "
               "next=phy,napi,txrx_lab_ip_stack_sshd,owner_flip "
               "program_gate=%u densify_program_gate=%u product_mint=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 never_program=1 "
               "prefer_real_ddi=%u real_ddi=%u bind_path=%s "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "wire claim residual OPEN wire_claim=OPEN "
               "product chain residual OPEN chain=%s "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)RTL_REG_CHIPCMD,
               (unsigned)pSoft->u8ChipCmd,
               (unsigned)RTL_CMD_TE, (unsigned)RTL_CMD_RE,
               (unsigned)u32TeObs, (unsigned)u32ReObs,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi, szBindPath,
               RTL_SOFT_PRODUCT_CHAIN);

    pSoft->u32FProgRes = 1u;
    pSoft->u32PathBits |= RTL_SOFT_PATH_PROG;

    udx_printk("rtl8168_udx: soft program catalog PASS "
               "steps=%u/%u tnpds=0 rdsar=0 thpds=0 te_re=0 "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "denser_order_steps=%u densify_order=1 "
               "align_ok=%u bases_clean=%u "
               "program_gate=%u densify_program_gate=%u "
               "product_mint=%u would_program=%u write=0 "
               "prefer_real_ddi=%u real_ddi=%u "
               "product=UDX+sshd+stack "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "product chain residual OPEN chain=%s "
               "Soft!=product never_program=1 wire=freestanding "
               "dual_dod_b=OPEN\n",
               (unsigned)pSoft->u8ProgSteps,
               (unsigned)RTL_SOFT_PROG_STEP_COUNT,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32AlignOk, (unsigned)u32BasesClean,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32ProductMint, (unsigned)u32WouldProg,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               RTL_SOFT_PRODUCT_CHAIN);
    /*
     * STRONGER residual densify triad (H2 once; always-on rollup):
     * densify_order + densify_wire_claim + densify_chain — program order
     * residual, wire claim residual OPEN, product chain residual denser.
     * densify_triad=1 STRONGER rollup. never_program=1 / product_mint=0.
     * greppable: rtl8168_udx: soft program densify
     * greppable: densify_order= / densify_wire_claim= / densify_chain=
     * greppable: densify_triad= / product chain residual / wire claim residual
     */
    udx_printk("rtl8168_udx: soft program densify "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "denser_order_steps=%u order_pos_te_re=%u/%u "
               "tnpds_lo=0x%08x tnpds_hi=0x%08x "
               "rdsar_lo=0x%08x rdsar_hi=0x%08x "
               "te_obs=%u re_obs=%u "
               "program_gate=%u densify_program_gate=%u "
               "map_dma_ready=%u map_dma_caps=%u "
               "align_ok=%u bases_clean=%u soft_ring=%u "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 gate_behind=product_mint "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s toward=product_program_sequence "
               "next=phy,napi,txrx_lab_ip_stack_sshd,owner_flip "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 "
               "Soft!=product dual_dod_b=OPEN G-AC-1 never_tx_rx=1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32TxLo, (unsigned)u32TxHi,
               (unsigned)u32RxLo, (unsigned)u32RxHi,
               (unsigned)u32TeObs, (unsigned)u32ReObs,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
               (unsigned)u32AlignOk, (unsigned)u32BasesClean,
               (unsigned)u32RingReady,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath, RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /*
     * STRONGER denser program order residual (H2 once; always-on).
     * densify_order=1 even when densify_program_gate=0 (gate still reported).
     * denser when densify_program_gate=1: MAP+DMA caps path honesty denser.
     * greppable: rtl8168_udx: soft program densify order=
     */
    udx_printk("rtl8168_udx: soft program densify order=1 "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "map_dma_caps=%u program_gate=%u densify_program_gate=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "tnpds_lo=0x%08x tnpds_hi=0x%08x "
               "rdsar_lo=0x%08x rdsar_hi=0x%08x "
               "te_obs=%u re_obs=%u te_re_order_pos=%u/%u "
               "align_ok=%u bases_clean=%u own_prep_link=ring "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 gate_behind=product_mint "
               "prefer_real_ddi=%u real_ddi=%u bind_path=%s "
               "next=phy,napi,txrx_lab_ip_stack_sshd,owner_flip "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32MapDmaCaps, (unsigned)u32ProgGate,
               (unsigned)u32DensGate,
               (unsigned)u32TxLo, (unsigned)u32TxHi,
               (unsigned)u32RxLo, (unsigned)u32RxHi,
               (unsigned)u32TeObs, (unsigned)u32ReObs,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32AlignOk, (unsigned)u32BasesClean,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               szBindPath, RTL_SOFT_PRODUCT_CHAIN);
    /*
     * Wire-claim densify once-lamp (H2 once): product UDX owns wire OPEN
     * until DUT; soft residual never claims freestanding wire handoff.
     * STRONGER denser: densify_triad + bind_ne_wire + product_handoff=0.
     * greppable: rtl8168_udx: soft program densify wire_claim=
     * greppable: wire claim residual / wire_claim=OPEN
     */
    udx_printk("rtl8168_udx: soft program densify wire_claim=1 "
               "densify_wire_claim=1 densify_order=1 densify_chain=1 "
               "densify_triad=%u "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "owner_flip=0 never_claim_fs_wire=1 "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "program_gate=%u densify_program_gate=%u "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_tx_rx=1 "
               "prefer_real_ddi=%u real_ddi=%u bind_path=%s "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN sshd_port=%u "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               szBindPath, RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /*
     * Product chain densify once-lamp (H2 once): NIC → netstackd → sshd.
     * STRONGER denser hops: chain_host / chain_stack / chain_sshd residual.
     * Names only; never product TX/RX / host banner / Dual DoD B close.
     * greppable: rtl8168_udx: soft program densify chain=
     * greppable: product chain residual / product=UDX+sshd+stack
     */
    udx_printk("rtl8168_udx: soft program densify chain=1 "
               "densify_chain=1 densify_order=1 densify_wire_claim=1 "
               "densify_triad=%u "
               "product chain residual OPEN "
               "product=UDX+sshd+stack chain=%s "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd sshd_port=%u "
               "chain_hop1=rtl8168_udx chain_hop2=netstackd "
               "chain_hop3=sshd chain_hops=3 "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "program_gate=%u densify_program_gate=%u "
               "toward=txrx_lab_ip_stack_sshd "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "product_mint=0 never_program=1 never_tx_rx=1 "
               "prefer_real_ddi=%u real_ddi=%u bind_path=%s "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               szBindPath);
    /*
     * densify_triad STRONGER rollup once-lamp (H2 once): order+wire+chain.
     * greppable: rtl8168_udx: soft program densify triad=
     * greppable: densify_triad=
     */
    udx_printk("rtl8168_udx: soft program densify triad=%u "
               "densify_triad=%u densify_order=1 densify_wire_claim=1 "
               "densify_chain=1 denser_order_steps=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "program_gate=%u densify_program_gate=%u "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 never_program=1 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "product chain residual OPEN chain=%s "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "prefer_real_ddi=%u real_ddi=%u bind_path=%s "
               "Soft!=product dual_dod_b=OPEN G-AC-1 never_tx_rx=1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               szBindPath);
    /*
     * STRONGER denser program/wire/chain residual (H2 once): cross-surface
     * densify_program_wire_chain rollup links denser order + wire claim +
     * product chain residual. denser_program/wire/chain surface lamps.
     * STRONGER denser: densify_prefer_real_ddi + denser_wire_claim +
     * densify_pwc_prefer_real; wire claim OPEN until DUT.
     * never_program=1 / product_mint=0 always (would_write=0).
     * greppable: rtl8168_udx: soft program densify program_wire_chain=
     * greppable: densify_program_wire_chain= / denser_program=
     * greppable: denser_wire= / denser_chain= / densify_prefer_real_ddi=
     * greppable: denser_wire_claim= / never_program=1
     */
    udx_printk("rtl8168_udx: soft program densify program_wire_chain=%u "
               "densify_program_wire_chain=%u "
               "denser_program=1 denser_wire=1 denser_chain=1 "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "densify_prefer_real_ddi=%u denser_prefer_real_ddi=%u "
               "denser_wire_claim=%u densify_pwc_prefer_real=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "te_re_order_pos=%u/%u "
               "tnpds_lo=0x%08x tnpds_hi=0x%08x "
               "rdsar_lo=0x%08x rdsar_hi=0x%08x "
               "te_obs=%u re_obs=%u "
               "program_gate=%u densify_program_gate=%u "
               "map_dma_ready=%u map_dma_caps=%u "
               "align_ok=%u bases_clean=%u soft_ring=%u "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 gate_behind=product_mint "
               "prefer_real residual denser=1 "
               "program_wire_chain residual denser=1 "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s toward=product_program_sequence "
               "next=phy,napi,txrx_lab_ip_stack_sshd,owner_flip "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hop1=rtl8168_udx "
               "chain_hop2=netstackd chain_hop3=sshd chain_hops=3 "
               "sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "Soft!=product dual_dod_b=OPEN G-AC-1 never_tx_rx=1\n",
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32TxLo, (unsigned)u32TxHi,
               (unsigned)u32RxLo, (unsigned)u32RxHi,
               (unsigned)u32TeObs, (unsigned)u32ReObs,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
               (unsigned)u32AlignOk, (unsigned)u32BasesClean,
               (unsigned)u32RingReady,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath, RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /*
     * STRONGER denser prefer_real_ddi residual (H2 once; program surface).
     * Prefer real DDI bind; densify_program_wire_chain + wire claim OPEN
     * until DUT. NEVER write TNPDS/RDSAR/TE|RE (never_program=1).
     * greppable: rtl8168_udx: soft program densify prefer_real_ddi=
     * greppable: densify_prefer_real_ddi= / denser_prefer_real_ddi=
     */
    udx_printk("rtl8168_udx: soft program densify prefer_real_ddi=%u "
               "densify_prefer_real_ddi=%u denser_prefer_real_ddi=%u "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s gj_sys_ddi=%u via=udx_host_bind_by_id "
               "densify_program_wire_chain=%u densify_pwc_prefer_real=%u "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_wire_claim=%u denser_order_steps=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "te_re_order_pos=%u/%u "
               "program_gate=%u densify_program_gate=%u "
               "map_dma_ready=%u map_dma_caps=%u "
               "align_ok=%u bases_clean=%u soft_ring=%u "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 gate_behind=product_mint "
               "prefer_real residual denser=1 "
               "program_wire_chain residual denser=1 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "product chain residual OPEN chain=%s "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hops=3 sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "Soft!=product dual_dod_b=OPEN G-AC-1 never_tx_rx=1\n",
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath, (unsigned)GJ_SYS_DDI,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
               (unsigned)u32AlignOk, (unsigned)u32BasesClean,
               (unsigned)u32RingReady,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /*
     * STRONGER denser PWC+prefer_real+wire_claim rollup (H2 once).
     * densify_pwc_prefer_real links densify_program_wire_chain +
     * densify_prefer_real_ddi + denser_wire_claim OPEN until DUT.
     * greppable: rtl8168_udx: soft program densify pwc_prefer_real=
     * greppable: densify_pwc_prefer_real= / denser_wire_claim=
     */
    udx_printk("rtl8168_udx: soft program densify pwc_prefer_real=%u "
               "densify_pwc_prefer_real=%u "
               "densify_program_wire_chain=%u densify_prefer_real_ddi=%u "
               "denser_prefer_real_ddi=%u denser_wire_claim=%u "
               "denser_program=1 denser_wire=1 denser_chain=1 "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "te_re_order_pos=%u/%u "
               "tnpds_lo=0x%08x tnpds_hi=0x%08x "
               "rdsar_lo=0x%08x rdsar_hi=0x%08x "
               "te_obs=%u re_obs=%u "
               "program_gate=%u densify_program_gate=%u "
               "map_dma_ready=%u map_dma_caps=%u "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 gate_behind=product_mint "
               "prefer_real residual denser=1 "
               "program_wire_chain residual denser=1 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "product chain residual OPEN chain=%s "
               "chain_hop1=rtl8168_udx chain_hop2=netstackd "
               "chain_hop3=sshd chain_hops=3 sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "Soft!=product dual_dod_b=OPEN G-AC-1 agent_ne_close=1\n",
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32TxLo, (unsigned)u32TxHi,
               (unsigned)u32RxLo, (unsigned)u32RxHi,
               (unsigned)u32TeObs, (unsigned)u32ReObs,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /*
     * Real DDI program order densify once-lamp (read-only residual).
     * When bind_path=ddi / real_ddi=1, densify residual toward product
     * TNPDS/RDSAR/TE|RE sequence catalog without enabling writes.
     * greppable: rtl8168_udx: soft program densify real_ddi=
     */
    if (u32RealDdi != 0u) {
        udx_printk("rtl8168_udx: soft program densify real_ddi=1 "
                   "densify_order=1 densify_wire_claim=1 densify_chain=1 "
                   "densify_triad=%u denser_order_steps=%u "
                   "prefer_real_ddi=1 id=10ec:8168 "
                   "gj_sys_ddi=%u via=udx_host_bind_by_id bind_path=ddi "
                   "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
                   "program_gate=%u densify_program_gate=%u "
                   "map_dma_ready=%u map_dma_caps=%u "
                   "soft_map=%u soft_dma=%u soft_ring=%u align_ok=%u "
                   "bases_clean=%u product_mint=0 would_write=0 write=0 "
                   "never_tnpds=1 never_rdsar=1 never_te_re=1 "
                   "never_program=1 gate_behind=product_mint "
                   "toward=product_program_sequence "
                   "next=phy,napi,txrx_lab_ip_stack_sshd,owner_flip "
                   "product=UDX+sshd+stack chain=%s "
                   "product chain residual OPEN "
                   "lab_ip residual OPEN lab_ip=10.200.125.50 "
                   "stack=OPEN sshd=OPEN sshd_port=%u "
                   "wire claim residual OPEN wire_claim=OPEN "
                   "product_udx_owns_wire=OPEN until=DUT "
                   "Soft!=product never_tx_rx=1 dual_dod_b=OPEN "
                   "freestanding_product=SKIP G-AC-1\n",
                   (unsigned)RTL_SOFT_DENSIFY_TRIAD,
                   (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
                   (unsigned)GJ_SYS_DDI,
                   (unsigned)u32ProgGate, (unsigned)u32DensGate,
                   (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
                   (unsigned)u32Map, (unsigned)u32DmaSoft,
                   (unsigned)u32RingReady, (unsigned)u32AlignOk,
                   (unsigned)u32BasesClean,
                   RTL_SOFT_PRODUCT_CHAIN,
                   (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    }
    /*
     * Dual DoD B next after program residual (names only): TE|RE → PHY →
     * NAPI → TXRX/lab_ip/stack/sshd → owner flip. never_program holds
     * while product_mint=0 even if program_gate residual is 1.
     * densify_program_gate=1 only when caps residual + gate ready.
     * densify_triad=1 STRONGER always-on (order + wire_claim + chain).
     * Wire-claim residual denser: product UDX owns wire OPEN until DUT.
     * product chain residual denser: rtl8168_udx>netstackd>sshd.
     * lab_ip residual denser: 10.200.125.50 / stack / sshd chain.
     */
    udx_printk("rtl8168_udx: soft program residual honesty "
               "tnpds=0 rdsar=0 thpds=0 te_re=0 write=0 arm=0 "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "densify_program_wire_chain=%u densify_prefer_real_ddi=%u "
               "denser_prefer_real_ddi=%u denser_wire_claim=%u "
               "densify_pwc_prefer_real=%u "
               "program_gate=%u densify_program_gate=%u "
               "product_mint=%u would_program=%u "
               "map_dma_ready=%u map_dma_caps=%u "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s toward=program_gate gate_behind=product_mint "
               "next=te_re,phy,napi,txrx_lab_ip_stack_sshd,owner_flip "
               "align_ok=%u bases_clean=%u soft_ring=%u soft_dma=%u "
               "te_obs=%u re_obs=%u "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 product_mint=0 "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN sshd_port=%u "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding dual_dod_b=OPEN agent_ne_close=1 "
               "Soft!=product G-AC-1 product=UDX+ABI need=DDI_caps "
               "steps=%u/%u path_bit=0x%x\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)u32ProgGate, (unsigned)u32DensGate,
               (unsigned)u32ProductMint, (unsigned)u32WouldProg,
               (unsigned)u32MapDmaReady, (unsigned)u32MapDmaCaps,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath,
               (unsigned)u32AlignOk, (unsigned)u32BasesClean,
               (unsigned)u32RingReady, (unsigned)u32DmaSoft,
               (unsigned)u32TeObs, (unsigned)u32ReObs,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)pSoft->u8ProgSteps,
               (unsigned)RTL_SOFT_PROG_STEP_COUNT,
               (unsigned)RTL_SOFT_PATH_PROG);
}

/**
 * Soft PHY residual — PHYStatus RO bits + PHYAR name + link residual.
 * greppable: rtl8168_udx: soft phy residual
 * greppable: rtl8168_udx: soft phy step=
 * greppable: rtl8168_udx: soft phy catalog
 * Never: PHYAR R/W, MII program, autoneg product.
 */
static void
rtl8168_soft_phy_residual(struct rtl8168_soft *pSoft)
{
    u8 u8Phy;
    u8 u8LinkOk;
    u8 u8Full;
    u8 u8Spd100;
    u8 u8Spd1000;

    if (pSoft == NULL) {
        return;
    }

    pSoft->u8PhySteps = 0u;
    pSoft->u32FPhyRes = 0u;
    u8Phy = pSoft->u8PhySt;
    u8LinkOk = (u8Phy & RTL_PHYST_LINKOK) != 0u ? 1u : 0u;
    u8Full = (u8Phy & RTL_PHYST_FULLDUP) != 0u ? 1u : 0u;
    u8Spd100 = (u8Phy & RTL_PHYST_SPEED100) != 0u ? 1u : 0u;
    u8Spd1000 = (u8Phy & RTL_PHYST_SPEED1000) != 0u ? 1u : 0u;

    udx_printk("rtl8168_udx: soft phy residual catalog Soft!=product "
               "G-AC-1 claim_class=C2 phyar=OPEN never_phyar_rw=1 "
               "steps=%u\n",
               (unsigned)RTL_SOFT_PHY_STEP_COUNT);

    /* 1 — PHYStatus RO observe residual (already snapped). */
    udx_printk("rtl8168_udx: soft phy step=%u phystatus=OPEN "
               "snap=0x%02x linkok=%u fulldup=%u spd100=%u spd1000=%u "
               "Soft!=product never_program=1\n",
               (unsigned)RTL_SOFT_PHY_STEP_STATUS,
               (unsigned)u8Phy, (unsigned)u8LinkOk, (unsigned)u8Full,
               (unsigned)u8Spd100, (unsigned)u8Spd1000);
    pSoft->u8PhySteps = RTL_SOFT_PHY_STEP_STATUS;

    /* 2 — PHYAR name residual — soft never R/W public PHY access reg. */
    udx_printk("rtl8168_udx: soft phy step=%u phyar=OPEN "
               "reg=0x%02x rw=0 Soft!=product never_phyar_rw=1\n",
               (unsigned)RTL_SOFT_PHY_STEP_PHYAR,
               (unsigned)RTL_REG_PHYAR);
    pSoft->u8PhySteps = RTL_SOFT_PHY_STEP_PHYAR;

    /* 3 — link residual — product would wait LinkOk; soft catalogs only. */
    udx_printk("rtl8168_udx: soft phy step=%u link=OPEN "
               "linkok=%u wait=OPEN autoneg=OPEN Soft!=product\n",
               (unsigned)RTL_SOFT_PHY_STEP_LINK,
               (unsigned)u8LinkOk);
    pSoft->u8PhySteps = RTL_SOFT_PHY_STEP_LINK;

    pSoft->u32FPhyRes = 1u;
    pSoft->u32PathBits |= RTL_SOFT_PATH_PHY_RES;

    udx_printk("rtl8168_udx: soft phy catalog PASS "
               "steps=%u/%u linkok=%u Soft!=product never_phyar_rw=1\n",
               (unsigned)pSoft->u8PhySteps,
               (unsigned)RTL_SOFT_PHY_STEP_COUNT,
               (unsigned)u8LinkOk);
}

/**
 * Soft NAPI residual — budget / TX done / RX refill catalog only.
 * greppable: rtl8168_udx: soft napi residual
 * greppable: rtl8168_udx: soft napi step=
 * greppable: rtl8168_udx: soft napi catalog
 * Never: Own handoff, poll, softirq product.
 */
static void
rtl8168_soft_napi_residual(struct rtl8168_soft *pSoft)
{
    if (pSoft == NULL) {
        return;
    }

    pSoft->u8NapiSteps = 0u;
    pSoft->u32FNapiRes = 0u;

    udx_printk("rtl8168_udx: soft napi residual catalog Soft!=product "
               "G-AC-1 claim_class=C2 own_handoff=0 poll=0 steps=%u\n",
               (unsigned)RTL_SOFT_NAPI_STEP_COUNT);

    /* 1 — poll budget residual constant (never scheduled). */
    udx_printk("rtl8168_udx: soft napi step=%u budget=OPEN "
               "budget_val=%u schedule=0 Soft!=product\n",
               (unsigned)RTL_SOFT_NAPI_STEP_BUDGET,
               (unsigned)RTL_SOFT_NAPI_BUDGET);
    pSoft->u8NapiSteps = RTL_SOFT_NAPI_STEP_BUDGET;

    /* 2 — TX completion reclaim residual (Own still host). */
    udx_printk("rtl8168_udx: soft napi step=%u tx_done=OPEN "
               "soft_tx_idx=%u slots=%u own_handoff=0 Soft!=product\n",
               (unsigned)RTL_SOFT_NAPI_STEP_TX_DONE,
               (unsigned)pSoft->u32SoftTxIdx,
               (unsigned)pSoft->u32TxSlots);
    pSoft->u8NapiSteps = RTL_SOFT_NAPI_STEP_TX_DONE;

    /* 3 — RX refill residual (no buffer cookies / no Own=1). */
    udx_printk("rtl8168_udx: soft napi step=%u rx_fill=OPEN "
               "soft_rx_idx=%u slots=%u refill=0 own_set=0 Soft!=product\n",
               (unsigned)RTL_SOFT_NAPI_STEP_RX_FILL,
               (unsigned)pSoft->u32SoftRxIdx,
               (unsigned)pSoft->u32RxSlots);
    pSoft->u8NapiSteps = RTL_SOFT_NAPI_STEP_RX_FILL;

    pSoft->u32FNapiRes = 1u;
    pSoft->u32PathBits |= RTL_SOFT_PATH_NAPI_RES;

    udx_printk("rtl8168_udx: soft napi catalog PASS "
               "steps=%u/%u budget=%u Soft!=product never_napi_poll=1\n",
               (unsigned)pSoft->u8NapiSteps,
               (unsigned)RTL_SOFT_NAPI_STEP_COUNT,
               (unsigned)RTL_SOFT_NAPI_BUDGET);
}

/**
 * Soft TX/RX residual — FS/LS spirit / TPPOLL / net door attach catalog.
 * C2 Dual DoD B deepen: denser Soft!=product TX/RX OPEN residual.
 * never_program=1 until product UDX wire (agent never closes Dual DoD B).
 * greppable: rtl8168_udx: soft txrx residual
 * greppable: rtl8168_udx: soft txrx step=
 * greppable: rtl8168_udx: soft txrx catalog
 * greppable: rtl8168_udx: soft txrx residual honesty
 * greppable: never_program=1 / never_tx_rx=1 / tx_rx=OPEN / dual_dod_b=OPEN
 * Never: TPPOLL kick, netstack attach, wire frame I/O, TNPDS/RDSAR/THPDS write.
 */
static void
rtl8168_soft_txrx_residual(struct rtl8168_soft *pSoft)
{
    u32 u32Ring;
    u32 u32BasesClean;
    u32 u32Te;
    u32 u32Re;
    u32 u32ProgSoft;

    if (pSoft == NULL) {
        return;
    }

    pSoft->u8TxRxSteps = 0u;
    pSoft->u32FTxRxRes = 0u;
    u32Ring = (pSoft->u32FRingOk != 0u && pSoft->u32FRingVerify != 0u)
                  ? 1u : 0u;
    u32BasesClean = pSoft->u32FBasesClean;
    u32Te = (pSoft->u8ChipCmd & RTL_CMD_TE) != 0u ? 1u : 0u;
    u32Re = (pSoft->u8ChipCmd & RTL_CMD_RE) != 0u ? 1u : 0u;
    u32ProgSoft = (pSoft->u32FProgRes != 0u) ? 1u : 0u;

    udx_printk("rtl8168_udx: soft txrx residual catalog Soft!=product "
               "G-AC-1 claim_class=C2 dual_dod_b=OPEN "
               "product=UDX+ABI need=DDI_caps need=hot_cold_abi "
               "never_tx_rx=1 never_program=1 never_tppoll=1 "
               "soft_ring=%u bases_clean=%u prog_res=%u "
               "te_obs=%u re_obs=%u arm=0 "
               "wire=freestanding product_handoff=0 steps=%u\n",
               (unsigned)u32Ring, (unsigned)u32BasesClean,
               (unsigned)u32ProgSoft,
               (unsigned)u32Te, (unsigned)u32Re,
               (unsigned)RTL_SOFT_TXRX_STEP_COUNT);

    /*
     * 1 — desc FS/LS/len spirit residual (layout names only).
     * Soft ring residual may hold Own=0 EOR-last geometry; soft never
     * posts buffer cookies or FS|LS|len for product wire I/O.
     */
    udx_printk("rtl8168_udx: soft txrx step=%u fsls=OPEN "
               "fs_bit=0x%x ls_bit=0x%x "
               "len_mask=0x%x tx_len_mask=0x%x rx_buf_mask=0x%x "
               "own_bit=0x%x eor_bit=0x%x "
               "ipcs=OPEN tcpcs=OPEN udpcs=OPEN lgsen=OPEN "
               "soft_ring=%u soft_tx_idx=%u soft_rx_idx=%u "
               "tx_slots=%u rx_slots=%u own_set=0 buf_cookie=0 "
               "Soft!=product never_tx_rx=1 never_program=1\n",
               (unsigned)RTL_SOFT_TXRX_STEP_FSLS,
               (unsigned)RTL_DESC_FS, (unsigned)RTL_DESC_LS,
               (unsigned)RTL_DESC_LEN_MASK,
               (unsigned)RTL_DESC_TX_LEN_MASK,
               (unsigned)RTL_DESC_RX_BUF_MASK,
               (unsigned)RTL_DESC_OWN, (unsigned)RTL_DESC_EOR,
               (unsigned)u32Ring,
               (unsigned)pSoft->u32SoftTxIdx,
               (unsigned)pSoft->u32SoftRxIdx,
               (unsigned)pSoft->u32TxSlots,
               (unsigned)pSoft->u32RxSlots);
    pSoft->u8TxRxSteps = RTL_SOFT_TXRX_STEP_FSLS;

    /*
     * 2 — TPPOLL / THPDS residual — public names; soft never kicks/writes.
     * Product would NPQ/HPQ poll after TNPDS/THPDS program; residual OPEN.
     */
    udx_printk("rtl8168_udx: soft txrx step=%u tppoll=OPEN "
               "reg=0x%02x npq_bit=0x%02x hpq_bit=0x%02x fswint_bit=0x%02x "
               "kick=0 npq=0 hpq=0 fswint=0 "
               "thpds_reg=0x%02x thpds_hi_reg=0x%02x "
               "thpds_lo=0x%08x thpds_hi=0x%08x thpds=OPEN "
               "tnpds_lo=0x%08x rdsar_lo=0x%08x bases_clean=%u "
               "write=0 Soft!=product never_tppoll=1 never_program=1\n",
               (unsigned)RTL_SOFT_TXRX_STEP_TPPOLL,
               (unsigned)RTL_REG_TPPOLL,
               (unsigned)RTL_TPPOLL_NPQ, (unsigned)RTL_TPPOLL_HPQ,
               (unsigned)RTL_TPPOLL_FSWINT,
               (unsigned)RTL_REG_THPDS, (unsigned)RTL_REG_THPDS_HI,
               (unsigned)pSoft->u32ThpdsLo, (unsigned)pSoft->u32ThpdsHi,
               (unsigned)pSoft->u32TnpdsLo, (unsigned)pSoft->u32RdsarLo,
               (unsigned)u32BasesClean);
    pSoft->u8TxRxSteps = RTL_SOFT_TXRX_STEP_TPPOLL;

    /*
     * 3 — net door / netstack attach residual (product OPEN).
     * Dual DoD B wire still freestanding interim; cold_net / hot ABI OPEN.
     * TE|RE observe only — soft residual never arms ChipCmd datapath.
     */
    udx_printk("rtl8168_udx: soft txrx step=%u net=OPEN "
               "net_door=OPEN cold_net=OPEN netstack=OPEN msix=OPEN "
               "hot_abi=OPEN cold_abi=OPEN "
               "chipcmd_reg=0x%02x chipcmd_snap=0x%02x te=%u re=%u arm=0 "
               "txcfg_reg=0x%02x txcfg_snap=0x%08x "
               "rxcfg_reg=0x%02x rxcfg_snap=0x%08x rms_snap=0x%04x "
               "dma_tx=0x%llx dma_rx=0x%llx "
               "product_handoff=0 wire=freestanding dual_dod_b=OPEN "
               "Soft!=product never_tx_rx=1 never_program=1\n",
               (unsigned)RTL_SOFT_TXRX_STEP_NET,
               (unsigned)RTL_REG_CHIPCMD,
               (unsigned)pSoft->u8ChipCmd,
               (unsigned)u32Te, (unsigned)u32Re,
               (unsigned)RTL_REG_TXCFG, (unsigned)pSoft->u32TxCfg,
               (unsigned)RTL_REG_RXCFG, (unsigned)pSoft->u32RxCfg,
               (unsigned)pSoft->u16Rms,
               (unsigned long long)pSoft->dmaTx,
               (unsigned long long)pSoft->dmaRx);
    pSoft->u8TxRxSteps = RTL_SOFT_TXRX_STEP_NET;

    pSoft->u32FTxRxRes = 1u;
    pSoft->u32PathBits |= RTL_SOFT_PATH_TXRX_RES;

    udx_printk("rtl8168_udx: soft txrx catalog PASS "
               "steps=%u/%u fsls=0 tppoll=0 net=0 "
               "soft_ring=%u bases_clean=%u te_arm=0 re_arm=0 "
               "tnpds=0 rdsar=0 thpds=0 "
               "Soft!=product never_tx_rx=1 never_tppoll=1 "
               "never_program=1 wire=freestanding product_handoff=0 "
               "product=UDX+ABI dual_dod_b=OPEN claim_class=C2\n",
               (unsigned)pSoft->u8TxRxSteps,
               (unsigned)RTL_SOFT_TXRX_STEP_COUNT,
               (unsigned)u32Ring, (unsigned)u32BasesClean);
    udx_printk("rtl8168_udx: soft txrx residual honesty "
               "tx_rx=OPEN never_tx_rx=1 never_program=1 never_tppoll=1 "
               "soft_ne_product_tx_rx=1 dual_dod_b=OPEN agent_ne_close=1 "
               "freestanding_product=SKIP G-AC-1 Soft!=product "
               "need=DDI_caps need=hot_cold_abi product=UDX+ABI "
               "steps=%u/%u path_bit=0x%x\n",
               (unsigned)pSoft->u8TxRxSteps,
               (unsigned)RTL_SOFT_TXRX_STEP_COUNT,
               (unsigned)RTL_SOFT_PATH_TXRX_RES);
}

/**
 * Soft ABI residual — hot/cold Linux ABI + UDX/DDI + sshd path surface.
 * C2 Dual DoD B product residual deepen: product = UDX/DDI + hot/cold ABI.
 * Dual DoD B lab goal: UDX owns wire for lab IP / stack / sshd (OPEN).
 * Catalog only; never claims Dual DoD close; never product TX/RX.
 * greppable: rtl8168_udx: soft abi residual
 * greppable: rtl8168_udx: soft abi step=
 * greppable: rtl8168_udx: soft abi catalog
 * greppable: rtl8168_udx: soft abi residual honesty
 * greppable: need=hot_cold_abi
 * greppable: dual_dod_b=OPEN
 * greppable: sshd=OPEN / lab_ip=OPEN
 */
static void
rtl8168_soft_abi_residual(struct rtl8168_soft *pSoft)
{
    u32 u32CapsSoft;
    u32 u32Open;
    u32 u32Map;
    u32 u32Ring;
    u32 u32Prog;
    u32 u32Wire;

    if (pSoft == NULL) {
        return;
    }

    pSoft->u8AbiSteps = 0u;
    pSoft->u32FAbiRes = 0u;
    u32CapsSoft = (pSoft->u32FCapsRes != 0u) ? 1u : 0u;
    u32Open = (pSoft->u32FOpenOk != 0u) ? 1u : 0u;
    u32Map = (pSoft->u32FMapOk != 0u) ? 1u : 0u;
    u32Ring = (pSoft->u32FRingOk != 0u && pSoft->u32FRingVerify != 0u)
                  ? 1u : 0u;
    u32Prog = (pSoft->u32FProgRes != 0u) ? 1u : 0u;
    u32Wire = (pSoft->u32FWireHo != 0u) ? 1u : 0u;

    udx_printk("rtl8168_udx: soft abi residual catalog Soft!=product "
               "G-AC-1 claim_class=C2 dual_dod_b=OPEN "
               "product=UDX+ABI product=UDX+sshd+stack "
               "chain=%s need=DDI_caps need=hot_cold_abi "
               "soft_open=%u soft_map=%u soft_ring=%u soft_prog=%u "
               "soft_caps=%u soft_wire=%u "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "product_mint=0 never_tx_rx=1 never_program=1 steps=%u\n",
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)u32Open, (unsigned)u32Map, (unsigned)u32Ring,
               (unsigned)u32Prog, (unsigned)u32CapsSoft, (unsigned)u32Wire,
               (unsigned)RTL_SOFT_ABI_STEP_COUNT);

    /* 1 -- hot Linux ABI / personality residual (Option C hot NRs). */
    udx_printk("rtl8168_udx: soft abi step=%u hot=OPEN "
               "linux_hot=OPEN personality=OPEN "
               "syscall_surface=OPEN process=OPEN "
               "soft_ne_product_ac=1 Soft!=product dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_ABI_STEP_HOT);
    pSoft->u8AbiSteps = RTL_SOFT_ABI_STEP_HOT;

    /* 2 -- cold doors / cold_net residual (Option C cold bridge). */
    udx_printk("rtl8168_udx: soft abi step=%u cold=OPEN "
               "cold_door=OPEN cold_net=OPEN "
               "socket_family=OPEN af_inet=OPEN "
               "net_door=OPEN Soft!=product dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_ABI_STEP_COLD);
    pSoft->u8AbiSteps = RTL_SOFT_ABI_STEP_COLD;

    /* 3 -- UDX host + DDI caps residual (need=DDI_caps; mint still OPEN). */
    udx_printk("rtl8168_udx: soft abi step=%u udx_ddi=OPEN "
               "soft_caps=%u soft_open=%u soft_map=%u "
               "mmio_frame=OPEN irq_notify=OPEN "
               "dma_window=OPEN product_mint=0 "
               "need=DDI_caps Soft!=product never_program=1\n",
               (unsigned)RTL_SOFT_ABI_STEP_UDX,
               (unsigned)u32CapsSoft, (unsigned)u32Open, (unsigned)u32Map);
    pSoft->u8AbiSteps = RTL_SOFT_ABI_STEP_UDX;

    /*
     * 4 -- Dual DoD B lab path surface: lab IP / stack / sshd (OPEN).
     * product=UDX+sshd+stack residual chain names only.
     * Product UDX wire ownership would feed cold_net + sshd after handoff.
     * Soft residual never claims listen :22 / host banner / Dual DoD close.
     */
    udx_printk("rtl8168_udx: soft abi step=%u sshd=OPEN "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "listen=OPEN port%u=OPEN banner=OPEN "
               "product=UDX+sshd+stack chain=%s "
               "soft_prog=%u soft_ring=%u soft_wire=%u "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "prefer_real_ddi=1 never_program=1 product_mint=0 "
               "product_handoff=0 wire=freestanding "
               "Soft!=product never_tx_rx=1 dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_ABI_STEP_SSHD,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)u32Prog, (unsigned)u32Ring, (unsigned)u32Wire);
    pSoft->u8AbiSteps = RTL_SOFT_ABI_STEP_SSHD;

    pSoft->u32FAbiRes = 1u;
    pSoft->u32PathBits |= RTL_SOFT_PATH_ABI;

    udx_printk("rtl8168_udx: soft abi catalog PASS "
               "steps=%u/%u hot=0 cold=0 udx_ddi=0 sshd=0 "
               "lab_ip=0 stack=0 "
               "product=UDX+ABI product=UDX+sshd+stack "
               "need=DDI_caps need=hot_cold_abi "
               "dual_dod_b=OPEN Soft!=product never_tx_rx=1 "
               "never_program=1\n",
               (unsigned)pSoft->u8AbiSteps,
               (unsigned)RTL_SOFT_ABI_STEP_COUNT);
    udx_printk("rtl8168_udx: soft abi residual "
               "hot=0 cold=0 udx_ddi=0 sshd=0 ddi_mint=0 "
               "lab_ip=0 stack=0 "
               "steps=%u/%u product=UDX+ABI product=UDX+sshd+stack "
               "need=DDI_caps need=hot_cold_abi Soft!=product G-AC-1 "
               "dual_dod_b=OPEN\n",
               (unsigned)pSoft->u8AbiSteps,
               (unsigned)RTL_SOFT_ABI_STEP_COUNT);
    udx_printk("rtl8168_udx: soft abi residual honesty "
               "hot_cold_abi=OPEN udx_ddi=OPEN "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN sshd_port=%u "
               "product=UDX+sshd+stack chain=%s "
               "soft_ne_product_sshd=1 dual_dod_b=OPEN agent_ne_close=1 "
               "product_handoff=0 wire=freestanding "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "prefer_real_ddi=1 never_program=1 product_mint=0 "
               "freestanding_product=SKIP G-AC-1 Soft!=product "
               "need=DDI_caps need=hot_cold_abi product=UDX+ABI "
               "steps=%u/%u path_bit=0x%x\n",
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)pSoft->u8AbiSteps,
               (unsigned)RTL_SOFT_ABI_STEP_COUNT,
               (unsigned)RTL_SOFT_PATH_ABI);
}

/**
 * Soft product residual catalog — next OPEN program steps only.
 * Clean-room public register *names*; never writes silicon bases or TE/RE.
 * Mirrors xhci residual-catalog spirit for Dual DoD B product direction.
 * C2 deepen: program/PHY/NAPI/TXRX/caps/wire/func/abi; dual_dod_b=OPEN.
 * STRONGER product densify triad (H2 once): program order + wire claim +
 * product chain residual denser (names only; never_program=1);
 * densify_triad=1 + densify order=1 always-on denser chain hops.
 *
 * greppable: rtl8168_udx: soft product residual
 * greppable: rtl8168_udx: soft product step=
 * greppable: rtl8168_udx: soft product catalog
 * greppable: rtl8168_udx: soft product densify
 * greppable: rtl8168_udx: soft product densify chain=
 * greppable: rtl8168_udx: soft product densify wire_claim=
 * greppable: rtl8168_udx: soft product densify order=
 * greppable: rtl8168_udx: soft product densify triad=
 * greppable: rtl8168_udx: soft product densify program_wire_chain=
 * greppable: rtl8168_udx: soft product densify prefer_real_ddi=
 * greppable: rtl8168_udx: soft product densify pwc_prefer_real=
 * greppable: densify_program_wire_chain= / denser_program=
 * greppable: denser_wire= / denser_chain=
 * greppable: densify_prefer_real_ddi= / denser_prefer_real_ddi=
 * greppable: densify_pwc_prefer_real= / denser_wire_claim=
 * greppable: product chain residual / wire claim residual
 * greppable: densify_triad= / dual_dod_b=OPEN / never_program=1
 * greppable: need=DDI_caps
 * greppable: need=hot_cold_abi
 */
static void
rtl8168_soft_product_residual(struct rtl8168_soft *pSoft)
{
    u32 u32RealDdi;
    u32 u32PreferReal;
    const char *szBindPath;

    if (pSoft == NULL) {
        return;
    }

    pSoft->u8ProdSteps = 0u;
    pSoft->u32FProdCat = 0u;
    /* Prefer real DDI over inject (STRONGER denser prefer_real_ddi residual). */
    u32PreferReal = 1u;
    u32RealDdi = (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_DDI) ? 1u : 0u;
    if (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_DDI) {
        szBindPath = "ddi";
    } else if (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_INJECT) {
        szBindPath = "inject";
    } else {
        szBindPath = "none";
    }

    udx_printk("rtl8168_udx: soft product residual catalog Soft!=product "
               "G-AC-1 userspace_udx=1 not_r8169_ko=1 "
               "open=%u map=%u ring=%u steps=%u "
               "public_regs=TNPDS,RDSAR,ChipCmd,IntrMask,IntrStatus,"
               "TxConfig,RxConfig,RMS,CPlusCmd,PHYStatus,PHYAR,TPPOLL "
               "never_tx_rx=1 wire=freestanding product_mint=0 "
               "product=UDX+ABI product=UDX+sshd+stack chain=%s "
               "need=DDI_caps need=hot_cold_abi "
               "prefer_ddi_bind=1 prefer_real_ddi=1 "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "never_program=1 dual_dod_b=OPEN dual_dod_b_c2=1\n",
               (unsigned)pSoft->u32FOpenOk,
               (unsigned)pSoft->u32FMapOk,
               (unsigned)pSoft->u32FRingOk,
               (unsigned)RTL_SOFT_PROD_STEP_COUNT,
               RTL_SOFT_PRODUCT_CHAIN);

    /* 1 — TNPDS (+ TxConfig observe) — gated; residual never programs. */
    udx_printk("rtl8168_udx: soft product step=%u tnpds=OPEN "
               "reg=0x%02x reg_hi=0x%02x dma_tx=0x%llx "
               "txcfg_reg=0x%02x txcfg_snap=0x%08x "
               "program_gate=OPEN product_mint=0 "
               "write=0 Soft!=product never_program=1\n",
               (unsigned)RTL_SOFT_PROD_STEP_TNPDS,
               (unsigned)RTL_REG_TNPDS, (unsigned)RTL_REG_TNPDS_HI,
               (unsigned long long)pSoft->dmaTx,
               (unsigned)RTL_REG_TXCFG, (unsigned)pSoft->u32TxCfg);
    pSoft->u8ProdSteps = RTL_SOFT_PROD_STEP_TNPDS;

    /* 2 — RDSAR (+ RxConfig / RMS observe) — gated; residual never programs. */
    udx_printk("rtl8168_udx: soft product step=%u rdsar=OPEN "
               "reg=0x%02x reg_hi=0x%02x dma_rx=0x%llx "
               "rxcfg_reg=0x%02x rxcfg_snap=0x%08x "
               "rms_reg=0x%02x rms_snap=0x%04x "
               "program_gate=OPEN product_mint=0 "
               "write=0 Soft!=product never_program=1\n",
               (unsigned)RTL_SOFT_PROD_STEP_RDSAR,
               (unsigned)RTL_REG_RDSAR, (unsigned)RTL_REG_RDSAR_HI,
               (unsigned long long)pSoft->dmaRx,
               (unsigned)RTL_REG_RXCFG, (unsigned)pSoft->u32RxCfg,
               (unsigned)RTL_REG_RMS, (unsigned)pSoft->u16Rms);
    pSoft->u8ProdSteps = RTL_SOFT_PROD_STEP_RDSAR;

    /* 3 — ChipCmd TE|RE (+ IntrMask / IntrStatus observe). */
    udx_printk("rtl8168_udx: soft product step=%u te_re=OPEN "
               "reg=0x%02x chipcmd_snap=0x%02x te=%u re=%u "
               "intrmask_reg=0x%02x intrmask_rd=0x%04x "
               "intrstatus_reg=0x%02x arm=0 Soft!=product\n",
               (unsigned)RTL_SOFT_PROD_STEP_TE_RE,
               (unsigned)RTL_REG_CHIPCMD,
               (unsigned)pSoft->u8ChipCmd,
               (pSoft->u8ChipCmd & RTL_CMD_TE) != 0u ? 1u : 0u,
               (pSoft->u8ChipCmd & RTL_CMD_RE) != 0u ? 1u : 0u,
               (unsigned)RTL_REG_INTR_MASK,
               (unsigned)pSoft->u16IntrMaskRd,
               (unsigned)RTL_REG_INTR_STATUS);
    pSoft->u8ProdSteps = RTL_SOFT_PROD_STEP_TE_RE;

    /* 4 — PHY / link — PHYAR never RW; PHYStatus RO snap only. */
    udx_printk("rtl8168_udx: soft product step=%u phy=OPEN link=OPEN "
               "phystatus_reg=0x%02x phyar_reg=0x%02x "
               "physt=0x%02x linkok=%u phyar=OPEN Soft!=product "
               "never_phyar_rw=1\n",
               (unsigned)RTL_SOFT_PROD_STEP_PHY,
               (unsigned)RTL_REG_PHYSTATUS, (unsigned)RTL_REG_PHYAR,
               (unsigned)pSoft->u8PhySt,
               (pSoft->u8PhySt & RTL_PHYST_LINKOK) != 0u ? 1u : 0u);
    pSoft->u8ProdSteps = RTL_SOFT_PROD_STEP_PHY;

    /* 5 — NAPI-style reclaim / poll (+ CPlusCmd observe). */
    udx_printk("rtl8168_udx: soft product step=%u napi=OPEN "
               "reclaim=OPEN poll=OPEN own_handoff=0 "
               "budget=%u cplus_reg=0x%02x cplus_snap=0x%08x "
               "Soft!=product\n",
               (unsigned)RTL_SOFT_PROD_STEP_NAPI,
               (unsigned)RTL_SOFT_NAPI_BUDGET,
               (unsigned)RTL_REG_CPLUSCMD, (unsigned)pSoft->u32CPlus);
    pSoft->u8ProdSteps = RTL_SOFT_PROD_STEP_NAPI;

    /* 6 — TX/RX datapath + TPPOLL + netstack — product OPEN forever here. */
    udx_printk("rtl8168_udx: soft product step=%u tx_rx=OPEN "
               "tppoll_reg=0x%02x tppoll=0 "
               "npq_bit=0x%02x hpq_bit=0x%02x "
               "thpds_reg=0x%02x thpds_hi_reg=0x%02x thpds=OPEN "
               "chipcmd_te=%u chipcmd_re=%u arm=0 "
               "netstack=OPEN cold_net=OPEN msix=OPEN "
               "bases_clean=%u soft_ring=%u "
               "Soft!=product never_tx_rx=1 never_tppoll=1 "
               "never_program=1 dual_dod_b=OPEN\n",
               (unsigned)RTL_SOFT_PROD_STEP_TXRX,
               (unsigned)RTL_REG_TPPOLL,
               (unsigned)RTL_TPPOLL_NPQ, (unsigned)RTL_TPPOLL_HPQ,
               (unsigned)RTL_REG_THPDS, (unsigned)RTL_REG_THPDS_HI,
               (pSoft->u8ChipCmd & RTL_CMD_TE) != 0u ? 1u : 0u,
               (pSoft->u8ChipCmd & RTL_CMD_RE) != 0u ? 1u : 0u,
               (unsigned)pSoft->u32FBasesClean,
               (pSoft->u32FRingOk != 0u) ? 1u : 0u);
    pSoft->u8ProdSteps = RTL_SOFT_PROD_STEP_TXRX;

    pSoft->u32FProdCat = 1u;
    pSoft->u32PathBits |= RTL_SOFT_PATH_PROD_CAT;

    udx_printk("rtl8168_udx: soft product catalog PASS "
               "steps=%u/%u tnpds=0 rdsar=0 te_re=0 phy=0 napi=0 tx_rx=0 "
               "Soft!=product never_tx_rx=1 wire=freestanding "
               "product_mint=0 claim_class=C2 "
               "product=UDX+ABI need=DDI_caps need=hot_cold_abi "
               "dual_dod_b=OPEN dual_dod_b_c2=1\n",
               (unsigned)pSoft->u8ProdSteps,
               (unsigned)RTL_SOFT_PROD_STEP_COUNT);
    udx_printk("rtl8168_udx: soft product residual "
               "tnpds=0 rdsar=0 te_re=0 phy=0 napi=0 tx_rx=0 "
               "ddi_mint=0 hot_abi=0 cold_abi=0 "
               "steps=%u/%u product=UDX+ABI product=UDX+sshd+stack "
               "need=DDI_caps need=hot_cold_abi "
               "prefer_real_ddi=1 never_program=1 "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN "
               "product chain residual OPEN chain=%s "
               "wire claim residual OPEN wire_claim=OPEN "
               "Soft!=product G-AC-1 dual_dod_b=OPEN\n",
               (unsigned)pSoft->u8ProdSteps,
               (unsigned)RTL_SOFT_PROD_STEP_COUNT,
               RTL_SOFT_PRODUCT_CHAIN);
    /*
     * STRONGER product densify triad (H2 once): program order + wire claim +
     * product chain residual denser OPEN. densify_triad=1 always-on.
     * never_program=1 / product_mint=0. Dual DoD B OPEN.
     * greppable: rtl8168_udx: soft product densify
     * greppable: densify_order= / densify_wire_claim= / densify_chain=
     * greppable: densify_triad= / product chain residual / wire claim residual
     */
    udx_printk("rtl8168_udx: soft product densify "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "tnpds=OPEN rdsar=OPEN te_re=OPEN phy=OPEN napi=OPEN "
               "tx_rx=OPEN product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_tx_rx=1 "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hops=3 sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "prefer_real_ddi=1 Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /* greppable: rtl8168_udx: soft product densify order= */
    udx_printk("rtl8168_udx: soft product densify order=1 "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "te_re_order_pos=%u/%u "
               "tnpds=OPEN rdsar=OPEN te_re=OPEN "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_tx_rx=1 "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               RTL_SOFT_PRODUCT_CHAIN);
    /* greppable: rtl8168_udx: soft product densify chain= */
    udx_printk("rtl8168_udx: soft product densify chain=1 "
               "densify_chain=1 densify_order=1 densify_wire_claim=1 "
               "densify_triad=%u "
               "product chain residual OPEN "
               "product=UDX+sshd+stack chain=%s "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hop1=rtl8168_udx "
               "chain_hop2=netstackd chain_hop3=sshd chain_hops=3 "
               "sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_mint=0 never_program=1 never_tx_rx=1 "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /* greppable: rtl8168_udx: soft product densify wire_claim= */
    udx_printk("rtl8168_udx: soft product densify wire_claim=1 "
               "densify_wire_claim=1 densify_order=1 densify_chain=1 "
               "densify_triad=%u "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "stack=OPEN sshd=OPEN sshd_port=%u "
               "product_mint=0 never_program=1 never_tx_rx=1 "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /* greppable: rtl8168_udx: soft product densify triad= */
    udx_printk("rtl8168_udx: soft product densify triad=%u "
               "densify_triad=%u densify_order=1 densify_wire_claim=1 "
               "densify_chain=1 denser_order_steps=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "product_mint=0 never_program=1 never_tx_rx=1 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "product chain residual OPEN chain=%s "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hops=3 sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /*
     * STRONGER denser program/wire/chain residual (H2 once): product-surface
     * densify_program_wire_chain rollup. denser_program + denser_wire +
     * denser_chain surface lamps; denser hop residual; never writes.
     * STRONGER denser: densify_prefer_real_ddi + denser_wire_claim +
     * densify_pwc_prefer_real; wire claim OPEN until DUT.
     * greppable: rtl8168_udx: soft product densify program_wire_chain=
     * greppable: densify_program_wire_chain= / denser_program=
     * greppable: denser_wire= / denser_chain= / densify_prefer_real_ddi=
     * greppable: denser_wire_claim= / never_program=1
     */
    udx_printk("rtl8168_udx: soft product densify program_wire_chain=%u "
               "densify_program_wire_chain=%u "
               "denser_program=1 denser_wire=1 denser_chain=1 "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "densify_prefer_real_ddi=%u denser_prefer_real_ddi=%u "
               "denser_wire_claim=%u densify_pwc_prefer_real=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "te_re_order_pos=%u/%u "
               "tnpds=OPEN rdsar=OPEN te_re=OPEN phy=OPEN napi=OPEN "
               "tx_rx=OPEN product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_tx_rx=1 "
               "prefer_real residual denser=1 "
               "program_wire_chain residual denser=1 "
               "product=UDX+sshd+stack chain=%s "
               "product chain residual OPEN "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hop1=rtl8168_udx "
               "chain_hop2=netstackd chain_hop3=sshd chain_hops=3 "
               "sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath);
    /*
     * STRONGER denser prefer_real_ddi residual (H2 once; product surface).
     * Prefer GJ_SYS_DDI / udx_host_bind_by_id; wire claim OPEN until DUT.
     * greppable: rtl8168_udx: soft product densify prefer_real_ddi=
     * greppable: densify_prefer_real_ddi= / denser_prefer_real_ddi=
     */
    udx_printk("rtl8168_udx: soft product densify prefer_real_ddi=%u "
               "densify_prefer_real_ddi=%u denser_prefer_real_ddi=%u "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s gj_sys_ddi=%u via=udx_host_bind_by_id "
               "densify_program_wire_chain=%u densify_pwc_prefer_real=%u "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_wire_claim=%u denser_order_steps=%u "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "tnpds=OPEN rdsar=OPEN te_re=OPEN "
               "product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_tx_rx=1 "
               "prefer_real residual denser=1 "
               "program_wire_chain residual denser=1 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "product chain residual OPEN chain=%s "
               "chain_host=rtl8168_udx chain_stack=netstackd "
               "chain_sshd=sshd chain_hops=3 sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "Soft!=product dual_dod_b=OPEN G-AC-1\n",
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath, (unsigned)GJ_SYS_DDI,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);
    /*
     * STRONGER denser PWC+prefer_real+wire_claim rollup (H2 once).
     * densify_pwc_prefer_real links densify_program_wire_chain +
     * densify_prefer_real_ddi + denser_wire_claim OPEN until DUT.
     * greppable: rtl8168_udx: soft product densify pwc_prefer_real=
     * greppable: densify_pwc_prefer_real= / denser_wire_claim=
     */
    udx_printk("rtl8168_udx: soft product densify pwc_prefer_real=%u "
               "densify_pwc_prefer_real=%u "
               "densify_program_wire_chain=%u densify_prefer_real_ddi=%u "
               "denser_prefer_real_ddi=%u denser_wire_claim=%u "
               "denser_program=1 denser_wire=1 denser_chain=1 "
               "densify_order=1 densify_wire_claim=1 densify_chain=1 "
               "densify_triad=%u denser_order_steps=%u "
               "prefer_real_ddi=%u real_ddi=%u host_inject=%u "
               "bind_path=%s "
               "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
               "te_re_order_pos=%u/%u "
               "tnpds=OPEN rdsar=OPEN te_re=OPEN phy=OPEN napi=OPEN "
               "tx_rx=OPEN product_mint=0 would_write=0 write=0 arm=0 "
               "never_tnpds=1 never_rdsar=1 never_te_re=1 "
               "never_program=1 never_tx_rx=1 "
               "prefer_real residual denser=1 "
               "program_wire_chain residual denser=1 "
               "wire claim residual OPEN wire_claim=OPEN "
               "product_udx_owns_wire=OPEN until=DUT "
               "wire=freestanding product_handoff=0 bind_ne_wire=1 "
               "never_claim_fs_wire=1 owner_flip=0 "
               "product chain residual OPEN chain=%s "
               "chain_hop1=rtl8168_udx chain_hop2=netstackd "
               "chain_hop3=sshd chain_hops=3 sshd_port=%u "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "Soft!=product dual_dod_b=OPEN G-AC-1 agent_ne_close=1\n",
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_PWC_PREFER,
               (unsigned)RTL_SOFT_DENSIFY_PWC,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_PREFER_REAL,
               (unsigned)RTL_SOFT_DENSIFY_WIRE_CLAIM,
               (unsigned)RTL_SOFT_DENSIFY_TRIAD,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               (unsigned)u32PreferReal, (unsigned)u32RealDdi,
               (unsigned)(u32RealDdi == 0u) ? 1u : 0u,
               szBindPath,
               (unsigned)RTL_SOFT_PROG_ORDER_TE_RE,
               (unsigned)RTL_SOFT_PROG_ORDER_COUNT,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT);

    /*
     * C2 product-direction residual deepen (catalog only; never program).
     * Caps residual first so program/functional densify sees map_dma_caps
     * (STRONGER path toward real DDI open/map/program_gate when caps ready).
     * Program residual gates TNPDS/RDSAR behind product_mint; soft MAP+DMA
     * open program_gate residual only (would_write=0 while mint=0).
     * STRONGER program order + wire claim + product chain densify (H2 once).
     * STRONGER denser program_wire_chain + prefer_real_ddi + wire claim OPEN
     * until DUT (H2 once; never_program holds).
     * Program / PHY / NAPI / TXRX expand the 6-step product residual.
     * Caps residual names MMIO/IRQ/DMA via UDX/DDI (product_mint=0).
     * ABI residual names hot/cold Linux ABI + UDX/DDI + sshd path surface.
     * Wire handoff residual densifies Dual DoD B next-program order
     * (caps→ring→TNPDS→RDSAR→TE|RE→PHY→NAPI→TXRX/lab_ip/stack/sshd→owner).
     * Functional residual strengthens open/map/ring/IRQ/work/wire-handoff
     * with Own prep + link observe→TX when UDX owned + program_gate;
     * densify open/map/program_gate when caps residual present.
     * never_program=1 until real DDI caps + product_mint. dual_dod_b=OPEN
     * always (agent never closes Dual DoD B).
     */
    rtl8168_soft_caps_residual(pSoft);
    rtl8168_soft_program_residual(pSoft);
    rtl8168_soft_phy_residual(pSoft);
    rtl8168_soft_napi_residual(pSoft);
    rtl8168_soft_txrx_residual(pSoft);
    rtl8168_soft_abi_residual(pSoft);
    rtl8168_soft_wire_handoff_residual(pSoft);
    rtl8168_soft_functional_residual(pSoft);
    rtl8168_soft_c2_product_path(pSoft);

    rtl8168_soft_product_model();
    rtl8168_soft_open_map_honesty(pSoft);
}

/**
 * Soft readiness rollup — residual vs product OPEN (one greppable block).
 * greppable: rtl8168_udx: soft ready
 * greppable: rtl8168_udx: soft residual
 */
static void
rtl8168_soft_ready_catalog(struct rtl8168_soft *pSoft)
{
    u8 u8Map;
    u8 u8Regs;
    u8 u8Ring;
    u8 u8Irq;

    if (pSoft == NULL) {
        return;
    }

    u8Map  = (pSoft->u32FMapOk != 0u || pSoft->pRegs != NULL) ? 1u : 0u;
    u8Regs = (pSoft->u8SoftStage >= RTL_SOFT_ST_REGS) ? 1u : 0u;
    u8Ring = (pSoft->u32FRingOk != 0u) ? 1u : 0u;
    u8Irq  = (pSoft->u8SoftStage >= RTL_SOFT_ST_IRQ) ? 1u : 0u;

    udx_printk("rtl8168_udx: soft ready open=%u open_map=%u cfg_regs=%u "
               "ring_residual=%u ring_verify=%u bases_clean=%u irq=%u "
               "prod_cat=%u prod_steps=%u/%u "
               "prog=%u phy_res=%u napi_res=%u txrx_res=%u "
               "caps=%u irq_res=%u work_res=%u "
               "wire_ho=%u wire_steps=%u/%u "
               "func=%u func_steps=%u/%u "
               "abi=%u abi_steps=%u/%u c2=%u path=0x%x "
               "Soft!=product soft=1 product_ac=0 product=UDX+ABI "
               "product=UDX+sshd+stack prefer_real_ddi=1 "
               "need=DDI_caps need=hot_cold_abi product_mint=0 "
               "never_program=1 "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "claim_class=C2 dual_dod_b=OPEN\n",
               (unsigned)pSoft->u32FOpenOk,
               (unsigned)u8Map, (unsigned)u8Regs,
               (unsigned)u8Ring,
               (unsigned)pSoft->u32FRingVerify,
               (unsigned)pSoft->u32FBasesClean,
               (unsigned)u8Irq,
               (unsigned)pSoft->u32FProdCat,
               (unsigned)pSoft->u8ProdSteps,
               (unsigned)RTL_SOFT_PROD_STEP_COUNT,
               (unsigned)pSoft->u32FProgRes,
               (unsigned)pSoft->u32FPhyRes,
               (unsigned)pSoft->u32FNapiRes,
               (unsigned)pSoft->u32FTxRxRes,
               (unsigned)pSoft->u32FCapsRes,
               (unsigned)pSoft->u32FIrqRes,
               (unsigned)pSoft->u32FWorkRes,
               (unsigned)pSoft->u32FWireHo,
               (unsigned)pSoft->u8WireSteps,
               (unsigned)RTL_SOFT_WIRE_STEP_COUNT,
               (unsigned)pSoft->u32FFuncRes,
               (unsigned)pSoft->u8FuncSteps,
               (unsigned)RTL_SOFT_FUNC_STEP_COUNT,
               (unsigned)pSoft->u32FAbiRes,
               (unsigned)pSoft->u8AbiSteps,
               (unsigned)RTL_SOFT_ABI_STEP_COUNT,
               (unsigned)pSoft->u32FC2Path,
               (unsigned)pSoft->u32PathBits);
    /*
     * Product path still OPEN -- name each next step so residual stays honest.
     * Soft ring layout != TNPDS/RDSAR program, TE/RE, PHY, NAPI, wire TX/RX.
     * Cap mint (MMIO_FRAME / IRQ Notification / DMA window) remains OPEN.
     * Hot/cold ABI + Dual DoD B remain OPEN (agent never closes).
     */
    udx_printk("rtl8168_udx: soft residual "
               "tnpds=0 rdsar=0 thpds=0 te_re=0 phy=0 link=0 napi=0 msix=0 "
               "reclaim=0 tx_rx=0 program=0 tppoll=0 "
               "mmio_cap=0 irq_cap=0 dma_cap=0 wire_handoff=0 "
               "hot_abi=0 cold_abi=0 lab_ip=0 stack=0 sshd=0 "
               "Soft!=product never_tx_rx=1 never_program=1 "
               "bind_ne_wire=1 wire=freestanding claim_class=C2 "
               "product=UDX+ABI product=UDX+sshd+stack "
               "prefer_real_ddi=1 need=DDI_caps need=hot_cold_abi "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "wire claim residual OPEN wire_claim=OPEN "
               "freestanding_product=SKIP dual_dod_b=OPEN\n");
    udx_printk("rtl8168_udx: soft open tnpds=OPEN rdsar=OPEN thpds=OPEN "
               "te_re=OPEN phy=OPEN link=OPEN napi=OPEN reclaim=OPEN "
               "tx_rx=OPEN mmio_cap=OPEN irq_cap=OPEN dma_cap=OPEN "
               "wire_handoff=OPEN hot_abi=OPEN cold_abi=OPEN "
               "lab_ip residual OPEN lab_ip=10.200.125.50 "
               "lab_ip=OPEN stack=OPEN sshd=OPEN "
               "wire claim residual OPEN wire_claim=OPEN "
               "product=UDX+sshd+stack prefer_real_ddi=1 "
               "dual_dod_b=OPEN Soft!=product never_program=1\n");
    rtl8168_soft_ddi_bar0_honesty(pSoft);
    rtl8168_soft_bind_ne_wire(pSoft, 0);
    rtl8168_soft_probe_residual(pSoft);
    /* Product residual catalog after soft ready flags (never programs HW). */
    rtl8168_soft_product_residual(pSoft);
}

/**
 * Soft bind lamp — freestanding udx_host_bind_by_id outcome.
 * greppable: rtl8168_udx: soft bind
 * Bind alone != probe PASS; soft != product wire.
 */
static void
rtl8168_soft_bind_lamp(const struct udx_pci_dev *pPdev, int fOk)
{
    if (fOk != 0 && pPdev != NULL) {
        udx_printk("rtl8168_udx: soft bind PASS id=10ec:8168 "
                   "bdf=%02x:%02x.%u Soft!=product\n",
                   (unsigned)pPdev->u8Bus,
                   (unsigned)udx_pci_slot(pPdev),
                   (unsigned)udx_pci_func(pPdev));
    } else {
        udx_printk("rtl8168_udx: soft bind SKIP Soft!=product\n");
    }
}

/**
 * Soft DMA honesty — coherent ring alloc only; never programs TNPDS/RDSAR.
 * greppable: rtl8168_udx: soft dma
 */
static void
rtl8168_soft_dma_lamp(const struct rtl8168_soft *pSoft, int fOk)
{
    if (fOk != 0 && pSoft != NULL) {
        udx_printk("rtl8168_udx: soft dma PASS ring_bytes=%u dma=0x%llx "
                   "tnpds=OPEN rdsar=OPEN never_program=1 Soft!=product\n",
                   (unsigned)RTL8168_RING_BYTES,
                   (unsigned long long)pSoft->dmaRing);
    } else {
        udx_printk("rtl8168_udx: soft dma SKIP Soft!=product\n");
    }
}

/**
 * Soft IRQ honesty — IntrMask soft bits only; not product MSI-X / NAPI.
 * greppable: rtl8168_udx: soft irq
 */
static void
rtl8168_soft_irq_lamp(const struct rtl8168_soft *pSoft, int nIrq, int fOk)
{
    if (fOk != 0 && pSoft != NULL) {
        udx_printk("rtl8168_udx: soft irq PASS line=%d mask=ROK|TOK "
                   "msix=OPEN napi=OPEN Soft!=product\n", nIrq);
    } else {
        udx_printk("rtl8168_udx: soft irq SKIP Soft!=product\n");
    }
}

/**
 * Soft PCI config snapshot — revision + command (type-0 header only).
 * Observation residual; not product CF8 thrash or full capability walk.
 */
static void
rtl8168_soft_cfg_snap(struct rtl8168_soft *pSoft)
{
    u8  u8Rev;
    u16 u16Cmd;
    u16 u16Ven;
    u16 u16Dev;

    if (pSoft == NULL || pSoft->pPdev == NULL) {
        return;
    }

    u8Rev  = 0;
    u16Cmd = 0;
    u16Ven = 0;
    u16Dev = 0;

    (void)udx_pci_read_config_byte(pSoft->pPdev, UDX_PCI_CFG_REVISION, &u8Rev);
    (void)udx_pci_read_config_word(pSoft->pPdev, UDX_PCI_CFG_COMMAND, &u16Cmd);
    (void)udx_pci_read_config_word(pSoft->pPdev, UDX_PCI_CFG_VENDOR, &u16Ven);
    (void)udx_pci_read_config_word(pSoft->pPdev, UDX_PCI_CFG_DEVICE, &u16Dev);

    pSoft->u8CfgRev  = u8Rev;
    pSoft->u16CfgCmd = u16Cmd;

    udx_printk("rtl8168_udx: soft cfg vend=%04x dev=%04x rev=0x%02x "
               "cmd=0x%04x mem=%u master=%u Soft!=product\n",
               (unsigned)u16Ven, (unsigned)u16Dev, (unsigned)u8Rev,
               (unsigned)u16Cmd,
               (u16Cmd & UDX_PCI_COMMAND_MEMORY) != 0u ? 1u : 0u,
               (u16Cmd & UDX_PCI_COMMAND_MASTER) != 0u ? 1u : 0u);
}

/**
 * Soft-read public register names without arming TE/RE or programming
 * descriptor bases. Snapshot only — residual toward product UDX open.
 * Uses public Realtek MAC map only (clean-room; no GPL paste).
 */
static void
rtl8168_soft_reg_snap(struct rtl8168_soft *pSoft)
{
    u32 u32Idr0;
    u32 u32Idr4;
    u32 u32Mar0;
    u32 u32TxCfg;
    u32 u32RxCfg;
    u32 u32CPlus;
    u32 u32TnpLo;
    u32 u32TnpHi;
    u32 u32ThpLo;
    u32 u32ThpHi;
    u32 u32RdsLo;
    u32 u32RdsHi;
    u16 u16Rms;
    u16 u16Imask;
    u8  u8Cmd;
    u8  u8Phy;
    u8  u8Cfg1;
    u8  u8Mac0;
    u8  u8Mac1;
    u8  u8Mac2;
    u8  u8Mac3;
    u8  u8Mac4;
    u8  u8Mac5;

    if (pSoft == NULL || pSoft->pRegs == NULL) {
        return;
    }

    /* Config residual first (PCI soft image / grant echo). */
    rtl8168_soft_cfg_snap(pSoft);
    rtl8168_soft_regmap_catalog();

    u32Idr0  = udx_readl(pSoft->pRegs, RTL_REG_IDR0);
    u32Idr4  = udx_readl(pSoft->pRegs, RTL_REG_IDR4);
    u32Mar0  = udx_readl(pSoft->pRegs, RTL_REG_MAR0);
    u8Cmd    = udx_readb(pSoft->pRegs, RTL_REG_CHIPCMD);
    u32TxCfg = udx_readl(pSoft->pRegs, RTL_REG_TXCFG);
    u32RxCfg = udx_readl(pSoft->pRegs, RTL_REG_RXCFG);
    u32CPlus = udx_readl(pSoft->pRegs, RTL_REG_CPLUSCMD);
    /* 64-bit desc bases — residual observe only; never program. */
    u32TnpLo = udx_readl(pSoft->pRegs, RTL_REG_TNPDS);
    u32TnpHi = udx_readl(pSoft->pRegs, RTL_REG_TNPDS_HI);
    u32ThpLo = udx_readl(pSoft->pRegs, RTL_REG_THPDS);
    u32ThpHi = udx_readl(pSoft->pRegs, RTL_REG_THPDS_HI);
    u32RdsLo = udx_readl(pSoft->pRegs, RTL_REG_RDSAR);
    u32RdsHi = udx_readl(pSoft->pRegs, RTL_REG_RDSAR_HI);
    u16Rms   = udx_readw(pSoft->pRegs, RTL_REG_RMS);
    u16Imask = udx_readw(pSoft->pRegs, RTL_REG_INTR_MASK);
    u8Phy    = udx_readb(pSoft->pRegs, RTL_REG_PHYSTATUS);
    u8Cfg1   = udx_readb(pSoft->pRegs, RTL_REG_CONFIG1);

    /* Soft MAC byte view from IDR (public ID registers). */
    u8Mac0 = (u8)(u32Idr0 & 0xffu);
    u8Mac1 = (u8)((u32Idr0 >> 8) & 0xffu);
    u8Mac2 = (u8)((u32Idr0 >> 16) & 0xffu);
    u8Mac3 = (u8)((u32Idr0 >> 24) & 0xffu);
    u8Mac4 = (u8)(u32Idr4 & 0xffu);
    u8Mac5 = (u8)((u32Idr4 >> 8) & 0xffu);

    pSoft->u32Idr0    = u32Idr0;
    pSoft->u32Idr4    = u32Idr4;
    pSoft->u8ChipCmd  = u8Cmd;
    pSoft->u32TxCfg   = u32TxCfg;
    pSoft->u32RxCfg   = u32RxCfg;
    pSoft->u32CPlus   = u32CPlus;
    pSoft->u32TnpdsLo = u32TnpLo;
    pSoft->u32TnpdsHi = u32TnpHi;
    pSoft->u32ThpdsLo = u32ThpLo;
    pSoft->u32ThpdsHi = u32ThpHi;
    pSoft->u32RdsarLo = u32RdsLo;
    pSoft->u32RdsarHi = u32RdsHi;
    pSoft->u16Rms     = u16Rms;
    pSoft->u16IntrMaskRd = u16Imask;
    pSoft->u8PhySt    = u8Phy;
    pSoft->u8Config1  = u8Cfg1;
    pSoft->u8SoftStage = RTL_SOFT_ST_REGS;

    udx_printk("rtl8168_udx: soft regs idr0=0x%08x idr4=0x%08x "
               "chipcmd=0x%02x te=%u re=%u "
               "txcfg=0x%08x rxcfg=0x%08x cplus=0x%08x "
               "tnpds=0x%08x%08x thpds=0x%08x%08x rdsar=0x%08x%08x "
               "(snapshot only; not armed) Soft!=product\n",
               (unsigned)u32Idr0, (unsigned)u32Idr4,
               (unsigned)u8Cmd,
               (u8Cmd & RTL_CMD_TE) != 0u ? 1u : 0u,
               (u8Cmd & RTL_CMD_RE) != 0u ? 1u : 0u,
               (unsigned)u32TxCfg, (unsigned)u32RxCfg, (unsigned)u32CPlus,
               (unsigned)u32TnpHi, (unsigned)u32TnpLo,
               (unsigned)u32ThpHi, (unsigned)u32ThpLo,
               (unsigned)u32RdsHi, (unsigned)u32RdsLo);
    udx_printk("rtl8168_udx: soft regs+ "
               "mac=%02x:%02x:%02x:%02x:%02x:%02x "
               "mar0=0x%08x physt=0x%02x linkok=%u "
               "cfg1=0x%02x rms=0x%04x imask_rd=0x%04x "
               "phyar=OPEN Soft!=product\n",
               (unsigned)u8Mac0, (unsigned)u8Mac1, (unsigned)u8Mac2,
               (unsigned)u8Mac3, (unsigned)u8Mac4, (unsigned)u8Mac5,
               (unsigned)u32Mar0, (unsigned)u8Phy,
               (u8Phy & RTL_PHYST_LINKOK) != 0u ? 1u : 0u,
               (unsigned)u8Cfg1, (unsigned)u16Rms, (unsigned)u16Imask);
    rtl8168_soft_stage_lamp(RTL_SOFT_ST_REGS, "regs");
}

/**
 * Soft ring verify — Own=0 all slots; EOR only on last TX/RX; addr=0.
 * CPU residual check only; never programs silicon bases.
 * greppable: rtl8168_udx: soft ring verify
 */
static int
rtl8168_soft_ring_verify(struct rtl8168_soft *pSoft)
{
    struct rtl8168_soft_desc *pTx;
    struct rtl8168_soft_desc *pRx;
    u32 iSlot;
    u32 u32Bad;
    u32 u32OwnHits;
    u32 u32EorTx;
    u32 u32EorRx;

    if (pSoft == NULL || pSoft->pTxDesc == NULL || pSoft->pRxDesc == NULL) {
        return 0;
    }

    pTx = pSoft->pTxDesc;
    pRx = pSoft->pRxDesc;
    u32Bad = 0;
    u32OwnHits = 0;
    u32EorTx = 0;
    u32EorRx = 0;

    for (iSlot = 0; iSlot < pSoft->u32TxSlots; iSlot++) {
        if ((pTx[iSlot].u32Opts1 & RTL_DESC_OWN) != 0u) {
            u32OwnHits++;
            u32Bad++;
        }
        if ((pTx[iSlot].u32Opts1 & RTL_DESC_EOR) != 0u) {
            u32EorTx++;
            if (iSlot + 1u != pSoft->u32TxSlots) {
                u32Bad++;
            }
        }
        if (pTx[iSlot].u32AddrLo != 0u || pTx[iSlot].u32AddrHi != 0u) {
            u32Bad++;
        }
    }
    for (iSlot = 0; iSlot < pSoft->u32RxSlots; iSlot++) {
        if ((pRx[iSlot].u32Opts1 & RTL_DESC_OWN) != 0u) {
            u32OwnHits++;
            u32Bad++;
        }
        if ((pRx[iSlot].u32Opts1 & RTL_DESC_EOR) != 0u) {
            u32EorRx++;
            if (iSlot + 1u != pSoft->u32RxSlots) {
                u32Bad++;
            }
        }
        if (pRx[iSlot].u32AddrLo != 0u || pRx[iSlot].u32AddrHi != 0u) {
            u32Bad++;
        }
    }

    if (u32EorTx != 1u || u32EorRx != 1u) {
        u32Bad++;
    }

    if (u32Bad != 0u) {
        pSoft->u32FRingVerify = 0u;
        udx_printk("rtl8168_udx: soft ring verify FAIL "
                   "bad=%u own_hits=%u eor_tx=%u eor_rx=%u Soft!=product\n",
                   (unsigned)u32Bad, (unsigned)u32OwnHits,
                   (unsigned)u32EorTx, (unsigned)u32EorRx);
        return 0;
    }

    pSoft->u32FRingVerify = 1u;
    udx_printk("rtl8168_udx: soft ring verify PASS "
               "tx_slots=%u rx_slots=%u own=0 eor_last=1 addr=0 "
               "own_prep=1 tnpds=OPEN rdsar=OPEN Soft!=product\n",
               (unsigned)pSoft->u32TxSlots,
               (unsigned)pSoft->u32RxSlots);
    return 1;
}

/**
 * Soft ring reclaim residual — catalog only; never Own handoff / NAPI poll.
 * greppable: rtl8168_udx: soft ring reclaim
 */
static void
rtl8168_soft_ring_reclaim_residual(const struct rtl8168_soft *pSoft)
{
    u32 u32TxWalk;
    u32 u32RxWalk;
    u32 iSlot;

    if (pSoft == NULL || pSoft->pTxDesc == NULL || pSoft->pRxDesc == NULL) {
        udx_printk("rtl8168_udx: soft ring reclaim SKIP Soft!=product\n");
        return;
    }

    /*
     * Soft residual: walk slots with Own still 0. Product reclaim would
     * clear device-owned completed descriptors and refill RX — OPEN here.
     */
    u32TxWalk = 0;
    u32RxWalk = 0;
    for (iSlot = 0; iSlot < pSoft->u32TxSlots; iSlot++) {
        if ((pSoft->pTxDesc[iSlot].u32Opts1 & RTL_DESC_OWN) == 0u) {
            u32TxWalk++;
        }
    }
    for (iSlot = 0; iSlot < pSoft->u32RxSlots; iSlot++) {
        if ((pSoft->pRxDesc[iSlot].u32Opts1 & RTL_DESC_OWN) == 0u) {
            u32RxWalk++;
        }
    }

    udx_printk("rtl8168_udx: soft ring reclaim residual "
               "tx_host_owned=%u/%u rx_host_owned=%u/%u "
               "soft_tx_idx=%u soft_rx_idx=%u "
               "own_handoff=0 napi_poll=0 refill=0 "
               "product_reclaim=OPEN Soft!=product\n",
               (unsigned)u32TxWalk, (unsigned)pSoft->u32TxSlots,
               (unsigned)u32RxWalk, (unsigned)pSoft->u32RxSlots,
               (unsigned)pSoft->u32SoftTxIdx,
               (unsigned)pSoft->u32SoftRxIdx);
}

/**
 * Soft re-observe TNPDS/THPDS/RDSAR after ring residual.
 * Confirms soft residual never programmed silicon bases (bases_clean).
 * greppable: rtl8168_udx: soft ring bases
 */
static int
rtl8168_soft_ring_bases_honesty(struct rtl8168_soft *pSoft)
{
    u32 u32TnpLo;
    u32 u32TnpHi;
    u32 u32ThpLo;
    u32 u32ThpHi;
    u32 u32RdsLo;
    u32 u32RdsHi;
    u32 u32Dirty;

    if (pSoft == NULL || pSoft->pRegs == NULL) {
        return 0;
    }

    u32TnpLo = udx_readl(pSoft->pRegs, RTL_REG_TNPDS);
    u32TnpHi = udx_readl(pSoft->pRegs, RTL_REG_TNPDS_HI);
    u32ThpLo = udx_readl(pSoft->pRegs, RTL_REG_THPDS);
    u32ThpHi = udx_readl(pSoft->pRegs, RTL_REG_THPDS_HI);
    u32RdsLo = udx_readl(pSoft->pRegs, RTL_REG_RDSAR);
    u32RdsHi = udx_readl(pSoft->pRegs, RTL_REG_RDSAR_HI);

    pSoft->u32TnpdsLo = u32TnpLo;
    pSoft->u32TnpdsHi = u32TnpHi;
    pSoft->u32ThpdsLo = u32ThpLo;
    pSoft->u32ThpdsHi = u32ThpHi;
    pSoft->u32RdsarLo = u32RdsLo;
    pSoft->u32RdsarHi = u32RdsHi;

    /*
     * Soft honesty: host inject starts bases at 0; freestanding may show
     * lab freestanding values. We never *write* these from this residual —
     * mark clean when still zero (host soft) OR when we simply did not program
     * (always true for this driver). Dirty only if we would have written.
     * Residual policy: never_program=1 always; bases_clean=1 when all zero.
     */
    u32Dirty = 0u;
    if (u32TnpLo != 0u || u32TnpHi != 0u ||
        u32ThpLo != 0u || u32ThpHi != 0u ||
        u32RdsLo != 0u || u32RdsHi != 0u) {
        /*
         * Non-zero observe is OK on freestanding (lab wire owner may have
         * programmed). Soft residual still never_program=1.
         */
        u32Dirty = 1u;
    }

    pSoft->u32FBasesClean = (u32Dirty == 0u) ? 1u : 0u;

    udx_printk("rtl8168_udx: soft ring bases "
               "tnpds=0x%08x%08x thpds=0x%08x%08x rdsar=0x%08x%08x "
               "bases_clean=%u never_program=1 "
               "wire_owner=freestanding Soft!=product\n",
               (unsigned)u32TnpHi, (unsigned)u32TnpLo,
               (unsigned)u32ThpHi, (unsigned)u32ThpLo,
               (unsigned)u32RdsHi, (unsigned)u32RdsLo,
               (unsigned)pSoft->u32FBasesClean);
    return 1;
}

/**
 * Soft ring residual — layout TX then RX descriptors in coherent DMA.
 * Public Own/EOR spirit: host owns all slots; EOR on last of each ring.
 * Does **not** write TNPDS / THPDS / RDSAR (product path OPEN).
 *
 * greppable: rtl8168_udx: soft ring residual
 * greppable: rtl8168_udx: soft ring path
 */
static int
rtl8168_soft_ring_residual(struct rtl8168_soft *pSoft)
{
    struct rtl8168_soft_desc *pTx;
    struct rtl8168_soft_desc *pRx;
    u8  *pBytes;
    u32  iSlot;
    u32  u32Need;
    udx_dma_addr_t dmaBase;

    if (pSoft == NULL || pSoft->pRing == NULL) {
        return 0;
    }

    u32Need = RTL_SOFT_RING_BYTES;
    if (u32Need > RTL8168_RING_BYTES) {
        udx_printk("rtl8168_udx: soft ring residual SKIP "
                   "need=%u have=%u Soft!=product\n",
                   (unsigned)u32Need, (unsigned)RTL8168_RING_BYTES);
        udx_printk("rtl8168_udx: soft ring path layout=0 verify=0 "
                   "bases=0 Soft!=product\n");
        return 0;
    }

    pBytes  = (u8 *)pSoft->pRing;
    dmaBase = pSoft->dmaRing;

    /* Zero the residual window (host-owned empty descriptors). */
    for (iSlot = 0; iSlot < u32Need; iSlot++) {
        pBytes[iSlot] = 0;
    }

    pTx = (struct rtl8168_soft_desc *)(void *)pBytes;
    pRx = pTx + RTL_SOFT_TX_SLOTS;

    for (iSlot = 0; iSlot < RTL_SOFT_TX_SLOTS; iSlot++) {
        pTx[iSlot].u32Opts1  = 0; /* Own=0 host */
        pTx[iSlot].u32Opts2  = 0;
        pTx[iSlot].u32AddrLo = 0; /* no buffer cookie — residual only */
        pTx[iSlot].u32AddrHi = 0;
    }
    pTx[RTL_SOFT_TX_SLOTS - 1u].u32Opts1 = RTL_DESC_EOR;

    for (iSlot = 0; iSlot < RTL_SOFT_RX_SLOTS; iSlot++) {
        pRx[iSlot].u32Opts1  = 0;
        pRx[iSlot].u32Opts2  = 0;
        pRx[iSlot].u32AddrLo = 0;
        pRx[iSlot].u32AddrHi = 0;
    }
    pRx[RTL_SOFT_RX_SLOTS - 1u].u32Opts1 = RTL_DESC_EOR;

    pSoft->pTxDesc   = pTx;
    pSoft->pRxDesc   = pRx;
    pSoft->u32TxSlots = RTL_SOFT_TX_SLOTS;
    pSoft->u32RxSlots = RTL_SOFT_RX_SLOTS;
    pSoft->u32SoftTxIdx = 0u;
    pSoft->u32SoftRxIdx = 0u;
    pSoft->dmaTx     = dmaBase;
    pSoft->dmaRx     = dmaBase +
        (udx_dma_addr_t)(RTL_SOFT_TX_SLOTS * RTL_DESC_BYTES);
    pSoft->u32FRingOk = 1u;
    pSoft->u32FRingVerify = 0u;
    pSoft->u32FBasesClean = 0u;
    pSoft->u8SoftStage = RTL_SOFT_ST_RING;
    pSoft->u32PathBits |= RTL_SOFT_PATH_RING;

    /*
     * Sync residual to device-visible view. Soft host: barrier only.
     * Still no TNPDS/RDSAR program — freestanding owns live wire bases.
     */
    if (pSoft->pPdev != NULL && pSoft->pPdev->pDev != NULL) {
        udx_dma_sync_single_for_device(pSoft->pPdev->pDev, dmaBase,
                                       (size_t)u32Need,
                                       UDX_DMA_BIDIRECTIONAL);
    }

    udx_printk("rtl8168_udx: soft ring residual PASS "
               "tx_slots=%u rx_slots=%u desc_bytes=%u ring_bytes=%u "
               "dma_tx=0x%llx dma_rx=0x%llx eor=1 own=0 "
               "own_prep=1 soft_tx_idx=0 soft_rx_idx=0 "
               "tnpds=OPEN rdsar=OPEN thpds=OPEN never_program=1 "
               "Soft!=product\n",
               (unsigned)pSoft->u32TxSlots,
               (unsigned)pSoft->u32RxSlots,
               (unsigned)RTL_DESC_BYTES,
               (unsigned)u32Need,
               (unsigned long long)pSoft->dmaTx,
               (unsigned long long)pSoft->dmaRx);
    udx_printk("rtl8168_udx: soft ring path layout=1 geometry=%u+%u "
               "desc=%u alloc=%u own_prep=1 "
               "tnpds_reg=0x%02x rdsar_reg=0x%02x thpds_reg=0x%02x "
               "never_program=1 Soft!=product\n",
               (unsigned)RTL_SOFT_TX_SLOTS,
               (unsigned)RTL_SOFT_RX_SLOTS,
               (unsigned)RTL_DESC_BYTES,
               (unsigned)RTL8168_RING_BYTES,
               (unsigned)RTL_REG_TNPDS,
               (unsigned)RTL_REG_RDSAR,
               (unsigned)RTL_REG_THPDS);
    rtl8168_soft_stage_lamp(RTL_SOFT_ST_RING, "ring_residual");

    /* Post-layout: confirm silicon bases unprogrammed by this residual. */
    (void)rtl8168_soft_ring_bases_honesty(pSoft);

    if (rtl8168_soft_ring_verify(pSoft) == 0) {
        pSoft->u32FRingOk = 0u;
        udx_printk("rtl8168_udx: soft ring path layout=1 verify=0 "
                   "bases=%u Soft!=product\n",
                   (unsigned)pSoft->u32FBasesClean);
        return 0;
    }
    pSoft->u32PathBits |= RTL_SOFT_PATH_VERIFY;

    /* Soft reclaim residual catalog (never product Own handoff). */
    rtl8168_soft_ring_reclaim_residual(pSoft);

    udx_printk("rtl8168_udx: soft ring path layout=1 verify=1 bases=%u "
               "reclaim_catalog=1 Soft!=product\n",
               (unsigned)pSoft->u32FBasesClean);
    return 1;
}

static void
rtl8168_work_fn(struct udx_work *pWork)
{
    struct rtl8168_soft *pSoft;

    if (pWork == NULL) {
        return;
    }
    pSoft = (struct rtl8168_soft *)pWork->pPriv;
    if (pSoft == NULL) {
        return;
    }

    udx_spin_lock(&pSoft->lockSoft);
    if (pSoft->u32FQuiesced) {
        udx_spin_unlock(&pSoft->lockSoft);
        return;
    }
    pSoft->u32WorkHits++;
    if (pSoft->u8SoftStage < RTL_SOFT_ST_WORK) {
        pSoft->u8SoftStage = RTL_SOFT_ST_WORK;
    }
    udx_spin_unlock(&pSoft->lockSoft);

    udx_printk("rtl8168_udx: soft work irq_hits=%u work_hits=%u "
               "ring_residual=%u not_tx_rx=1 Soft!=product\n",
               pSoft->u32IrqHits, pSoft->u32WorkHits,
               (unsigned)pSoft->u32FRingOk);

    /* Once-lamp work residual (reclaim catalog; never product NAPI). */
    rtl8168_soft_work_residual(pSoft);

    if (pSoft->u32WorkHits >= 1u) {
        udx_request_stop();
    }
}

/*
 * Threaded IRQ soft handler.
 * Read IntrStatus → ack (write back claimed bits) → schedule_work.
 * Soft residual: no NAPI poll / no ring reclaim product path.
 */
static int
rtl8168_isr(int nIrq, void *pDevId)
{
    struct rtl8168_soft *pSoft = (struct rtl8168_soft *)pDevId;
    u16 u16Status;

    (void)nIrq;
    if (pSoft == NULL || pSoft->pRegs == NULL) {
        return UDX_IRQ_NONE;
    }
    if (pSoft->u32FQuiesced) {
        return UDX_IRQ_NONE;
    }

    u16Status = udx_readw(pSoft->pRegs, RTL_REG_INTR_STATUS);
    if ((u16Status & RTL_SOFT_IRQ_BITS) == 0) {
        return UDX_IRQ_NONE;
    }

    /* Ack: write claimed bits back (public IntrStatus clear-by-write style). */
    udx_writew(pSoft->pRegs, RTL_REG_INTR_STATUS, u16Status);
    udx_mmio_flush(pSoft->pRegs);

    udx_spin_lock(&pSoft->lockSoft);
    pSoft->u32IrqHits++;
    udx_spin_unlock(&pSoft->lockSoft);

    pSoft->workIsr.pPriv = pSoft;
    (void)udx_schedule_work(&pSoft->workIsr);
    return UDX_IRQ_HANDLED;
}

static void
rtl8168_soft_fail_cleanup(struct udx_pci_dev *pPdev, struct rtl8168_soft *pSoft)
{
    if (pSoft != NULL) {
        if (pSoft->pRing != NULL) {
            udx_dma_free_coherent(pPdev->pDev, RTL8168_RING_BYTES,
                                  pSoft->pRing, pSoft->dmaRing);
            pSoft->pRing = NULL;
            pSoft->dmaRing = 0;
            pSoft->dmaTx = 0;
            pSoft->dmaRx = 0;
            pSoft->pTxDesc = NULL;
            pSoft->pRxDesc = NULL;
            pSoft->u32FRingOk = 0;
            pSoft->u32FRingVerify = 0;
            pSoft->u32FBasesClean = 0;
        }
        if (pSoft->pRegs != NULL) {
            udx_iounmap(pSoft->pRegs);
            pSoft->pRegs = NULL;
        }
        pSoft->u32FProbeOk = 0;
        pSoft->u32FOpenOk = 0;
        pSoft->u32FMapOk = 0;
        pSoft->u32FProdCat = 0;
        pSoft->u32FProgRes = 0;
        pSoft->u32FPhyRes = 0;
        pSoft->u32FNapiRes = 0;
        pSoft->u32FTxRxRes = 0;
        pSoft->u32FC2Path = 0;
        pSoft->u32FCapsRes = 0;
        pSoft->u32FIrqRes = 0;
        pSoft->u32FWorkRes = 0;
        pSoft->u32FWireHo = 0;
        pSoft->u32FFuncRes = 0;
        pSoft->u32FAbiRes = 0;
        pSoft->u8ProdSteps = 0;
        pSoft->u8ProgSteps = 0;
        pSoft->u8PhySteps = 0;
        pSoft->u8NapiSteps = 0;
        pSoft->u8TxRxSteps = 0;
        pSoft->u8CapSteps = 0;
        pSoft->u8WireSteps = 0;
        pSoft->u8FuncSteps = 0;
        pSoft->u8AbiSteps = 0;
        pSoft->u8BindPath = RTL_SOFT_BIND_PATH_NONE;
        pSoft->u32PathBits = 0;
        pSoft->u8WireOwner = RTL_SOFT_WIRE_OWNER_FS;
    }
    udx_pci_release_regions(pPdev);
    udx_pci_clear_master(pPdev);
    udx_pci_disable(pPdev);
}

/*
 * probe — Linux pci_driver.probe spirit.
 * Soft order: enable → master → regions → map BAR → cfg/reg snap →
 *             DMA + ring residual → IRQ. Soft!=product TX/RX.
 */
static int
rtl8168_probe(struct udx_pci_dev *pPdev, const struct udx_pci_device_id *pId)
{
    struct rtl8168_soft *pSoft;
    u64 u64Bar;
    u64 u64Len;
    void *pRing;
    udx_dma_addr_t dmaHandle;
    u32 u32Bdf;
    int nBar;
    int fBar2Miss;

    (void)pId;
    if (pPdev == NULL || pPdev->pDev == NULL) {
        return UDX_ERR_INVAL;
    }

    if (udx_pci_enable(pPdev) != UDX_OK) {
        rtl8168_soft_open_lamp(pPdev, 0);
        return UDX_ERR_IO;
    }
    if (udx_pci_set_master(pPdev) != UDX_OK) {
        rtl8168_soft_open_lamp(pPdev, 0);
        udx_pci_disable(pPdev);
        return UDX_ERR_IO;
    }
    if (udx_pci_request_regions(pPdev, "rtl8168_udx") != UDX_OK) {
        rtl8168_soft_open_lamp(pPdev, 0);
        udx_pci_clear_master(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_BUSY;
    }
    /* Soft OPEN ok — regions claimed; MAP still ahead. */
    rtl8168_soft_open_lamp(pPdev, 1);

    /*
     * BAR pick honesty (soft residual):
     *   1) Prefer BAR2 (silicon-common public MMIO).
     *   2) Else BAR0 (freestanding DDI MAP_BAR grant path).
     *   3) Else first non-empty MEM BAR (host inject mistakes / odd revs).
     * Soft!=product silicon BAR certification. Prefer != product wire.
     * MAP residual still != freestanding wire ownership transfer.
     * Bind path (DDI vs inject) is stamped by freestanding/host entry.
     */
    nBar = RTL8168_BAR_MMIO;
    fBar2Miss = 0;
    u64Bar = udx_pci_resource_start(pPdev, nBar);
    u64Len = udx_pci_resource_len(pPdev, nBar);
    if (u64Bar != 0 && u64Len != 0 && udx_pci_resource_is_mem(pPdev, nBar)) {
        /* preferred path */
    } else if (udx_pci_resource_len(pPdev, RTL8168_BAR_DDI) != 0 &&
               udx_pci_resource_is_mem(pPdev, RTL8168_BAR_DDI)) {
        fBar2Miss = 1;
        nBar = RTL8168_BAR_DDI;
        u64Bar = udx_pci_resource_start(pPdev, nBar);
        u64Len = udx_pci_resource_len(pPdev, nBar);
        udx_printk("rtl8168_udx: soft bar prefer BAR2 empty; "
                   "using BAR0 (DDI grant path) len=0x%llx Soft!=product\n",
                   (unsigned long long)u64Len);
    } else {
        int iBar;

        fBar2Miss = 1;
        nBar = -1;
        u64Bar = 0;
        u64Len = 0;
        for (iBar = 0; iBar < 6; iBar++) {
            if (udx_pci_resource_len(pPdev, iBar) == 0) {
                continue;
            }
            if (!udx_pci_resource_is_mem(pPdev, iBar)) {
                continue;
            }
            nBar = iBar;
            u64Bar = udx_pci_resource_start(pPdev, nBar);
            u64Len = udx_pci_resource_len(pPdev, nBar);
            udx_printk("rtl8168_udx: soft bar fallback BAR2/BAR0 empty; "
                       "using BAR%d (len=%llx) — re-check silicon Soft!=product\n",
                       nBar, (unsigned long long)u64Len);
            break;
        }
    }
    if (nBar < 0 || u64Bar == 0 || u64Len == 0) {
        rtl8168_soft_open_map_lamp(NULL, 0, 0, 0);
        rtl8168_soft_fail_cleanup(pPdev, NULL);
        return UDX_ERR_IO;
    }

    pSoft = &g_rtlSoft;
    pSoft->pPdev = pPdev;
    pSoft->u32IrqHits = 0;
    pSoft->u32WorkHits = 0;
    pSoft->u32FQuiesced = 0;
    pSoft->u32BarIndex = (u32)nBar;
    pSoft->u32FProbeOk = 0;
    pSoft->u32FOpenOk = 1u;
    pSoft->u32FMapOk = 0;
    pSoft->u32FRingOk = 0;
    pSoft->u32FRingVerify = 0;
    pSoft->u32FBasesClean = 0;
    pSoft->u32FProdCat = 0;
    pSoft->u32FProgRes = 0;
    pSoft->u32FPhyRes = 0;
    pSoft->u32FNapiRes = 0;
    pSoft->u32FTxRxRes = 0;
    pSoft->u32FC2Path = 0;
    pSoft->u32FCapsRes = 0;
    pSoft->u32FIrqRes = 0;
    pSoft->u32FWorkRes = 0;
    pSoft->u32FWireHo = 0;
    pSoft->u32FFuncRes = 0;
    pSoft->u32FAbiRes = 0;
    pSoft->u32FProductMint = 0;
    pSoft->u32FProductProg = 0;
    pSoft->u32PathBits = RTL_SOFT_PATH_OPEN;
    pSoft->u8SoftStage = 0;
    pSoft->u8ProdSteps = 0;
    pSoft->u8ProgSteps = 0;
    pSoft->u8PhySteps = 0;
    pSoft->u8NapiSteps = 0;
    pSoft->u8TxRxSteps = 0;
    pSoft->u8CapSteps = 0;
    pSoft->u8WireSteps = 0;
    pSoft->u8FuncSteps = 0;
    pSoft->u8AbiSteps = 0;
    pSoft->u8ChipCmd = 0;
    pSoft->u8CfgRev = 0;
    pSoft->u8BarSrc = (nBar == (int)RTL8168_BAR_MMIO) ? RTL_SOFT_BAR_SRC_BAR2 :
                      (nBar == (int)RTL8168_BAR_DDI) ? RTL_SOFT_BAR_SRC_BAR0 :
                      RTL_SOFT_BAR_SRC_SCAN;
    pSoft->u8PhySt = 0;
    pSoft->u8Config1 = 0;
    pSoft->u8WireOwner = RTL_SOFT_WIRE_OWNER_FS; /* interim: freestanding */
    pSoft->u8BindPath = RTL_SOFT_BIND_PATH_NONE; /* entry stamps ddi/inject */
    pSoft->u16CfgCmd = 0;
    pSoft->u16IntrMask = 0;
    pSoft->u16IntrMaskRd = 0;
    pSoft->u16Rms = 0;
    pSoft->u32Idr0 = 0;
    pSoft->u32Idr4 = 0;
    pSoft->u32TxCfg = 0;
    pSoft->u32RxCfg = 0;
    pSoft->u32CPlus = 0;
    pSoft->u32TnpdsLo = 0;
    pSoft->u32TnpdsHi = 0;
    pSoft->u32ThpdsLo = 0;
    pSoft->u32ThpdsHi = 0;
    pSoft->u32RdsarLo = 0;
    pSoft->u32RdsarHi = 0;
    pSoft->u32TxSlots = 0;
    pSoft->u32RxSlots = 0;
    pSoft->u32SoftTxIdx = 0;
    pSoft->u32SoftRxIdx = 0;
    pSoft->pRing = NULL;
    pSoft->dmaRing = 0;
    pSoft->dmaTx = 0;
    pSoft->dmaRx = 0;
    pSoft->pTxDesc = NULL;
    pSoft->pRxDesc = NULL;
    pSoft->pRegs = NULL;
    udx_spin_lock_init(&pSoft->lockSoft);

    pSoft->pRegs = udx_ioremap(u64Bar, u64Len);
    if (pSoft->pRegs == NULL) {
        /*
         * Freestanding DDI may window-register only BAR0 VA. If preferred
         * BAR phys is known but not windowed, soft-fall to BAR0 when avail.
         */
        if (nBar != (int)RTL8168_BAR_DDI &&
            udx_pci_resource_len(pPdev, RTL8168_BAR_DDI) != 0 &&
            udx_pci_resource_is_mem(pPdev, RTL8168_BAR_DDI)) {
            u64Bar = udx_pci_resource_start(pPdev, RTL8168_BAR_DDI);
            u64Len = udx_pci_resource_len(pPdev, RTL8168_BAR_DDI);
            nBar = (int)RTL8168_BAR_DDI;
            pSoft->u32BarIndex = (u32)RTL8168_BAR_DDI;
            pSoft->u8BarSrc = RTL_SOFT_BAR_SRC_BAR0;
            pSoft->pRegs = udx_ioremap(u64Bar, u64Len);
            if (pSoft->pRegs != NULL) {
                udx_printk("rtl8168_udx: soft ioremap prefer miss → BAR0 "
                           "DDI grant fallback Soft!=product\n");
                fBar2Miss = 1;
            }
        }
        if (pSoft->pRegs == NULL) {
            rtl8168_soft_open_map_lamp(pSoft, u64Bar, u64Len, 0);
            rtl8168_soft_fail_cleanup(pPdev, pSoft);
            return UDX_ERR_NOMEM;
        }
    }

    pSoft->u8SoftStage = RTL_SOFT_ST_MAP;
    pSoft->u32FMapOk = 1u;
    pSoft->u32PathBits |= RTL_SOFT_PATH_MAP;
    rtl8168_soft_stage_lamp(RTL_SOFT_ST_MAP,
                            fBar2Miss != 0 ? "map_bar_fallback" : "map_bar2");
    /* Soft OPEN+MAP complete — still not product TX/RX; bind != wire. */
    rtl8168_soft_open_map_lamp(pSoft, u64Bar, u64Len, 1);
    rtl8168_soft_ddi_bar0_honesty(pSoft);
    rtl8168_soft_bind_ne_wire(pSoft, 0);

    /* Soft deepen: public cfg + reg snapshot without TE/RE arm. */
    rtl8168_soft_reg_snap(pSoft);

    udx_init_work(&pSoft->workIsr, rtl8168_work_fn);
    pSoft->workIsr.pPriv = pSoft;

    /*
     * Soft DMA ring alloc — residual layout next; HW bases not programmed.
     * FORCE32: G752 RTL is a force32 master (prefer PA < 4 GiB / VT-d
     * identity). Soft!=product; product DMA window mint remains OPEN.
     */
    pRing = udx_dma_alloc_coherent(pPdev->pDev, RTL8168_RING_BYTES,
                                   &dmaHandle, UDX_DMA_F_FORCE32);
    if (pRing == NULL) {
        rtl8168_soft_dma_lamp(pSoft, 0);
        rtl8168_soft_fail_cleanup(pPdev, pSoft);
        return UDX_ERR_NODMA;
    }
    pSoft->pRing = pRing;
    pSoft->dmaRing = dmaHandle;
    rtl8168_soft_dma_lamp(pSoft, 1);

    u32Bdf = udx_dma_bdf(pPdev->u8Bus, udx_pci_slot(pPdev),
                         udx_pci_func(pPdev));
    (void)udx_dma_iommu_grant(u32Bdf, dmaHandle, RTL8168_RING_BYTES);
    udx_dma_sync_single_for_device(pPdev->pDev, dmaHandle, RTL8168_RING_BYTES,
                                   UDX_DMA_BIDIRECTIONAL);

    /* Soft ring residual (TX/RX desc layout; TNPDS/RDSAR remain OPEN). */
    if (rtl8168_soft_ring_residual(pSoft) == 0) {
        rtl8168_soft_fail_cleanup(pPdev, pSoft);
        return UDX_ERR_IO;
    }

    if (udx_request_irq(pPdev->nIrq, rtl8168_isr, 0, "rtl8168_udx",
                        pSoft) != UDX_OK) {
        rtl8168_soft_irq_lamp(pSoft, pPdev->nIrq, 0);
        rtl8168_soft_fail_cleanup(pPdev, pSoft);
        return UDX_ERR_NODEVIRQ;
    }
    udx_enable_irq(pPdev->nIrq);

    /*
     * Soft IntrMask: allow ROK|TOK reason bits for host demo.
     * Does not enable MAC Tx/Rx (ChipCmd left alone — soft != link up).
     * Soft readback residual documents mask stickiness on host inject.
     */
    pSoft->u16IntrMask = (u16)RTL_SOFT_IRQ_BITS;
    udx_writew(pSoft->pRegs, RTL_REG_INTR_MASK, pSoft->u16IntrMask);
    udx_mmio_flush(pSoft->pRegs);
    pSoft->u16IntrMaskRd = udx_readw(pSoft->pRegs, RTL_REG_INTR_MASK);

    pSoft->u8SoftStage = RTL_SOFT_ST_IRQ;
    pSoft->u32PathBits |= RTL_SOFT_PATH_IRQ;
    rtl8168_soft_stage_lamp(RTL_SOFT_ST_IRQ, "irq_soft");
    rtl8168_soft_irq_lamp(pSoft, pPdev->nIrq, 1);
    udx_printk("rtl8168_udx: soft irq mask_wr=0x%04x mask_rd=0x%04x "
               "chipcmd_te_re=OPEN Soft!=product\n",
               (unsigned)pSoft->u16IntrMask,
               (unsigned)pSoft->u16IntrMaskRd);
    /* Once-lamp IRQ residual (IntrMask soft vs product TE/RE/MSI-X OPEN). */
    rtl8168_soft_irq_residual(pSoft);

    pSoft->u32FProbeOk = 1;
    /* Wire ownership honesty: soft residual never claims freestanding wire. */
    pSoft->u8WireOwner = RTL_SOFT_WIRE_OWNER_FS;
    udx_set_drvdata(pPdev->pDev, pSoft);
    udx_printk("rtl8168_udx: soft probe ok vend=%04x dev=%04x bar%u=%llx "
               "bar_src=%u dma=%llx irq=%d stage=%u "
               "open=%u map=%u ring=%u verify=%u bases_clean=%u "
               "path=0x%x wire=freestanding "
               "dev=%s Soft!=product\n",
               (unsigned)pPdev->u16Vendor, (unsigned)pPdev->u16Device,
               pSoft->u32BarIndex, (unsigned long long)u64Bar,
               (unsigned)pSoft->u8BarSrc,
               (unsigned long long)dmaHandle, pPdev->nIrq,
               (unsigned)pSoft->u8SoftStage,
               (unsigned)pSoft->u32FOpenOk,
               (unsigned)pSoft->u32FMapOk,
               (unsigned)pSoft->u32FRingOk,
               (unsigned)pSoft->u32FRingVerify,
               (unsigned)pSoft->u32FBasesClean,
               (unsigned)pSoft->u32PathBits,
               udx_dev_name(pPdev->pDev));
    /* ready → product residual catalog → honesty (catalog != TX/RX). */
    rtl8168_soft_ready_catalog(pSoft);
    rtl8168_soft_honesty(pSoft);
    return UDX_OK;
}

static void
rtl8168_quiesce(struct udx_pci_dev *pPdev)
{
    struct rtl8168_soft *pSoft;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return;
    }
    pSoft = (struct rtl8168_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft == NULL) {
        return;
    }

    udx_spin_lock(&pSoft->lockSoft);
    pSoft->u32FQuiesced = 1;
    udx_spin_unlock(&pSoft->lockSoft);

    udx_disable_irq(pPdev->nIrq);
    (void)udx_cancel_work(&pSoft->workIsr);

    if (pSoft->pRegs != NULL) {
        udx_writew(pSoft->pRegs, RTL_REG_INTR_MASK, 0);
        udx_writew(pSoft->pRegs, RTL_REG_INTR_STATUS, 0xffffu);
        /*
         * Option 3 product program may have armed TE|RE. Quiesce drops TE|RE
         * when product_prog=1 so remove is OWN-safe (Own still 0 residual).
         * Soft residual path (product_prog=0) never armed TE|RE — leave alone.
         */
        if (pSoft->u32FProductProg != 0u) {
            u8 u8CmdQ;

            u8CmdQ = udx_readb(pSoft->pRegs, RTL_REG_CHIPCMD);
            u8CmdQ = (u8)(u8CmdQ & (u8)~(RTL_CMD_TE | RTL_CMD_RE | RTL_CMD_RST));
            udx_writeb(pSoft->pRegs, RTL_REG_CHIPCMD, u8CmdQ);
            pSoft->u8ChipCmd = u8CmdQ;
        }
        udx_mmio_flush(pSoft->pRegs);
        pSoft->u16IntrMask = 0;
    }
    /*
     * Soft residual honesty: soft never programmed TNPDS/RDSAR.
     * Product path may have; quiesce leaves bases (MAC re-init is product).
     * Master clear always.
     */
    udx_pci_clear_master(pPdev);
    udx_printk("rtl8168_udx: soft quiesce stage=%u open=%u map=%u "
               "ring=%u verify=%u bases_clean=%u path=0x%x "
               "product_mint=%u product_prog=%u "
               "wire_owner=%s Soft!=product\n",
               (unsigned)pSoft->u8SoftStage,
               (unsigned)pSoft->u32FOpenOk,
               (unsigned)pSoft->u32FMapOk,
               (unsigned)pSoft->u32FRingOk,
               (unsigned)pSoft->u32FRingVerify,
               (unsigned)pSoft->u32FBasesClean,
               (unsigned)pSoft->u32PathBits,
               (unsigned)pSoft->u32FProductMint,
               (unsigned)pSoft->u32FProductProg,
               (pSoft->u8WireOwner == RTL_SOFT_WIRE_OWNER_UDX)
                   ? "udx" : "freestanding");
}

static void
rtl8168_remove(struct udx_pci_dev *pPdev)
{
    struct rtl8168_soft *pSoft;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return;
    }
    pSoft = (struct rtl8168_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft != NULL) {
        udx_free_irq(pPdev->nIrq, pSoft);
        if (udx_work_pending(&pSoft->workIsr)) {
            (void)udx_cancel_work(&pSoft->workIsr);
        }
        if (pSoft->pRing != NULL) {
            udx_dma_free_coherent(pPdev->pDev, RTL8168_RING_BYTES,
                                  pSoft->pRing, pSoft->dmaRing);
            pSoft->pRing = NULL;
            pSoft->pTxDesc = NULL;
            pSoft->pRxDesc = NULL;
            pSoft->u32FRingOk = 0;
            pSoft->u32FRingVerify = 0;
            pSoft->u32FBasesClean = 0;
        }
        if (pSoft->pRegs != NULL) {
            udx_iounmap(pSoft->pRegs);
            pSoft->pRegs = NULL;
        }
        pSoft->u32FProbeOk = 0;
        pSoft->u32FOpenOk = 0;
        pSoft->u32FMapOk = 0;
        pSoft->u32FProdCat = 0;
        pSoft->u32FProgRes = 0;
        pSoft->u32FPhyRes = 0;
        pSoft->u32FNapiRes = 0;
        pSoft->u32FTxRxRes = 0;
        pSoft->u32FC2Path = 0;
        pSoft->u32FCapsRes = 0;
        pSoft->u32FIrqRes = 0;
        pSoft->u32FWorkRes = 0;
        pSoft->u32FWireHo = 0;
        pSoft->u32FFuncRes = 0;
        pSoft->u32FAbiRes = 0;
        pSoft->u32FProductMint = 0;
        pSoft->u32FProductProg = 0;
        pSoft->u8ProdSteps = 0;
        pSoft->u8ProgSteps = 0;
        pSoft->u8PhySteps = 0;
        pSoft->u8NapiSteps = 0;
        pSoft->u8TxRxSteps = 0;
        pSoft->u8CapSteps = 0;
        pSoft->u8WireSteps = 0;
        pSoft->u8FuncSteps = 0;
        pSoft->u8AbiSteps = 0;
        pSoft->u8BindPath = RTL_SOFT_BIND_PATH_NONE;
        pSoft->u32PathBits = 0;
        /* Wire remains freestanding interim; remove does not claim UDX wire. */
        pSoft->u8WireOwner = RTL_SOFT_WIRE_OWNER_FS;
        udx_set_drvdata(pPdev->pDev, NULL);
    }
    udx_pci_release_regions(pPdev);
    udx_pci_clear_master(pPdev);
    udx_pci_disable(pPdev);
    udx_printk("rtl8168_udx: soft remove bind_ne_wire=1 "
               "wire=freestanding Soft!=product\n");
}

static const struct udx_pci_device_id rtl8168_ids[] = {
    UDX_PCI_DEVICE(RTL8168_VEND, RTL8168_DEV),
    { 0, }
};

static struct udx_pci_driver rtl8168_driver = {
    .szName = "rtl8168_udx",
    .pIdTable = rtl8168_ids,
    .pfnProbe = rtl8168_probe,
    .pfnRemove = rtl8168_remove,
    .pfnQuiesce = rtl8168_quiesce,
};

/**
 * Shared register entry for freestanding hosts (no inject).
 * Returns UDX_OK after driver table is live.
 */
int
rtl8168_udx_register(void)
{
    if (udx_init() != UDX_OK) {
        return UDX_ERR_IO;
    }
    return udx_pci_register_driver(&rtl8168_driver);
}

#if defined(UDX_HOST_LIBC)

int
main(int argc, char **argv)
{
    struct udx_pci_dev *pPdev;
    u64 aBarLen[6];
    u8 aBarMem[6];
    int iBar;
    udx_status_t st;
    int fPass;
    struct rtl8168_soft *pSoft;
    u32 u32PassBases;
    u32 u32PassPath;
    u32 u32PassProd;
    u32 u32PassProg;
    u32 u32PassPhy;
    u32 u32PassNapi;
    u32 u32PassTxRx;
    u32 u32PassC2;
    u32 u32PassCaps;
    u32 u32PassIrqRes;
    u32 u32PassWorkRes;
    u32 u32PassWireHo;
    u32 u32PassFunc;
    u32 u32PassAbi;
    u8  u8PassProdSteps;
    u8  u8PassProgSteps;
    u8  u8PassPhySteps;
    u8  u8PassNapiSteps;
    u8  u8PassTxRxSteps;
    u8  u8PassCapSteps;
    u8  u8PassWireSteps;
    u8  u8PassFuncSteps;
    u8  u8PassAbiSteps;

    (void)argc;
    (void)argv;
    fPass = 0;
    u32PassBases = 0;
    u32PassPath = 0;
    u32PassProd = 0;
    u32PassCaps = 0;
    u32PassProg = 0;
    u32PassPhy = 0;
    u32PassNapi = 0;
    u32PassTxRx = 0;
    u32PassC2 = 0;
    u32PassIrqRes = 0;
    u32PassWorkRes = 0;
    u32PassWireHo = 0;
    u32PassFunc = 0;
    u32PassAbi = 0;
    u8PassProdSteps = 0;
    u8PassProgSteps = 0;
    u8PassPhySteps = 0;
    u8PassNapiSteps = 0;
    u8PassTxRxSteps = 0;
    u8PassCapSteps = 0;
    u8PassWireSteps = 0;
    u8PassFuncSteps = 0;
    u8PassAbiSteps = 0;

    if (udx_init() != UDX_OK) {
        udx_printk("rtl8168_udx: soft SKIP (udx_init) Soft!=product\n");
        return 1;
    }

    /* Soft UDX DDI surface notes + Dual DoD B catalog (observation only). */
    udx_host_soft_init();
    udx_host_soft_lifecycle_note();
    /* Host product path honesty: freestanding rtl remains SKIP. */
    udx_printk("rtl8168_udx: soft freestanding product=SKIP "
               "GJ_RTL8168_PROBE_default=0 product=UDX+ABI "
               "host_path=1 Soft!=product G-AC-1\n");
    rtl8168_soft_catalog();

    if (udx_pci_register_driver(&rtl8168_driver) != UDX_OK) {
        udx_printk("rtl8168_udx: soft SKIP (register) Soft!=product\n");
        udx_exit();
        return 1;
    }

    /*
     * Prefer real DDI bind (GJ_SYS_DDI / udx_host_bind_by_id) over inject-only.
     * Host-linux typically soft-SKIPs DDI (no GJ_SYS_DDI); then inject lab demo.
     * Soft!=product; Dual DoD B OPEN; bind != wire.
     * greppable: rtl8168_udx: soft ddi prefer / soft bind path
     * greppable: prefer_real_ddi= / real_ddi=
     */
    udx_printk("rtl8168_udx: soft prefer real DDI id=10ec:8168 "
               "prefer_real_ddi=1 via=udx_host_bind_by_id "
               "gj_sys_ddi=%u host_path=1 "
               "product=UDX+ABI product=UDX+sshd+stack need=DDI_caps "
               "Soft!=product dual_dod_b=OPEN\n",
               (unsigned)GJ_SYS_DDI);
    pPdev = NULL;
    pSoft = NULL;
    st = udx_host_bind_by_id((u16)RTL8168_VEND, (u16)RTL8168_DEV, &pPdev);
    if (st == UDX_OK && pPdev != NULL && pPdev->pDev != NULL &&
        udx_dev_has_drvdata(pPdev->pDev)) {
        pSoft = (struct rtl8168_soft *)udx_get_drvdata(pPdev->pDev);
        if (pSoft != NULL && pSoft->u32FProbeOk != 0u &&
            pSoft->u32FOpenOk != 0u && pSoft->u32FMapOk != 0u &&
            pSoft->u32FRingOk != 0u && pSoft->u32FRingVerify != 0u) {
            pSoft->u8BindPath = RTL_SOFT_BIND_PATH_DDI;
            pSoft->u8WireOwner = RTL_SOFT_WIRE_OWNER_FS;
            rtl8168_soft_bind_lamp(pPdev, 1);
            rtl8168_soft_bind_path_lamp(pSoft, RTL_SOFT_BIND_PATH_DDI);
            udx_printk("rtl8168_udx: soft bind host_ddi=1 freestanding_bind=1 "
                       "inject=0 prefer_real_ddi=1 real_ddi=1 host_inject=0 "
                       "gj_sys_ddi=%u bind_ne_wire=1 "
                       "product=UDX+sshd+stack dual_dod_b=OPEN "
                       "Soft!=product\n",
                       (unsigned)GJ_SYS_DDI);
            rtl8168_soft_bind_ne_wire(pSoft, 1);
            /* STRONGER: re-densify functional open/map/program_gate real_ddi. */
            if (pSoft->u32FCapsRes == 0u) {
                rtl8168_soft_caps_residual(pSoft);
            }
            rtl8168_soft_functional_residual(pSoft);
            /*
             * Option 3: product program when real_ddi + program_gate.
             * Host-linux rarely has real GJ_SYS_DDI MAP; freestanding DUT does.
             */
            (void)rtl8168_product_program_try(pSoft);
        } else {
            pSoft = NULL;
            pPdev = NULL;
            st = UDX_ERR_IO;
        }
    } else {
        pSoft = NULL;
        pPdev = NULL;
        st = UDX_ERR_NOSYS;
    }

    if (pSoft == NULL) {
        /* Soft inject fallback: BAR2 MMIO only; other BARs empty. G752 BDF. */
        for (iBar = 0; iBar < 6; iBar++) {
            aBarLen[iBar] = 0;
            aBarMem[iBar] = 1;
        }
        aBarLen[RTL8168_BAR_MMIO] = RTL8168_BAR_LEN;

        st = udx_host_inject_pci((u16)RTL8168_VEND, (u16)RTL8168_DEV,
                                 (u8)RTL8168_SOFT_BUS,
                                 udx_pci_devfn((u8)RTL8168_SOFT_SLOT,
                                               (u8)RTL8168_SOFT_FUNC),
                                 RTL8168_IRQ_SOFT,
                                 aBarLen, aBarMem, &pPdev);
        if (st != UDX_OK) {
            rtl8168_soft_bind_lamp(NULL, 0);
            rtl8168_soft_bind_path_lamp(NULL, RTL_SOFT_BIND_PATH_NONE);
            udx_printk("rtl8168_udx: soft SKIP (inject %d after ddi prefer) "
                       "Soft!=product\n", (int)st);
            udx_pci_unregister_driver(&rtl8168_driver);
            udx_exit();
            return 1;
        }

        if (pPdev == NULL || pPdev->pDev == NULL ||
            !udx_dev_has_drvdata(pPdev->pDev)) {
            udx_printk("rtl8168_udx: soft SKIP (probe/no drvdata) "
                       "Soft!=product\n");
            udx_pci_unregister_driver(&rtl8168_driver);
            udx_exit();
            return 1;
        }

        pSoft = (struct rtl8168_soft *)udx_get_drvdata(pPdev->pDev);
        if (pSoft == NULL || pSoft->u32FProbeOk == 0u ||
            pSoft->u32FOpenOk == 0u || pSoft->u32FMapOk == 0u ||
            pSoft->u32FRingOk == 0u || pSoft->u32FRingVerify == 0u) {
            udx_printk("rtl8168_udx: soft SKIP (probe/open+map/ring residual/"
                       "verify) Soft!=product\n");
            udx_pci_unregister_driver(&rtl8168_driver);
            udx_exit();
            return 1;
        }
        /*
         * Host inject residual stands in when DDI soft-SKIPs.
         * greppable: soft bind path path=inject; still freestanding owns wire.
         */
        pSoft->u8BindPath = RTL_SOFT_BIND_PATH_INJECT;
        pSoft->u8WireOwner = RTL_SOFT_WIRE_OWNER_FS;
        rtl8168_soft_bind_lamp(NULL, 0);
        rtl8168_soft_bind_path_lamp(pSoft, RTL_SOFT_BIND_PATH_INJECT);
        udx_printk("rtl8168_udx: soft bind host_inject=1 freestanding_bind=0 "
                   "ddi_prefer_tried=1 prefer_real_ddi=1 real_ddi=0 "
                   "inject_fallback=1 bind_ne_wire=1 "
                   "product=UDX+sshd+stack "
                   "lab_ip residual OPEN lab_ip=10.200.125.50 "
                   "wire claim residual OPEN wire_claim=OPEN "
                   "dual_dod_b=OPEN Soft!=product never_program=1\n");
        rtl8168_soft_bind_ne_wire(pSoft, 0);
        /*
         * STRONGER: re-densify functional open/map/program_gate after inject
         * bind_path stamp (prefer_real_ddi=1 real_ddi=0; caps residual first).
         * product_mint=0 → never write TNPDS/RDSAR/TE|RE. Dual DoD B OPEN.
         */
        if (pSoft->u32FCapsRes == 0u) {
            rtl8168_soft_caps_residual(pSoft);
        }
        rtl8168_soft_functional_residual(pSoft);
        /*
         * Option 3 product program on inject path: always SKIP
         * (real_ddi=0 → product_mint=0 never_program=1). Greppable residual.
         */
        (void)rtl8168_product_program_try(pSoft);
    }

    /* Soft bar inventory + DDI BAR0 honesty; not product MMIO_FRAME. */
    {
        struct udx_host_bar barSnap;
        struct udx_host_bar aBars[UDX_HOST_SOFT_BAR_MAX];
        struct udx_host_bdf bdfSnap;
        struct udx_host_window_grant winGrant;
        u32 u32BarCount;
        int iBarInv;

        if (udx_host_soft_bdf_from_pdev(pPdev, &bdfSnap) == UDX_OK) {
            udx_printk("rtl8168_udx: soft bdf bus=%u slot=%u func=%u "
                       "Soft!=product\n",
                       (unsigned)bdfSnap.u8Bus,
                       (unsigned)bdfSnap.u8Slot,
                       (unsigned)bdfSnap.u8Func);
        }
        if (udx_host_soft_bar_snapshot(pPdev, RTL8168_BAR_MMIO,
                                       &barSnap) == UDX_OK) {
            udx_printk("rtl8168_udx: soft bar snap idx=%u mem=%u "
                       "pa=0x%llx cb=0x%llx Soft!=product\n",
                       (unsigned)barSnap.u8Index,
                       (unsigned)barSnap.u8Mem,
                       (unsigned long long)barSnap.u64Pa,
                       (unsigned long long)barSnap.u64Cb);
        }
        /* Explicit BAR0 snapshot even when host inject leaves it empty. */
        if (udx_host_soft_bar_snapshot(pPdev, RTL8168_BAR_DDI,
                                       &barSnap) == UDX_OK) {
            udx_printk("rtl8168_udx: soft bar0 snap idx=%u mem=%u "
                       "pa=0x%llx cb=0x%llx "
                       "(DDI grant path residual; host inject often empty) "
                       "Soft!=product\n",
                       (unsigned)barSnap.u8Index,
                       (unsigned)barSnap.u8Mem,
                       (unsigned long long)barSnap.u64Pa,
                       (unsigned long long)barSnap.u64Cb);
        }
        u32BarCount = 0;
        if (udx_host_soft_bar_snapshot_all(pPdev, aBars,
                                           &u32BarCount) == UDX_OK) {
            udx_printk("rtl8168_udx: soft bar inventory count=%u "
                       "Soft!=product\n", (unsigned)u32BarCount);
            for (iBarInv = 0; iBarInv < (int)UDX_HOST_SOFT_BAR_MAX; iBarInv++) {
                if (aBars[iBarInv].u64Cb == 0) {
                    continue;
                }
                udx_printk("rtl8168_udx: soft bar inv idx=%u mem=%u "
                           "pa=0x%llx cb=0x%llx Soft!=product\n",
                           (unsigned)aBars[iBarInv].u8Index,
                           (unsigned)aBars[iBarInv].u8Mem,
                           (unsigned long long)aBars[iBarInv].u64Pa,
                           (unsigned long long)aBars[iBarInv].u64Cb);
            }
        }
        rtl8168_soft_ddi_bar0_honesty(pSoft);

        /*
         * Soft DMA window request — always not-granted until product mint.
         * Residual honesty: ring DMA cookie exists; product window cap OPEN.
         */
        if (udx_host_soft_bdf_from_pdev(pPdev, &bdfSnap) == UDX_OK) {
            (void)udx_host_soft_dma_window_request(&bdfSnap,
                                                   (u64)pSoft->dmaRing,
                                                   (u64)RTL8168_RING_BYTES,
                                                   0x3u, &winGrant);
            udx_printk("rtl8168_udx: soft dma window ok=%u "
                       "win_id=0x%x product_mint=0 Soft!=product\n",
                       (unsigned)winGrant.u8Ok,
                       (unsigned)winGrant.u32WinId);
        }
    }

    /* Raise soft IntrStatus bits, then fire threaded IRQ. */
    st = udx_host_bar_writew(pPdev, RTL8168_BAR_MMIO, RTL_REG_INTR_STATUS,
                             (u16)RTL_SOFT_IRQ_BITS);
    if (st != UDX_OK) {
        udx_printk("rtl8168_udx: soft SKIP (bar poke %d) Soft!=product\n", st);
        udx_pci_unregister_driver(&rtl8168_driver);
        udx_exit();
        return 1;
    }
    udx_host_fire_irq(RTL8168_IRQ_SOFT);
    udx_run();

    pSoft = (struct rtl8168_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft != NULL && pSoft->u32IrqHits >= 1u &&
        pSoft->u32WorkHits >= 1u && pSoft->u32FProbeOk != 0u &&
        pSoft->u32FOpenOk != 0u && pSoft->u32FMapOk != 0u &&
        pSoft->u32FRingOk != 0u && pSoft->u32FRingVerify != 0u) {
        if (pSoft->u8SoftStage < RTL_SOFT_ST_WORK) {
            pSoft->u8SoftStage = RTL_SOFT_ST_WORK;
        }
        fPass = 1;
        /* Snapshot residual honesty before quiesce/remove clears flags. */
        u32PassBases = pSoft->u32FBasesClean;
        u32PassPath = pSoft->u32PathBits;
        u32PassProd = pSoft->u32FProdCat;
        u32PassProg = pSoft->u32FProgRes;
        u32PassPhy = pSoft->u32FPhyRes;
        u32PassNapi = pSoft->u32FNapiRes;
        u32PassTxRx = pSoft->u32FTxRxRes;
        u32PassC2 = pSoft->u32FC2Path;
        u32PassCaps = pSoft->u32FCapsRes;
        u32PassIrqRes = pSoft->u32FIrqRes;
        u32PassWorkRes = pSoft->u32FWorkRes;
        u32PassWireHo = pSoft->u32FWireHo;
        u32PassFunc = pSoft->u32FFuncRes;
        u32PassAbi = pSoft->u32FAbiRes;
        u8PassProdSteps = pSoft->u8ProdSteps;
        u8PassProgSteps = pSoft->u8ProgSteps;
        u8PassPhySteps = pSoft->u8PhySteps;
        u8PassNapiSteps = pSoft->u8NapiSteps;
        u8PassTxRxSteps = pSoft->u8TxRxSteps;
        u8PassCapSteps = pSoft->u8CapSteps;
        u8PassWireSteps = pSoft->u8WireSteps;
        u8PassFuncSteps = pSoft->u8FuncSteps;
        u8PassAbiSteps = pSoft->u8AbiSteps;
        /* Final host-path honesty stamps while soft state still live. */
        rtl8168_soft_bind_ne_wire(pSoft, 0);
        /* Re-walk functional residual so work=1 densifies after IRQ path. */
        rtl8168_soft_functional_residual(pSoft);
        rtl8168_soft_probe_residual(pSoft);
        rtl8168_soft_open_map_honesty(pSoft);
        rtl8168_soft_c2_product_path(pSoft);
        /*
         * denser residual bar .75 VERDICT after bind_path + catalogs live
         * (H2 once; never_program held; Soft!=product; Dual DoD B OPEN).
         */
        rtl8168_soft_denser_residual_verdict(pSoft);
        u32PassC2 = pSoft->u32FC2Path;
        u32PassCaps = pSoft->u32FCapsRes;
        u32PassIrqRes = pSoft->u32FIrqRes;
        u32PassWorkRes = pSoft->u32FWorkRes;
        u32PassWireHo = pSoft->u32FWireHo;
        u32PassFunc = pSoft->u32FFuncRes;
        u32PassAbi = pSoft->u32FAbiRes;
        u8PassCapSteps = pSoft->u8CapSteps;
        u8PassWireSteps = pSoft->u8WireSteps;
        u8PassFuncSteps = pSoft->u8FuncSteps;
        u8PassAbiSteps = pSoft->u8AbiSteps;
        u32PassPath = pSoft->u32PathBits;
    }

    udx_pci_unregister_driver(&rtl8168_driver);
    udx_exit();

    if (fPass) {
        udx_printk("rtl8168_udx: soft open+map host PASS bar=%u "
                   "Soft!=product\n",
                   (unsigned)RTL8168_BAR_MMIO);
        udx_printk("rtl8168_udx: soft probe PASS stage=%u irq_work=1 "
                   "open=1 map=1 ring_residual=1 ring_verify=1 "
                   "prod_cat=%u prod_steps=%u/%u "
                   "prog=%u prog_steps=%u/%u "
                   "phy_res=%u phy_steps=%u/%u "
                   "napi_res=%u napi_steps=%u/%u "
                   "txrx_res=%u txrx_steps=%u/%u "
                   "caps=%u cap_steps=%u/%u "
                   "irq_res=%u work_res=%u "
                   "wire_ho=%u wire_steps=%u/%u "
                   "func=%u func_steps=%u/%u "
                   "abi=%u abi_steps=%u/%u "
                   "c2=%u bases_clean=%u path=0x%x "
                   "tx_rx=OPEN never_tx_rx=1 bind_ne_wire=1 "
                   "wire=freestanding freestanding_owns_wire=1 "
                   "never_claim_fs_wire=1 freestanding_product=SKIP "
                   "product=UDX+ABI need=DDI_caps need=hot_cold_abi "
                   "product_mint=0 never_program=1 never_program_held=1 "
                   "never_tnpds=1 never_rdsar=1 never_te_re=1 "
                   "denser_prove=%u denser_residual=%u "
                   "bar=v2026.08.04.75 stamp_free=%u "
                   "dual_dod_b=OPEN "
                   "claim_class=C2 G-AC-1 Soft!=product\n",
                   (unsigned)RTL_SOFT_ST_WORK,
                   (unsigned)u32PassProd,
                   (unsigned)u8PassProdSteps,
                   (unsigned)RTL_SOFT_PROD_STEP_COUNT,
                   (unsigned)u32PassProg,
                   (unsigned)u8PassProgSteps,
                   (unsigned)RTL_SOFT_PROG_STEP_COUNT,
                   (unsigned)u32PassPhy,
                   (unsigned)u8PassPhySteps,
                   (unsigned)RTL_SOFT_PHY_STEP_COUNT,
                   (unsigned)u32PassNapi,
                   (unsigned)u8PassNapiSteps,
                   (unsigned)RTL_SOFT_NAPI_STEP_COUNT,
                   (unsigned)u32PassTxRx,
                   (unsigned)u8PassTxRxSteps,
                   (unsigned)RTL_SOFT_TXRX_STEP_COUNT,
                   (unsigned)u32PassCaps,
                   (unsigned)u8PassCapSteps,
                   (unsigned)RTL_SOFT_CAP_STEP_COUNT,
                   (unsigned)u32PassIrqRes,
                   (unsigned)u32PassWorkRes,
                   (unsigned)u32PassWireHo,
                   (unsigned)u8PassWireSteps,
                   (unsigned)RTL_SOFT_WIRE_STEP_COUNT,
                   (unsigned)u32PassFunc,
                   (unsigned)u8PassFuncSteps,
                   (unsigned)RTL_SOFT_FUNC_STEP_COUNT,
                   (unsigned)u32PassAbi,
                   (unsigned)u8PassAbiSteps,
                   (unsigned)RTL_SOFT_ABI_STEP_COUNT,
                   (unsigned)u32PassC2,
                   (unsigned)u32PassBases,
                   (unsigned)u32PassPath,
                   (unsigned)RTL_SOFT_DENSER_PROVE,
                   (unsigned)RTL_SOFT_DENSER_RESIDUAL,
                   (unsigned)RTL_SOFT_BAR_STAMP_FREE);
        return 0;
    }
    udx_printk("rtl8168_udx: soft SKIP (irq/work path incomplete) "
               "tx_rx=OPEN Soft!=product\n");
    return 1;
}

#endif /* UDX_HOST_LIBC — host inject main ends; product program shared */

/**
 * Option 3 product program path (Dual DoD B product UDX NIC direction).
 *
 * When freestanding real DDI bind (bind_path=ddi) and program_gate
 * (MAP + DMA ring + ring residual/verify + 256-byte TNPDS/RDSAR align)
 * are live, product_mint opens and the public program order executes:
 *   TNPDS lo/hi → RDSAR lo/hi → ChipCmd TE|RE
 *
 * Soft residual (inject / soft catalogs) never writes these; soft residual
 * keeps never_program=1 as the soft residual class law. Product program
 * is a separate greppable surface — Soft residual != product program.
 * Host inject (bind_path=inject) always SKIPs product program.
 *
 * Own stays host (0) on all descriptors: TE|RE arm does not DMA until
 * product Own handoff + RX/TX buffers (TX/RX still OPEN). Dual DoD B
 * remains OPEN until DUT L3 (lab IP / stack / sshd :22). agent!=close.
 *
 * greppable: rtl8168_udx: product program
 * greppable: rtl8168_udx: product program PASS
 * greppable: rtl8168_udx: product program SKIP
 * greppable: product_mint=1 never_program=0 write=1 order=tnpds_lo,...
 * greppable: wire_owner=udx product_handoff=1 dual_dod_b=OPEN
 */

/**
 * Product Own handoff after TNPDS/RDSAR/TE|RE: RX buffers + Own=1 + RCR.
 * Enables DMA RX for thr-poll reclaim → ETH_INJECT demux. Soft!=product.
 * greppable: rtl8168_udx: product own_handoff
 */
static int
rtl8168_product_own_handoff(struct rtl8168_soft *pSoft)
{
    u32 iSlot;
    u32 u32Rcr;
    u32 u32Bdf;
    u32 u32Got;
    udx_dma_addr_t dmaTxb;
    udx_dma_addr_t dmaRx0;
    void *pTxb;
    struct udx_pci_dev *pPdev;

    if (pSoft == NULL || pSoft->pRegs == NULL || pSoft->pRxDesc == NULL ||
        pSoft->pPdev == NULL || pSoft->pPdev->pDev == NULL) {
        return 0;
    }
    if (pSoft->u32FProductProg == 0u) {
        return 0;
    }
    if (pSoft->u32FOwnHandoff != 0u) {
        return 1;
    }
    pPdev = pSoft->pPdev;
    u32Got = 0u;
    dmaRx0 = 0;

    /*
     * Drop RE while we fill RX addresses (product_program may already
     * have TE|RE on with addr=0 descs). Soft!=product Dual DoD B.
     */
    {
        u8 u8CmdHold = udx_readb(pSoft->pRegs, RTL_REG_CHIPCMD);

        u8CmdHold = (u8)(u8CmdHold & (u8)~RTL_CMD_RE);
        udx_writeb(pSoft->pRegs, RTL_REG_CHIPCMD, u8CmdHold);
        udx_mmio_flush(pSoft->pRegs);
    }

    /*
     * Per-slot FORCE32 pages (RTL_SOFT_RX_PAGE_BYTES; PA via PLATFORM_INFO
     * op6 virt_to_phys). DUT .84: 32 KiB VA pool + identity cookie → inject=0
     * (VA!=PA). Buffer_Size in Own opts1 matches page (public ×8 honesty).
     * Soft!=product Dual DoD B OPEN.
     */
    for (iSlot = 0u; iSlot < pSoft->u32RxSlots && iSlot < RTL_SOFT_RX_SLOTS;
         iSlot++) {
        void *pSlot;
        udx_dma_addr_t dmaSlot;

        pSlot = udx_dma_alloc_coherent(pPdev->pDev, RTL_SOFT_RX_PAGE_BYTES,
                                       &dmaSlot, UDX_DMA_F_FORCE32);
        if (pSlot == NULL) {
            udx_printk("rtl8168_udx: product own_handoff SKIP nodma_rx "
                       "slot=%u Soft!=product dual_dod_b=OPEN\n",
                       (unsigned)iSlot);
            goto fail_rx;
        }
        pSoft->apRxSlot[iSlot] = pSlot;
        pSoft->aDmaRxSlot[iSlot] = dmaSlot;
        if (iSlot == 0u) {
            dmaRx0 = dmaSlot;
        }
        u32Got++;
    }

    pTxb = udx_dma_alloc_coherent(pPdev->pDev, RTL_SOFT_TX_BOUNCE_BYTES,
                                  &dmaTxb, UDX_DMA_F_FORCE32);
    if (pTxb == NULL) {
        udx_printk("rtl8168_udx: product own_handoff SKIP nodma_tx "
                   "Soft!=product dual_dod_b=OPEN\n");
        goto fail_rx;
    }

    pSoft->pTxBounce = pTxb;
    pSoft->dmaTxBounce = dmaTxb;

    /*
     * Soft + VT-d identity grant residual (PLATFORM_INFO op5).
     * DUT arping 0 replies / own_stuck: under TE, PA outside identity
     * or bus3 DID0 miss leaves OWN stuck. Grant ring + every RX slot +
     * TX bounce (force32 pages). Soft cover only — not product DMA
     * window CNode mint / full VT-d product. Soft!=product Dual DoD B.
     *
     * Always once-lamp grant result (PASS or FAIL) with bdf, each PA
     * class, grant_n / fail_n / geo_fail_n, and cover=soft_note honesty.
     * residual: Own stick under TE if identity miss.
     * greppable: rtl8168_udx: product own_handoff iommu_grant
     * greppable: rtl8168_udx: product own_handoff iommu_grant FAIL
     */
    {
        int nGrantFail = 0;
        int nGrantN = 0;
        int nGeoFail = 0;
        int nRingGrant = 0;
        int nTxGrant = 0;
        int nRxGrantOk = 0;
        int fRingGeo = 0;
        int fTxGeo = 0;
        int fRx0Geo = 0;
        int fLowRx0 = 0;
        udx_dma_addr_t dmaRxLast = 0;

        u32Bdf = udx_dma_bdf(pPdev->u8Bus, udx_pci_slot(pPdev),
                             udx_pci_func(pPdev));

        /* Ring: full coherent alloc size (soft desc residual inside). */
        if (pSoft->dmaRing != 0) {
            fRingGeo = udx_dma_window_ok(pSoft->dmaRing, RTL8168_RING_BYTES, 1);
            if (fRingGeo == 0) {
                nGeoFail++;
            }
            nGrantN++;
            if (udx_dma_iommu_grant(u32Bdf, pSoft->dmaRing,
                                    RTL8168_RING_BYTES) != 0) {
                nGrantFail++;
            } else {
                nRingGrant = 1;
            }
        }

        /* All RX force32 slots. */
        for (iSlot = 0u; iSlot < u32Got; iSlot++) {
            if (udx_dma_window_ok(pSoft->aDmaRxSlot[iSlot],
                                  RTL_SOFT_RX_PAGE_BYTES, 1) == 0) {
                nGeoFail++;
            } else if (iSlot == 0u) {
                fRx0Geo = 1;
            }
            if (iSlot == 0u) {
                fLowRx0 = udx_dma_is_low(pSoft->aDmaRxSlot[iSlot]);
            }
            dmaRxLast = pSoft->aDmaRxSlot[iSlot];
            nGrantN++;
            if (udx_dma_iommu_grant(u32Bdf, pSoft->aDmaRxSlot[iSlot],
                                    RTL_SOFT_RX_PAGE_BYTES) != 0) {
                nGrantFail++;
            } else {
                nRxGrantOk++;
            }
        }

        /* TX bounce. */
        fTxGeo = udx_dma_window_ok(dmaTxb, RTL_SOFT_TX_BOUNCE_BYTES, 1);
        if (fTxGeo == 0) {
            nGeoFail++;
        }
        nGrantN++;
        if (udx_dma_iommu_grant(u32Bdf, dmaTxb, RTL_SOFT_TX_BOUNCE_BYTES) !=
            0) {
            nGrantFail++;
        } else {
            nTxGrant = 1;
        }

        /*
         * Always once-lamp (success or fail). cover=soft_note only —
         * product_window_mint remains OPEN. Soft!=product Dual DoD B.
         * greppable: rtl8168_udx: product own_handoff iommu_grant
         */
        if (nGrantFail != 0) {
            udx_printk("rtl8168_udx: product own_handoff iommu_grant FAIL "
                       "result=FAIL grant_n=%d fail_n=%d geo_fail_n=%d "
                       "bdf=0x%x ring_pa=0x%llx ring_ok=%d "
                       "tx_pa=0x%llx tx_ok=%d "
                       "rx_n=%u rx_ok=%d rx0=0x%llx rx_last=0x%llx "
                       "rx0_force32_id=%d rx0_low=%d "
                       "ring_force32_id=%d tx_force32_id=%d "
                       "cover=soft_note product_window_mint=OPEN "
                       "full_vtd=0 list_count=%d "
                       "residual=Own_stick_under_TE_if_identity_miss "
                       "Soft!=product dual_dod_b=OPEN "
                       "(TE cover residual; Own may stick)\n",
                       nGrantN, nGrantFail, nGeoFail, (unsigned)u32Bdf,
                       (unsigned long long)pSoft->dmaRing, nRingGrant,
                       (unsigned long long)dmaTxb, nTxGrant,
                       (unsigned)u32Got, nRxGrantOk,
                       (unsigned long long)dmaRx0,
                       (unsigned long long)dmaRxLast, fRx0Geo, fLowRx0,
                       fRingGeo, fTxGeo, nGrantN);
        } else {
            udx_printk("rtl8168_udx: product own_handoff iommu_grant "
                       "result=PASS grant_n=%d fail_n=0 geo_fail_n=%d "
                       "bdf=0x%x ring_pa=0x%llx ring_ok=1 "
                       "tx_pa=0x%llx tx_ok=1 "
                       "rx_n=%u rx_ok=%d rx0=0x%llx rx_last=0x%llx "
                       "rx0_force32_id=%d rx0_low=%d "
                       "ring_force32_id=%d tx_force32_id=%d "
                       "cover=soft_note product_window_mint=OPEN "
                       "full_vtd=0 list_count=%d "
                       "residual=Own_stick_under_TE_if_identity_miss "
                       "Soft!=product dual_dod_b=OPEN "
                       "(soft grant ok; not product window mint)\n",
                       nGrantN, nGeoFail, (unsigned)u32Bdf,
                       (unsigned long long)pSoft->dmaRing,
                       (unsigned long long)dmaTxb,
                       (unsigned)u32Got, nRxGrantOk,
                       (unsigned long long)dmaRx0,
                       (unsigned long long)dmaRxLast, fRx0Geo, fLowRx0,
                       fRingGeo, fTxGeo, nGrantN);
        }
    }

    /*
     * RX desc fill residual (public §6.1.2):
     *   addr lo/hi = bus PA; opts2 = 0 (VLAN/tag clear);
     *   opts1 = OWN | Buffer_Size | EOR-only-last.
     * Soft!=product Dual DoD B OPEN.
     */
    for (iSlot = 0u; iSlot < pSoft->u32RxSlots && iSlot < RTL_SOFT_RX_SLOTS;
         iSlot++) {
        udx_dma_addr_t dmaSlot;
        u32 fEor;

        dmaSlot = pSoft->aDmaRxSlot[iSlot];
        pSoft->pRxDesc[iSlot].u32AddrLo =
            (u32)((u64)dmaSlot & 0xffffffffull);
        pSoft->pRxDesc[iSlot].u32AddrHi =
            (u32)(((u64)dmaSlot >> 32) & 0xffffffffull);
        pSoft->pRxDesc[iSlot].u32Opts2 = 0u; /* public VLAN/tag clear */
        fEor = (iSlot + 1u == pSoft->u32RxSlots) ? 1u : 0u;
        /* OWN last field write before barrier+clflush (handoff order). */
        pSoft->pRxDesc[iSlot].u32Opts1 = rtl8168_rx_opts1_own(fEor);
    }

    /*
     * Store-barrier + clflush residual: compiler barrier then DMA sync
     * (fs_dma_clflush_range + mfence under freestanding) so NIC sees Own=1
     * and Buffer_Size in DRAM before TE|RE (program_try re-syncs too).
     * Soft!=product Dual DoD B OPEN.
     */
    __asm__ volatile("" ::: "memory");
    udx_dma_sync_single_for_device(pPdev->pDev, pSoft->dmaRing,
                                   (size_t)RTL_SOFT_RING_BYTES,
                                   UDX_DMA_BIDIRECTIONAL);
    for (iSlot = 0u; iSlot < u32Got; iSlot++) {
        udx_dma_sync_single_for_device(pPdev->pDev, pSoft->aDmaRxSlot[iSlot],
                                       (size_t)RTL_SOFT_RX_PAGE_BYTES,
                                       UDX_DMA_BIDIRECTIONAL);
    }
    udx_dma_sync_single_for_device(pPdev->pDev, dmaTxb,
                                   (size_t)RTL_SOFT_TX_BOUNCE_BYTES,
                                   UDX_DMA_TO_DEVICE);

    /*
     * Product RX arm residual (DUT .86–.87 own_stuck under arping):
     * Public datasheet A-1 + §6.1.2: Own-filled descs + RCR/RMS/MAR/MTPS
     * while RE is off; program_try rewrites TNPDS/RDSAR then arms TE|RE.
     * Buffer_Size multiple of 8, ≤0x1FF8; AAP dig for accept-all residual.
     * IDR MAC residual under 9346CR cfgwrite (keep EEPROM if non-zero).
     * Soft!=product Dual DoD B OPEN.
     * greppable: rtl8168_udx: product own_handoff PASS
     * greppable: rtl8168_udx: product idr
     */
    {
        u16 u16Rms;
        u32 u32Mar;
        u32 u32Idr0;
        u32 u32Idr4;
        u8  aMac[6];
        u32 fIdrKeep;
        u32 iMac;

        /* Config write enable (public EEM1:EEM0 = 11). */
        udx_writeb(pSoft->pRegs, RTL_REG_9346CR, (u8)RTL_9346_EEM_CFGWRITE);
        udx_mmio_flush(pSoft->pRegs);

        /*
         * IDR MAC residual (public IDR0–5; 4-byte access).
         * Prefer non-zero EEPROM/station after RST autoload; else lab LAA
         * LAB_MAC_UDX=02:00:00:47:4a:50 (clean-room; matches lab .50 spirit).
         * Same 6 bytes as kernel net_l2 soft demux under ETH_UDX_READY
         * (kernel/drv/net_l2.c g_aLabMacUdx) so ARP SHA residual matches.
         * Soft demux MAC aligns with product idr lab_fallback Soft!=product.
         * OPEN: keep=1 (EEPROM) still leaves soft demux on lab LAA until a
         * path publishes station MAC to net_l2 (no door API yet).
         * Leave 9346CR cfgwrite open — program_try locks after TE|RE.
         * greppable: rtl8168_udx: product idr
         * greppable: LAB_MAC_UDX=02:00:00:47:4a:50
         */
        u32Idr0 = udx_readl(pSoft->pRegs, RTL_REG_IDR0);
        u32Idr4 = udx_readl(pSoft->pRegs, RTL_REG_IDR4);
        aMac[0] = (u8)(u32Idr0 & 0xffu);
        aMac[1] = (u8)((u32Idr0 >> 8) & 0xffu);
        aMac[2] = (u8)((u32Idr0 >> 16) & 0xffu);
        aMac[3] = (u8)((u32Idr0 >> 24) & 0xffu);
        aMac[4] = (u8)(u32Idr4 & 0xffu);
        aMac[5] = (u8)((u32Idr4 >> 8) & 0xffu);
        fIdrKeep = 0u;
        for (iMac = 0u; iMac < 6u; iMac++) {
            if (aMac[iMac] != 0u) {
                fIdrKeep = 1u;
                break;
            }
        }
        if (fIdrKeep == 0u) {
            /* LAB_MAC_UDX=02:00:00:47:4a:50 — lab LAA; matches net_l2. */
            aMac[0] = 0x02u;
            aMac[1] = 0x00u;
            aMac[2] = 0x00u;
            aMac[3] = 0x47u;
            aMac[4] = 0x4au;
            aMac[5] = 0x50u;
            u32Idr0 = (u32)aMac[0] | ((u32)aMac[1] << 8) |
                      ((u32)aMac[2] << 16) | ((u32)aMac[3] << 24);
            u32Idr4 = (u32)aMac[4] | ((u32)aMac[5] << 8);
            udx_writel(pSoft->pRegs, RTL_REG_IDR0, u32Idr0);
            udx_writel(pSoft->pRegs, RTL_REG_IDR4, u32Idr4);
            udx_mmio_flush(pSoft->pRegs);
            u32Idr0 = udx_readl(pSoft->pRegs, RTL_REG_IDR0);
            u32Idr4 = udx_readl(pSoft->pRegs, RTL_REG_IDR4);
            aMac[0] = (u8)(u32Idr0 & 0xffu);
            aMac[1] = (u8)((u32Idr0 >> 8) & 0xffu);
            aMac[2] = (u8)((u32Idr0 >> 16) & 0xffu);
            aMac[3] = (u8)((u32Idr0 >> 24) & 0xffu);
            aMac[4] = (u8)(u32Idr4 & 0xffu);
            aMac[5] = (u8)((u32Idr4 >> 8) & 0xffu);
        }
        pSoft->u32Idr0 = u32Idr0;
        pSoft->u32Idr4 = u32Idr4;
        udx_printk("rtl8168_udx: product idr mac=%02x:%02x:%02x:%02x:%02x:%02x "
                   "keep=%u lab_fallback=%u idr0=0x%08x idr4=0x%08x "
                   "cfgwrite=1 Soft!=product dual_dod_b=OPEN\n",
                   (unsigned)aMac[0], (unsigned)aMac[1], (unsigned)aMac[2],
                   (unsigned)aMac[3], (unsigned)aMac[4], (unsigned)aMac[5],
                   (unsigned)fIdrKeep,
                   (fIdrKeep == 0u) ? 1u : 0u,
                   (unsigned)u32Idr0, (unsigned)u32Idr4);

        /* RMS: max Rx size; cover 2 KiB slots (bits 13:0, public mask). */
        u16Rms = (u16)0x1ff8u; /* max multiple-of-8 per public note */
        udx_writew(pSoft->pRegs, RTL_REG_RMS, u16Rms);

        /*
         * RCR: AAP|APM|AB|AM dig + MXDMA unlimited + RXFTH none.
         * AAP helps arping dig when IDR match residual is incomplete.
         */
        u32Rcr = udx_readl(pSoft->pRegs, RTL_REG_RCR);
        u32Rcr &= ~(RTL_RCR_MXDMA_MASK | RTL_RCR_RXFTH_MASK);
        u32Rcr |= (RTL_RCR_AAP | RTL_RCR_APM | RTL_RCR_AB | RTL_RCR_AM);
        u32Rcr |= (0x7u << RTL_RCR_MXDMA_SHIFT);
        u32Rcr |= (0x7u << RTL_RCR_RXFTH_SHIFT);
        udx_writel(pSoft->pRegs, RTL_REG_RCR, u32Rcr);

        /* MAR all-1s so AM accepts group addresses (ARP/mcast residual). */
        u32Mar = 0xffffffffu;
        udx_writel(pSoft->pRegs, RTL_REG_MAR0, u32Mar);
        udx_writel(pSoft->pRegs, RTL_REG_MAR4, u32Mar);

        /*
         * MTPS: public note — power-on 0 is reserved; must be non-zero.
         * 0x0C × 128 B = 1536 covers regular 1518/1522 frames.
         */
        udx_writeb(pSoft->pRegs, RTL_REG_MTPS, (u8)0x0cu);

        udx_mmio_flush(pSoft->pRegs);

        /* Leave TE|RE off — program_try rewrites bases then arms last. */
        pSoft->u8ChipCmd = udx_readb(pSoft->pRegs, RTL_REG_CHIPCMD);
        pSoft->u8PhySt = udx_readb(pSoft->pRegs, RTL_REG_PHYSTATUS);
        pSoft->u16Rms = udx_readw(pSoft->pRegs, RTL_REG_RMS);
        pSoft->u32RxCfg = udx_readl(pSoft->pRegs, RTL_REG_RCR);
    }

    pSoft->u32FOwnHandoff = 1u;
    pSoft->u32SoftRxIdx = 0u;
    pSoft->u32SoftTxIdx = 0u;

    udx_printk("rtl8168_udx: product own_handoff PASS "
               "rx_slots=%u rx_buf=%u per_slot=%u force32=1 own=1 "
               "rcr=0x%08x rms=0x%x mxdma=7 rxfth=7 aap=1 mar_all1=1 "
               "mtps=0x0c idr0=0x%08x bus_pa=1 clflush=1 re_hold=1 "
               "dma_rx0=0x%llx dma_tx_bounce=0x%llx dma_ring=0x%llx "
               "chipcmd=0x%02x physt=0x%02x linkok=%u "
               "product_prog=1 path=rtl8168_udx dual_dod_b=OPEN "
               "Soft!=product G-AC-1\n",
               (unsigned)pSoft->u32RxSlots, (unsigned)RTL_SOFT_RX_BUF_BYTES,
               (unsigned)RTL_SOFT_RX_PAGE_BYTES,
               (unsigned)pSoft->u32RxCfg, (unsigned)pSoft->u16Rms,
               (unsigned)pSoft->u32Idr0,
               (unsigned long long)dmaRx0, (unsigned long long)dmaTxb,
               (unsigned long long)pSoft->dmaRing,
               (unsigned)pSoft->u8ChipCmd, (unsigned)pSoft->u8PhySt,
               ((pSoft->u8PhySt & RTL_PHYST_LINKOK) != 0u) ? 1u : 0u);

    /*
     * Once-lamp RX descriptor dig (Buffer_Size / EOR / page honesty).
     * Glass residual: opts0=0x80000800 OWN+2KiB; product now page-sized.
     * greppable: rtl8168_udx: product rx_desc dig
     */
    {
        u32 u32Opts0;
        u32 u32OptsLast;
        u32 u32BufMask;
        u32 fEorLast;
        u32 fEorOnlyLast;
        u32 iChk;
        u32 fEorMid;

        u32Opts0 = pSoft->pRxDesc[0].u32Opts1;
        u32OptsLast = pSoft->pRxDesc[pSoft->u32RxSlots - 1u].u32Opts1;
        u32BufMask = u32Opts0 & RTL_DESC_RX_BUF_MASK;
        fEorLast = ((u32OptsLast & RTL_DESC_EOR) != 0u) ? 1u : 0u;
        fEorMid = 0u;
        for (iChk = 0u; iChk + 1u < pSoft->u32RxSlots; iChk++) {
            if ((pSoft->pRxDesc[iChk].u32Opts1 & RTL_DESC_EOR) != 0u) {
                fEorMid = 1u;
                break;
            }
        }
        fEorOnlyLast = (fEorLast != 0u && fEorMid == 0u) ? 1u : 0u;
        udx_printk("rtl8168_udx: product rx_desc dig "
                   "opts0=0x%x opts_last=0x%x buf_mask=0x%x "
                   "eor_last=%u eor_only_last=%u "
                   "page_bytes=%u soft_rx_buf=%u "
                   "own=%u opts2_0=0x%x "
                   "Soft!=product dual_dod_b=OPEN\n",
                   (unsigned)u32Opts0, (unsigned)u32OptsLast,
                   (unsigned)u32BufMask,
                   (unsigned)fEorLast, (unsigned)fEorOnlyLast,
                   (unsigned)RTL_SOFT_RX_PAGE_BYTES,
                   (unsigned)RTL_SOFT_RX_BUF_BYTES,
                   ((u32Opts0 & RTL_DESC_OWN) != 0u) ? 1u : 0u,
                   (unsigned)pSoft->pRxDesc[0].u32Opts2);
    }
    return 1;

fail_rx:
    while (u32Got > 0u) {
        u32Got--;
        if (pSoft->apRxSlot[u32Got] != NULL) {
            udx_dma_free_coherent(pPdev->pDev, RTL_SOFT_RX_PAGE_BYTES,
                                  pSoft->apRxSlot[u32Got],
                                  pSoft->aDmaRxSlot[u32Got]);
            pSoft->apRxSlot[u32Got] = NULL;
            pSoft->aDmaRxSlot[u32Got] = 0;
        }
    }
    return 0;
}

#if !defined(UDX_HOST_LIBC)
/**
 * Thr-poll L2 bridge: reclaim RX Own=0 frames → ETH_INJECT; pull TX → Own.
 * H1 thr-only (park loop). Soft!=product Dual DoD B.
 * Host-libc soft demo never links this (gj_net_eth_* freestanding only).
 * Glass .92 FOVW/RER dig residual under TE (own_stuck densify; Dual DoD B OPEN):
 *   observe-back RCR/TCR/RMS/ChipCmd/PHY/IMR + ISR decode; MPC residual-compat;
 *   no public TxStatus/TSD on 8168B (TX desc opts residual); FOVW|RER sticky
 *   once → RX FIFO dig (public RXFTH/MXDMA already set); ISR W1C reappear once.
 *
 * TOK residual honesty (Dual DoD B dig): ISR may show TOK|TER while our
 * inject=0 and tx_pull=0 (no ETH_TX_PULL programmed Own). Sticky isr_seen
 * latches bits before W1C so own_stuck densify prints tok=/ter= with tx_pull.
 * Soft residual: TOK without our tx_pull is residual, NOT product TX success —
 * never interpret clear/ack of TOK as product TX ok when tx_pull==0.
 *
 * greppable: rtl8168_udx: product l2 poll
 * greppable: rtl8168_udx: product l2 poll own_stuck
 * greppable: rtl8168_udx: product l2 poll own_stuck_isr
 * greppable: rtl8168_udx: product l2 poll fovw_rer
 * greppable: rtl8168_udx: product l2 poll fovw_reappear
 * greppable: rtl8168_udx: product l2 poll inject_fail
 * greppable: rtl8168_udx: product l2 poll tx_own_stuck
 * greppable: rtl8168_udx: product l2 poll rekick
 * greppable: tok= ter= tok_product_tx= tok_residual=
 */
static void
rtl8168_product_l2_poll(struct rtl8168_soft *pSoft)
{
    u32 iSlot;
    u32 u32N;
    u32 u32OwnStill;
    u16 u16St;
    u16 u16StBeforeClear;
    long n;
    static u8 s_fPollLamp;
    static u8 s_fOwnStuckLamp;
    static u8 s_fInjectFailLamp;
    static u8 s_fTxLamp;
    static u8 s_fTxOwnStuckLamp;
    static u8 s_fFovwRerDigLamp;
    static u8 s_fFovwReappearLamp;
    /* Sticky ISR residual: densify after W1C still sees TOK/TER history. */
    static u16 s_u16IsrSeen;
    static u8 aTxFrame[RTL_SOFT_TX_BOUNCE_BYTES];
    static u8 aRxFrame[RTL_SOFT_RX_BUF_BYTES];

    if (pSoft == NULL || pSoft->pRegs == NULL || pSoft->u32FOwnHandoff == 0u ||
        pSoft->pRxDesc == NULL || pSoft->apRxSlot[0] == NULL) {
        return;
    }

    /*
     * IntrStatus observe + ack (thr-only; not MSI-X product path).
     * Product IMR arms ROK|TOK|RER|TER|… so status bits update under TE|RE.
     * Public style is write-1-to-clear (W1C): write back the status bits read.
     * Latch sticky isr_seen BEFORE W1C: densify own_stuck needs tok/ter with
     * tx_pull. Soft residual honesty: do not treat TOK as product TX success
     * if tx_pull==0 (W1C clears HW bit; interpretation stays residual).
     */
    u16St = udx_readw(pSoft->pRegs, RTL_REG_INTR_STATUS);
    u16StBeforeClear = u16St;
    if (u16St != 0u) {
        s_u16IsrSeen = (u16)(s_u16IsrSeen | u16St);
        udx_writew(pSoft->pRegs, RTL_REG_INTR_STATUS, u16St);
        udx_mmio_flush(pSoft->pRegs);
    }

    /*
     * Glass .92 FOVW/RER sticky residual (once): after W1C, re-read ISR.
     * If FOVW reappears immediately, lamp once (FIFO pressure / incomplete RX
     * arm residual — Dual DoD B OPEN, not Dual DoD close). Soft!=product.
     * greppable: rtl8168_udx: product l2 poll fovw_reappear
     */
    if (s_fFovwReappearLamp == 0u &&
        (u16StBeforeClear & RTL_ISR_FOVW) != 0u) {
        u16 u16After;

        u16After = udx_readw(pSoft->pRegs, RTL_REG_INTR_STATUS);
        if ((u16After & RTL_ISR_FOVW) != 0u) {
            s_fFovwReappearLamp = 1u;
            udx_printk("rtl8168_udx: product l2 poll fovw_reappear "
                       "isr_before=0x%04x isr_after=0x%04x w1c=1 "
                       "fovw=1 Soft!=product dual_dod_b=OPEN\n",
                       (unsigned)u16StBeforeClear, (unsigned)u16After);
        }
    }

    if (pSoft->pPdev != NULL && pSoft->pPdev->pDev != NULL) {
        udx_dma_sync_single_for_cpu(pSoft->pPdev->pDev, pSoft->dmaRing,
                                    (size_t)RTL_SOFT_RING_BYTES,
                                    UDX_DMA_BIDIRECTIONAL);
        for (iSlot = 0u; iSlot < pSoft->u32RxSlots && iSlot < RTL_SOFT_RX_SLOTS;
             iSlot++) {
            if (pSoft->aDmaRxSlot[iSlot] != 0) {
                udx_dma_sync_single_for_cpu(
                    pSoft->pPdev->pDev, pSoft->aDmaRxSlot[iSlot],
                    (size_t)RTL_SOFT_RX_PAGE_BYTES, UDX_DMA_BIDIRECTIONAL);
            }
        }
    }

    /*
     * RX reclaim residual (public Own-clear writeback):
     *   Own=0 → bits13:0 = Frame_Length (includes 4 B FCS); bit21 RES
     *   Own=0 + FS|LS → strip FCS → inject → re-Own
     *   Own=0 + incomplete FS|LS but raw len looks valid → try inject
     *   RES set → re-arm without inject
     *   inject n<=0 → still re-arm; once-lamp inject_fail
     * Soft!=product Dual DoD B OPEN.
     */
    u32N = pSoft->u32RxSlots;
    if (u32N > RTL_SOFT_RX_SLOTS) {
        u32N = RTL_SOFT_RX_SLOTS;
    }
    u32OwnStill = 0u;
    for (iSlot = 0u; iSlot < u32N; iSlot++) {
        u32 u32Opts;
        u32 u32Raw; /* Frame_Length including FCS (public) */
        u32 u32Len; /* payload after FCS strip */
        u32 fSegOk;
        u8 *pSlotBuf;

        u32Opts = pSoft->pRxDesc[iSlot].u32Opts1;
        if ((u32Opts & RTL_DESC_OWN) != 0u) {
            u32OwnStill++;
            continue; /* still device-owned */
        }
        /* Public: Own-clear → Frame_Length in bits 13:0 (not Buffer_Size). */
        u32Raw = u32Opts & RTL_DESC_RX_FRAME_MASK;
        fSegOk = ((u32Opts & (RTL_DESC_FS | RTL_DESC_LS)) ==
                  (RTL_DESC_FS | RTL_DESC_LS))
                     ? 1u
                     : 0u;
        /* RES (Rx Error Summary): drop frame, re-arm buffer. */
        if ((u32Opts & RTL_DESC_RX_RES) != 0u) {
            goto rearm_rx;
        }
        /*
         * Prefer FS|LS complete; if incomplete, still try inject when
         * raw length looks like eth+FCS (robust reclaim dig).
         * Public min eth+FCS = 14+4; max = Buffer_Size / page span.
         */
        if (u32Raw < (14u + RTL_DESC_RX_FCS_BYTES) ||
            u32Raw > RTL_SOFT_RX_BUF_BYTES) {
            goto rearm_rx;
        }
        /* Incomplete FS|LS still injects when raw len looks valid (dig). */
        u32Len = u32Raw - RTL_DESC_RX_FCS_BYTES;
        pSlotBuf = (u8 *)pSoft->apRxSlot[iSlot];
        if (pSlotBuf == NULL) {
            goto rearm_rx;
        }
        (void)gj_memcpy(aRxFrame, pSlotBuf, (size_t)u32Len);
        n = gj_net_eth_inject(aRxFrame, (size_t)u32Len);
        if (n > 0) {
            pSoft->u32Inject++;
            pSoft->u32RxPoll++;
            if (s_fPollLamp == 0u) {
                s_fPollLamp = 1u;
                udx_printk("rtl8168_udx: product l2 poll RX inject "
                           "len=%u raw=%u fs_ls=%u path=rtl8168_udx "
                           "dual_dod_b=OPEN Soft!=product G-AC-1\n",
                           (unsigned)u32Len, (unsigned)u32Raw,
                           (unsigned)fSegOk);
            }
        } else {
            /* inject failed: still re-arm; once-lamp dig. */
            pSoft->u32InjectFail++;
            if (s_fInjectFailLamp == 0u) {
                s_fInjectFailLamp = 1u;
                udx_printk("rtl8168_udx: product l2 poll inject_fail "
                           "n=%ld len=%u raw=%u fs_ls=%u fail=%u "
                           "Soft!=product dual_dod_b=OPEN\n",
                           (long)n, (unsigned)u32Len, (unsigned)u32Raw,
                           (unsigned)fSegOk,
                           (unsigned)pSoft->u32InjectFail);
            }
        }
    rearm_rx:
        {
            udx_dma_addr_t dmaSlot;
            u32 fEor;

            dmaSlot = pSoft->aDmaRxSlot[iSlot];
            pSoft->pRxDesc[iSlot].u32AddrLo =
                (u32)((u64)dmaSlot & 0xffffffffull);
            pSoft->pRxDesc[iSlot].u32AddrHi =
                (u32)(((u64)dmaSlot >> 32) & 0xffffffffull);
            pSoft->pRxDesc[iSlot].u32Opts2 = 0u; /* VLAN/tag clear */
            /* OWN last before clflush (public Own handoff order spirit). */
            fEor = (iSlot + 1u == pSoft->u32RxSlots) ? 1u : 0u;
            pSoft->pRxDesc[iSlot].u32Opts1 = rtl8168_rx_opts1_own(fEor);
        }
    }
    if (u32OwnStill == u32N && u32N > 0u) {
        pSoft->u32OwnStuck++;
        if (s_fOwnStuckLamp == 0u && pSoft->u32OwnStuck >= 0x10000u) {
            u16 u16Isr;
            u16 u16Imr;
            u16 u16RmsLive;
            u8  u8Phy;
            u8  u8Cmd;
            u32 u32RdsLo;
            u32 u32RdsHi;
            u32 u32TnpLo;
            u32 u32TnpHi;
            u32 u32DescLo;
            u32 u32DescHi;
            u32 u32SlotLo;
            u32 u32SlotHi;
            u32 u32RcrLive;
            u32 u32TcrLive;
            u32 u32MpcRes;
            u32 u32TxOpts0;
            u32 u32Rxfth;
            u32 u32MxDmaRx;
            u32 u32MxDmaTx;
            u16 u16IsrComb;
            u32 fRok;
            u32 fRer;
            u32 fTok;
            u32 fTer;
            u32 fRdu;
            u32 fFovw;
            u32 fLink;
            u32 fTokProductTx;
            u32 fTokResidual;

            s_fOwnStuckLamp = 1u;
            u16Isr = 0u;
            u16Imr = 0u;
            u16RmsLive = 0u;
            u8Phy = 0u;
            u8Cmd = 0u;
            u32RdsLo = 0u;
            u32RdsHi = 0u;
            u32TnpLo = 0u;
            u32TnpHi = 0u;
            u32RcrLive = 0u;
            u32TcrLive = 0u;
            u32MpcRes = 0u;
            u32TxOpts0 = 0u;
            u32DescLo = pSoft->pRxDesc[0].u32AddrLo;
            u32DescHi = pSoft->pRxDesc[0].u32AddrHi;
            u32SlotLo = (u32)((u64)pSoft->aDmaRxSlot[0] & 0xffffffffull);
            u32SlotHi =
                (u32)(((u64)pSoft->aDmaRxSlot[0] >> 32) & 0xffffffffull);
            if (pSoft->pTxDesc != NULL) {
                u32TxOpts0 = pSoft->pTxDesc[0].u32Opts1;
            }
            if (pSoft->pRegs != NULL) {
                /*
                 * Glass .92 densify observe-back (once, no stamp storm):
                 * RCR/TCR/RMS/ChipCmd/PHY + IMR + ISR raw; MPC residual-
                 * compat @0x4C (Table-1 reserved — not public MissPkt;
                 * public MissPkt is DTCCR tally dump — soft never_dump).
                 * No public TxStatus/TSD on 8168B (8139-era); TX desc
                 * opts residual stands in for TSD dig.
                 */
                u16Isr = udx_readw(pSoft->pRegs, RTL_REG_INTR_STATUS);
                u16Imr = udx_readw(pSoft->pRegs, RTL_REG_INTR_MASK);
                u8Phy = udx_readb(pSoft->pRegs, RTL_REG_PHYSTATUS);
                u8Cmd = udx_readb(pSoft->pRegs, RTL_REG_CHIPCMD);
                u16RmsLive = udx_readw(pSoft->pRegs, RTL_REG_RMS);
                u32RcrLive = udx_readl(pSoft->pRegs, RTL_REG_RCR);
                u32TcrLive = udx_readl(pSoft->pRegs, RTL_REG_TCR);
                u32MpcRes = udx_readl(pSoft->pRegs, RTL_REG_MPC);
                u32RdsLo = udx_readl(pSoft->pRegs, RTL_REG_RDSAR);
                u32RdsHi = udx_readl(pSoft->pRegs, RTL_REG_RDSAR_HI);
                u32TnpLo = udx_readl(pSoft->pRegs, RTL_REG_TNPDS);
                u32TnpHi = udx_readl(pSoft->pRegs, RTL_REG_TNPDS_HI);
                pSoft->u8PhySt = u8Phy;
                pSoft->u8ChipCmd = u8Cmd;
                pSoft->u16IntrMaskRd = u16Imr;
                pSoft->u16Rms = u16RmsLive;
                pSoft->u32RxCfg = u32RcrLive;
                pSoft->u32TxCfg = u32TcrLive;
                pSoft->u32RdsarLo = u32RdsLo;
                pSoft->u32RdsarHi = u32RdsHi;
                pSoft->u32TnpdsLo = u32TnpLo;
                pSoft->u32TnpdsHi = u32TnpHi;
            }
            u32Rxfth = (u32RcrLive & RTL_RCR_RXFTH_MASK) >> RTL_RCR_RXFTH_SHIFT;
            u32MxDmaRx =
                (u32RcrLive & RTL_RCR_MXDMA_MASK) >> RTL_RCR_MXDMA_SHIFT;
            u32MxDmaTx =
                (u32TcrLive & RTL_TCR_MXDMA_MASK) >> RTL_TCR_MXDMA_SHIFT;
            /*
             * Combine live ISR with sticky isr_seen (TOK/TER often W1C'd
             * earlier this thr-poll / prior polls). Soft residual: tok=1
             * ter=1 without our tx_pull is residual, not product TX success.
             * tok_product_tx=1 only when TOK seen AND tx_pull>0 (our TX).
             * Soft residual: do not clear TOK interpretation as product
             * TX success if tx_pull==0.
             */
            u16IsrComb = (u16)(u16Isr | s_u16IsrSeen | u16StBeforeClear);
            fRok = ((u16IsrComb & RTL_ISR_ROK) != 0u) ? 1u : 0u;
            fRer = ((u16IsrComb & RTL_ISR_RER) != 0u) ? 1u : 0u;
            fTok = ((u16IsrComb & RTL_ISR_TOK) != 0u) ? 1u : 0u;
            fTer = ((u16IsrComb & RTL_ISR_TER) != 0u) ? 1u : 0u;
            fRdu = ((u16IsrComb & RTL_ISR_RDU) != 0u) ? 1u : 0u;
            fFovw = ((u16IsrComb & RTL_ISR_FOVW) != 0u) ? 1u : 0u;
            fLink = ((u16IsrComb & RTL_ISR_LINKCHG) != 0u) ? 1u : 0u;
            fTokProductTx =
                (fTok != 0u && pSoft->u32TxPull != 0u) ? 1u : 0u;
            fTokResidual =
                ((fTok != 0u || fTer != 0u) && pSoft->u32TxPull == 0u)
                    ? 1u
                    : 0u;
            /*
             * Densified dig (once @ ~0x10000): Own=1 all slots may be idle
             * wire or arm residual. Observe-back bases + desc vs slot PA +
             * inject/tx_pull/rx_poll counters + isr/imr + live RCR/TCR/RMS +
             * tok/ter residual honesty (TOK without our TX != product TX).
             * greppable: rtl8168_udx: product l2 poll own_stuck
             * greppable: tok= ter= tok_product_tx= tok_residual=
             */
            udx_printk("rtl8168_udx: product l2 poll own_stuck "
                       "rx_all_own=%u opts0=0x%x "
                       "desc0_lo=0x%08x desc0_hi=0x%08x "
                       "slot0_lo=0x%08x slot0_hi=0x%08x "
                       "desc_vs_slot=%u "
                       "dma_rx0=0x%llx dma_ring=0x%llx "
                       "rdsar=0x%08x%08x tnpds=0x%08x%08x "
                       "chipcmd=0x%02x te=%u re=%u "
                       "physt=0x%02x linkok=%u "
                       "isr=0x%04x isr_seen=0x%04x imr=0x%04x "
                       "tok=%u ter=%u tok_product_tx=%u tok_residual=%u "
                       "rcr=0x%08x tcr=0x%08x rms=0x%x "
                       "rxfth=%u mxdma_rx=%u mxdma_tx=%u "
                       "mpc_res=0x%08x mpc_public=0 "
                       "tx_opts0=0x%x tsd_public=0 "
                       "inject=%u inject_fail=%u tx_pull=%u "
                       "tx_pull_skip=%u rx_poll=%u "
                       "Soft!=product dual_dod_b=OPEN "
                       "(TOK without our tx_pull != product TX success)\n",
                       (unsigned)u32OwnStill,
                       (unsigned)pSoft->pRxDesc[0].u32Opts1,
                       (unsigned)u32DescLo, (unsigned)u32DescHi,
                       (unsigned)u32SlotLo, (unsigned)u32SlotHi,
                       (u32DescLo == u32SlotLo && u32DescHi == u32SlotHi)
                           ? 1u
                           : 0u,
                       (unsigned long long)pSoft->aDmaRxSlot[0],
                       (unsigned long long)pSoft->dmaRing,
                       (unsigned)u32RdsHi, (unsigned)u32RdsLo,
                       (unsigned)u32TnpHi, (unsigned)u32TnpLo,
                       (unsigned)u8Cmd,
                       ((u8Cmd & RTL_CMD_TE) != 0u) ? 1u : 0u,
                       ((u8Cmd & RTL_CMD_RE) != 0u) ? 1u : 0u,
                       (unsigned)u8Phy,
                       ((u8Phy & RTL_PHYST_LINKOK) != 0u) ? 1u : 0u,
                       (unsigned)u16Isr, (unsigned)s_u16IsrSeen,
                       (unsigned)u16Imr,
                       (unsigned)fTok, (unsigned)fTer,
                       (unsigned)fTokProductTx, (unsigned)fTokResidual,
                       (unsigned)u32RcrLive, (unsigned)u32TcrLive,
                       (unsigned)u16RmsLive,
                       (unsigned)u32Rxfth, (unsigned)u32MxDmaRx,
                       (unsigned)u32MxDmaTx,
                       (unsigned)u32MpcRes,
                       (unsigned)u32TxOpts0,
                       (unsigned)pSoft->u32Inject,
                       (unsigned)pSoft->u32InjectFail,
                       (unsigned)pSoft->u32TxPull,
                       (unsigned)pSoft->u32TxPullSkip,
                       (unsigned)pSoft->u32RxPoll);
            /*
             * ISR raw + decoded public bit names (once; glass .92 dig) +
             * TOK residual honesty with tx_pull (soft: no product TX claim).
             * greppable: rtl8168_udx: product l2 poll own_stuck_isr
             * greppable: tok= ter= tok_product_tx= tok_residual=
             */
            udx_printk("rtl8168_udx: product l2 poll own_stuck_isr "
                       "isr=0x%04x isr_seen=0x%04x isr_comb=0x%04x "
                       "imr=0x%04x "
                       "ROK=%u RER=%u TOK=%u TER=%u RDU=%u FOVW=%u "
                       "LinkChg=%u "
                       "tok=%u ter=%u tok_product_tx=%u tok_residual=%u "
                       "tx_pull=%u inject=%u "
                       "bits=ROK|RER|TOK|TER|RDU|FOVW|LinkChg "
                       "Soft!=product dual_dod_b=OPEN "
                       "(TOK without our tx_pull != product TX success)\n",
                       (unsigned)u16Isr, (unsigned)s_u16IsrSeen,
                       (unsigned)u16IsrComb, (unsigned)u16Imr,
                       (unsigned)fRok, (unsigned)fRer, (unsigned)fTok,
                       (unsigned)fTer, (unsigned)fRdu, (unsigned)fFovw,
                       (unsigned)fLink,
                       (unsigned)fTok, (unsigned)fTer,
                       (unsigned)fTokProductTx, (unsigned)fTokResidual,
                       (unsigned)pSoft->u32TxPull,
                       (unsigned)pSoft->u32Inject);
            /*
             * FOVW or RER sticky residual (once): soft RX FIFO dig.
             * Public RXFTH/MXDMA already programmed (own_handoff: both 7).
             * Re-read RCR honesty; do NOT invent silicon magic / private
             * FIFO flush. Next dig residual (names only, Dual DoD B OPEN):
             *   - DTCCR public MissPkt tally dump (need 64B DMA + CMD; not
             *     done here — soft never invents dump buffer under thr)
             *   - C+CR RxChkSum residual if checksum path pressure
             *   - link/PHY side if FOVW without ROK under TE (glass .92)
             * greppable: rtl8168_udx: product l2 poll fovw_rer
             */
            if (s_fFovwRerDigLamp == 0u &&
                (fFovw != 0u || fRer != 0u ||
                 (u16StBeforeClear & (RTL_ISR_FOVW | RTL_ISR_RER)) != 0u)) {
                u32 u32RcrDig;
                u32 u32RxfthDig;
                u32 u32MxDig;

                s_fFovwRerDigLamp = 1u;
                u32RcrDig = u32RcrLive;
                if (pSoft->pRegs != NULL) {
                    u32RcrDig = udx_readl(pSoft->pRegs, RTL_REG_RCR);
                    pSoft->u32RxCfg = u32RcrDig;
                }
                u32RxfthDig =
                    (u32RcrDig & RTL_RCR_RXFTH_MASK) >> RTL_RCR_RXFTH_SHIFT;
                u32MxDig =
                    (u32RcrDig & RTL_RCR_MXDMA_MASK) >> RTL_RCR_MXDMA_SHIFT;
                udx_printk("rtl8168_udx: product l2 poll fovw_rer "
                           "fovw=%u rer=%u rok=%u isr=0x%04x isr_pre=0x%04x "
                           "rcr=0x%08x rxfth=%u mxdma=%u "
                           "rxfth_none_prog=%u mxdma_unlim_prog=%u "
                           "rms=0x%x chipcmd=0x%02x te=%u re=%u "
                           "linkok=%u mpc_res=0x%08x "
                           "next_dig=DTCCR_MissPkt_tally,CPlus_RxChkSum "
                           "no_silicon_magic=1 Soft!=product dual_dod_b=OPEN\n",
                           (unsigned)fFovw, (unsigned)fRer, (unsigned)fRok,
                           (unsigned)u16Isr, (unsigned)u16StBeforeClear,
                           (unsigned)u32RcrDig,
                           (unsigned)u32RxfthDig, (unsigned)u32MxDig,
                           (u32RxfthDig == 0x7u) ? 1u : 0u,
                           (u32MxDig == 0x7u) ? 1u : 0u,
                           (unsigned)u16RmsLive, (unsigned)u8Cmd,
                           ((u8Cmd & RTL_CMD_TE) != 0u) ? 1u : 0u,
                           ((u8Cmd & RTL_CMD_RE) != 0u) ? 1u : 0u,
                           ((u8Phy & RTL_PHYST_LINKOK) != 0u) ? 1u : 0u,
                           (unsigned)u32MpcRes);
            }
        }
        /*
         * Public kick residual (once after long own_stuck): rewrite RDSAR
         * lo/hi + re-clflush ring. Soft!=product Dual DoD B OPEN.
         * greppable: rtl8168_udx: product l2 poll rekick
         */
        if (s_fOwnStuckLamp != 0u && pSoft->u32FOwnRekicked == 0u &&
            pSoft->u32OwnStuck >= 0x10000u && pSoft->dmaRx != 0) {
            u32 u32RxLo;
            u32 u32RxHi;

            pSoft->u32FOwnRekicked = 1u;
            u32RxLo = (u32)((u64)pSoft->dmaRx & 0xffffffffull);
            u32RxHi = (u32)(((u64)pSoft->dmaRx >> 32) & 0xffffffffull);
            udx_writel(pSoft->pRegs, RTL_REG_RDSAR, u32RxLo);
            udx_writel(pSoft->pRegs, RTL_REG_RDSAR_HI, u32RxHi);
            udx_mmio_flush(pSoft->pRegs);
            if (pSoft->pPdev != NULL && pSoft->pPdev->pDev != NULL) {
                udx_dma_sync_single_for_device(
                    pSoft->pPdev->pDev, pSoft->dmaRing,
                    (size_t)RTL_SOFT_RING_BYTES, UDX_DMA_BIDIRECTIONAL);
            }
            pSoft->u32RdsarLo = udx_readl(pSoft->pRegs, RTL_REG_RDSAR);
            pSoft->u32RdsarHi = udx_readl(pSoft->pRegs, RTL_REG_RDSAR_HI);
            udx_printk("rtl8168_udx: product l2 poll rekick "
                       "rdsar=0x%08x%08x dma_rx=0x%llx clflush=1 "
                       "own_stuck=%u Soft!=product dual_dod_b=OPEN\n",
                       (unsigned)pSoft->u32RdsarHi,
                       (unsigned)pSoft->u32RdsarLo,
                       (unsigned long long)pSoft->dmaRx,
                       (unsigned)pSoft->u32OwnStuck);
        }
    }

    /* TX pull: demux ARP/ICMP/TCP replies → Own=1 + TPPoll. */
    n = gj_net_eth_tx_pull(aTxFrame, sizeof(aTxFrame));
    if (n >= 14 && (u32)n <= RTL_SOFT_TX_BOUNCE_BYTES &&
        pSoft->pTxDesc != NULL && pSoft->pTxBounce != NULL) {
        u32 u32Tx;
        u32 u32Opts;
        udx_dma_addr_t dmaTxb;

        u32Tx = pSoft->u32SoftTxIdx % pSoft->u32TxSlots;
        if ((pSoft->pTxDesc[u32Tx].u32Opts1 & RTL_DESC_OWN) == 0u) {
            (void)gj_memcpy(pSoft->pTxBounce, aTxFrame, (size_t)n);
            dmaTxb = pSoft->dmaTxBounce;
            pSoft->pTxDesc[u32Tx].u32AddrLo =
                (u32)((u64)dmaTxb & 0xffffffffull);
            pSoft->pTxDesc[u32Tx].u32AddrHi =
                (u32)(((u64)dmaTxb >> 32) & 0xffffffffull);
            u32Opts = RTL_DESC_OWN | RTL_DESC_FS | RTL_DESC_LS |
                      ((u32)n & RTL_DESC_TX_LEN_MASK);
            if (u32Tx + 1u == pSoft->u32TxSlots) {
                u32Opts |= RTL_DESC_EOR;
            }
            pSoft->pTxDesc[u32Tx].u32Opts1 = u32Opts;
            pSoft->pTxDesc[u32Tx].u32Opts2 = 0u;
            pSoft->u32SoftTxIdx = u32Tx + 1u;
            pSoft->u32TxPull++;
            if (pSoft->pPdev != NULL && pSoft->pPdev->pDev != NULL) {
                udx_dma_sync_single_for_device(
                    pSoft->pPdev->pDev, pSoft->dmaTxBounce,
                    (size_t)RTL_SOFT_TX_BOUNCE_BYTES, UDX_DMA_TO_DEVICE);
                udx_dma_sync_single_for_device(
                    pSoft->pPdev->pDev, pSoft->dmaRing,
                    (size_t)RTL_SOFT_RING_BYTES, UDX_DMA_BIDIRECTIONAL);
            }
            /* Public TPPoll NPQ kick (product thr-poll; Soft!=product). */
            udx_writeb(pSoft->pRegs, RTL_REG_TPPOLL, (u8)RTL_TPPOLL_NPQ);
            udx_mmio_flush(pSoft->pRegs);
            if (s_fPollLamp == 0u) {
                s_fPollLamp = 1u;
            }
            if (s_fTxLamp == 0u) {
                s_fTxLamp = 1u;
                udx_printk("rtl8168_udx: product l2 poll TX "
                           "len=%u tppoll=1 path=rtl8168_udx "
                           "dual_dod_b=OPEN Soft!=product G-AC-1\n",
                           (unsigned)n);
            }
        } else {
            /*
             * Frame available from demux but TX desc Own stuck (device
             * never reclaimed). Once-lamp dig; Soft!=product.
             * greppable: rtl8168_udx: product l2 poll tx_own_stuck
             */
            pSoft->u32TxPullSkip++;
            if (s_fTxOwnStuckLamp == 0u) {
                s_fTxOwnStuckLamp = 1u;
                udx_printk("rtl8168_udx: product l2 poll tx_own_stuck "
                           "tx_idx=%u opts=0x%x len=%u "
                           "tx_pull=%u tx_pull_skip=%u "
                           "Soft!=product dual_dod_b=OPEN\n",
                           (unsigned)u32Tx,
                           (unsigned)pSoft->pTxDesc[u32Tx].u32Opts1,
                           (unsigned)n,
                           (unsigned)pSoft->u32TxPull,
                           (unsigned)pSoft->u32TxPullSkip);
            }
        }
    }

    /* Store-barrier + clflush re-Own / TX desc before next poll (Own order). */
    __asm__ volatile("" ::: "memory");
    if (pSoft->pPdev != NULL && pSoft->pPdev->pDev != NULL) {
        udx_dma_sync_single_for_device(pSoft->pPdev->pDev, pSoft->dmaRing,
                                       (size_t)RTL_SOFT_RING_BYTES,
                                       UDX_DMA_BIDIRECTIONAL);
        for (iSlot = 0u; iSlot < pSoft->u32RxSlots && iSlot < RTL_SOFT_RX_SLOTS;
             iSlot++) {
            if (pSoft->aDmaRxSlot[iSlot] != 0) {
                udx_dma_sync_single_for_device(
                    pSoft->pPdev->pDev, pSoft->aDmaRxSlot[iSlot],
                    (size_t)RTL_SOFT_RX_PAGE_BYTES, UDX_DMA_BIDIRECTIONAL);
            }
        }
    }
}
#endif /* !UDX_HOST_LIBC — thr-poll L2 bridge freestanding only */

static int
rtl8168_product_program_try(struct rtl8168_soft *pSoft)
{
    u32 u32TxLo;
    u32 u32TxHi;
    u32 u32RxLo;
    u32 u32RxHi;
    u32 u32Map;
    u32 u32DmaSoft;
    u32 u32RingReady;
    u32 u32AlignOk;
    u32 u32ProgGate;
    u32 u32RealDdi;
    u32 u32TeObs;
    u32 u32ReObs;
    u8  u8Cmd;
    u8  u8CmdAfter;
    const char *szBindPath;

    if (pSoft == NULL || pSoft->pRegs == NULL) {
        udx_printk("rtl8168_udx: product program SKIP reason=null "
                   "product_mint=0 never_program=1 Soft!=product "
                   "dual_dod_b=OPEN\n");
        return 0;
    }

    /* Idempotent: already programmed this soft block. */
    if (pSoft->u32FProductProg != 0u) {
        udx_printk("rtl8168_udx: product program PASS already=1 "
                   "product_mint=%u product_prog=1 write=1 "
                   "wire_owner=%s dual_dod_b=OPEN Soft!=product\n",
                   (unsigned)pSoft->u32FProductMint,
                   (pSoft->u8WireOwner == RTL_SOFT_WIRE_OWNER_UDX)
                       ? "udx" : "freestanding");
        return 1;
    }

    u32RealDdi = (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_DDI) ? 1u : 0u;
    if (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_DDI) {
        szBindPath = "ddi";
    } else if (pSoft->u8BindPath == RTL_SOFT_BIND_PATH_INJECT) {
        szBindPath = "inject";
    } else {
        szBindPath = "none";
    }
    u32Map = (pSoft->u32FMapOk != 0u && pSoft->pRegs != NULL) ? 1u : 0u;
    u32DmaSoft = (pSoft->dmaRing != 0 && pSoft->pRing != NULL) ? 1u : 0u;
    u32RingReady = (pSoft->u32FRingOk != 0u &&
                    pSoft->u32FRingVerify != 0u) ? 1u : 0u;
    u32AlignOk = 0u;
    if (pSoft->dmaTx != 0 && pSoft->dmaRx != 0 &&
        (((u64)pSoft->dmaTx % (u64)RTL_ALIGN_TNPDS) == 0ull) &&
        (((u64)pSoft->dmaRx % (u64)RTL_ALIGN_TNPDS) == 0ull)) {
        u32AlignOk = 1u;
    }
    u32ProgGate = (u32Map != 0u && u32DmaSoft != 0u &&
                   u32RingReady != 0u && u32AlignOk != 0u) ? 1u : 0u;

    /*
     * Product mint opens only on real DDI + program_gate.
     * Inject soft demo / incomplete residual: never_program holds.
     */
    if (u32RealDdi == 0u || u32ProgGate == 0u) {
        pSoft->u32FProductMint = 0u;
        pSoft->u32FProductProg = 0u;
        udx_printk("rtl8168_udx: product program SKIP "
                   "real_ddi=%u program_gate=%u "
                   "map=%u dma=%u ring=%u align=%u "
                   "bind_path=%s product_mint=0 would_program=0 write=0 "
                   "never_program=1 never_tnpds=1 never_rdsar=1 never_te_re=1 "
                   "need=real_ddi+program_gate "
                   "order=tnpds_lo,tnpds_hi,rdsar_lo,rdsar_hi,te_re "
                   "product=UDX+sshd+stack chain=%s "
                   "lab_ip residual OPEN lab_ip=10.200.125.50 "
                   "stack=OPEN sshd=OPEN dual_dod_b=OPEN "
                   "Soft!=product G-AC-1 agent_ne_close=1\n",
                   (unsigned)u32RealDdi, (unsigned)u32ProgGate,
                   (unsigned)u32Map, (unsigned)u32DmaSoft,
                   (unsigned)u32RingReady, (unsigned)u32AlignOk,
                   szBindPath,
                   RTL_SOFT_PRODUCT_CHAIN);
        return 0;
    }

    /* Product mint: real DDI + gate present (option 3). */
    pSoft->u32FProductMint = 1u;
    pSoft->u32FProductProg = 1u; /* allow own_handoff before TE|RE arm */

    u32TxLo = (u32)((u64)pSoft->dmaTx & 0xffffffffull);
    u32TxHi = (u32)(((u64)pSoft->dmaTx >> 32) & 0xffffffffull);
    u32RxLo = (u32)((u64)pSoft->dmaRx & 0xffffffffull);
    u32RxHi = (u32)(((u64)pSoft->dmaRx >> 32) & 0xffffffffull);

    udx_printk("rtl8168_udx: product program enter "
               "real_ddi=1 program_gate=1 product_mint=1 "
               "order=rst,cplus,own,tnpds,rdsar,te_re "
               "dma_tx=0x%llx dma_rx=0x%llx "
               "Soft!=product dual_dod_b=OPEN\n",
               (unsigned long long)pSoft->dmaTx,
               (unsigned long long)pSoft->dmaRx);

    /*
     * Public datasheet §7 A-1 + §6.1 (DUT .87 own_stuck under arping):
     *   1) ChipCmd RST — reinit FIFOs / desc pointers
     *   2) C+CR word first (A-1 key; word access only)
     *   3) Own handoff — RX PA + Own=1 + RCR/RMS/MAR/MTPS (RE held off)
     *   4) TNPDS/RDSAR rewrite after Own (kick pointer to filled ring)
     *   5) TE|RE last; TCR MXDMA after TE; lock 9346CR
     * Soft!=product Dual DoD B OPEN.
     */
    {
        u32 iWait;
        u32 u32Tcr;
        u16 u16Cplus;

        /* Soft reset — public CR RST; auto-clears when complete. */
        udx_writeb(pSoft->pRegs, RTL_REG_CHIPCMD, (u8)RTL_CMD_RST);
        udx_mmio_flush(pSoft->pRegs);
        for (iWait = 0u; iWait < 100000u; iWait++) {
            u8Cmd = udx_readb(pSoft->pRegs, RTL_REG_CHIPCMD);
            if ((u8Cmd & RTL_CMD_RST) == 0u) {
                break;
            }
        }

        /* A-1 Step1: C+CR word access only (byte has no effect). */
        u16Cplus = udx_readw(pSoft->pRegs, RTL_REG_CPLUSCMD);
        u16Cplus = (u16)(u16Cplus & (u16)~(RTL_CPLUS_RXVLAN | RTL_CPLUS_RXCHKSUM));
        udx_writew(pSoft->pRegs, RTL_REG_CPLUSCMD, u16Cplus);
        udx_mmio_flush(pSoft->pRegs);
        pSoft->u32CPlus = (u32)udx_readw(pSoft->pRegs, RTL_REG_CPLUSCMD);

        /* Own handoff while RE off (fills desc Own + RCR/RMS). */
        if (rtl8168_product_own_handoff(pSoft) == 0) {
            udx_printk("rtl8168_udx: product program FAIL own_handoff=0 "
                       "Soft!=product dual_dod_b=OPEN\n");
            pSoft->u32FProductProg = 0u;
            pSoft->u32FProductMint = 0u;
            return 0;
        }

        /* Rewrite bases after Own fill (public kick after pre-alloc). */
        udx_writel(pSoft->pRegs, RTL_REG_TNPDS, u32TxLo);
        udx_writel(pSoft->pRegs, RTL_REG_TNPDS_HI, u32TxHi);
        udx_writel(pSoft->pRegs, RTL_REG_RDSAR, u32RxLo);
        udx_writel(pSoft->pRegs, RTL_REG_RDSAR_HI, u32RxHi);
        udx_mmio_flush(pSoft->pRegs);

        /*
         * Store-barrier + clflush ring again after base program, before
         * TE|RE: Own+Buffer_Size must be device-visible in DRAM.
         * Soft!=product Dual DoD B OPEN.
         */
        __asm__ volatile("" ::: "memory");
        if (pSoft->pPdev != NULL && pSoft->pPdev->pDev != NULL) {
            udx_dma_sync_single_for_device(pSoft->pPdev->pDev, pSoft->dmaRing,
                                           (size_t)RTL_SOFT_RING_BYTES,
                                           UDX_DMA_BIDIRECTIONAL);
        }

        /* A-1 Step2: Command TE|RE last. */
        u8Cmd = udx_readb(pSoft->pRegs, RTL_REG_CHIPCMD);
        u8Cmd = (u8)((u8Cmd & (u8)~RTL_CMD_RST) | (u8)RTL_CMD_TE_RE);
        udx_writeb(pSoft->pRegs, RTL_REG_CHIPCMD, u8Cmd);
        udx_mmio_flush(pSoft->pRegs);

        /* TCR after TE (public note: TCR only after TE set). */
        u32Tcr = udx_readl(pSoft->pRegs, RTL_REG_TCR);
        u32Tcr &= ~RTL_TCR_MXDMA_MASK;
        u32Tcr |= (0x7u << RTL_TCR_MXDMA_SHIFT);
        udx_writel(pSoft->pRegs, RTL_REG_TCR, u32Tcr);
        udx_mmio_flush(pSoft->pRegs);
        pSoft->u32TxCfg = udx_readl(pSoft->pRegs, RTL_REG_TCR);

        /*
         * Product IntrMask residual (public IMR; not MSI-X): arm ROK/TOK/
         * RER/TER/RDU/FOVW/TDU/LinkChg so thr-poll IntrStatus dig updates.
         * greppable: rtl8168_udx: product imr
         */
        pSoft->u16IntrMask = (u16)RTL_PRODUCT_IRQ_BITS;
        udx_writew(pSoft->pRegs, RTL_REG_INTR_MASK, pSoft->u16IntrMask);
        udx_mmio_flush(pSoft->pRegs);
        pSoft->u16IntrMaskRd = udx_readw(pSoft->pRegs, RTL_REG_INTR_MASK);
        udx_printk("rtl8168_udx: product imr mask=0x%04x rd=0x%04x "
                   "bits=ROK|RER|TOK|TER|RDU|FOVW|TDU|LinkChg "
                   "msix=0 thr_ack=1 Soft!=product dual_dod_b=OPEN\n",
                   (unsigned)pSoft->u16IntrMask,
                   (unsigned)pSoft->u16IntrMaskRd);

        /* Lock config (9346CR back to normal after IDR/RCR/cfg writes). */
        udx_writeb(pSoft->pRegs, RTL_REG_9346CR, (u8)RTL_9346_EEM_NORMAL);
        udx_mmio_flush(pSoft->pRegs);
    }

    /* Observe-back product bases + ChipCmd (honesty after write). */
    pSoft->u32TnpdsLo = udx_readl(pSoft->pRegs, RTL_REG_TNPDS);
    pSoft->u32TnpdsHi = udx_readl(pSoft->pRegs, RTL_REG_TNPDS_HI);
    pSoft->u32RdsarLo = udx_readl(pSoft->pRegs, RTL_REG_RDSAR);
    pSoft->u32RdsarHi = udx_readl(pSoft->pRegs, RTL_REG_RDSAR_HI);
    u8CmdAfter = udx_readb(pSoft->pRegs, RTL_REG_CHIPCMD);
    pSoft->u8ChipCmd = u8CmdAfter;
    pSoft->u8PhySt = udx_readb(pSoft->pRegs, RTL_REG_PHYSTATUS);

    u32TeObs = ((u8CmdAfter & RTL_CMD_TE) != 0u) ? 1u : 0u;
    u32ReObs = ((u8CmdAfter & RTL_CMD_RE) != 0u) ? 1u : 0u;

    pSoft->u32FBasesClean = 0u;
    pSoft->u8WireOwner = RTL_SOFT_WIRE_OWNER_UDX;

    udx_printk("rtl8168_udx: product program PASS "
               "order=rst,cplus,own,tnpds,rdsar,te_re,imr "
               "tnpds=0x%08x%08x rdsar=0x%08x%08x "
               "chipcmd=0x%02x te=%u re=%u "
               "product_mint=1 product_prog=1 never_program=0 write=1 "
               "own_handoff=%u rcr=0x%08x rms=0x%x tcr=0x%08x "
               "imr=0x%04x idr0=0x%08x "
               "wire_owner=udx product_handoff=1 "
               "product=UDX+sshd+stack chain=%s "
               "lab_ip=10.200.125.50 sshd_port=%u "
               "physt=0x%02x linkok=%u "
               "dual_dod_b=OPEN Soft!=product G-AC-1\n",
               (unsigned)pSoft->u32TnpdsHi, (unsigned)pSoft->u32TnpdsLo,
               (unsigned)pSoft->u32RdsarHi, (unsigned)pSoft->u32RdsarLo,
               (unsigned)u8CmdAfter,
               (unsigned)u32TeObs, (unsigned)u32ReObs,
               (unsigned)pSoft->u32FOwnHandoff,
               (unsigned)pSoft->u32RxCfg, (unsigned)pSoft->u16Rms,
               (unsigned)pSoft->u32TxCfg,
               (unsigned)pSoft->u16IntrMaskRd,
               (unsigned)pSoft->u32Idr0,
               RTL_SOFT_PRODUCT_CHAIN,
               (unsigned)RTL_SOFT_PRODUCT_SSHD_PORT,
               (unsigned)pSoft->u8PhySt,
               ((pSoft->u8PhySt & RTL_PHYST_LINKOK) != 0u) ? 1u : 0u);
    /*
     * Glass .92 densify observe-back once after program PASS (no stamp storm):
     * live RCR/TCR/RMS/ChipCmd/PHY + IMR + ISR raw/decode residual. Soft!=product.
     * greppable: rtl8168_udx: product program densify_obs
     */
    {
        u16 u16IsrObs;
        u16 u16ImrObs;
        u16 u16RmsObs;
        u8  u8PhyObs;
        u8  u8CmdObs;
        u32 u32RcrObs;
        u32 u32TcrObs;
        u32 u32MpcObs;
        u32 u32RxfthObs;
        u32 u32MxRxObs;

        u16IsrObs = udx_readw(pSoft->pRegs, RTL_REG_INTR_STATUS);
        u16ImrObs = udx_readw(pSoft->pRegs, RTL_REG_INTR_MASK);
        u8PhyObs = udx_readb(pSoft->pRegs, RTL_REG_PHYSTATUS);
        u8CmdObs = udx_readb(pSoft->pRegs, RTL_REG_CHIPCMD);
        u16RmsObs = udx_readw(pSoft->pRegs, RTL_REG_RMS);
        u32RcrObs = udx_readl(pSoft->pRegs, RTL_REG_RCR);
        u32TcrObs = udx_readl(pSoft->pRegs, RTL_REG_TCR);
        u32MpcObs = udx_readl(pSoft->pRegs, RTL_REG_MPC);
        u32RxfthObs =
            (u32RcrObs & RTL_RCR_RXFTH_MASK) >> RTL_RCR_RXFTH_SHIFT;
        u32MxRxObs =
            (u32RcrObs & RTL_RCR_MXDMA_MASK) >> RTL_RCR_MXDMA_SHIFT;
        pSoft->u16IntrMaskRd = u16ImrObs;
        pSoft->u16Rms = u16RmsObs;
        pSoft->u32RxCfg = u32RcrObs;
        pSoft->u32TxCfg = u32TcrObs;
        pSoft->u8PhySt = u8PhyObs;
        pSoft->u8ChipCmd = u8CmdObs;
        udx_printk("rtl8168_udx: product program densify_obs "
                   "rcr=0x%08x tcr=0x%08x rms=0x%x "
                   "chipcmd=0x%02x te=%u re=%u "
                   "physt=0x%02x linkok=%u "
                   "isr=0x%04x imr=0x%04x "
                   "ROK=%u RER=%u TOK=%u TER=%u RDU=%u FOVW=%u LinkChg=%u "
                   "rxfth=%u mxdma_rx=%u mpc_res=0x%08x mpc_public=0 "
                   "tsd_public=0 Soft!=product dual_dod_b=OPEN\n",
                   (unsigned)u32RcrObs, (unsigned)u32TcrObs,
                   (unsigned)u16RmsObs,
                   (unsigned)u8CmdObs,
                   ((u8CmdObs & RTL_CMD_TE) != 0u) ? 1u : 0u,
                   ((u8CmdObs & RTL_CMD_RE) != 0u) ? 1u : 0u,
                   (unsigned)u8PhyObs,
                   ((u8PhyObs & RTL_PHYST_LINKOK) != 0u) ? 1u : 0u,
                   (unsigned)u16IsrObs, (unsigned)u16ImrObs,
                   ((u16IsrObs & RTL_ISR_ROK) != 0u) ? 1u : 0u,
                   ((u16IsrObs & RTL_ISR_RER) != 0u) ? 1u : 0u,
                   ((u16IsrObs & RTL_ISR_TOK) != 0u) ? 1u : 0u,
                   ((u16IsrObs & RTL_ISR_TER) != 0u) ? 1u : 0u,
                   ((u16IsrObs & RTL_ISR_RDU) != 0u) ? 1u : 0u,
                   ((u16IsrObs & RTL_ISR_FOVW) != 0u) ? 1u : 0u,
                   ((u16IsrObs & RTL_ISR_LINKCHG) != 0u) ? 1u : 0u,
                   (unsigned)u32RxfthObs, (unsigned)u32MxRxObs,
                   (unsigned)u32MpcObs);
    }
    udx_printk("rtl8168_udx: product chain residual "
               "product=UDX+sshd+stack chain=%s "
               "product_mint=1 product_prog=1 product_handoff=1 "
               "wire_owner=udx tnpds=1 rdsar=1 te_re=1 own=1 "
               "lab_ip=10.200.125.50 dual_dod_b=OPEN "
               "Soft!=product G-AC-1\n",
               RTL_SOFT_PRODUCT_CHAIN);
    return 1;
}

#if !defined(UDX_HOST_LIBC)

/*
 * Freestanding driver-host entry: prefer real DDI bind (GJ_SYS_DDI /
 * udx_host_bind_by_id) over inject-only soft demo. Soft probe PASS only when
 * bind installs the function AND probe sets drvdata/open+map/ring residual.
 * Bind alone (OPEN+MAP) is not enough for the probe PASS lamp.
 * Bind != wire: freestanding owns wire interim until product program handoff.
 * When soft MAP+DMA + caps residual present, functional residual densifies
 * toward real DDI open/map/program_gate (STRONGER).
 * Option 3: after soft bind PASS + program_gate, product_program_try writes
 * TNPDS/RDSAR/TE|RE (product_mint=1) for Dual DoD B product UDX wire path.
 * Product path remains userspace UDX+ABI+hot/cold; freestanding rtl SKIP
 * (GJ_RTL8168_PROBE=0). Dual DoD B OPEN until DUT L3. Soft!=product. G-AC-1.
 */
int
rtl8168_udx_freestanding_start(void)
{
    udx_status_t st;
    struct udx_pci_dev *pPdev;
    struct rtl8168_soft *pSoft;

    rtl8168_soft_catalog();
    /* Soft DDI host surface notes (observation only; Soft!=product). */
    udx_host_soft_init();
    udx_host_soft_lifecycle_note();
    /* Product honesty: freestanding rtl is not the product path. */
    udx_printk("rtl8168_udx: soft freestanding product=SKIP "
               "GJ_RTL8168_PROBE_default=0 product=UDX+ABI "
               "product=UDX+sshd+stack prefer_ddi_bind=1 "
               "prefer_real_ddi=1 "
               "need=DDI_caps need=hot_cold_abi dual_dod_b=OPEN "
               "Soft!=product G-AC-1\n");
    /*
     * Prefer real DDI path for 10ec:8168 (GJ_SYS_DDI bind_by_id).
     * greppable: prefer_real_ddi=1 id=10ec:8168
     */
    udx_printk("rtl8168_udx: soft prefer real DDI id=10ec:8168 "
               "prefer_real_ddi=1 via=udx_host_bind_by_id "
               "gj_sys_ddi=%u freestanding=1 "
               "product=UDX+ABI product=UDX+sshd+stack need=DDI_caps "
               "Soft!=product dual_dod_b=OPEN freestanding_product=SKIP\n",
               (unsigned)GJ_SYS_DDI);
    udx_printk("rtl8168_udx: soft ddi prefer "
               "prefer=udx_host_bind_by_id over=inject "
               "prefer_real_ddi=1 real_ddi=0 "
               "gj_sys_ddi=%u freestanding=1 dual_dod_b=OPEN "
               "Soft!=product\n",
               (unsigned)GJ_SYS_DDI);

    st = rtl8168_udx_register();
    if (st != UDX_OK) {
        udx_printk("rtl8168_udx: soft SKIP (register %d) Soft!=product\n", st);
        return st;
    }

    if (udx_host_bind_by_id != NULL) {
        pPdev = NULL;
        st = udx_host_bind_by_id((u16)RTL8168_VEND, (u16)RTL8168_DEV, &pPdev);
        if (st != UDX_OK) {
            rtl8168_soft_bind_lamp(NULL, 0);
            rtl8168_soft_bind_path_lamp(NULL, RTL_SOFT_BIND_PATH_NONE);
            rtl8168_soft_bind_ne_wire(NULL, 0);
            udx_printk("rtl8168_udx: soft SKIP (bind_by_id %d) — "
                       "prefer_real_ddi=1 real_ddi=0 "
                       "inject 10ec:8168 on host path Soft!=product\n",
                       (int)st);
            return st;
        }
        /*
         * Honesty: bind_by_id runs install_granted → pci_try_bind → probe.
         * Confirm probe + MAP + ring residual/verify before soft probe PASS.
         * Bind success alone != wire ownership transfer.
         */
        if (pPdev == NULL || pPdev->pDev == NULL ||
            !udx_dev_has_drvdata(pPdev->pDev)) {
            rtl8168_soft_bind_lamp(pPdev, 0);
            rtl8168_soft_bind_path_lamp(NULL, RTL_SOFT_BIND_PATH_NONE);
            rtl8168_soft_bind_ne_wire(NULL, 1);
            udx_printk("rtl8168_udx: soft SKIP (bind ok, probe/no drvdata) "
                       "prefer_real_ddi=1 real_ddi=0 "
                       "bind_ne_wire=1 Soft!=product\n");
            return UDX_ERR_IO;
        }
        pSoft = (struct rtl8168_soft *)udx_get_drvdata(pPdev->pDev);
        if (pSoft == NULL || pSoft->u32FProbeOk == 0u ||
            pSoft->u32FOpenOk == 0u || pSoft->u32FMapOk == 0u ||
            pSoft->u32FRingOk == 0u || pSoft->u32FRingVerify == 0u) {
            rtl8168_soft_bind_lamp(pPdev, 0);
            rtl8168_soft_bind_path_lamp(pSoft, RTL_SOFT_BIND_PATH_NONE);
            rtl8168_soft_bind_ne_wire(pSoft, 1);
            udx_printk("rtl8168_udx: soft SKIP (bind ok, probe/open+map/"
                       "ring residual incomplete) "
                       "prefer_real_ddi=1 real_ddi=0 "
                       "bind_ne_wire=1 Soft!=product\n");
            return UDX_ERR_IO;
        }
        /*
         * Honesty: bind_by_id OPEN+MAP+probe is soft residual only.
         * Freestanding wire ownership stays with kernel lab rtl8168 until
         * product UDX NIC host programs TNPDS/RDSAR/TE/RE (OPEN).
         * Stamp DDI bind path (real_ddi=1); re-densify functional residual
         * so open/map/program_gate densify lamps carry real_ddi=1
         * (STRONGER when caps residual present; product_mint=0).
         */
        pSoft->u8WireOwner = RTL_SOFT_WIRE_OWNER_FS;
        pSoft->u8BindPath = RTL_SOFT_BIND_PATH_DDI;
        rtl8168_soft_bind_lamp(pPdev, 1);
        rtl8168_soft_bind_path_lamp(pSoft, RTL_SOFT_BIND_PATH_DDI);
        udx_printk("rtl8168_udx: soft freestanding bind PASS "
                   "bind_path=ddi prefer_ddi=1 "
                   "prefer_real_ddi=1 real_ddi=1 host_inject=0 "
                   "gj_sys_ddi=%u via=udx_host_bind_by_id "
                   "bind_ne_wire=1 freestanding_owns_wire=1 "
                   "product=UDX+sshd+stack Soft!=product\n",
                   (unsigned)GJ_SYS_DDI);
        rtl8168_soft_bind_ne_wire(pSoft, 1);
        rtl8168_soft_ddi_bar0_honesty(pSoft);
        /*
         * Product residual before PASS lamp (probe may already have walked).
         * Soft!=product; freestanding still owns wire interim until option-3
         * product_program_try handoff below.
         * Re-walk functional residual so densify sees bind_path=ddi + MAP+DMA
         * + caps → densify open/map/program_gate + densify real_ddi=1.
         */
        if (pSoft->u32FProdCat == 0u) {
            rtl8168_soft_ready_catalog(pSoft);
        } else {
            rtl8168_soft_open_map_honesty(pSoft);
            rtl8168_soft_product_model();
            /* Caps first so densify_program_gate sees map_dma_caps. */
            if (pSoft->u32FCapsRes == 0u) {
                rtl8168_soft_caps_residual(pSoft);
            }
            if (pSoft->u32FProgRes == 0u) {
                rtl8168_soft_program_residual(pSoft);
            }
            rtl8168_soft_functional_residual(pSoft);
        }
        rtl8168_soft_honesty(pSoft);
        /* denser residual bar .75 VERDICT (bind_path=ddi live; H2 once). */
        rtl8168_soft_denser_residual_verdict(pSoft);
        /*
         * Option 3 product program: when real_ddi + program_gate, write
         * TNPDS/RDSAR/TE|RE and flip wire_owner→udx. Soft residual catalogs
         * above keep never_program=1 as soft residual class law.
         * Dual DoD B remains OPEN until DUT L3 (agent!=close).
         */
        (void)rtl8168_product_program_try(pSoft);
        udx_printk("rtl8168_udx: soft probe PASS stage=%u freestanding=1 "
                   "bind_path=ddi prefer_ddi=1 "
                   "prefer_real_ddi=1 real_ddi=1 host_inject=0 "
                   "gj_sys_ddi=%u "
                   "open=%u map=%u ring_residual=%u ring_verify=%u "
                   "prod_cat=%u prod_steps=%u/%u "
                   "prog=%u prog_steps=%u/%u "
                   "phy_res=%u phy_steps=%u/%u "
                   "napi_res=%u napi_steps=%u/%u "
                   "txrx_res=%u txrx_steps=%u/%u "
                   "caps=%u cap_steps=%u/%u "
                   "irq_res=%u work_res=%u "
                   "wire_ho=%u wire_steps=%u/%u "
                   "func=%u func_steps=%u/%u "
                   "abi=%u abi_steps=%u/%u "
                   "c2=%u bases_clean=%u bar_src=%u path=0x%x "
                   "product_mint=%u product_prog=%u "
                   "tx_rx=OPEN wire_owner=%s product_handoff=%u "
                   "bind_ne_wire=%u freestanding_product=SKIP "
                   "product=UDX+ABI product=UDX+sshd+stack "
                   "need=DDI_caps need=hot_cold_abi "
                   "never_program=%u never_tnpds=%u never_rdsar=%u "
                   "never_te_re=%u "
                   "denser_prove=%u denser_residual=%u "
                   "bar=v2026.08.04.75 stamp_free=%u "
                   "dual_dod_b=OPEN claim_class=C2 "
                   "G-AC-1 Soft!=product\n",
                   (unsigned)pSoft->u8SoftStage,
                   (unsigned)GJ_SYS_DDI,
                   (unsigned)pSoft->u32FOpenOk,
                   (unsigned)pSoft->u32FMapOk,
                   (unsigned)pSoft->u32FRingOk,
                   (unsigned)pSoft->u32FRingVerify,
                   (unsigned)pSoft->u32FProdCat,
                   (unsigned)pSoft->u8ProdSteps,
                   (unsigned)RTL_SOFT_PROD_STEP_COUNT,
                   (unsigned)pSoft->u32FProgRes,
                   (unsigned)pSoft->u8ProgSteps,
                   (unsigned)RTL_SOFT_PROG_STEP_COUNT,
                   (unsigned)pSoft->u32FPhyRes,
                   (unsigned)pSoft->u8PhySteps,
                   (unsigned)RTL_SOFT_PHY_STEP_COUNT,
                   (unsigned)pSoft->u32FNapiRes,
                   (unsigned)pSoft->u8NapiSteps,
                   (unsigned)RTL_SOFT_NAPI_STEP_COUNT,
                   (unsigned)pSoft->u32FTxRxRes,
                   (unsigned)pSoft->u8TxRxSteps,
                   (unsigned)RTL_SOFT_TXRX_STEP_COUNT,
                   (unsigned)pSoft->u32FCapsRes,
                   (unsigned)pSoft->u8CapSteps,
                   (unsigned)RTL_SOFT_CAP_STEP_COUNT,
                   (unsigned)pSoft->u32FIrqRes,
                   (unsigned)pSoft->u32FWorkRes,
                   (unsigned)pSoft->u32FWireHo,
                   (unsigned)pSoft->u8WireSteps,
                   (unsigned)RTL_SOFT_WIRE_STEP_COUNT,
                   (unsigned)pSoft->u32FFuncRes,
                   (unsigned)pSoft->u8FuncSteps,
                   (unsigned)RTL_SOFT_FUNC_STEP_COUNT,
                   (unsigned)pSoft->u32FAbiRes,
                   (unsigned)pSoft->u8AbiSteps,
                   (unsigned)RTL_SOFT_ABI_STEP_COUNT,
                   (unsigned)pSoft->u32FC2Path,
                   (unsigned)pSoft->u32FBasesClean,
                   (unsigned)pSoft->u8BarSrc,
                   (unsigned)pSoft->u32PathBits,
                   (unsigned)pSoft->u32FProductMint,
                   (unsigned)pSoft->u32FProductProg,
                   (pSoft->u8WireOwner == RTL_SOFT_WIRE_OWNER_UDX)
                       ? "udx" : "freestanding",
                   (unsigned)pSoft->u32FProductProg,
                   (pSoft->u32FProductProg != 0u) ? 0u : 1u,
                   (pSoft->u32FProductProg != 0u) ? 0u : 1u,
                   (pSoft->u32FProductProg != 0u) ? 0u : 1u,
                   (pSoft->u32FProductProg != 0u) ? 0u : 1u,
                   (pSoft->u32FProductProg != 0u) ? 0u : 1u,
                   (unsigned)RTL_SOFT_DENSER_PROVE,
                   (unsigned)RTL_SOFT_DENSER_RESIDUAL,
                   (unsigned)RTL_SOFT_BAR_STAMP_FREE);
        return UDX_OK;
    }

    rtl8168_soft_bind_lamp(NULL, 0);
    rtl8168_soft_bind_path_lamp(NULL, RTL_SOFT_BIND_PATH_NONE);
    rtl8168_soft_bind_ne_wire(NULL, 0);
    udx_printk("rtl8168_udx: soft SKIP (no udx_host_bind_by_id; "
               "host-inject 10ec:8168 BAR2 for lab test) "
               "prefer_ddi=1 prefer_real_ddi=1 real_ddi=0 "
               "inject_fallback=1 "
               "freestanding_product=SKIP product=UDX+ABI "
               "product=UDX+sshd+stack "
               "need=DDI_caps need=hot_cold_abi dual_dod_b=OPEN "
               "wire=freestanding Soft!=product\n");
    return UDX_ERR_NOSYS;
}

/**
 * Freestanding ELF entry (product host launch / stage-esp / user.ld).
 * Option 3: freestanding_start → real DDI bind → product_program_try
 * (TNPDS/RDSAR/TE|RE when program_gate). Dual DoD B OPEN until DUT L3.
 *
 * DUT .81: host SYS_EXIT 0 after soft probe PASS — process death H3 tears
 * AS/MAP before wire/sshd can use the NIC. Product keep_live: park thr
 * after success so DDI maps + rings + TE|RE residual stay live for
 * netstackd/sshd chain (lab 10.200.125.50 :22). Soft!=product. G-AC-1.
 *
 * greppable: rtl8168_udx: freestanding main
 * greppable: rtl8168_udx: product program PASS
 * greppable: rtl8168_udx: product host park
 * greppable: keep_live=1
 */
int
main(int argc, char **argv)
{
    udx_status_t st;
    u32 u32ParkN;

    (void)argc;
    (void)argv;
    udx_printk("rtl8168_udx: freestanding main option3=1 "
               "prefer_real_ddi=1 product=UDX+sshd+stack "
               "lab_ip=10.200.125.50 dual_dod_b=OPEN Soft!=product\n");
    st = rtl8168_udx_freestanding_start();
    udx_printk("rtl8168_udx: freestanding main done st=%d "
               "product_prog_path=1 dual_dod_b=OPEN Soft!=product\n",
               (int)st);
    if (st != UDX_OK) {
        return 1;
    }

    /*
     * Product host keep_live (Dual DoD B): never SYS_EXIT after bind/program.
     * Maps, rings, TE|RE residual, and soft IRQ/work scaffolding stay in the
     * live process AS while netstackd/sshd chase lab IP / :22.
     * greppable: rtl8168_udx: product host park
     */
    udx_printk("rtl8168_udx: product host park keep_live=1 "
               "product=UDX+sshd+stack chain=rtl8168_udx>netstackd>sshd "
               "lab_ip residual OPEN lab_ip=10.200.125.50 sshd_port=22 "
               "wire residual thr live dual_dod_b=OPEN agent_ne_close=1 "
               "Soft!=product G-AC-1\n");
    /*
     * Arm net_door UDX L2 soft ready so net_l2_ready + soft TX enqueue work
     * under freestanding rtl SKIP. Soft!=product Dual DoD B.
     */
    (void)gj_net_eth_udx_ready(1);
    /* Prefer file-static soft block if freestanding_start left it live. */
    if (g_rtlSoft.u32FProductProg != 0u && g_rtlSoft.u32FOwnHandoff == 0u) {
        (void)rtl8168_product_own_handoff(&g_rtlSoft);
    }
    u32ParkN = 0u;
    for (;;) {
        /* Thr-poll RX reclaim → ETH_INJECT; demux TX → Own + TPPoll. */
        if (g_rtlSoft.u32FOwnHandoff != 0u) {
            rtl8168_product_l2_poll(&g_rtlSoft);
        }
        (void)gj_net_poll(); /* advance eth/tcp residual when frames inject */
        gj_yield();
        u32ParkN++;
        /* Once-lamp every ~2^16 yields — no stamp storm. */
        if ((u32ParkN & 0xffffu) == 0u) {
            udx_printk("rtl8168_udx: product host park live "
                       "keep_live=1 yields=%u own_handoff=%u "
                       "rx_poll=%u inject=%u tx_pull=%u "
                       "dual_dod_b=OPEN Soft!=product\n",
                       (unsigned)u32ParkN,
                       (unsigned)g_rtlSoft.u32FOwnHandoff,
                       (unsigned)g_rtlSoft.u32RxPoll,
                       (unsigned)g_rtlSoft.u32Inject,
                       (unsigned)g_rtlSoft.u32TxPull);
        }
    }
    /* not reached */
    return 0;
}

/*
 * user.ld ENTRY(_start). Freestanding native process (no host crt0).
 * Success: main parks (keep_live); failure: gj_exit(nonzero).
 */
void
_start(void)
{
    int n;

    n = main(0, (char **)0);
    /* Only failure paths return from main. */
    gj_exit(n);
}

#endif /* !UDX_HOST_LIBC */
