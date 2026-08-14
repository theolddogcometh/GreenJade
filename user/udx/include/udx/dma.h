/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Linux-shaped DMA API over GreenJade IOMMU window caps (hidden).
 *
 * Host (UDX_HOST_LIBC): calloc identity cookies + soft DDI DMA_BUF slots.
 * Freestanding: static DMA slab (no libc heap); grant via PLATFORM_INFO op5;
 *               DDI DMA_BUF via GJ_SYS_DDI ops 10/11/12.
 *
 * Product UDX hosts (rtl8168_udx / xhci_udx) use this surface for:
 *   - coherent maps  : udx_dma_alloc_coherent / free_coherent
 *   - stream maps    : udx_dma_map_single / unmap_single + sync_single_*
 *   - force32 notes  : geometric low / VT-d identity helpers (Soft!=product)
 *   - DDI DMA_BUF    : soft door hooks (ops 10/11/12) + host soft slots
 *
 * Product-host shape residual (Dual DoD A/B OPEN seed; Soft!=product):
 *   rtl8168_udx (DoD B): coherent ring + iommu_grant + sync_for_device
 *   xhci_udx    (DoD A): coherent / stream residual (TRB/xfer later)
 *   both: force32 geometric note; DDI DMA_BUF chain when door present
 *   Never claims Dual DoD A/B close; product DMA window cap mint OPEN.
 *
 * Soft residual deepen (UDX product hosts; Soft!=product; G-AC-1):
 *   coherent residual  -- udx_dma_alloc_coherent / free_coherent;
 *                          identity cookie = CPU VA cast (soft);
 *                          != product DMA window cap mint / VT-d pages
 *   stream residual    -- udx_dma_map_single / unmap_single + mapping_error;
 *                          identity cookie; refuse size > UDX_DMA_MAP_MAX;
 *                          soft map-live bookkeeping (impl); != IOMMU PTE
 *   sync residual      -- udx_dma_sync_single_for_{cpu,device};
 *                          host mfence / barrier only; != full CWB + IOTLB
 *   iommu residual     -- udx_dma_iommu_grant + bdf pack;
 *                          freestanding PLATFORM_INFO op5; host nop;
 *                          soft mint=0 (not CNode DMA window grant)
 *   force32 residual   -- UDX_DMA_F_FORCE32 note + is_low / in_vtd_identity
 *                          / window_ok geometric; DDI DMA_BUF bit0 prefer;
 *                          != product force32 page mint under TE
 *   mask residual      -- set_mask / set_coherent_mask / get_* /
 *                          max_mapping_size; process-global soft only;
 *                          != struct device mask embed / IOMMU program
 *   ddi_buf residual   -- ops 10/11/12 + host soft slots + chain
 *                          ALLOC→MAP→FREE; Soft!=product door mint
 *   window residual    -- geometric window_ok / range_ok; != live IOMMU PTE
 *                          query / product DMA window grant (OPEN)
 *   mint honesty       -- identity cookie / host nop / force32 note /
 *                          ddi_buf soft slots never claim DMA window cap,
 *                          CNode mint, or live VT-d program (OPEN);
 *                          soft_mint=0 / product_mint=OPEN always today
 *   product_host residual -- rtl8168_udx DoD B OPEN / xhci_udx DoD A OPEN
 *                          catalog only; Dual DoD never closed here
 *   dual_dod residual  -- Dual DoD A (xhci_udx) / B (rtl8168_udx) OPEN;
 *                          residual lamps / greppables / helpers never close
 *   soft_ne_product    -- soft_ne_product=1 always; Soft!=product law;
 *                          never .ko product AC (G-AC-1)
 *
 * Soft API honesty (Soft!=product; dual MIT OR Apache-2.0; G-AC-1):
 *   soft path = identity cookie + host nop / soft iommu_grant
 *   product   = DMA window cap + live VT-d (OPEN; not claimed here)
 *   force32   = geometric note / DDI DMA_BUF bit0; not product mint
 *   Dual DoD  = A (xhci_udx) / B (rtl8168_udx) remain OPEN
 *   soft_ne_product=1 always (Soft!=product; never product close)
 * greppable: udx: dma soft residual lean
 * greppable: udx: dma soft residual lean UDX eng
 * greppable: udx: dma soft residual lean layout_ver=
 * greppable: udx: dma soft residual coherent
 * greppable: udx: dma soft residual stream
 * greppable: udx: dma soft residual sync
 * greppable: udx: dma soft residual iommu
 * greppable: udx: dma soft residual force32
 * greppable: udx: dma soft residual mask
 * greppable: udx: dma soft residual ddi_buf
 * greppable: udx: dma soft residual window
 * greppable: udx: dma soft residual mint
 * greppable: udx: dma soft residual product_host
 * greppable: udx: dma soft residual dual_dod
 * greppable: udx: dma soft residual dual_dod_open
 * greppable: udx: dma soft residual soft_ne_product
 * greppable: udx: dma soft api honesty
 * greppable: udx: dma soft force32
 * greppable: udx: dma soft ddi_buf
 * greppable: udx: dma soft product host
 * greppable: udx: dma soft mint honesty
 * greppable: soft_ne_product=1
 * No stamp storms. No version stamp. No invent stamp. Lean residual only.
 * Bar honesty v2026.08.04.75 stamp-free. NEVER bump GJ_IMAGE_VERSION.
 * Dual MIT OR Apache-2.0. No GPL. G-AC-1 (no .ko product AC).
 * Dual DoD A/B OPEN. Soft!=product.
 */
#pragma once

#include <udx/types.h>

struct udx_device;

/*
 * Soft residual lean layout version (eng only; Soft!=host ABI; not stamp).
 * Bump when product-host DMA residual surface grows (coherent/stream/
 * force32/ddi_buf/window/dual_dod). Not GJ_IMAGE_VERSION. Not product ABI.
 * ver1: coherent + stream + force32 note + DDI DMA_BUF hooks + mask residual
 * ver2: host soft DDI DMA_BUF slots; ddi_buf_chain; get_mask; shape_ok;
 *       product host catalog (rtl8168_udx/xhci_udx); soft mint honesty;
 *       residual lean UDX eng once-lamp.
 * ver3: soft residual deepen catalog (coherent/stream/sync/iommu/force32/
 *       mask/ddi_buf/mint/product_host greppables); header geometric
 *       helpers (cookie_from_cpu / dir_ok / map_size_ok / ddi_pages_*);
 *       peer residual honesty vs mmio/irq/work deepen style.
 * ver4: window + dual_dod + soft_ne_product residual greppables; soft
 *       path-bit catalog; mint_open / dual_dod_open / soft_ne_product
 *       header helpers; range_ok geometric; Dual DoD A/B OPEN catalog
 *       parity with mmio/work/device residual style (Soft!=product).
 * Grep: udx: dma soft residual lean layout_ver=
 */
#define UDX_DMA_LAYOUT_VER           4u

/* Soft DDI DMA_BUF page size (kernel dma_buf residual; Soft!=product). */
#define UDX_DMA_BUF_PAGE_CB          4096u

/* Soft host DDI DMA_BUF residual slot budget (Soft!=product quota). */
#define UDX_DMA_SOFT_DDI_SLOT_MAX    16u

/* Soft DDI DMA_BUF page bound (match ddi_door residual; Soft!=product). */
#define UDX_DMA_SOFT_DDI_PAGES_MAX   16u

/* Soft stream map-live residual slots (bookkeeping only). */
#define UDX_DMA_SOFT_MAP_SLOT_MAX    32u

/*
 * Soft residual mint honesty constants (Soft!=product; always soft today).
 * greppable: udx: dma soft residual mint
 * greppable: udx: dma soft mint honesty
 * greppable: soft_ne_product=1
 */
#define UDX_DMA_SOFT_MINT            0u /* no DMA window / CNode mint today */
#define UDX_DMA_SOFT_NE_PRODUCT      1u /* soft_ne_product=1 always */

/*
 * Dual DoD residual seed (Soft!=product; never claim close from soft).
 * Aligns types.h UDX_DOD_* spirit; header-local greppable catalog only.
 *   A -- Linux-shaped USB userspace host (xhci_udx + DDI)  OPEN
 *   B -- Linux-shaped NIC userspace host (rtl8168_udx + DDI) OPEN
 * greppable: udx: dma soft residual dual_dod
 * greppable: udx: dma soft residual dual_dod_open
 */
#define UDX_DMA_DOD_A_HOST           "xhci_udx"
#define UDX_DMA_DOD_B_HOST           "rtl8168_udx"
#define UDX_DMA_DOD_STATE            "OPEN"

/*
 * Soft residual path-bit catalog (once-lamp spirit for product hosts).
 * Hosts may OR into private soft path masks; UDX never hard-gates Dual DoD.
 * greppable: udx: dma soft residual lean
 */
#define UDX_DMA_SOFT_PATH_COHERENT     (1u << 0)
#define UDX_DMA_SOFT_PATH_STREAM       (1u << 1)
#define UDX_DMA_SOFT_PATH_SYNC         (1u << 2)
#define UDX_DMA_SOFT_PATH_IOMMU        (1u << 3)
#define UDX_DMA_SOFT_PATH_FORCE32      (1u << 4)
#define UDX_DMA_SOFT_PATH_MASK         (1u << 5)
#define UDX_DMA_SOFT_PATH_DDI_BUF      (1u << 6)
#define UDX_DMA_SOFT_PATH_WINDOW       (1u << 7)
#define UDX_DMA_SOFT_PATH_MINT         (1u << 8)
#define UDX_DMA_SOFT_PATH_PRODUCT_HOST (1u << 9)
#define UDX_DMA_SOFT_PATH_DUAL_DOD     (1u << 10)
/* Full soft residual surface present at layout_ver >= 4 (catalog only). */
#define UDX_DMA_SOFT_PATH_SURFACE \
    (UDX_DMA_SOFT_PATH_COHERENT | UDX_DMA_SOFT_PATH_STREAM | \
     UDX_DMA_SOFT_PATH_SYNC | UDX_DMA_SOFT_PATH_IOMMU | \
     UDX_DMA_SOFT_PATH_FORCE32 | UDX_DMA_SOFT_PATH_MASK | \
     UDX_DMA_SOFT_PATH_DDI_BUF | UDX_DMA_SOFT_PATH_WINDOW | \
     UDX_DMA_SOFT_PATH_MINT | UDX_DMA_SOFT_PATH_PRODUCT_HOST | \
     UDX_DMA_SOFT_PATH_DUAL_DOD)

/* ---- Direction (Linux dma_data_direction ordinals) --------------------- */

enum udx_dma_data_direction {
    UDX_DMA_BIDIRECTIONAL = 0,
    UDX_DMA_TO_DEVICE     = 1,
    UDX_DMA_FROM_DEVICE   = 2,
    UDX_DMA_NONE          = 3,
};

/*
 * Geometric ceilings (mirror kernel dma_buf / GJ_DMA_*; Soft!=product).
 * Prefer DMA cookies in VT-d identity [0, 1 GiB) for force32 masters
 * (rtl8168-class addr_hi=0 under TE). Soft checks only — no live VT-d.
 */
#define UDX_DMA_LOW_MAX              (0x100000000ull)              /* 4 GiB */
#define UDX_DMA_VTD_IDENTITY_LIMIT   (1024ull * 1024ull * 1024ull) /* 1 GiB */

/**
 * Soft single-map size ceiling honesty (identity window story).
 * Soft!=product IOMMU limit. Aligns with linux_dma_soft MAP_MAX spirit.
 */
#define UDX_DMA_MAP_MAX              UDX_DMA_LOW_MAX

/*
 * udx_dma_alloc_coherent flags (Linux attrs-lite residual).
 * Bit0 FORCE32: geometric prefer / note for force32 masters. Host soft
 * path still hands identity cookies; strict force32 pages are DDI
 * DMA_BUF_ALLOC (product freestanding door / host soft slots) or
 * kernel dma_buf_*. Soft!=product: flag does not mint DMA window caps.
 */
#define UDX_DMA_F_FORCE32            1u

/* ---- Soft residual geometric / gate helpers (header-only; Soft!=product) */

/**
 * Soft identity cookie from CPU pointer (host / freestanding soft path).
 * Soft!=product: cast only — never mints a DMA window cap or VT-d PTE.
 * greppable residual: udx: dma soft residual coherent
 * greppable residual: udx: dma soft residual stream
 * greppable residual: udx: dma soft residual mint
 */
static inline udx_dma_addr_t
udx_dma_cookie_from_cpu(const void *pCpu)
{
    return (udx_dma_addr_t)(uintptr_t)pCpu;
}

/**
 * Soft direction ordinal gate (Linux dma_data_direction residual).
 * Non-zero when eDir is a defined ordinal (incl. UDX_DMA_NONE).
 * Observation / fail-closed seed; never hard-gates product DoD.
 * greppable residual: udx: dma soft residual stream
 */
static inline int
udx_dma_dir_ok(enum udx_dma_data_direction eDir)
{
    return ((unsigned)eDir <= (unsigned)UDX_DMA_NONE) ? 1 : 0;
}

/**
 * Soft map-size refuse residual (identity window story).
 * Non-zero when 0 < cbSize <= UDX_DMA_MAP_MAX. Soft!=product IOMMU limit.
 * greppable residual: udx: dma soft residual stream
 */
static inline int
udx_dma_map_size_ok(size_t cbSize)
{
    return (cbSize > 0 && cbSize <= (size_t)UDX_DMA_MAP_MAX) ? 1 : 0;
}

/**
 * Soft DDI DMA_BUF page-count residual gate.
 * cPages 0 → treat as 1 (match ddi_buf_alloc). Non-zero when in bound.
 * greppable residual: udx: dma soft residual ddi_buf
 */
static inline int
udx_dma_ddi_pages_ok(u32 cPages)
{
    u32 cPg = (cPages == 0u) ? 1u : cPages;

    return (cPg <= UDX_DMA_SOFT_DDI_PAGES_MAX) ? 1 : 0;
}

/**
 * Soft DDI DMA_BUF byte size from page count (cPages 0 → 1).
 * Returns 0 if out of soft pages bound (fail-closed residual).
 * greppable residual: udx: dma soft residual ddi_buf
 */
static inline size_t
udx_dma_ddi_pages_to_cb(u32 cPages)
{
    u32 cPg = (cPages == 0u) ? 1u : cPages;

    if (cPg > UDX_DMA_SOFT_DDI_PAGES_MAX) {
        return 0;
    }
    return (size_t)cPg * (size_t)UDX_DMA_BUF_PAGE_CB;
}

/**
 * Soft DMA range residual: non-zero cookie, non-zero size, no overflow.
 * Geometric only — Soft!=product live IOMMU PTE / window grant.
 * greppable residual: udx: dma soft residual window
 * greppable residual: udx: dma soft residual force32
 */
static inline int
udx_dma_range_ok(udx_dma_addr_t dma, size_t cb)
{
    udx_dma_addr_t end;

    if (dma == 0 || cb == 0) {
        return 0;
    }
    end = dma + (udx_dma_addr_t)cb;
    if (end < dma) {
        return 0; /* overflow */
    }
    return 1;
}

/**
 * Soft!=product law residual (always 1 today).
 * soft_ne_product=1; never product DMA window close / .ko AC (G-AC-1).
 * greppable residual: udx: dma soft residual soft_ne_product
 * greppable: soft_ne_product=1
 */
static inline int
udx_dma_soft_ne_product(void)
{
    return (int)UDX_DMA_SOFT_NE_PRODUCT;
}

/**
 * Soft mint honesty: non-zero when product DMA window cap mint is OPEN
 * (not claimed from this soft surface). soft_mint=0 always today.
 * greppable residual: udx: dma soft residual mint
 * greppable: udx: dma soft mint honesty
 */
static inline int
udx_dma_mint_open(void)
{
    /* product mint OPEN; soft mint stays 0 (UDX_DMA_SOFT_MINT). */
    return (UDX_DMA_SOFT_MINT == 0u) ? 1 : 0;
}

/**
 * Dual DoD residual OPEN honesty (observation only; Soft!=product).
 * Always non-zero today — residual never closes Dual DoD A/B.
 * greppable residual: udx: dma soft residual dual_dod
 * greppable residual: udx: dma soft residual dual_dod_open
 * greppable residual: udx: dma soft residual product_host
 */
static inline int
udx_dma_dual_dod_open(void)
{
    /* Dual DoD A/B remain OPEN; soft residual != product close. */
    (void)UDX_DMA_DOD_A_HOST;
    (void)UDX_DMA_DOD_B_HOST;
    (void)UDX_DMA_DOD_STATE;
    return 1;
}

/**
 * Soft residual path surface mask present at this layout_ver (catalog).
 * Observation only — never hard-gates Dual DoD or product DoD lamps.
 * greppable residual: udx: dma soft residual lean
 */
static inline u32
udx_dma_soft_path_surface(void)
{
    return (UDX_DMA_LAYOUT_VER >= 4u) ? (u32)UDX_DMA_SOFT_PATH_SURFACE : 0u;
}

/* ---- Coherent maps (Linux dma_alloc_coherent / free_coherent) ---------- */

/**
 * Linux dma_alloc_coherent shape.
 * u32Flags: 0 or UDX_DMA_F_FORCE32 (soft note; Soft!=product).
 * Returns CPU VA; *pDmaHandle = identity bus cookie (soft).
 * Product hosts: rtl8168_udx ring / xhci_udx TRB residual.
 * greppable residual (impl): udx: dma soft residual coherent
 * greppable residual (impl): udx: dma soft residual mint
 */
void *udx_dma_alloc_coherent(struct udx_device *pDev, size_t cbSize,
                             udx_dma_addr_t *pDmaHandle, u32 u32Flags);

/**
 * Free coherent soft residual. Null cpu cookie is a soft free_null path.
 * Soft!=product: no DMA window cap revoke / Phase-A destroy.
 * greppable residual (impl): udx: dma soft residual coherent
 */
void udx_dma_free_coherent(struct udx_device *pDev, size_t cbSize, void *pCpu,
                           udx_dma_addr_t dmaHandle);

/* ---- Stream maps (Linux dma_map_single / unmap_single) ----------------- */

/**
 * Streaming map: CPU buffer → bus cookie for one transfer window.
 * Soft: identity cookie = (udx_dma_addr_t)(uintptr_t)pCpu.
 * Soft refuse: size > UDX_DMA_MAP_MAX → 0 (mapping_error).
 * Pair with sync_single_for_{cpu,device} around device ownership.
 * Product hosts: xhci_udx xfer residual / rtl stream later.
 * greppable residual (impl): udx: dma soft residual stream
 * greppable residual (impl): udx: dma soft residual mint
 */
udx_dma_addr_t udx_dma_map_single(struct udx_device *pDev, void *pCpu,
                                  size_t cbSize,
                                  enum udx_dma_data_direction eDir);

/**
 * Unmap stream soft residual (soft map-live drop in impl).
 * Soft!=product: no IOMMU PTE unmap claim.
 * greppable residual (impl): udx: dma soft residual stream
 */
void udx_dma_unmap_single(struct udx_device *pDev, udx_dma_addr_t dmaHandle,
                          size_t cbSize, enum udx_dma_data_direction eDir);

/**
 * Linux dma_mapping_error — non-zero means map_single failed.
 * Soft residual: zero cookie only (identity path). Soft!=product.
 * greppable residual: udx: dma soft residual stream
 */
static inline int
udx_dma_mapping_error(struct udx_device *pDev, udx_dma_addr_t dmaHandle)
{
    (void)pDev;
    return dmaHandle == 0;
}

/**
 * Sync helpers (Linux dma_sync_single_*).
 * Host/identity: mfence barrier only. Full GJ may clflush / IOTLB flush.
 * Soft!=product full CWB / IOMMU IOTLB.
 * Product hosts: rtl8168_udx re-post path uses sync_for_device.
 * greppable residual (impl): udx: dma soft residual sync
 */
void udx_dma_sync_single_for_cpu(struct udx_device *pDev, udx_dma_addr_t dma,
                                 size_t cbSize,
                                 enum udx_dma_data_direction eDir);
void udx_dma_sync_single_for_device(struct udx_device *pDev, udx_dma_addr_t dma,
                                    size_t cbSize,
                                    enum udx_dma_data_direction eDir);

/* ---- IOMMU grant residual (soft / freestanding op5) -------------------- */

/**
 * Best-effort IOMMU window grant for a DMA cookie.
 * Freestanding: PLATFORM_INFO op5 (bdf, pa, size).
 * Host (UDX_HOST_LIBC): no-op success (identity map).
 * Soft!=product: not a DMA window cap mint into host CNode.
 *
 * Success under TE: soft software window + identity cover honesty when
 * tables ready — not product DMA window mint / full VT-d program.
 * Fail under TE cover miss (PA outside [0,1 GiB) identity): returns -1;
 * residual Own stick under TE if identity miss. Pair with
 * udx_dma_window_ok / udx_dma_in_vtd_identity force32 geometric checks.
 * greppable residual (impl): udx: dma soft residual iommu
 * greppable residual (impl): udx: dma soft residual iommu FAIL
 * greppable residual (impl): udx: dma soft residual mint
 */
int udx_dma_iommu_grant(u32 u32Bdf, udx_dma_addr_t dma, size_t cbSize);

/**
 * Pack BDF for udx_dma_iommu_grant (bus, slot 5-bit, func 3-bit).
 * greppable residual: udx: dma soft residual iommu
 */
static inline u32
udx_dma_bdf(u8 u8Bus, u8 u8Slot, u8 u8Func)
{
    return ((u32)u8Bus << 16) | (((u32)u8Slot & 0x1fu) << 8) |
           ((u32)u8Func & 0x7u);
}

/**
 * Bus3/TE densify (PLATFORM_INFO op7) for rtl8168_udx Own-stuck under TE.
 * Re-runs kernel bus3 identity residual; returns packed status:
 *   bit0 te_armed bit1 te_hw bit2 vtd_ready bit3 bus3_ok bit4 id1g
 * Negative on residual fail / ENOSYS. Soft!=product Dual DoD B OPEN.
 * greppable: udx: soft residual bus3_te
 */
long udx_dma_bus3_te_densify(void);

/** Kernel wbinvd via PLATFORM_INFO op8 (not ring3). Soft!=product. */
long udx_dma_wbinvd(void);

/** TE disarm dig via PLATFORM_INFO op9. Soft!=product Dual DoD B. */
long udx_dma_te_disarm(void);

/**
 * Kernel read u32 at bus physical address (PLATFORM_INFO op10).
 * Dual DoD B dig: CPU volatile vs DRAM at programmed PA (Own/cookie).
 * Returns zero-extended u32 as non-negative long, or negative errno.
 * Soft!=product.
 * greppable: udx: soft residual phys_read32
 */
long udx_dma_phys_read32(udx_dma_addr_t dmaPa);

/**
 * Pin short STATUS hold line (PLATFORM_INFO op11) for glass densify.
 * Prefer line 14 (dual DoD B residual). Soft!=product.
 * greppable: udx: soft residual panel_hold
 */
long udx_dma_panel_hold(u32 u32Line, const char *szText);

/* ---- force32 geometric notes (Soft!=product; no live VT-d) ------------- */

/**
 * Non-zero if dma cookie is in preferred low zone (PA/cookie < 4 GiB).
 * greppable residual: udx: dma soft residual force32
 * greppable: udx: dma soft force32
 */
static inline int
udx_dma_is_low(udx_dma_addr_t dma)
{
    return (dma != 0 && dma < UDX_DMA_LOW_MAX) ? 1 : 0;
}

/**
 * Non-zero if [dma, dma+cb) sits in bring-up VT-d identity [0, 1 GiB).
 * Soft geometric only — does not query live IOMMU tables.
 * greppable residual: udx: dma soft residual force32
 */
static inline int
udx_dma_in_vtd_identity(udx_dma_addr_t dma, size_t cb)
{
    udx_dma_addr_t end;

    if (dma == 0 || cb == 0) {
        return 0;
    }
    end = dma + (udx_dma_addr_t)cb;
    if (end < dma) {
        return 0; /* overflow */
    }
    if (end > UDX_DMA_VTD_IDENTITY_LIMIT) {
        return 0;
    }
    return 1;
}

/**
 * Soft geometric ok for a DMA window grant note (UDX / DDI residual).
 * fForce32 non-zero: range must sit in VT-d identity [0, 1 GiB).
 * Non-force32: non-zero non-overflowing range only (caller risk under TE).
 * Soft!=product: does not mint caps or call iommu_window_grant.
 * greppable residual (impl): udx: dma soft residual force32
 * greppable residual (impl): udx: dma soft residual window
 * greppable inventory: udx: dma soft force32 / window
 */
int udx_dma_window_ok(udx_dma_addr_t dma, size_t cb, int fForce32);

/* ---- Mask residual (Linux dma_set_mask*; Soft!=product) ---------------- */

/**
 * Soft dma_set_mask: record mask for stream maps (process-global soft).
 * Soft!=product: no struct device embedding / no IOMMU program.
 * Returns 0 on accept, -1 on zero mask.
 * greppable residual (impl): udx: dma soft residual mask
 */
int udx_dma_set_mask(struct udx_device *pDev, u64 u64Mask);

/**
 * Soft dma_set_coherent_mask: coherent alloc path mask residual.
 * greppable residual (impl): udx: dma soft residual mask
 */
int udx_dma_set_coherent_mask(struct udx_device *pDev, u64 u64Mask);

/**
 * Soft: set both masks (stream then coherent).
 * greppable residual (impl): udx: dma soft residual mask
 */
int udx_dma_set_mask_and_coherent(struct udx_device *pDev, u64 u64Mask);

/**
 * Soft dma_max_mapping_size: identity soft ceiling (UDX_DMA_MAP_MAX).
 * Soft!=product IOMMU limit.
 * greppable residual (impl): udx: dma soft residual mask
 * greppable residual (impl): udx: dma soft residual stream
 */
u64 udx_dma_max_mapping_size(struct udx_device *pDev);

/**
 * Soft get last stream / coherent mask residual (process-global soft).
 * Soft!=product: not struct device mask fields.
 * greppable residual (impl): udx: dma soft residual mask
 */
u64 udx_dma_get_mask(struct udx_device *pDev);
u64 udx_dma_get_coherent_mask(struct udx_device *pDev);

/* ---- DDI DMA_BUF soft hooks (kernel ddi_door DDI_OP_DMA_BUF_*) --------- */
/*
 * Already patterned in kernel/include/gj/ddi_door.h + ddi_host_gj:
 *   DMA_BUF_ALLOC=10 FREE=11 MAP=12; flags bit0 = force32.
 * Product freestanding UDX hosts: GJ_SYS_DDI after OPEN.
 * Host-linux inject: soft residual slots (calloc pages; Soft!=product)
 *   so product hosts can exercise ALLOC→MAP→FREE without GJ door.
 * Soft!=product: door residual / soft slots != DMA window CNode mint.
 * greppable: udx: dma soft ddi_buf
 * greppable: udx: dma soft residual ddi_buf
 */
#define UDX_DDI_OP_DMA_BUF_ALLOC  10u
#define UDX_DDI_OP_DMA_BUF_FREE   11u
#define UDX_DDI_OP_DMA_BUF_MAP    12u
/** arg3 flags for UDX_DDI_OP_DMA_BUF_ALLOC (match DDI_DMA_BUF_F_FORCE32). */
#define UDX_DMA_BUF_F_FORCE32     1u

/**
 * Soft DDI DMA_BUF_ALLOC: freestanding GJ_SYS_DDI op10; host soft slots.
 * cPages 0 → 1. u32Flags: UDX_DMA_BUF_F_FORCE32 for identity prefer.
 * Returns PA/cookie as long when ok; negative / -1 on fail. Soft!=product.
 * greppable residual (impl): udx: dma soft residual ddi_buf
 */
long udx_dma_ddi_buf_alloc(u32 u32Handle, u32 cPages, u32 u32Flags);

/**
 * Soft DDI DMA_BUF_FREE: freestanding op11; host soft slots free.
 * Returns 0 ok; negative / -1 fail.
 * greppable residual (impl): udx: dma soft residual ddi_buf
 */
long udx_dma_ddi_buf_free(u32 u32Handle, udx_dma_addr_t pa, u32 cPages);

/**
 * Soft DDI DMA_BUF_MAP: freestanding op12; host soft identity bus=cookie.
 * Returns bus cookie as long when ok; negative / -1 fail.
 * Soft!=product: not product IOMMU mint.
 * greppable residual (impl): udx: dma soft residual ddi_buf
 * greppable residual (impl): udx: dma soft residual mint
 */
long udx_dma_ddi_buf_map(u32 u32Handle, udx_dma_addr_t pa, size_t cb);

/**
 * Soft DDI DMA_BUF chain residual (product host seed):
 *   ALLOC → MAP → FREE (ops 10 / 12 / 11 spirit).
 * Host soft slots or freestanding door. Soft mint honesty: mint=0.
 * cPages 0 → 1. u32Flags: UDX_DMA_BUF_F_FORCE32 optional.
 * Returns 0 if full chain soft-ok; -1 on any step fail.
 * greppable: udx: dma soft ddi_buf chain
 * greppable residual (impl): udx: dma soft residual ddi_buf
 * Soft!=product; Dual DoD A/B OPEN.
 */
int udx_dma_ddi_buf_chain(u32 u32Handle, u32 cPages, u32 u32Flags);

/* ---- Soft residual queries (observation only; Soft!=product) ----------- */

/**
 * Soft residual lean layout version (UDX_DMA_LAYOUT_VER).
 * Grep: udx: dma soft residual lean layout_ver=
 * Not GJ_IMAGE_VERSION. Not product ABI stamp. Bar stamp-free.
 */
static inline u32
udx_dma_layout_ver(void)
{
    return UDX_DMA_LAYOUT_VER;
}

/**
 * Non-zero when soft product-host DMA surface is present:
 * coherent / free / stream map+unmap / sync / force32 note /
 * mask / DDI DMA_BUF / chain + residual deepen helpers +
 * Dual DoD OPEN catalog (layout_ver >= 2; path surface at >= 4).
 * Soft!=product; eng residual only. Never hard-gates Dual DoD.
 * greppable: udx: dma soft product host shape
 * greppable residual: udx: dma soft residual product_host
 * greppable residual: udx: dma soft residual dual_dod
 * greppable residual: udx: dma soft residual dual_dod_open
 */
int udx_dma_shape_ok(void);

/**
 * Once-lamp soft residual inventory (host console when UDX_HOST_LIBC).
 * Observation only; never hard-gates. Soft!=product; G-AC-1.
 * Returns 1 if lamp fired this call, 0 if already emitted.
 * Dual DoD A/B remain OPEN; soft_ne_product=1; product mint OPEN.
 * greppable: udx: dma soft residual lean
 * greppable: udx: dma soft residual lean UDX eng
 * greppable: udx: dma soft residual mint
 * greppable: udx: dma soft residual product_host
 * greppable: udx: dma soft residual dual_dod
 * greppable: udx: dma soft residual soft_ne_product
 */
int udx_dma_soft_residual_lean_lamp_once(void);
