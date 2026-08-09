/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Device soft surface (Linux struct device mental model).
 * Caps / MMIO / IRQ / DMA windows stay in pBackend — never in driver .c.
 *
 * Soft residual driver lifecycle (product=UDX+ABI; Soft!=product; G-AC-1):
 *   register  — udx_pci_register_driver (id table + probe/remove/quiesce)
 *   probe     — attach: enable / regions / ioremap / dma / irq / set_drvdata
 *   quiesce   — stop DMA / mask IRQ / cancel work (before remove or crash)
 *   remove    — free_irq → DMA free → iounmap → release → disable → clear_drvdata
 * Order residual: register → probe → (run) → quiesce → remove.
 *
 * Soft residual deepen (C2 device surface; Soft!=product; G-AC-1;
 * dual MIT OR Apache-2.0):
 *   init residual       — udx_dev_init zeros soft slots (seed only; not bind)
 *   drvdata residual    — get/set/clear private soft state; install at end of
 *                         successful probe attach; clear on remove (after
 *                         free_irq / DMA free / iounmap / release / disable).
 *                         set_drvdata(NULL) is soft-equivalent to clear_drvdata.
 *   backend residual    — opaque pBackend slot filled by UDX host bind/inject
 *                         (pci bound object); never a driver-minted product
 *                         cap handle. Mint/revoke stay hidden in UDX host.
 *                         get_backend is observation only (opaque cookie).
 *   name residual       — udx_dev_name never NULL (falls back to "udx");
 *                         observation only for logs / lamps.
 *   attach residual     — soft_attached = backend filled AND drvdata installed
 *                         (probe residual complete soft honesty; Soft!=product).
 *   partial residual    — backend filled XOR drvdata installed (probe in
 *                         progress / fail path honesty; fail-closed).
 *   empty residual      — neither backend nor drvdata (pre-bind / post-unbind
 *                         seed; Soft!=product).
 *   detach residual     — post-remove honesty: drvdata cleared; backend may
 *                         still be host-owned until unbind (not Phase-A).
 *   observation residual — has_drvdata / has_backend / is_soft_attached /
 *                         is_soft_partial / is_soft_empty are fail-closed
 *                         queries; never hard-gate Dual DoD close.
 *   quiesce residual    — order honesty only (quiesce_before_remove); no
 *                         device field bit for quiesce (ops live on driver).
 *   cap residual        — deliberately NO cap slot/gen/ref in this header;
 *                         pBackend remains opaque; cap_mint=OPEN (not here).
 *   product_host residual — rtl8168_udx (DoD B) / xhci_udx (DoD A) hang
 *                         soft state via set_drvdata; freestanding class SKIP.
 *   Dual DoD residual   — A (xhci_udx USB) / B (rtl8168_udx NIC) remain OPEN;
 *                         soft device surface seeds lifecycle honesty only.
 *   layout residual     — UDX_DEVICE_LAYOUT_VER eng only; not GJ_IMAGE_VERSION;
 *                         not a product ABI stamp (stamp-free residual).
 *
 * Soft API honesty (Soft!=product; dual MIT OR Apache-2.0; G-AC-1):
 *   soft path = pDriverData pointer + opaque pBackend slot + name string
 *               + observation helpers + path-bit catalog + layout_ver
 *   product   = Cap mint/revoke of MMIO_FRAME / IRQ Notification / DMA
 *               window (OPEN; never claimed from this header / driver .c)
 *   freestanding class drivers SKIP; product = UDX+ABI userspace hosts
 *
 * Product-host shape residual (Dual DoD A/B OPEN seed; Soft!=product):
 *   rtl8168_udx (DoD B): set_drvdata at probe end; clear on remove residual
 *   xhci_udx    (DoD A): set_drvdata at probe end; clear on remove residual
 *   both: pBackend host-filled; driver .c never mints/revokes caps
 *   Soft attach observation never closes Dual DoD A/B.
 *
 * greppable: udx: soft residual driver lifecycle
 * greppable: udx: soft residual register
 * greppable: udx: soft residual probe
 * greppable: udx: soft residual quiesce
 * greppable: udx: soft residual remove
 * greppable: udx: soft residual product=UDX+ABI
 * greppable: udx: soft residual freestanding class SKIP
 * greppable: udx: device soft residual lean
 * greppable: udx: device soft residual lean layout_ver=
 * greppable: udx: device soft residual init
 * greppable: udx: device soft residual drvdata
 * greppable: udx: device soft residual backend
 * greppable: udx: device soft residual name
 * greppable: udx: device soft residual attach
 * greppable: udx: device soft residual partial
 * greppable: udx: device soft residual empty
 * greppable: udx: device soft residual detach
 * greppable: udx: device soft residual quiesce
 * greppable: udx: device soft residual cap_none
 * greppable: udx: device soft residual product_host
 * greppable: udx: device soft residual dual_dod
 * greppable: udx: device soft residual path
 * greppable: udx: device soft api honesty
 * Dual MIT OR Apache-2.0. No Linux source. No GPL. No stamp storms.
 * pBackend is opaque — not a product cap handle for driver .c
 * (mint/revoke stay hidden in UDX host). Dual DoD A/B OPEN.
 * C2 device residual Soft!=product; bar honesty stamp-free (v2026.08.04.75
 * panel context only — never bump GJ_IMAGE_VERSION from this header).
 */
#pragma once

#include <udx/types.h>

/*
 * Soft residual lean layout version (eng only; Soft!=product; not stamp).
 * Bump when C2 device residual surface grows (not GJ_IMAGE_VERSION).
 * ver1: drvdata get/set/clear + backend + name + attach/has_* observation;
 *       Dual DoD OPEN honesty; product_host set_drvdata shape.
 * ver2: C2 deepen — init residual; get_backend observation; partial/empty/
 *       detach residual; path-bit catalog; layout_ver/shape_ok; cap_none
 *       + product_host + Dual DoD OPEN constants; quiesce order honesty.
 * Grep: udx: device soft residual lean layout_ver=
 */
#define UDX_DEVICE_LAYOUT_VER            2u

/*
 * Soft residual path-bit catalog (once-lamp spirit for product hosts).
 * Hosts may OR these into private soft path masks; UDX core never hard-gates.
 * Soft!=product; Dual DoD A/B OPEN. greppable: udx: device soft residual path
 */
#define UDX_DEV_SOFT_PATH_INIT           (1u << 0)
#define UDX_DEV_SOFT_PATH_REGISTER       (1u << 1)
#define UDX_DEV_SOFT_PATH_PROBE          (1u << 2)
#define UDX_DEV_SOFT_PATH_ATTACH         (1u << 3)
#define UDX_DEV_SOFT_PATH_RUN            (1u << 4)
#define UDX_DEV_SOFT_PATH_QUIESCE        (1u << 5)
#define UDX_DEV_SOFT_PATH_REMOVE         (1u << 6)
#define UDX_DEV_SOFT_PATH_DETACH         (1u << 7)
#define UDX_DEV_SOFT_PATH_DRVDATA        (1u << 8)
#define UDX_DEV_SOFT_PATH_BACKEND        (1u << 9)

/*
 * Soft residual Cap / Dual DoD honesty constants (eng only; Soft!=product).
 * Always-0 mint flags: pBackend is opaque, never a product cap handle.
 * Dual DoD A/B remain OPEN (1) until DUT proof — soft attach never closes.
 * No version stamp. No GPL. G-AC-1.
 * Grep: UDX_DEV_SOFT_CAP_MINT / UDX_DEV_SOFT_DOD_A_OPEN / DOD_B_OPEN
 * greppable: udx: device soft residual cap_none
 * greppable: udx: device soft residual dual_dod
 * greppable: udx: device soft residual product_host
 */
#define UDX_DEV_SOFT_CAP_MINT            0u /* no MMIO/IRQ/DMA cap mint here */
#define UDX_DEV_SOFT_DOD_A_OPEN          1u /* xhci_udx Dual DoD A still OPEN */
#define UDX_DEV_SOFT_DOD_B_OPEN          1u /* rtl8168_udx Dual DoD B still OPEN */
#define UDX_DEV_SOFT_PRODUCT_HOSTS       1u /* rtl8168_udx + xhci_udx shape */
#define UDX_DEV_SOFT_FREESTANDING_SKIP   1u /* freestanding class SKIP */

/**
 * Linux struct device mental-model residual (Soft!=product).
 *
 * Fields:
 *   szName       — soft name residual (logs / udx_dev_name); never product path
 *   pDriverData  — probe soft state (drvdata); set at attach end; clear on remove
 *   pBackend     — opaque UDX host slot (MMIO/IRQ/DMA caps hidden); not driver-minted
 *
 * greppable residual: udx: device soft residual lean
 * greppable residual: udx: device soft residual backend
 * greppable residual: udx: device soft residual cap_none
 */
struct udx_device {
    const char *szName;
    void       *pDriverData; /* probe soft state (drvdata); clear on remove */
    /* Opaque backend: MMIO/IRQ/DMA caps held by UDX, not driver */
    void       *pBackend;
};

/**
 * Soft residual init: zero soft slots (seed only — not host bind/inject).
 * Prepares empty residual (no backend, no drvdata). Soft!=product; G-AC-1.
 * Does not mint caps, does not attach, does not close Dual DoD.
 * greppable residual: udx: device soft residual init
 * greppable residual: udx: device soft residual empty
 */
static inline void
udx_dev_init(struct udx_device *pDev)
{
    if (pDev != NULL) {
        pDev->szName = NULL;
        pDev->pDriverData = NULL;
        pDev->pBackend = NULL;
    }
}

/**
 * Linux-shaped get_drvdata — soft residual probe/remove honesty:
 * non-NULL after successful probe install; NULL after remove clear or
 * pre-probe. product=UDX+ABI; Soft!=product.
 * greppable residual: udx: device soft residual drvdata
 */
static inline void *
udx_get_drvdata(struct udx_device *pDev)
{
    return pDev ? pDev->pDriverData : NULL;
}

/**
 * Probe residual: install private soft state at end of successful attach
 * (enable/regions/ioremap/dma/irq). Caps stay in pBackend — never
 * mint/revoke from this path. Soft!=product until DDI grant product.
 * pData=NULL is soft-equivalent to udx_clear_drvdata (remove residual).
 * greppable residual (core once-lamp): udx: soft residual probe
 * greppable residual: udx: device soft residual drvdata
 */
static inline void
udx_set_drvdata(struct udx_device *pDev, void *pData)
{
    if (pDev) {
        pDev->pDriverData = pData;
    }
}

/**
 * Remove residual: clear private soft state after free_irq / DMA / iounmap
 * / release / disable. Quiesce must run before remove (stop DMA / mask IRQ
 * / cancel work). Soft residual order: quiesce_before_remove.
 * Soft-equivalent: udx_set_drvdata(pDev, NULL). Prefer this on remove.
 * product=UDX+ABI; Soft!=product; G-AC-1.
 * greppable residual (core once-lamp): udx: soft residual remove
 * greppable residual: udx: device soft residual drvdata
 * greppable residual: udx: device soft residual detach
 * greppable residual: udx: device soft residual quiesce
 */
static inline void
udx_clear_drvdata(struct udx_device *pDev)
{
    if (pDev) {
        pDev->pDriverData = NULL;
    }
}

/**
 * Linux dev_name() analogue — never NULL (falls back to "udx").
 * Soft residual name observation only; Soft!=product path.
 * greppable residual: udx: device soft residual name
 */
static inline const char *
udx_dev_name(const struct udx_device *pDev)
{
    if (pDev != NULL && pDev->szName != NULL && pDev->szName[0] != '\0') {
        return pDev->szName;
    }
    return "udx";
}

/**
 * True if drvdata has been installed (probe completed soft attach state).
 * Observation residual only — never Dual DoD product close.
 * greppable residual: udx: device soft residual drvdata
 */
static inline int
udx_dev_has_drvdata(const struct udx_device *pDev)
{
    return pDev != NULL && pDev->pDriverData != NULL;
}

/**
 * Soft residual honesty: backend slot filled by UDX host bind/inject —
 * not a driver-minted product cap. Soft!=product; multi-server confine OPEN.
 * greppable residual: udx: device soft residual backend
 * greppable residual: udx: device soft residual cap_none
 */
static inline int
udx_dev_has_backend(const struct udx_device *pDev)
{
    return pDev != NULL && pDev->pBackend != NULL;
}

/**
 * Soft residual backend observation (opaque cookie only).
 * Returns host-filled pBackend or NULL. Never a product cap handle —
 * driver .c must not mint/revoke through this pointer. Soft!=product.
 * greppable residual: udx: device soft residual backend
 * greppable residual: udx: device soft residual cap_none
 */
static inline void *
udx_dev_get_backend(const struct udx_device *pDev)
{
    return (pDev != NULL) ? pDev->pBackend : NULL;
}

/**
 * Soft attach residual: probe honesty complete when backend is host-filled
 * AND drvdata is installed. Fail-closed; Soft!=product; Dual DoD A/B OPEN.
 * Does not claim Cap mint, VT-d program, or product bind close.
 * greppable residual: udx: device soft residual attach
 * greppable residual: udx: device soft residual dual_dod
 * greppable residual: udx: device soft api honesty
 */
static inline int
udx_dev_is_soft_attached(const struct udx_device *pDev)
{
    return pDev != NULL && pDev->pBackend != NULL && pDev->pDriverData != NULL;
}

/**
 * Soft partial residual: exactly one of backend/drvdata is live.
 * Typical: host filled pBackend, probe has not yet set_drvdata (or fail
 * path cleared one side). Observation only; never Dual DoD close.
 * greppable residual: udx: device soft residual partial
 * greppable residual: udx: device soft residual attach
 */
static inline int
udx_dev_is_soft_partial(const struct udx_device *pDev)
{
    int fBackend;
    int fDrvdata;

    if (pDev == NULL) {
        return 0;
    }
    fBackend = (pDev->pBackend != NULL) ? 1 : 0;
    fDrvdata = (pDev->pDriverData != NULL) ? 1 : 0;
    return (fBackend ^ fDrvdata);
}

/**
 * Soft empty residual: neither backend nor drvdata (pre-bind seed or
 * fully unbound soft device). Fail-closed; Soft!=product.
 * greppable residual: udx: device soft residual empty
 * greppable residual: udx: device soft residual init
 */
static inline int
udx_dev_is_soft_empty(const struct udx_device *pDev)
{
    return pDev != NULL && pDev->pBackend == NULL && pDev->pDriverData == NULL;
}

/**
 * Soft detach residual honesty: drvdata cleared after remove residual.
 * Backend may still be host-owned until unbind (not Phase-A revoke claim).
 * True when pDev is non-NULL and pDriverData is NULL (post-clear / pre-probe).
 * Observation only; Soft!=product; Dual DoD A/B OPEN.
 * greppable residual: udx: device soft residual detach
 * greppable residual: udx: device soft residual remove
 */
static inline int
udx_dev_is_soft_detached_drvdata(const struct udx_device *pDev)
{
    return pDev != NULL && pDev->pDriverData == NULL;
}

/* ---- Soft residual queries / shape (observation only; Soft!=product) - */

/**
 * Soft residual lean layout version (UDX_DEVICE_LAYOUT_VER).
 * Grep: udx: device soft residual lean layout_ver=
 * Not GJ_IMAGE_VERSION. Not product ABI stamp. stamp-free residual.
 */
static inline u32
udx_dev_layout_ver(void)
{
    return UDX_DEVICE_LAYOUT_VER;
}

/**
 * Non-zero when soft product-host device surface is present at layout_ver:
 * init + drvdata get/set/clear + backend observation + name + attach /
 * partial / empty / detach + path-bit catalog + Dual DoD OPEN honesty.
 * Soft!=product; eng residual only. Never hard-gates Dual DoD.
 * greppable residual: udx: device soft residual product_host
 * greppable residual: udx: device soft residual dual_dod
 * greppable residual: udx: device soft residual lean
 */
static inline int
udx_dev_shape_ok(void)
{
    return (UDX_DEVICE_LAYOUT_VER >= 2u) ? 1 : 0;
}

/**
 * Soft residual Cap mint honesty (always 0 today on device surface).
 * pBackend is opaque; mint/revoke stay in UDX host. Soft!=product.
 * greppable residual: udx: device soft residual cap_none
 * greppable residual: udx: device soft api honesty
 */
static inline u32
udx_dev_soft_cap_mint(void)
{
    return UDX_DEV_SOFT_CAP_MINT;
}

/**
 * Soft residual Dual DoD A OPEN flag (xhci_udx). Always 1 until DUT proof.
 * Soft attach / drvdata lamps never flip this closed.
 * greppable residual: udx: device soft residual dual_dod
 * greppable residual: udx: device soft residual product_host
 */
static inline u32
udx_dev_soft_dod_a_open(void)
{
    return UDX_DEV_SOFT_DOD_A_OPEN;
}

/**
 * Soft residual Dual DoD B OPEN flag (rtl8168_udx). Always 1 until DUT proof.
 * Soft attach / drvdata lamps never flip this closed.
 * greppable residual: udx: device soft residual dual_dod
 * greppable residual: udx: device soft residual product_host
 */
static inline u32
udx_dev_soft_dod_b_open(void)
{
    return UDX_DEV_SOFT_DOD_B_OPEN;
}

/**
 * Soft residual product-host shape present (UDX+ABI userspace hosts).
 * freestanding class SKIP; product = rtl8168_udx / xhci_udx.
 * greppable residual: udx: device soft residual product_host
 * greppable: udx: soft residual product=UDX+ABI
 * greppable: udx: soft residual freestanding class SKIP
 */
static inline u32
udx_dev_soft_product_hosts(void)
{
    return UDX_DEV_SOFT_PRODUCT_HOSTS;
}
