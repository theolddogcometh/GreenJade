/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host / test inject surface for Linux porters.
 * Use on Linux/macOS hosts to exercise drivers before GreenJade kernel
 * wires real PCI/IOMMU. Not available inside freestanding GJ userland
 * until devmgr replaces inject with cap grants.
 *
 * Soft path for PCI skeleton demos:
 *   inject → probe → host_bar_writel(status) → fire_irq → run → work
 *
 * ---------------------------------------------------------------------------
 * Soft DDI host surface (ABI-first; docs/DDI_SOFT.md shape — local types only)
 * ---------------------------------------------------------------------------
 * Userspace driver host lifecycle (Solaris DDI spirit; Linux-shaped names):
 *
 *   probe    — match + attach: enable, regions, ioremap, dma, request_irq,
 *              set_drvdata. Device is live after successful probe.
 *   quiesce  — stop DMA / soft-mask IRQs / cancel work *before* remove or
 *              process death. Optional pfnQuiesce on udx_pci_driver; product
 *              hosts must still best-effort quiesce on crash path.
 *   remove   — free_irq → DMA free → iounmap → release regions → disable.
 *              Called after quiesce (unregister path). Caps stay hidden —
 *              driver .c never mints or revokes.
 *
 * Soft != product: these notes and structs seed the UDX <-> devmgr contract.
 * They do **not** mint MMIO/IRQ/DMA caps, program VT-d, or claim multi-server
 * confine. Product order: ABI + virtio T0 -> DDI caps -> real HW hosts.
 * License: dual MIT OR Apache-2.0 only; **no GPL**, no Linux .ko product.
 * G-AC-1: no Linux .ko product AC.
 *
 * ---------------------------------------------------------------------------
 * Soft residual deepen (C2 host surface; Soft!=product; dual MIT OR Apache-2.0)
 * ---------------------------------------------------------------------------
 * Cap residual honesty (lean; never claim product mint from host.h):
 *   soft path = window table + inject lab + GJ_SYS_DDI residual bind
 *   product   = MMIO_FRAME + IRQ Notification + DMA window cap mint (OPEN)
 *   multi_server / confine = 0 (soft residual; not product multi-server)
 *   Cap mint residual: cap_mint=0 always today; greppable lean + api honesty
 *   soft_ne_product=1 always; Soft!=product is the claim-class law
 *
 * Cap residual surfaces (per product host path; Soft!=product):
 *   MMIO residual  — MAP preferred BARs → window_register → udx_ioremap;
 *                    != product MMIO_FRAME CNode mint (OPEN)
 *   IRQ residual   — IRQ_BIND soft badge note + udx_host_fire_irq /
 *                    request_irq table; != product Notification mint (OPEN)
 *   DMA residual   — DMA_NOTE + soft_dma_window_request not-granted;
 *                    != product DMA window / VT-d mint (OPEN)
 *   CLOSE residual — handle retained on install residual (close=0);
 *                    product hosts keep OPEN handle for lifecycle; no
 *                    CLOSE residual on bind PASS path
 *   inject residual — host-linux lab only (UDX_HOST_LIBC); soft SKIP
 *                    freestanding bind; inject != Dual DoD close
 *   INVENTORY residual — OP_INVENTORY product-surface once-lamp after SCAN;
 *                    != product surface AC / Dual DoD close (OPEN)
 *
 * Dual DoD residual honesty (C2 UDX track; lamps alone never close):
 *   Dual DoD A OPEN — xhci_udx (8086:a12f) Linux-shaped USB userspace;
 *                     preferred BAR0; freestanding xhci_msc SKIP
 *   Dual DoD B OPEN — rtl8168_udx (10ec:8168) Linux-shaped NIC userspace;
 *                     preferred BAR0+BAR2; freestanding rtl8168 SKIP
 *   dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 (agent!=close)
 *   Soft ddi bind PASS / residual path != Dual DoD A/B closed.
 *   product=UDX+ABI; freestanding class SKIP; G-AC-1; no version stamp.
 *   never_fs_rtl_usb=1 — no freestanding class re-enable from this header.
 *
 * Product direction (lean residual honesty):
 *   freestanding class drivers (kernel rtl8168 / xhci_msc) = SKIP default
 *   product drivers = Linux-shaped userspace UDX (rtl8168_udx / xhci_udx)
 *   product_hosts=xhci_udx,rtl8168_udx product_path=UDX+ABI
 *
 * Product residual (C2; Dual DoD B OPEN; stamp-free bar v2026.08.04.75):
 *   product=UDX+sshd+stack — host bind residual for rtl8168_udx so
 *   netstackd + sshd can product-serve when Cap mint lands.
 *   Soft bind PASS != Dual DoD B wire close (agent!=close).
 * greppable: udx: soft product residual product=UDX+sshd+stack
 *
 * Freestanding bind residual (not host-inject-only):
 *   udx_host_bind_scan() / udx_host_bind_by_id() use GJ_SYS_DDI (103) when
 *   available. Full residual walk (product hosts; matches host.c):
 *     SCAN → INVENTORY → GET → OPEN → CFG_READ → MAP preferred BARs →
 *     MAP_REMAP → DMA_NOTE → IRQ_BIND → window_register →
 *     install_granted → probe
 *   Core greppable chain: SCAN,GET,OPEN,MAP_BAR (+ WIN|INSTALL on PASS).
 *   Side residual (deepen honesty; Soft!=product mint OPEN):
 *     INVENTORY (product-surface once-lamp) →
 *     CFG_READ (ident+cmdst) → MAP_REMAP (idempotent grant slot) →
 *     DMA_NOTE (first preferred PA) → IRQ_BIND (soft badge bit0).
 *   Soft life bitmask UDX_DDI_LIFE_* (see ddi.h); core =
 *     SCAN|GET|OPEN|MAP|WIN|INSTALL (UDX_DDI_LIFE_CORE).
 *   Handle is retained for product hosts (no CLOSE on install path).
 *   Soft SKIP + inject path for host-linux tests otherwise.
 *   Product hosts: rtl8168_udx (10ec:8168 bars 0+2), xhci_udx (8086:a12f
 *   bar 0). On PASS, mapped BAR PA/VA windows are registered so ioremap
 *   works. product=UDX+ABI; freestanding class SKIP. No version stamp.
 *   Soft bind PASS != Dual DoD A/B close; Soft!=product.
 *
 * Greppable markers (stable for smoke; Soft!=product):
 *   udx: soft ddi host note PASS   -- first host soft init
 *   udx: soft ddi-ready            -- future attach readiness (observation)
 *   udx: soft ddi lifecycle note   -- probe/remove/quiesce catalog
 *   udx: soft ddi dma not-granted  -- window request soft fail until product
 *   udx: soft ddi bind PASS        -- freestanding GJ_SYS_DDI residual PASS
 *   udx: soft ddi bind SKIP        -- syscall unknown / no match / host-linux
 *   udx: soft ddi bind residual    -- SCAN→GET→OPEN→MAP_BAR residual tallies
 *   udx: soft ddi bind residual path -- per-install residual + life mask
 *   udx: soft ddi bind residual scan  -- SCAN n residual (bind_by_id walk)
 *   udx: soft ddi bind residual match -- GET vend:dev hit residual
 *   udx: soft ddi residual MAP_BAR -- per preferred-BAR MAP residual
 *   udx: soft ddi residual CFG     -- OPEN identity/cmdst CFG_READ residual
 *   udx: soft ddi residual MAP_REMAP -- first-mapped BAR re-map residual
 *   udx: soft ddi residual DMA_NOTE -- soft DMA window note residual
 *   udx: soft ddi residual IRQ_BIND -- soft IRQ note residual
 *   udx: soft ddi residual INVENTORY -- OP_INVENTORY product-surface residual
 *   udx: host soft residual lean   -- Cap/MMIO/IRQ/DMA soft residual
 *   udx: host soft api honesty     -- cap mint OPEN honesty
 *   udx: host soft inventory       -- window/BAR/IRQ soft inventory dump
 *   udx: host soft window          -- window table live/peak residual
 *   udx: host soft lookup          -- window lookup hit/miss residual
 *   udx: host soft bar             -- host BAR r/w residual tallies
 *   udx: host soft irq             -- fire_irq residual tally
 *   udx: host soft path            -- host soft surface path catalog
 *   udx: host soft wave            -- soft inventory wave stamp (eng only)
 *   udx: host soft honesty         -- multi_server=0 confine=0 residual
 *   udx: host soft ddi_bind        -- bind enter/pass/skip inventory
 *   udx: soft residual freestanding class SKIP
 *   udx: soft residual product=UDX+ABI
 *   udx: soft residual Dual DoD A OPEN
 *   udx: soft residual Dual DoD B OPEN
 *   udx: soft residual Dual DoD A/B OPEN
 *   udx: soft product residual product=UDX+sshd+stack
 *   dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0
 *   Soft!=product  -- claim-class law (soft residual != product AC)
 * Law: dual MIT OR Apache-2.0; Soft!=product; G-AC-1; Dual DoD A/B OPEN;
 * no freestanding class re-enable; no GPL; no version stamp / no stamp storms.
 * Bar honesty (stamp-free residual): v2026.08.04.75 — NEVER bump
 * GJ_IMAGE_VERSION from this header.
 */
#pragma once

#include <udx/types.h>
#include <udx/pci.h>
#include <udx/ddi.h>

/* Soft inventory bounds (mirror gj_devmgr soft table idea; local UDX only). */
#define UDX_HOST_SOFT_BDF_MAX  32u
#define UDX_HOST_SOFT_BAR_MAX  6u /* PCI type-0 BARs 0..5 */
#define UDX_HOST_SOFT_WIN_MAX  16u

/*
 * Soft residual lean layout version (eng only; Soft!=product; not stamp).
 * Bump when host residual surface grows (cap honesty / Dual DoD / INVENTORY
 * side residual / bind chain docs). Not GJ_IMAGE_VERSION. Not product ABI.
 * ver1: Cap mint OPEN constants + Dual DoD A/B OPEN + handle retain +
 *       CLOSE=0 on bind + INVENTORY side residual + product_host catalog.
 * Grep: UDX_HOST_LAYOUT_VER / udx: host soft residual lean
 */
#define UDX_HOST_LAYOUT_VER           1u

/*
 * Soft residual Cap / Dual DoD honesty constants (eng only; Soft!=product).
 * Always-0 mint/server/confine flags match host soft residual lean emit.
 * Dual DoD A/B remain OPEN (1) until DUT proof closes them — lamps alone
 * never flip these to closed. No version stamp. No GPL. G-AC-1.
 * Grep: UDX_HOST_SOFT_CAP_MINT / UDX_HOST_SOFT_DOD_A_OPEN / DOD_B_OPEN
 * Grep: dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX Soft!=product
 */
#define UDX_HOST_SOFT_CAP_MINT        0u /* no MMIO/IRQ/DMA cap mint today */
#define UDX_HOST_SOFT_MULTI_SERVER    0u /* soft residual; not multi-server */
#define UDX_HOST_SOFT_CONFINE         0u /* soft residual; not product confine */
#define UDX_HOST_SOFT_DOD_A_OPEN      1u /* xhci_udx Dual DoD A still OPEN */
#define UDX_HOST_SOFT_DOD_B_OPEN      1u /* rtl8168_udx Dual DoD B still OPEN */
#define UDX_HOST_SOFT_HANDLE_RETAIN   1u /* install residual keeps OPEN h */
#define UDX_HOST_SOFT_CLOSE_ON_BIND   0u /* no CLOSE residual on bind PASS */
#define UDX_HOST_SOFT_NEVER_FS_RTL_USB 1u /* no freestanding class re-enable */
#define UDX_HOST_SOFT_NE_PRODUCT      1u /* Soft!=product claim-class law */
#define UDX_HOST_SOFT_PRODUCT_MINT    0u /* product_mint=0 greppable honesty */

/* Product-host name catalog (C2 UDX; Dual DoD A/B OPEN seed; Soft!=product). */
#define UDX_HOST_SOFT_DOD_A_NAME      "xhci_udx"
#define UDX_HOST_SOFT_DOD_B_NAME      "rtl8168_udx"
#define UDX_HOST_SOFT_PRODUCT_EQ      "UDX+ABI"
/*
 * Product residual seed (Soft!=product; Dual DoD B OPEN; stamp-free).
 * product=UDX+sshd+stack — host bind residual for stack+sshd product path.
 * greppable: udx: soft product residual product=UDX+sshd+stack
 */
#define UDX_HOST_SOFT_PRODUCT_UDX_SSHD_STACK  1u
#define UDX_HOST_SOFT_PRODUCT_CHAIN           "rtl8168_udx>netstackd>sshd"

/**
 * Soft residual lean layout version (UDX_HOST_LAYOUT_VER).
 * Eng residual only; Soft!=product; not GJ_IMAGE_VERSION / product ABI.
 * greppable residual: udx: host soft residual lean
 */
static inline u32
udx_host_layout_ver(void)
{
    return UDX_HOST_LAYOUT_VER;
}

/**
 * Soft product residual seed toward product=UDX+sshd+stack.
 * Returns 1 always — catalog honesty only (Soft!=product; Dual DoD OPEN).
 * greppable residual: udx: soft product residual product=UDX+sshd+stack
 */
static inline u32
udx_host_product_udx_sshd_stack(void)
{
    return UDX_HOST_SOFT_PRODUCT_UDX_SSHD_STACK;
}

/**
 * Soft!=product claim-class residual (always non-zero today).
 * greppable: Soft!=product soft_ne_product=1
 */
static inline u32
udx_host_soft_ne_product(void)
{
    return UDX_HOST_SOFT_NE_PRODUCT;
}

/**
 * Dual DoD A/B OPEN residual (1 = still OPEN; lamps alone never close).
 * greppable: dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0
 */
static inline u32
udx_host_soft_dod_a_open(void)
{
    return UDX_HOST_SOFT_DOD_A_OPEN;
}

static inline u32
udx_host_soft_dod_b_open(void)
{
    return UDX_HOST_SOFT_DOD_B_OPEN;
}

/**
 * Soft PCI BDF identity (bus / slot / function).
 * Shape mirrors kernel gj_devmgr_bdf; UDX does not include kernel headers.
 * Soft inventory only — BDF snapshot != Dual DoD close / cap mint.
 */
struct udx_host_bdf {
    u8  u8Bus;
    u8  u8Slot; /* device number on bus (5-bit) */
    u8  u8Func;
    u8  u8Pad;
};

/**
 * Soft BAR snapshot (PA + size when known).
 * u8Mem: non-zero = memory BAR; zero = I/O BAR (soft inventory only).
 * u8Prefetch: non-zero when prefetchable (memory BARs; soft, often 0).
 * u64Pa / u64Cb: 0 if unresolved / not injected.
 */
struct udx_host_bar {
    u8  u8Index;    /* 0..5 */
    u8  u8Mem;
    u8  u8Prefetch;
    u8  u8Pad;
    u64 u64Pa;
    u64 u64Cb;
};

/**
 * Soft DMA window grant result (devmgr-shaped note; not a live cap).
 * Product path later: devmgr mints a real window cap into the host CNode.
 *
 * Soft policy today (honest fail-closed for product claims):
 *   u8Ok always 0 (not granted); u32WinId = 0xffffffff.
 *   Request APIs return UDX_ERR_NOSYS / soft not-granted — observation only.
 * Soft residual Cap honesty: UDX_HOST_SOFT_CAP_MINT=0; DMA window mint OPEN;
 * multi_server=0; confine=0; product_mint=0 (UDX_HOST_SOFT_PRODUCT_MINT).
 * Soft != Dual DoD A/B close; Soft!=product (UDX_HOST_SOFT_NE_PRODUCT=1).
 */
struct udx_host_window_grant {
    struct udx_host_bdf bdf;
    u64 u64PaBase;
    u64 u64Cb;
    u8  u8Ok;   /* non-zero only when product grants exist (never today) */
    u8  u8Rw;   /* bit0 R, bit1 W (soft flags; product uses cap rights) */
    u8  u8Pad[2];
    u32 u32WinId; /* soft window index, or 0xffffffff if not granted */
};

/**
 * Inject a synthetic PCI function for probe matching.
 * BAR lengths: non-zero bars get host-backed MMIO (ioremap works).
 * aBarLen[6] / aBarIsMem[6] may be NULL (all zero / all memory).
 *
 * Returns UDX_OK and fills *ppOut on success (owned by UDX until remove).
 */
udx_status_t udx_host_inject_pci(u16 u16Vendor, u16 u16Device,
                                 u8 u8Bus, u8 u8Devfn, int nIrq,
                                 const u64 *aBarLen, const u8 *aBarIsMem,
                                 struct udx_pci_dev **ppOut);

/**
 * Extended inject: subsystem IDs + 24-bit class code for id-table match.
 * u32Class is class_code << 8 | subclass << 8 | prog-if style 24-bit
 * (same packing as Linux pci_device_id.class with class_mask).
 */
udx_status_t udx_host_inject_pci_ex(u16 u16Vendor, u16 u16Device,
                                    u16 u16SubVendor, u16 u16SubDevice,
                                    u32 u32Class,
                                    u8 u8Bus, u8 u8Devfn, int nIrq,
                                    const u64 *aBarLen, const u8 *aBarIsMem,
                                    struct udx_pci_dev **ppOut);

/**
 * Unbind + free a host-injected function (BAR windows + soft cfg).
 * Safe no-op when pPdev is NULL or not host-injected.
 * Returns UDX_ERR_NOSYS when not built with UDX_HOST_LIBC.
 */
udx_status_t udx_host_remove_pci(struct udx_pci_dev *pPdev);

/** Fire threaded IRQ path as if kernel Notification arrived. */
void udx_host_fire_irq(int nIrq);

/**
 * Soft DDI host init (idempotent).
 * First call emits greppable "udx: soft ddi host note PASS".
 * Safe anytime; never gates skeleton PASS. Soft != product grant path.
 * Residual honesty: does not mint Cap/MMIO/IRQ/DMA; Soft!=product.
 * dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0; product_mint=0;
 * freestanding class SKIP; G-AC-1; no version stamp.
 */
void udx_host_soft_init(void);

/** Non-zero after successful soft DDI host init. */
int  udx_host_soft_ready(void);

/**
 * Soft note for future DDI attachment (ABI-first pivot).
 * Emits greppable "udx: soft ddi-ready"; observation only — never gates
 * skeleton PASS. Safe to call any number of times from host demos.
 * Also ensures soft host init has run (PASS lamp once).
 * Soft residual only — not Dual DoD A/B close.
 */
void udx_host_soft_note_ddi(void);

/**
 * Soft lifecycle catalog (probe / remove / quiesce).
 * Emits greppable "udx: soft ddi lifecycle note" — documentation lamp only.
 * Does not attach or detach devices. Caps stay hidden (no mint/revoke).
 */
void udx_host_soft_lifecycle_note(void);

/**
 * Fill soft BDF from a PCI function (bus + slot/func from devfn).
 * Returns UDX_ERR_INVAL if pPdev or pOut is NULL.
 */
udx_status_t udx_host_soft_bdf_from_pdev(const struct udx_pci_dev *pPdev,
                                         struct udx_host_bdf *pOut);

/**
 * Soft BAR snapshot for one BAR index (0..5) from a PCI function.
 * Mirrors inject / grant soft fields only — not a live MMIO_FRAME cap.
 * Returns UDX_ERR_INVAL on bad args / bar index.
 */
udx_status_t udx_host_soft_bar_snapshot(const struct udx_pci_dev *pPdev,
                                        int nBar,
                                        struct udx_host_bar *pOut);

/**
 * Soft BAR snapshot for all six type-0 BARs.
 * aOut must have room for UDX_HOST_SOFT_BAR_MAX entries.
 * *pu32CountOut (optional) receives how many BARs had non-zero length.
 */
udx_status_t udx_host_soft_bar_snapshot_all(const struct udx_pci_dev *pPdev,
                                            struct udx_host_bar *aOut,
                                            u32 *pu32CountOut);

/**
 * Soft DMA window grant **request** (devmgr-shaped; not a live cap).
 *
 * Product later: host asks devmgr for a DMA window; kernel mints window cap
 * + optional VT-d cover. Today: always soft fail / not-granted.
 *
 * Soft residual DMA honesty (Soft!=product; G-AC-1):
 *   soft path = request echo + not-granted (u8Ok=0, winid=0xffffffff)
 *   product   = DMA window cap + live VT-d (OPEN; UDX_HOST_SOFT_CAP_MINT=0)
 * Returns UDX_ERR_NOSYS (not granted until product). When pGrantOut is
 * non-NULL, fills bdf/range/rw with request echo, u8Ok=0,
 * u32WinId=0xffffffff. Emits greppable "udx: soft ddi dma not-granted".
 * Bad args → UDX_ERR_INVAL (and no PASS claim). Dual DoD B DMA path residual
 * only — not wire product close.
 */
udx_status_t udx_host_soft_dma_window_request(const struct udx_host_bdf *pBdf,
                                              u64 u64PaBase, u64 u64Cb,
                                              u8 u8Rw,
                                              struct udx_host_window_grant *pGrantOut);

/**
 * Soft inventory tallies (diagnostics; wrap OK).
 * Residual only — never gates skeleton PASS or Dual DoD A/B.
 */
u32 udx_host_soft_init_calls(void);
u32 udx_host_soft_dma_requests(void);
u32 udx_host_soft_dma_rejects(void);

/** Match all unbound injected devices against registered drivers (probe). */
void udx_host_rescan_pci(void);

/**
 * Freestanding soft DDI bind residual: SCAN→GET→OPEN→MAP_BAR install.
 *
 * When !UDX_HOST_LIBC (or GJ_FREESTANDING): GJ_SYS_DDI (103) residual:
 *   SCAN → INVENTORY → GET(index) → OPEN → CFG_READ → MAP preferred BARs →
 *   MAP_REMAP → DMA_NOTE → IRQ_BIND → window_register →
 *   udx_host_install_granted_pci → pci match/probe.
 * Core greppable chain: SCAN,GET,OPEN,MAP_BAR (side residual deepens
 * honesty; handle retained — no CLOSE). Side residual:
 *   INVENTORY,CFG,MAP_REMAP,DMA_NOTE,IRQ_BIND (Soft!=product mint OPEN).
 * Soft life bitmask UDX_DDI_LIFE_*; life core on PASS ≈ UDX_DDI_LIFE_CORE
 * (SCAN|GET|OPEN|MAP|WIN|INSTALL).
 * Product-host preferred BARs (Soft residual; matches kernel main smoke):
 *   10ec:8168 / rtl8168_udx — BAR0 + BAR2   (Dual DoD B residual OPEN)
 *   8086:a12f / xhci_udx    — BAR0          (Dual DoD A residual OPEN)
 *   other IDs               — BAR0
 *
 * Soft != product: kernel may soft-stub (-ENOSYS) → greppable SKIP.
 * Host-linux (UDX_HOST_LIBC without freestanding): always soft SKIP;
 * keep udx_host_inject_pci for lab tests. Soft residual freestanding
 * class SKIP; product=UDX+ABI. Soft bind PASS != Dual DoD A/B close.
 * Cap residual: MMIO_FRAME / IRQ Notification / DMA window mint OPEN
 * (UDX_HOST_SOFT_CAP_MINT=0; multi_server=0; confine=0;
 *  UDX_HOST_SOFT_NEVER_FS_RTL_USB=1; UDX_HOST_SOFT_NE_PRODUCT=1).
 * dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 (agent!=close).
 *
 * Returns UDX_OK if at least one function was installed (PASS lamp).
 * Returns UDX_ERR_NOSYS / UDX_ERR_NODEV on SKIP.
 * greppable: udx: soft ddi bind PASS|SKIP|residual[ path]
 * greppable: udx: soft ddi residual INVENTORY
 * greppable: udx: soft residual Dual DoD A/B OPEN
 */
udx_status_t udx_host_bind_scan(void);

/**
 * Freestanding soft DDI bind-by-id residual for product UDX hosts.
 *
 * Preferred first targets on G752 lab (docs / HCL / product hosts):
 *   10ec:8168  Realtek RTL8111/8168 → rtl8168_udx (bars 0+2)  [DoD B OPEN]
 *   8086:a12f  Intel 100 Series xHCI → xhci_udx (bar 0)       [DoD A OPEN]
 *
 * Residual: SCAN → INVENTORY → GET match vend:dev → OPEN → CFG_READ →
 * MAP preferred BARs → MAP_REMAP → DMA_NOTE → IRQ_BIND →
 * window_register each mapped BAR → install_granted → probe.
 * Same soft SKIP / inject-fallback rules as udx_host_bind_scan.
 * On success: *ppOut (optional) receives the installed pci_dev; mapped
 * BAR PAs are window-registered for udx_ioremap. Soft mint OPEN
 * (UDX_HOST_SOFT_CAP_MINT=0). Handle retained for product host
 * lifecycle (UDX_HOST_SOFT_HANDLE_RETAIN=1; UDX_HOST_SOFT_CLOSE_ON_BIND=0).
 * Soft residual path != Dual DoD A/B closed. G-AC-1; no GPL.
 * dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 Soft!=product.
 * greppable: udx: soft ddi bind residual match|path|PASS|SKIP
 * greppable: udx: soft residual Dual DoD A OPEN / Dual DoD B OPEN
 */
udx_status_t udx_host_bind_by_id(u16 u16Vendor, u16 u16Device,
                                 struct udx_pci_dev **ppOut);

/**
 * Last retained DDI OPEN handle from bind_scan / bind_by_id (0 if none).
 * Product hosts keep this for post-IMAN gj_ddi_irq_bind (Dual DoD A ABI).
 * Handle is not CLOSEd on bind PASS (UDX_HOST_SOFT_HANDLE_RETAIN=1).
 * Soft!=product; not a product IRQ Notification cap.
 * greppable: handle_retain=1 close_on_bind=0
 */
long udx_host_ddi_handle(void);

/**
 * Install a DDI-granted PCI function into the UDX device table.
 *
 * Fills BAR phys/len from grant; if pBar0Va is non-NULL and BAR0 length
 * is non-zero, registers the host window so udx_ioremap(bar0_pa, len)
 * succeeds. Bind residual also window-registers additional preferred
 * BAR VAs (e.g. rtl8168 BAR2) after install. Then match/probe.
 *
 * Soft residual honesty: window table only — not product MMIO_FRAME mint.
 * Freestanding: static grant pool (no libc heap).
 * Host: heap-backed (same soft shape as inject).
 * Soft != product multi-server confine (UDX_HOST_SOFT_MULTI_SERVER=0;
 * UDX_HOST_SOFT_CONFINE=0). Soft!=product; product_mint=0.
 */
udx_status_t udx_host_install_granted_pci(
    u16 u16Vendor, u16 u16Device,
    u16 u16SubVendor, u16 u16SubDevice, u32 u32Class,
    u8 u8Bus, u8 u8Devfn, int nIrq,
    const u64 *aBarPhys, const u64 *aBarLen, const u8 *aBarIsMem,
    void *pBar0Va, struct udx_pci_dev **ppOut);

/**
 * Host-only BAR poke without driver soft pointer.
 * Used by skeleton main to raise a fake status bit before fire_irq.
 * Returns UDX_ERR_NOSYS when not built with UDX_HOST_LIBC.
 */
udx_status_t udx_host_bar_writel(struct udx_pci_dev *pPdev, int nBar,
                                 u64 u64Off, u32 u32Val);

/** Host-only BAR dword read (symmetric to writel). */
udx_status_t udx_host_bar_readl(struct udx_pci_dev *pPdev, int nBar,
                                u64 u64Off, u32 *pu32Val);

udx_status_t udx_host_bar_writew(struct udx_pci_dev *pPdev, int nBar,
                                 u64 u64Off, u16 u16Val);
udx_status_t udx_host_bar_readw(struct udx_pci_dev *pPdev, int nBar,
                                u64 u64Off, u16 *pu16Val);

udx_status_t udx_host_bar_writeb(struct udx_pci_dev *pPdev, int nBar,
                                 u64 u64Off, u8 u8Val);
udx_status_t udx_host_bar_readb(struct udx_pci_dev *pPdev, int nBar,
                                u64 u64Off, u8 *pu8Val);

udx_status_t udx_host_bar_writeq(struct udx_pci_dev *pPdev, int nBar,
                                 u64 u64Off, u64 u64Val);
udx_status_t udx_host_bar_readq(struct udx_pci_dev *pPdev, int nBar,
                                u64 u64Off, u64 *pu64Val);

/**
 * Host-only bulk BAR copy (for firmware blobs / ring seed in demos).
 * cb bytes at BAR offset; bounds-checked against BAR length.
 */
udx_status_t udx_host_bar_memcpy_to(struct udx_pci_dev *pPdev, int nBar,
                                    u64 u64Off, const void *pSrc, size_t cb);
udx_status_t udx_host_bar_memcpy_from(struct udx_pci_dev *pPdev, int nBar,
                                      u64 u64Off, void *pDst, size_t cb);
