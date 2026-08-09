/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * UDX basic types (Linux-porter surface). No kernel caps exposed.
 *
 * Greppable product markers (also in README / init log):
 *   GREENJADE_UDX
 *   UDX_PRODUCT
 *   UDX+ABI          -- product direction (Linux-shaped userspace hosts)
 *
 * Soft residual honesty (once-lamps lean; Soft!=product; G-AC-1):
 *   udx: soft residual lean
 *   udx: soft residual product=UDX+ABI
 *   udx: soft residual driver lifecycle   (rollup)
 *   udx: soft residual register / probe / quiesce / remove  (phase deepen)
 *   udx: soft residual status             (OK / ERR_* catalog + helpers)
 *   udx: soft residual cap_none           (no cap handle typedef here)
 *   udx: soft residual dual_dod           (A/B OPEN seed; never close)
 *   udx: soft residual freestanding class SKIP
 *   udx: soft residual api honesty
 *
 * Soft residual deepen (C2 types surface; Soft!=product; dual MIT OR Apache-2.0):
 *   scalar residual   -- fixed-width uN/sN aliases; freestanding-safe
 *   address residual  -- udx_dma_addr_t / udx_phys_addr_t (cookie/PA only;
 *                       never a DMA window / MMIO_FRAME / IRQ Notification
 *                       product cap handle -- mint OPEN elsewhere)
 *   status residual   -- Linux-ish 0 / negative ERR_xx; helpers
 *                       ok/failed/str/is_known/is_retry (observation +
 *                       fail-closed; never gate Dual DoD close)
 *   product residual  -- GREENJADE_UDX / UDX_PRODUCT / product=UDX+ABI
 *                       hosts=rtl8168_udx,xhci_udx; freestanding class SKIP
 *   Dual DoD residual -- A=xhci_udx USB OPEN; B=rtl8168_udx NIC OPEN
 *                       (soft seed only; Soft!=product; G-AC-1 no .ko AC)
 *   cap residual      -- deliberately NO cap slot/gen/ref typedef in this
 *                       header; pBackend remains opaque; driver .c never
 *                       mints/revokes (see device.h / host.h / dma.h);
 *                       UDX_CAP_MINT_SOFT=0 (mint honesty OPEN product)
 *   product_host residual -- rtl8168_udx DoD B / xhci_udx DoD A catalog;
 *                       shape_ok / layout_ver observation only
 *   freestanding residual -- class drivers SKIP default; product=UDX+ABI
 *                       userspace hosts only (G-AC-1)
 *
 * Soft API honesty (Soft!=product; dual MIT OR Apache-2.0; G-AC-1):
 *   soft path = scalar/address/status typedefs + observation helpers +
 *               product/DoD catalog macros (seed only)
 *   product   = Cap mint/revoke of MMIO_FRAME / IRQ Notification / DMA
 *               window + Dual DoD A/B close (OPEN; never claimed here)
 *   freestanding class drivers (kernel rtl8168 / xhci_msc) = SKIP default
 *   product hosts = Linux-shaped userspace UDX (rtl8168_udx / xhci_udx)
 *   soft_ne_product=1; cap_mint_soft=0; Dual DoD A/B remain OPEN
 *
 * greppable: udx: soft residual lean
 * greppable: udx: soft residual lean layout_ver=
 * greppable: udx: soft residual product=UDX+ABI
 * greppable: udx: soft residual driver lifecycle
 * greppable: udx: soft residual register
 * greppable: udx: soft residual probe
 * greppable: udx: soft residual quiesce
 * greppable: udx: soft residual remove
 * greppable: udx: soft residual status
 * greppable: udx: soft residual scalar
 * greppable: udx: soft residual address
 * greppable: udx: soft residual cap_none
 * greppable: udx: soft residual dual_dod
 * greppable: udx: soft residual freestanding class SKIP
 * greppable: udx: soft residual api honesty
 * greppable: udx: soft residual product_host
 * greppable: udx: types soft residual lean
 * greppable: udx: types soft residual scalar
 * greppable: udx: types soft residual address
 * greppable: udx: types soft residual status
 * greppable: udx: types soft residual dual_dod
 * greppable: udx: types soft residual cap_none
 * greppable: udx: types soft residual product_host
 * greppable: udx: types soft api honesty
 * Dual MIT OR Apache-2.0. Freestanding class drivers SKIP. No GPL.
 * No version stamp. No GJ_IMAGE_VERSION. Dual DoD A/B remain OPEN.
 * soft_ne_product=1 cap_mint_soft=0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

/*
 * Soft residual lean layout version (eng only; Soft!=product ABI stamp).
 * Not GJ_IMAGE_VERSION. Not product close. Bump only when types residual
 * surface grows (helpers / catalog honesty), never for flash bar stamps.
 * ver1: product markers + scalar/address/status + Dual DoD seed macros
 * ver2: status helpers (ok/failed/str) + residual deepen catalog
 * ver3: Soft API honesty + freestanding SKIP + product_host residual +
 *       status is_known/is_retry + address null helpers + Dual DoD /
 *       cap_mint observation helpers + layout_ver/shape_ok (C2 residual)
 * Grep: udx: soft residual lean layout_ver=
 * Grep: udx: types soft residual lean
 */
#define UDX_TYPES_LAYOUT_VER         3u

/* Soft!=product honesty seed (observation macros; never product lamps). */
#define UDX_SOFT_NE_PRODUCT          1u  /* soft residual != product close */
#define UDX_CAP_MINT_SOFT            0u  /* no cap handle/mint in this header */
#define UDX_FREESTANDING_CLASS_SKIP  1u  /* kernel class drivers SKIP default */

/* Product identity -- greppable in binaries and docs. */
#define UDX_PRODUCT_NAME      "GREENJADE_UDX"
#define UDX_PRODUCT_TAG       "UDX_PRODUCT"
#define UDX_PRODUCT_VERSION   "1.3"
/*
 * Product direction residual (Soft!=product honesty):
 * Linux-shaped userspace UDX hosts over hot+cold ABI + DDI*UDX --
 * not freestanding kernel class drivers, not in-kernel .ko AC (G-AC-1).
 * greppable: UDX+ABI / product=UDX+ABI
 * greppable residual: udx: soft residual product=UDX+ABI
 * greppable residual: udx: soft residual freestanding class SKIP
 */
#define UDX_PRODUCT_DIRECTION "UDX+ABI"

/*
 * Dual DoD residual seed (Soft!=product; never claim close from soft).
 *   A -- Linux-shaped USB userspace host (xhci_udx + DDI)  OPEN
 *   B -- Linux-shaped NIC userspace host (rtl8168_udx + DDI) OPEN
 * greppable: udx: soft residual dual_dod / Dual DoD A/B OPEN
 * greppable: udx: types soft residual dual_dod
 * greppable: udx: soft residual product_host
 */
#define UDX_DOD_A_HOST   "xhci_udx"
#define UDX_DOD_B_HOST   "rtl8168_udx"
#define UDX_DOD_STATE    "OPEN"

/* ---- Fixed-width scalars (freestanding-safe; Linux-porter short names) -- */
/*
 * Prefer these over bare int/long in driver soft state and wire formats.
 * Signed s* are for Linux-ish status edges / offsets; prefer u* for sizes,
 * bitfields, and generation counters. Soft residual: scalar residual only --
 * not a product ABI claim beyond the UDX header surface.
 * greppable residual: udx: soft residual scalar
 * greppable residual: udx: types soft residual scalar
 */
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

/*
 * Address residual (Soft!=product):
 *   udx_dma_addr_t  -- DMA cookie / bus address bookkeeping (identity soft
 *                     path today; product DMA window cap mint OPEN)
 *   udx_phys_addr_t -- BAR / map grant PA bookkeeping (ioremap window lookup;
 *                     product MMIO_FRAME cap mint OPEN)
 * Neither is a capability handle. Cap mint/revoke stay hidden in host/backend.
 * greppable: udx: soft residual cap_none
 * greppable: udx: soft residual address
 * greppable: udx: types soft residual address
 * greppable: udx: types soft residual cap_none
 */
typedef u64 udx_dma_addr_t;
typedef u64 udx_phys_addr_t;

/**
 * Soft address residual: true iff DMA cookie is the null/zero cookie.
 * Identity soft path uses zero as mapping_error; not a cap null handle.
 * Observation only; Soft!=product; never Dual DoD close.
 * greppable residual: udx: soft residual address
 * greppable residual: udx: types soft residual address
 * greppable residual: udx: soft residual cap_none
 */
static inline int
udx_dma_addr_is_null(udx_dma_addr_t dma)
{
    return dma == (udx_dma_addr_t)0;
}

/**
 * Soft address residual: true iff phys PA bookkeeping is zero.
 * Not an MMIO_FRAME cap null; not a product map grant claim.
 * greppable residual: udx: soft residual address
 * greppable residual: udx: types soft residual address
 * greppable residual: udx: soft residual cap_none
 */
static inline int
udx_phys_addr_is_null(udx_phys_addr_t pa)
{
    return pa == (udx_phys_addr_t)0;
}

/*
 * Status residual (Linux-ish; Soft!=product):
 *   0            success (UDX_OK)
 *   negative     UDX_ERR_* failure class
 * Never a positive "errno success". Callers: st == UDX_OK / st < 0.
 * Helpers below are observation + fail-closed shape -- they never gate
 * Dual DoD A/B close or product DoD lamps.
 * greppable: udx: soft residual status
 * greppable: udx: types soft residual status
 */
typedef int udx_status_t;

#define UDX_OK            0
#define UDX_ERR_INVAL    (-1)  /* bad arg / null / span */
#define UDX_ERR_NOMEM    (-2)  /* soft alloc / pool exhausted */
#define UDX_ERR_NODEV    (-3)  /* no match / empty inventory */
#define UDX_ERR_BUSY     (-4)  /* claim overlap / in-flight */
#define UDX_ERR_IO       (-5)  /* map / enable / host I/O fail */
#define UDX_ERR_NOSYS    (-6)  /* soft not-granted / no syscall */
#define UDX_ERR_NODEVIRQ (-7)  /* irq table / line residual fail */
#define UDX_ERR_NODMA    (-8)  /* dma cookie / window residual fail */
#define UDX_ERR_AGAIN    (-9)  /* soft retry / would-block residual */

/**
 * Soft residual: true iff status is exact success (UDX_OK).
 * greppable residual: udx: soft residual status
 * greppable residual: udx: types soft residual status
 */
static inline int
udx_status_ok(udx_status_t st)
{
    return st == UDX_OK;
}

/**
 * Soft residual: true iff status is a negative ERR_* class.
 * Positive nonzero is not success and not a defined ERR_* class.
 * greppable residual: udx: soft residual status
 */
static inline int
udx_status_failed(udx_status_t st)
{
    return st < 0;
}

/**
 * Soft residual: true iff status is a catalogued UDX_OK / UDX_ERR_* code.
 * Unknown (incl. positive nonzero) → 0 (fail-closed known residual).
 * Observation only; never Dual DoD product close.
 * greppable residual: udx: soft residual status
 * greppable residual: udx: types soft residual status
 */
static inline int
udx_status_is_known(udx_status_t st)
{
    switch (st) {
    case UDX_OK:
    case UDX_ERR_INVAL:
    case UDX_ERR_NOMEM:
    case UDX_ERR_NODEV:
    case UDX_ERR_BUSY:
    case UDX_ERR_IO:
    case UDX_ERR_NOSYS:
    case UDX_ERR_NODEVIRQ:
    case UDX_ERR_NODMA:
    case UDX_ERR_AGAIN:
        return 1;
    default:
        return 0;
    }
}

/**
 * Soft residual: true iff status is soft-retry (UDX_ERR_AGAIN).
 * Would-block / soft retry residual only — not product poll ABI.
 * greppable residual: udx: soft residual status
 * greppable residual: udx: types soft residual status
 */
static inline int
udx_status_is_retry(udx_status_t st)
{
    return st == UDX_ERR_AGAIN;
}

/**
 * Soft residual status name (observation / host logs only).
 * Stable string tokens for greppable soft diagnostics -- not product ABI.
 * Unknown codes return "UDX_ERR_UNKNOWN" (fail-closed naming).
 * greppable: udx: soft residual status
 * greppable: udx: types soft residual status
 */
static inline const char *
udx_status_str(udx_status_t st)
{
    switch (st) {
    case UDX_OK:            return "UDX_OK";
    case UDX_ERR_INVAL:     return "UDX_ERR_INVAL";
    case UDX_ERR_NOMEM:     return "UDX_ERR_NOMEM";
    case UDX_ERR_NODEV:     return "UDX_ERR_NODEV";
    case UDX_ERR_BUSY:      return "UDX_ERR_BUSY";
    case UDX_ERR_IO:        return "UDX_ERR_IO";
    case UDX_ERR_NOSYS:     return "UDX_ERR_NOSYS";
    case UDX_ERR_NODEVIRQ:  return "UDX_ERR_NODEVIRQ";
    case UDX_ERR_NODMA:     return "UDX_ERR_NODMA";
    case UDX_ERR_AGAIN:     return "UDX_ERR_AGAIN";
    default:                return "UDX_ERR_UNKNOWN";
    }
}

/* ---- Soft residual queries (observation only; Soft!=product) ----------- */

/**
 * Soft residual lean layout version (UDX_TYPES_LAYOUT_VER).
 * Grep: udx: soft residual lean layout_ver=
 * Grep: udx: types soft residual lean
 * Not GJ_IMAGE_VERSION. Not product ABI stamp.
 */
static inline u32
udx_types_layout_ver(void)
{
    return UDX_TYPES_LAYOUT_VER;
}

/**
 * Soft residual: Dual DoD A/B state is OPEN (soft seed honesty).
 * Always true while UDX_DOD_STATE is "OPEN". Soft residual never closes
 * Dual DoD from this header. Soft!=product; G-AC-1.
 * greppable residual: udx: soft residual dual_dod
 * greppable residual: udx: types soft residual dual_dod
 */
static inline int
udx_dod_is_open(void)
{
    return (UDX_DOD_STATE[0] == 'O' &&
            UDX_DOD_STATE[1] == 'P' &&
            UDX_DOD_STATE[2] == 'E' &&
            UDX_DOD_STATE[3] == 'N' &&
            UDX_DOD_STATE[4] == '\0') ? 1 : 0;
}

/**
 * Soft residual: product direction is UDX+ABI (Linux-shaped userspace).
 * Observation only; freestanding class SKIP remains default.
 * greppable residual: udx: soft residual product=UDX+ABI
 * greppable residual: udx: soft residual freestanding class SKIP
 * greppable residual: udx: soft residual product_host
 */
static inline int
udx_product_direction_is_udx_abi(void)
{
    return (UDX_PRODUCT_DIRECTION[0] == 'U' &&
            UDX_PRODUCT_DIRECTION[1] == 'D' &&
            UDX_PRODUCT_DIRECTION[2] == 'X' &&
            UDX_PRODUCT_DIRECTION[3] == '+' &&
            UDX_PRODUCT_DIRECTION[4] == 'A' &&
            UDX_PRODUCT_DIRECTION[5] == 'B' &&
            UDX_PRODUCT_DIRECTION[6] == 'I' &&
            UDX_PRODUCT_DIRECTION[7] == '\0') ? 1 : 0;
}

/**
 * Soft residual cap-mint honesty for this header: always 0.
 * types.h deliberately exposes no cap slot/gen/ref; mint stays OPEN
 * in host/backend (device.h / host.h / dma.h). Soft!=product; G-AC-1.
 * greppable residual: udx: soft residual cap_none
 * greppable residual: udx: types soft residual cap_none
 * greppable residual: udx: soft residual api honesty
 */
static inline u32
udx_cap_mint_soft(void)
{
    return UDX_CAP_MINT_SOFT;
}

/**
 * Soft residual freestanding class SKIP honesty (fixed seed).
 * Kernel freestanding class drivers are not product; product=UDX+ABI.
 * greppable residual: udx: soft residual freestanding class SKIP
 * greppable residual: udx: soft residual product=UDX+ABI
 */
static inline u32
udx_freestanding_class_skip(void)
{
    return UDX_FREESTANDING_CLASS_SKIP;
}

/**
 * Soft residual Soft!=product honesty seed (always 1).
 * greppable residual: udx: soft residual api honesty
 * greppable residual: udx: types soft api honesty
 */
static inline u32
udx_soft_ne_product(void)
{
    return UDX_SOFT_NE_PRODUCT;
}

/**
 * Non-zero when soft types residual surface is present at layout_ver:
 * scalars + address cookies + status catalog/helpers + Dual DoD OPEN
 * seed + product=UDX+ABI + freestanding SKIP + cap_none mint honesty.
 * Soft!=product; eng residual only. Never hard-gates Dual DoD.
 * greppable: udx: soft residual product_host
 * greppable: udx: types soft residual product_host
 * greppable: udx: types soft residual lean
 * greppable: udx: soft residual api honesty
 * greppable: udx: types soft api honesty
 */
static inline int
udx_types_shape_ok(void)
{
    return (UDX_TYPES_LAYOUT_VER >= 1u &&
            UDX_SOFT_NE_PRODUCT != 0u &&
            UDX_CAP_MINT_SOFT == 0u &&
            UDX_FREESTANDING_CLASS_SKIP != 0u &&
            udx_dod_is_open() != 0 &&
            udx_product_direction_is_udx_abi() != 0) ? 1 : 0;
}

#ifndef NULL
#define NULL ((void *)0)
#endif
