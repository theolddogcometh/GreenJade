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
 * Soft ≠ product: these notes and structs seed the UDX ↔ devmgr contract.
 * They do **not** mint MMIO/IRQ/DMA caps, program VT-d, or claim multi-server
 * confine. Product order: ABI + virtio T0 → DDI caps → real HW hosts.
 * License: dual MIT OR Apache-2.0 only; **no GPL**, no Linux .ko product.
 *
 * Greppable markers (stable for smoke):
 *   udx: soft ddi host note PASS   — first host soft init
 *   udx: soft ddi-ready            — future attach readiness (observation)
 *   udx: soft ddi lifecycle note   — probe/remove/quiesce catalog
 *   udx: soft ddi dma not-granted  — window request soft fail until product
 *   udx: soft ddi bind PASS        — freestanding GJ_SYS_DDI open+map BAR0
 *   udx: soft ddi bind SKIP        — syscall unknown / no match / host-linux
 *
 * Freestanding bind (not host-inject-only):
 *   udx_host_bind_scan() / udx_host_bind_by_id() use GJ_SYS_DDI (103) when
 *   available; soft SKIP + keep inject path for host-linux tests otherwise.
 *   On PASS, BAR0 PA/VA is window-registered so udx_ioremap works.
 *   G752 first targets: 10ec:8168, 8086:a12f (see udx/ddi.h).
 */
#pragma once

#include <udx/types.h>
#include <udx/pci.h>
#include <udx/ddi.h>

/* Soft inventory bounds (mirror gj_devmgr soft table idea; local UDX only). */
#define UDX_HOST_SOFT_BDF_MAX  32u
#define UDX_HOST_SOFT_BAR_MAX  6u /* PCI type-0 BARs 0..5 */
#define UDX_HOST_SOFT_WIN_MAX  16u

/**
 * Soft PCI BDF identity (bus / slot / function).
 * Shape mirrors kernel gj_devmgr_bdf; UDX does not include kernel headers.
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
 * Safe anytime; never gates skeleton PASS. Soft ≠ product grant path.
 */
void udx_host_soft_init(void);

/** Non-zero after successful soft DDI host init. */
int  udx_host_soft_ready(void);

/**
 * Soft note for future DDI attachment (ABI-first pivot).
 * Emits greppable "udx: soft ddi-ready"; observation only — never gates
 * skeleton PASS. Safe to call any number of times from host demos.
 * Also ensures soft host init has run (PASS lamp once).
 */
void udx_host_soft_note_ddi(void);

/**
 * Soft lifecycle catalog (probe / remove / quiesce).
 * Emits greppable "udx: soft ddi lifecycle note" — documentation lamp only.
 * Does not attach or detach devices.
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
 * Returns UDX_ERR_NOSYS (not granted until product). When pGrantOut is
 * non-NULL, fills bdf/range/rw with request echo, u8Ok=0,
 * u32WinId=0xffffffff. Emits greppable "udx: soft ddi dma not-granted".
 * Bad args → UDX_ERR_INVAL (and no PASS claim).
 */
udx_status_t udx_host_soft_dma_window_request(const struct udx_host_bdf *pBdf,
                                              u64 u64PaBase, u64 u64Cb,
                                              u8 u8Rw,
                                              struct udx_host_window_grant *pGrantOut);

/** Soft inventory tallies (diagnostics; wrap OK). */
u32 udx_host_soft_init_calls(void);
u32 udx_host_soft_dma_requests(void);
u32 udx_host_soft_dma_rejects(void);

/** Match all unbound injected devices against registered drivers (probe). */
void udx_host_rescan_pci(void);

/**
 * Freestanding soft DDI bind: scan via GJ_SYS_DDI and install grants.
 *
 * When !UDX_HOST_LIBC (or GJ_FREESTANDING): attempts syscall GJ_SYS_DDI
 * (103) OP_SCAN, then open+map BAR0 for each entry; window-registers
 * granted BAR PA so ioremap works; runs pci match/probe.
 *
 * Soft ≠ product: kernel may soft-stub (-ENOSYS) → greppable SKIP.
 * Host-linux (UDX_HOST_LIBC without freestanding): always soft SKIP;
 * keep udx_host_inject_pci for lab tests.
 *
 * Returns UDX_OK if at least one function was installed (PASS lamp).
 * Returns UDX_ERR_NOSYS / UDX_ERR_NODEV on SKIP.
 */
udx_status_t udx_host_bind_scan(void);

/**
 * Freestanding soft DDI bind-by-id: open vend:dev, map BAR0, install.
 *
 * Preferred first targets on G752 lab (docs / HCL):
 *   10ec:8168  Realtek RTL8111/8168
 *   8086:a12f  Intel 100 Series xHCI
 *
 * Same soft SKIP / inject-fallback rules as udx_host_bind_scan.
 * On success: *ppOut (optional) receives the installed pci_dev; BAR0
 * granted PA is window-registered for udx_ioremap.
 */
udx_status_t udx_host_bind_by_id(u16 u16Vendor, u16 u16Device,
                                 struct udx_pci_dev **ppOut);

/**
 * Install a DDI-granted PCI function into the UDX device table.
 *
 * Fills BAR phys/len from grant; if pBar0Va is non-NULL and BAR0 length
 * is non-zero, registers the host window so udx_ioremap(bar0_pa, len)
 * succeeds. Then attempts driver match/probe (same as inject).
 *
 * Freestanding: static grant pool (no libc heap).
 * Host: heap-backed (same soft shape as inject).
 * Soft ≠ product multi-server confine.
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
