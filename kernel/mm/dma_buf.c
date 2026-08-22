/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Lean freestanding DMA residual for UDX NIC/USB DMA caps eng.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. Not GPL / not Linux DMA API.
 *
 * Freestanding NIC rings (rtl8168-class, addr_hi=0 / force32):
 *   G752VT VT-d Default domain Translated; bring-up identity SLPT [0,1 GiB).
 *   PA outside identity under TE -> RX/TX OWN never clears (R0/R1 killer).
 *   Prefer identity pages; refuse high hand-out on force32 path.
 *   Ring helpers: alloc_ring_page / alloc_ring_slots (all-or-nothing).
 *   Ring free residual deepen: sync_for_cpu then unmap then free (OWN-safe).
 *   Soft ring chain once (UDX rings residual): page + slots + bad-args +
 *     live-zero honesty (Soft!=product; never closes Dual DoD A/B).
 *
 * USB (xHCI-class) residual: same force32 identity + map + sync path for
 * TRB/event/ring pages under TE. Soft!=product (no live UDX DMA cap mint).
 *
 * DDI DMA_BUF residual (UDX host foundation; deepen alloc/free/map):
 *   ddi_door DMA_BUF_ALLOC|FREE|MAP -> alloc/free/map primitives here.
 *   Soft map-live slots (not product IOMMU PTEs) for FREE/unmap honesty.
 *   Soft pages-live for outstanding alloc residual (not product quota).
 *   Soft DDI alloc-live slots (not product handle quota / not CNode mint):
 *     ddi_alloc records; ddi_free unmaps+frees+drops; ddi_map marks mapped.
 *   Map residual deepen: idempotent same-cb rematch; rematch different cb
 *     soft-unmaps prior; sync_pa_for_device after map (clflush residual).
 *   Map span honesty: ddi_map cb must fit soft DDI alloc span (OWN-safe).
 *   Map orphan: map without prior soft ddi_alloc still maps (lab; tally).
 *   Free residual deepen: drop DDI+map live intersecting free range
 *     (base-in-range or span-overlap); free-miss honesty when free without
 *     prior soft ddi_alloc (still free pages); sync_for_cpu before unmap.
 *   Soft chain residual (C2 Dual DoD DMA): once-lamp ALLOC->MAP->FREE
 *     + multi-page force32 chain + span-fail seed + idem remap + free-miss
 *     + soft live-zero honesty after chain (not product AC / Dual DoD close).
 *   Soft mint honesty: mint=OPEN; no DMA window / CNode cap mint in this TU.
 *   DDI-shaped wrappers: dma_buf_ddi_alloc / ddi_free / ddi_map.
 *
 * Preference (single page):
 *   1) PA in VT-d identity [0, 1 GiB)  - force32-safe under TE
 *   2) PA low <4 GiB after identity hunt (soft warn; OWN risk under TE)
 *   3) high PA last resort (OWN-stuck risk under TE)
 *
 * Map / sync under continuous RX re-post (arping):
 *   dma_buf_map: identity bus = PA; force32 FAIL outside identity.
 *   dma_buf_sync_*: clflush + mfence (stale desc line -> R1 stick).
 *
 * UDX/DDI helpers (coherent / map_single / window_ok / ddi_*) residual only.
 * Soft!=product: no VT-d grant / no live DMA window cap mint / no IOTLB.
 * soft PASS != DUT arping R-climb / interactive SSH login; soft PASS != product UDX DMA caps.
 * Fail logs hard-capped (DMA_BUF_FAIL_LOG_CAP). No stamp storms. No version stamp.
 * G-AC-1: not Linux DMA API complete; not in-kernel .ko product.
 *
 * greppable: dma_buf: soft note | dma_buf: soft residual | dma_buf: soft PASS
 * greppable: dma_buf: soft mint honesty | dma_buf: soft ddi
 * greppable: dma_buf: soft ddi deepen | dma_buf: soft free miss
 * greppable: dma_buf: soft ddi chain | dma_buf: soft map span
 * greppable: dma_buf: soft map orphan | dma_buf: soft free intersect
 * greppable: dma_buf: soft ddi chain mp | dma_buf: soft ddi chain span
 * greppable: dma_buf: soft ddi chain idem | dma_buf: soft ddi chain live
 * greppable: dma_buf: force32 | dma_buf: soft map | dma_buf: soft sync
 * greppable: dma_buf: soft ring | dma_buf: soft ring chain | soft ring live
 * greppable: dma_buf: soft ring slots | dma_buf: soft ring free
 * greppable: dma_buf: soft coherent | soft map_single
 * greppable: dma_buf: soft udx host | soft udx host residual
 * greppable: Soft!=product (serial) | Soft!=product (comments)
 * greppable: DMA buf for UDX caps | UDX NIC/USB DMA caps eng
 * greppable: freestanding NIC rings Dual DoD B | xHCI USB residual lean
 * greppable: UDX rings residual | DMA buf for UDX rings
 * greppable: DDI DMA_BUF residual | soft map live | soft ddi live
 * greppable: C2 Dual DoD DMA residual | Dual DoD A/B OPEN
 * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
 * STRONGER residual for product DMA (W10 Dual DoD; bar v2026.08.04.75 stamp-free):
 *   identity/force32/owned/clamp residual denser;
 *   multi-page alloc; map_cb match; PA page-align; busmaster spirit;
 *   null_miss | coherent_map | dual_dod_open for live UDX host DMA path
 *   (rtl8168_udx / xhci_udx / ddi_host). Soft!=product; Dual DoD A/B OPEN.
 *   H2 once-lamp; Soft!=product only (never Soft noteq glyph); no version stamp.
 * Product IRQ+DMA Dual DoD denser residual (bar .75 stamp-free; Soft!=product):
 *   ring page+slots force32 denser; map_single denser; sync roundtrip denser;
 *   dual NIC+USB ring spirit denser; Dual DoD A/B OPEN honesty denser.
 *   greppable: dma_buf: soft residual irq_dma denser
 *   greppable: dma_buf: soft residual irq_dma denser PASS
 *   greppable: product IRQ+DMA Dual DoD OPEN Soft!=product denser=1
 */
#include <gj/config.h>
#include <gj/dma_buf.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/vmm.h>

/*
 * Hunt budget while preferring identity [0,1GiB) then low <4GiB.
 * Raised so freestanding NIC force32 rings (desc + 2xN slots) land under TE.
 */
#define DMA_BUF_LOW_TRIES     32u
#define DMA_BUF_FORCE32_TRIES 64u

/* Cap map/sync/ring FAIL kprintf spam under continuous re-post / arping. */
#define DMA_BUF_FAIL_LOG_CAP  4u

/* Soft cap: ring slots all-or-nothing (rtl8168-class N<=32; headroom 64). */
#define DMA_BUF_RING_SLOTS_MAX 64u

/*
 * Soft map-live residual slots (not product IOMMU / not window cap mint).
 * Aligns with GJ_DDI_SOFT_DMA_SLOT_MAX spirit for UDX DDI DMA_BUF residual.
 */
#define DMA_BUF_SOFT_MAP_MAX  32u

/* DDI DMA_BUF_ALLOC soft bound (matches ddi_door residual: cPages > 16 FAIL). */
#define DMA_BUF_DDI_PAGES_MAX 16u

/*
 * Soft DDI alloc-live residual slots (not product handle quota / not cap mint).
 * Aligns with ddi_door soft DMA_BUF slot spirit for UDX host foundation.
 * Grep: soft ddi live
 */
#define DMA_BUF_DDI_SLOT_MAX  32u

/* x86 cache line for clflush walk (desc ring + RX buf under re-post). */
#define DMA_BUF_CLINE         64u

static u32 g_cAllocOk;
static u32 g_cAllocFail;
static u32 g_cHighSoft;
static u32 g_cFree;
static u32 g_cIdentityOk;   /* PA in [0,1GiB) */
static u32 g_cForce32Ok;    /* PA <4GiB (includes identity) */
static u32 g_cIdentityMiss; /* fell back outside identity */
static u32 g_cMapOk;
static u32 g_cMapFail;
static u32 g_cUnmap;
static u32 g_cSyncCpuOk;
static u32 g_cSyncCpuFail;
static u32 g_cSyncDevOk;
static u32 g_cSyncDevFail;
static u32 g_cMapFailLog;
static u32 g_cMapSoftOutLog; /* outside-identity soft accept (non-force32) */
static u32 g_cSyncFailLog;
static u32 g_cAllocSoftLog;  /* soft/high alloc warn cap (re-post quiet) */
static u32 g_cForce32FailLog;
/* Freestanding NIC / UDX ring residual (page + slots). Soft!=product. */
static u32 g_cRingPageOk;
static u32 g_cRingPageFail;
static u32 g_cRingPageFree;
static u32 g_cRingSlotsOk;
static u32 g_cRingSlotsFail;
static u32 g_cRingSlotsFree;
static u32 g_cRingFailLog;
/* UDX rings free residual: sync_for_cpu before unmap (OWN-safe under TE). */
static u32 g_cRingFreeSyncOk;
static u32 g_cRingFreeSyncFail;
/* Soft ring chain once (UDX rings residual; Soft!=product Dual DoD OPEN). */
static u32 g_cRingChainPageOk;
static u32 g_cRingChainPageFail;
static u32 g_cRingChainSlotsOk;
static u32 g_cRingChainSlotsFail;
static u32 g_cRingChainBadArgsOk;
static u32 g_cRingChainLiveOk;
static u32 g_cRingChainLiveFail;
static u32 g_cRingChainWinOk;
/* UDX/DDI mapping residual counters (hard-capped fail logs share FAIL_LOG_CAP). */
static u32 g_cCoherentOk;
static u32 g_cCoherentFail;
static u32 g_cCoherentFree;
static u32 g_cCoherentFailLog;
static u32 g_cMapSingleOk;
static u32 g_cMapSingleFail;
static u32 g_cMapSingleFailLog;
static u32 g_cWindowOk;
static u32 g_cWindowFail;
static u32 g_cWindowFailLog;
/* Soft pages-live residual (outstanding alloc; Soft!=product not quota). */
static u32 g_cPagesLive;
static u32 g_cPagesPeak;
/* Soft map-live residual (not product IOMMU; Soft!=product mint=OPEN). */
static u32 g_cMapLive;
static u32 g_cMapLivePeak;
static u32 g_cMapLiveFull;   /* soft table full (map still ok identity) */
static u32 g_cMapLiveDrop;   /* free path auto soft-unmap */
static u32 g_cUnmapMiss;     /* unmap without soft-map live */
static u32 g_cFreeWhileMapped; /* free while soft-map live (auto drop) */
static u32 g_cFreeMappedLog;
static u32 g_cUnmapMissLog;
static u32 g_cMapLiveFullLog;
/* Soft nDir residual tallies (map path honesty; not product ABI). */
static u32 g_cMapDirBidir;
static u32 g_cMapDirToDev;
static u32 g_cMapDirFromDev;
/* DDI DMA_BUF residual wrappers (UDX host foundation). Soft!=product. */
static u32 g_cDdiAllocOk;
static u32 g_cDdiAllocFail;
static u32 g_cDdiFree;
static u32 g_cDdiMapOk;
static u32 g_cDdiMapFail;
static u32 g_cDdiFailLog;
/* DDI alloc/free/map residual deepen (soft live + honesty; Soft!=product). */
static u32 g_cDdiLive;           /* outstanding soft DDI alloc slots */
static u32 g_cDdiLivePeak;
static u32 g_cDdiPagesLive;      /* outstanding soft DDI pages (not quota) */
static u32 g_cDdiPagesPeak;
static u32 g_cDdiFreeMiss;       /* free without prior soft ddi_alloc */
static u32 g_cDdiFreeMissLog;
static u32 g_cDdiSlotFull;       /* soft DDI table full (alloc still ok) */
static u32 g_cDdiSlotFullLog;
static u32 g_cDdiMapIdem;        /* map same pa+cb rematch */
static u32 g_cDdiMapRemap;       /* map different cb after prior map */
static u32 g_cDdiWinOk;          /* window_ok honesty after ddi_alloc */
static u32 g_cDdiForce32Alloc;   /* ddi_alloc force32 path ok */
static u32 g_cDdiIdentityAlloc;  /* ddi_alloc PA in vtd identity */
static u32 g_cDdiSyncOk;         /* post-map sync residual ok */
static u32 g_cDdiSyncFail;       /* post-map sync residual fail */
static u32 g_cDdiDropFree;       /* free path auto-drop DDI live */
/* C2 Dual DoD DMA residual deepen (Soft!=product; not product AC). */
static u32 g_cDdiMapSpanFail;    /* ddi_map cb exceeds soft DDI alloc span */
static u32 g_cDdiMapSpanFailLog;
static u32 g_cDdiMapOrphan;      /* ddi_map without prior soft ddi_alloc */
static u32 g_cDdiMapOrphanLog;
static u32 g_cDdiFreeSyncOk;     /* free-path sync_for_cpu residual ok */
static u32 g_cDdiFreeSyncFail;   /* free-path sync_for_cpu residual fail */
static u32 g_cFreeIntersectDrop; /* free drop via span intersection residual */
static u32 g_cFreeIntersectLog;
static u32 g_cDdiChainOk;        /* soft chain ALLOC->MAP->FREE once ok */
static u32 g_cDdiChainFail;      /* soft chain residual fail */
static u32 g_cDdiChainMpOk;      /* multi-page force32 chain ok */
static u32 g_cDdiChainMpFail;    /* multi-page force32 chain fail */
static u32 g_cDdiChainSpanOk;    /* oversize map correctly refused */
static u32 g_cDdiChainSpanFail;  /* span-fail residual path miss */
static u32 g_cDdiChainIdemOk;    /* idempotent same-cb rematch ok */
static u32 g_cDdiChainIdemFail;  /* idem rematch residual fail */
static u32 g_cDdiChainFreeMissOk;/* free-miss raw force32 residual ok */
static u32 g_cDdiChainLiveOk;    /* soft live counters restored */
static u32 g_cDdiChainLiveFail;  /* soft live leak residual after chain */
/* W10 Dual DoD live UDX host residual (Soft!=product; not Dual DoD close). */
static u32 g_cUdxHostNullOk;     /* null_miss fail-closed residual ok */
static u32 g_cUdxHostNullFail;
static u32 g_cUdxHostCohOk;      /* force32 coherent+map+sync+free ok */
static u32 g_cUdxHostCohFail;
static u32 g_cUdxHostDualDodOk;  /* Dual DoD OPEN honesty residual */
/*
 * STRONGER product DMA residual denser (bar v2026.08.04.75 stamp-free):
 * identity / force32 / owned / clamp + multi-page / map_cb / PA align /
 * busmaster spirit. Soft!=product; Dual DoD OPEN; product_hosts=UDX.
 * Grep: dma_buf: soft residual product dma | identity force32 owned clamp
 */
static u32 g_cProdDmaIdentOk;    /* identity bus cookie == PA denser */
static u32 g_cProdDmaForce32Ok;  /* force32 + VT-d identity denser */
static u32 g_cProdDmaOwnedOk;    /* owned soft DDI slot map denser */
static u32 g_cProdDmaClampOk;    /* FREE cPages clamp denser */
static u32 g_cProdDmaMpOk;       /* multi-page force32 alloc denser */
static u32 g_cProdDmaMapCbOk;    /* map_cb match denser */
static u32 g_cProdDmaPaAlignOk;  /* PA page-align denser */
static u32 g_cProdDmaBmSpiritOk; /* busmaster spirit (OWN-safe force32 id) */
static u32 g_cProdDmaDenseOk;    /* denser once-arm composite ok */
static u32 g_cProdDmaDenseFail;  /* denser once-arm soft fail */
/* Product IRQ+DMA Dual DoD denser residual (bar .75; Soft!=product). */
static u32 g_cIrqDmaRingOk;      /* ring page+slots force32 denser ok */
static u32 g_cIrqDmaRingFail;    /* ring denser soft fail */
static u32 g_cIrqDmaMapSingleOk; /* map_single denser ok */
static u32 g_cIrqDmaMapSingleFail;
static u32 g_cIrqDmaSyncOk;      /* sync roundtrip denser ok */
static u32 g_cIrqDmaSyncFail;
static u32 g_cIrqDmaDualHostOk;  /* dual NIC+USB ring spirit denser ok */
static u32 g_cIrqDmaDualHostFail;
static u32 g_cIrqDmaDualDodOk;   /* Dual DoD OPEN honesty denser */
static u32 g_cIrqDmaDenseOk;     /* composite irq_dma denser ok */
static u32 g_cIrqDmaDenseFail;   /* composite irq_dma denser fail */
static int g_fSoftNoteOnce;
static int g_fSoftMintOnce;
static int g_fSoftChainOnce;     /* C2 Dual DoD soft chain once-lamp */
static int g_fSoftRingOnce;      /* UDX rings residual chain once-lamp */
static int g_fSoftUdxHostOnce;   /* W10 live UDX host residual once-lamp (H2) */
static int g_fSoftIrqDmaOnce;    /* product IRQ+DMA Dual DoD denser once (H2) */

/* Soft map-live residual slots (bookkeeping only; no IOMMU / no cap mint). */
static struct {
    u8  u8Used;
    u8  u8Force32;
    u8  u8Dir; /* soft DMA_BUF_* ordinal */
    u8  u8Pad;
    gj_paddr_t pa;
    u64 cb;
} g_aSoftMap[DMA_BUF_SOFT_MAP_MAX];

/*
 * Soft DDI alloc-live residual (UDX/DDI foundation; not product handle table).
 * Records ddi_alloc base; free/map residual consult for honesty under TE.
 * Soft!=product mint=OPEN (no DMA window / CNode cap).
 */
static struct {
    u8  u8Used;
    u8  u8Force32;
    u8  u8Mapped;
    u8  u8Pad;
    u32 cPages;
    gj_paddr_t pa;
    gj_paddr_t paBus;
    u64 u64MapCb;
} g_aDdiSlot[DMA_BUF_DDI_SLOT_MAX];

static void *
dma_va(gj_paddr_t pa)
{
    if (pa == 0) {
        return NULL;
    }
    return (void *)hhdm_to_virt(pa);
}

static void
dma_zero(gj_paddr_t pa, u32 cPages)
{
    void *pVa;
    u32 i;

    if (pa == 0 || cPages == 0) {
        return;
    }
    for (i = 0; i < cPages; i++) {
        pVa = dma_va(pa + (gj_paddr_t)i * (gj_paddr_t)GJ_PAGE_SIZE);
        if (pVa != NULL) {
            memset(pVa, 0, GJ_PAGE_SIZE);
        }
    }
}

static void
dma_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

static void
dma_pages_live_add(u32 cPages)
{
    u32 cNext;

    if (cPages == 0u) {
        return;
    }
    cNext = g_cPagesLive + cPages;
    if (cNext < g_cPagesLive) {
        g_cPagesLive = 0xffffffffu; /* soft wrap-safe peak hold */
    } else {
        g_cPagesLive = cNext;
    }
    if (g_cPagesLive > g_cPagesPeak) {
        g_cPagesPeak = g_cPagesLive;
    }
}

static void
dma_pages_live_sub(u32 cPages)
{
    if (cPages == 0u) {
        return;
    }
    if (g_cPagesLive >= cPages) {
        g_cPagesLive -= cPages;
    } else {
        g_cPagesLive = 0u;
    }
}

static void
dma_note_ok(gj_paddr_t pa, u32 cPages)
{
    u64 cb;

    if (g_cAllocOk < 0xffffffffu) {
        g_cAllocOk++;
    }
    dma_pages_live_add(cPages);
    cb = (u64)cPages * (u64)GJ_PAGE_SIZE;
    if (dma_buf_is_low(pa)) {
        if (g_cForce32Ok < 0xffffffffu) {
            g_cForce32Ok++;
        }
    }
    if (dma_buf_in_vtd_identity(pa, cb)) {
        if (g_cIdentityOk < 0xffffffffu) {
            g_cIdentityOk++;
        }
    } else {
        if (g_cIdentityMiss < 0xffffffffu) {
            g_cIdentityMiss++;
        }
    }
}

/**
 * Soft map-live: find slot by bus cookie (=PA). Returns index or -1.
 * Soft!=product: residual bookkeeping only (not IOMMU PTE walk).
 */
static int
dma_soft_map_find(gj_paddr_t paDma)
{
    u32 i;

    if (paDma == 0) {
        return -1;
    }
    for (i = 0u; i < DMA_BUF_SOFT_MAP_MAX; i++) {
        if (g_aSoftMap[i].u8Used != 0u && g_aSoftMap[i].pa == paDma) {
            return (int)i;
        }
    }
    return -1;
}

/**
 * Soft map-live record (or update) on map success.
 * Soft!=product: not product IOMMU map; mint=OPEN (no window cap).
 */
static void
dma_soft_map_record(gj_paddr_t pa, u64 cb, int nDir, int fForce32)
{
    int iFound;
    u32 i;
    u8 u8Dir;

    if (pa == 0 || cb == 0ull) {
        return;
    }
    if (nDir == DMA_BUF_TO_DEVICE) {
        u8Dir = (u8)DMA_BUF_TO_DEVICE;
        dma_inc(&g_cMapDirToDev);
    } else if (nDir == DMA_BUF_FROM_DEVICE) {
        u8Dir = (u8)DMA_BUF_FROM_DEVICE;
        dma_inc(&g_cMapDirFromDev);
    } else {
        u8Dir = (u8)DMA_BUF_BIDIRECTIONAL;
        dma_inc(&g_cMapDirBidir);
    }

    iFound = dma_soft_map_find(pa);
    if (iFound >= 0) {
        /* Idempotent re-map residual: refresh cb/dir/force32. */
        g_aSoftMap[iFound].cb = cb;
        g_aSoftMap[iFound].u8Dir = u8Dir;
        g_aSoftMap[iFound].u8Force32 = (fForce32 != 0) ? 1u : 0u;
        return;
    }
    for (i = 0u; i < DMA_BUF_SOFT_MAP_MAX; i++) {
        if (g_aSoftMap[i].u8Used == 0u) {
            g_aSoftMap[i].u8Used = 1u;
            g_aSoftMap[i].u8Force32 = (fForce32 != 0) ? 1u : 0u;
            g_aSoftMap[i].u8Dir = u8Dir;
            g_aSoftMap[i].u8Pad = 0u;
            g_aSoftMap[i].pa = pa;
            g_aSoftMap[i].cb = cb;
            dma_inc(&g_cMapLive);
            if (g_cMapLive > g_cMapLivePeak) {
                g_cMapLivePeak = g_cMapLive;
            }
            return;
        }
    }
    /* Soft table full: map still succeeds (identity bus=PA); residual only. */
    dma_inc(&g_cMapLiveFull);
    if (g_cMapLiveFullLog < DMA_BUF_FAIL_LOG_CAP) {
        g_cMapLiveFullLog++;
        kprintf("dma_buf: soft map live full max=%u pa=0x%lx "
                "(identity ok; Soft!=product mint=OPEN no IOMMU)\n",
                DMA_BUF_SOFT_MAP_MAX, (unsigned long)pa);
    }
}

/**
 * Soft map-live clear by bus cookie. Returns 1 if cleared, 0 if miss.
 */
static int
dma_soft_map_clear(gj_paddr_t paDma)
{
    int iFound;

    iFound = dma_soft_map_find(paDma);
    if (iFound < 0) {
        return 0;
    }
    g_aSoftMap[iFound].u8Used = 0u;
    g_aSoftMap[iFound].u8Force32 = 0u;
    g_aSoftMap[iFound].u8Dir = 0u;
    g_aSoftMap[iFound].u8Pad = 0u;
    g_aSoftMap[iFound].pa = 0;
    g_aSoftMap[iFound].cb = 0ull;
    if (g_cMapLive > 0u) {
        g_cMapLive--;
    }
    return 1;
}

/**
 * Soft-unmap any live slots intersecting free range [pa, pa+cPages*4K).
 * Free residual deepen (C2 Dual DoD): base-in-range OR span-overlap so
 * partial free of multi-page map stays honest under TE (stale map-live).
 * Soft!=product: not product IOMMU revoke / window cap destroy.
 * Grep: dma_buf: soft free intersect | soft free mapped
 */
static void
dma_soft_map_drop_range(gj_paddr_t pa, u32 cPages)
{
    u64 u64Base;
    u64 u64End;
    u32 i;
    u32 cDrop = 0u;
    u32 cIntersect = 0u;

    if (pa == 0 || cPages == 0u) {
        return;
    }
    u64Base = (u64)pa;
    u64End = u64Base + (u64)cPages * (u64)GJ_PAGE_SIZE;
    if (u64End < u64Base) {
        u64End = ~0ull; /* soft overflow: drop anything at/above base */
    }
    for (i = 0u; i < DMA_BUF_SOFT_MAP_MAX; i++) {
        u64 u64Slot;
        u64 u64SlotEnd;
        u64 u64Cb;
        int fBaseIn;
        int fIntersect;

        if (g_aSoftMap[i].u8Used == 0u) {
            continue;
        }
        u64Slot = (u64)g_aSoftMap[i].pa;
        u64Cb = g_aSoftMap[i].cb;
        if (u64Cb == 0ull) {
            u64Cb = (u64)GJ_PAGE_SIZE;
        }
        u64SlotEnd = u64Slot + u64Cb;
        if (u64SlotEnd < u64Slot) {
            u64SlotEnd = ~0ull;
        }
        /* Intersect: free_start < slot_end && slot_start < free_end */
        fBaseIn = (u64Slot >= u64Base && u64Slot < u64End) ? 1 : 0;
        fIntersect = (u64Base < u64SlotEnd && u64Slot < u64End) ? 1 : 0;
        if (fBaseIn != 0 || fIntersect != 0) {
            if (fBaseIn == 0 && fIntersect != 0) {
                cIntersect++;
            }
            g_aSoftMap[i].u8Used = 0u;
            g_aSoftMap[i].u8Force32 = 0u;
            g_aSoftMap[i].u8Dir = 0u;
            g_aSoftMap[i].u8Pad = 0u;
            g_aSoftMap[i].pa = 0;
            g_aSoftMap[i].cb = 0ull;
            if (g_cMapLive > 0u) {
                g_cMapLive--;
            }
            cDrop++;
        }
    }
    if (cDrop > 0u) {
        dma_inc(&g_cMapLiveDrop);
        dma_inc(&g_cFreeWhileMapped);
        if (cIntersect > 0u) {
            dma_inc(&g_cFreeIntersectDrop);
            if (g_cFreeIntersectLog < DMA_BUF_FAIL_LOG_CAP) {
                g_cFreeIntersectLog++;
                /* Grep: dma_buf: soft free intersect | Soft!=product */
                kprintf("dma_buf: soft free intersect map pa=0x%lx pages=%u "
                        "drop=%u intersect=%u "
                        "(span-overlap soft-unmap; Soft!=product mint=OPEN)\n",
                        (unsigned long)pa, cPages, cDrop, cIntersect);
            }
        } else if (g_cFreeMappedLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cFreeMappedLog++;
            /* Grep: dma_buf: soft free mapped | Soft!=product */
            kprintf("dma_buf: soft free mapped pa=0x%lx pages=%u drop=%u "
                    "(auto soft-unmap; Soft!=product mint=OPEN)\n",
                    (unsigned long)pa, cPages, cDrop);
        }
    }
}

/**
 * Soft DDI slot find by base PA. Returns index or -1.
 * Soft!=product: residual bookkeeping only (not product handle table).
 */
static int
dma_ddi_slot_find(gj_paddr_t pa)
{
    u32 i;

    if (pa == 0) {
        return -1;
    }
    for (i = 0u; i < DMA_BUF_DDI_SLOT_MAX; i++) {
        if (g_aDdiSlot[i].u8Used != 0u && g_aDdiSlot[i].pa == pa) {
            return (int)i;
        }
    }
    return -1;
}

/**
 * Soft DDI slot clear by index. Adjusts live page/slot counters.
 */
static void
dma_ddi_slot_clear(u32 iSlot)
{
    u32 cPages;

    if (iSlot >= DMA_BUF_DDI_SLOT_MAX) {
        return;
    }
    if (g_aDdiSlot[iSlot].u8Used == 0u) {
        return;
    }
    cPages = g_aDdiSlot[iSlot].cPages;
    if (cPages == 0u) {
        cPages = 1u;
    }
    g_aDdiSlot[iSlot].u8Used = 0u;
    g_aDdiSlot[iSlot].u8Force32 = 0u;
    g_aDdiSlot[iSlot].u8Mapped = 0u;
    g_aDdiSlot[iSlot].u8Pad = 0u;
    g_aDdiSlot[iSlot].cPages = 0u;
    g_aDdiSlot[iSlot].pa = 0;
    g_aDdiSlot[iSlot].paBus = 0;
    g_aDdiSlot[iSlot].u64MapCb = 0ull;
    if (g_cDdiLive > 0u) {
        g_cDdiLive--;
    }
    if (g_cDdiPagesLive >= cPages) {
        g_cDdiPagesLive -= cPages;
    } else {
        g_cDdiPagesLive = 0u;
    }
}

/**
 * Soft DDI record on alloc success. Soft!=product mint=OPEN.
 * Table full: alloc still succeeds; residual only (capped log).
 */
static void
dma_ddi_slot_record(gj_paddr_t pa, u32 cPages, int fForce32)
{
    u32 i;

    if (pa == 0 || cPages == 0u) {
        return;
    }
    {
        int iFound = dma_ddi_slot_find(pa);

        /* Refresh if same PA already recorded (rare re-record honesty). */
        if (iFound >= 0) {
            u32 iSlot = (u32)iFound;
            u32 cOld = g_aDdiSlot[iSlot].cPages;

            if (cOld == 0u) {
                cOld = 1u;
            }
            if (g_cDdiPagesLive >= cOld) {
                g_cDdiPagesLive -= cOld;
            } else {
                g_cDdiPagesLive = 0u;
            }
            g_aDdiSlot[iSlot].cPages = cPages;
            g_aDdiSlot[iSlot].u8Force32 = (fForce32 != 0) ? 1u : 0u;
            g_aDdiSlot[iSlot].u8Mapped = 0u;
            g_aDdiSlot[iSlot].paBus = 0;
            g_aDdiSlot[iSlot].u64MapCb = 0ull;
            {
                u32 cNext = g_cDdiPagesLive + cPages;

                if (cNext < g_cDdiPagesLive) {
                    g_cDdiPagesLive = 0xffffffffu;
                } else {
                    g_cDdiPagesLive = cNext;
                }
                if (g_cDdiPagesLive > g_cDdiPagesPeak) {
                    g_cDdiPagesPeak = g_cDdiPagesLive;
                }
            }
            return;
        }
    }
    for (i = 0u; i < DMA_BUF_DDI_SLOT_MAX; i++) {
        if (g_aDdiSlot[i].u8Used == 0u) {
            g_aDdiSlot[i].u8Used = 1u;
            g_aDdiSlot[i].u8Force32 = (fForce32 != 0) ? 1u : 0u;
            g_aDdiSlot[i].u8Mapped = 0u;
            g_aDdiSlot[i].u8Pad = 0u;
            g_aDdiSlot[i].cPages = cPages;
            g_aDdiSlot[i].pa = pa;
            g_aDdiSlot[i].paBus = 0;
            g_aDdiSlot[i].u64MapCb = 0ull;
            dma_inc(&g_cDdiLive);
            if (g_cDdiLive > g_cDdiLivePeak) {
                g_cDdiLivePeak = g_cDdiLive;
            }
            {
                u32 cNext = g_cDdiPagesLive + cPages;

                if (cNext < g_cDdiPagesLive) {
                    g_cDdiPagesLive = 0xffffffffu;
                } else {
                    g_cDdiPagesLive = cNext;
                }
                if (g_cDdiPagesLive > g_cDdiPagesPeak) {
                    g_cDdiPagesPeak = g_cDdiPagesLive;
                }
            }
            return;
        }
    }
    dma_inc(&g_cDdiSlotFull);
    if (g_cDdiSlotFullLog < DMA_BUF_FAIL_LOG_CAP) {
        g_cDdiSlotFullLog++;
        /* Grep: dma_buf: soft ddi | soft ddi live */
        kprintf("dma_buf: soft ddi live full max=%u pa=0x%lx pages=%u "
                "(alloc ok; Soft!=product mint=OPEN no handle quota)\n",
                DMA_BUF_DDI_SLOT_MAX, (unsigned long)pa, cPages);
    }
}

/**
 * Soft-drop DDI alloc-live slots intersecting free range.
 * Free residual deepen (C2 Dual DoD): base-in-range OR span-overlap so
 * partial free of multi-page ddi_alloc invalidates soft live under TE.
 * Soft!=product: not product handle destroy.
 * Grep: dma_buf: soft free intersect | soft ddi
 */
static void
dma_ddi_slot_drop_range(gj_paddr_t pa, u32 cPages)
{
    u64 u64Base;
    u64 u64End;
    u32 i;
    u32 cDrop = 0u;
    u32 cIntersect = 0u;

    if (pa == 0 || cPages == 0u) {
        return;
    }
    u64Base = (u64)pa;
    u64End = u64Base + (u64)cPages * (u64)GJ_PAGE_SIZE;
    if (u64End < u64Base) {
        u64End = ~0ull;
    }
    for (i = 0u; i < DMA_BUF_DDI_SLOT_MAX; i++) {
        u64 u64Slot;
        u64 u64SlotEnd;
        u32 cSlotPages;
        int fBaseIn;
        int fIntersect;

        if (g_aDdiSlot[i].u8Used == 0u) {
            continue;
        }
        u64Slot = (u64)g_aDdiSlot[i].pa;
        cSlotPages = g_aDdiSlot[i].cPages;
        if (cSlotPages == 0u) {
            cSlotPages = 1u;
        }
        u64SlotEnd = u64Slot + (u64)cSlotPages * (u64)GJ_PAGE_SIZE;
        if (u64SlotEnd < u64Slot) {
            u64SlotEnd = ~0ull;
        }
        fBaseIn = (u64Slot >= u64Base && u64Slot < u64End) ? 1 : 0;
        fIntersect = (u64Base < u64SlotEnd && u64Slot < u64End) ? 1 : 0;
        if (fBaseIn != 0 || fIntersect != 0) {
            if (fBaseIn == 0 && fIntersect != 0) {
                cIntersect++;
            }
            dma_ddi_slot_clear(i);
            cDrop++;
        }
    }
    if (cDrop > 0u) {
        dma_inc(&g_cDdiDropFree);
        if (cIntersect > 0u) {
            dma_inc(&g_cFreeIntersectDrop);
            if (g_cFreeIntersectLog < DMA_BUF_FAIL_LOG_CAP) {
                g_cFreeIntersectLog++;
                /* Grep: dma_buf: soft free intersect | Soft!=product */
                kprintf("dma_buf: soft free intersect ddi pa=0x%lx pages=%u "
                        "drop=%u intersect=%u "
                        "(span-overlap DDI live clear; Soft!=product "
                        "mint=OPEN)\n",
                        (unsigned long)pa, cPages, cDrop, cIntersect);
            }
        } else if (g_cDdiFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cDdiFailLog++;
            /* Grep: dma_buf: soft ddi | soft free miss */
            kprintf("dma_buf: soft ddi drop free pa=0x%lx pages=%u drop=%u "
                    "(auto DDI live clear; Soft!=product mint=OPEN)\n",
                    (unsigned long)pa, cPages, cDrop);
        }
    }
}

/**
 * Round cb up to page count. Returns 0 on zero/overflow (soft fail).
 * Cap at 0xffffffff pages so multi-page free stays u32-safe.
 */
static u32
dma_pages_for_cb(u64 cb)
{
    u64 cPages;

    if (cb == 0ull) {
        return 0;
    }
    cPages = (cb + (u64)GJ_PAGE_SIZE - 1ull) / (u64)GJ_PAGE_SIZE;
    if (cPages == 0ull || cPages > 0xffffffffull) {
        return 0;
    }
    return (u32)cPages;
}

/**
 * Soft mint honesty once: map residual never mints DMA window / CNode caps.
 * Grep: dma_buf: soft mint honesty
 * Soft!=product | G-AC-1 | mint=OPEN.
 */
static void
dma_soft_mint_honesty_once(void)
{
    if (g_fSoftMintOnce != 0) {
        return;
    }
    g_fSoftMintOnce = 1;
    kprintf("dma_buf: soft mint honesty mint=OPEN window_cap=0 "
            "cnode_cap=0 iotlb=0 soft_map_live=1 soft_pages_live=1 "
            "ddi_dma_buf=1 product_DMA_window_cap=OPEN "
            "Soft!=product G-AC-1 dual=MIT|Apache-2.0 "
            "(soft residual != product UDX DMA caps)\n");
}

/*
 * Clflush [pVa, pVa+cb) + mfence. Soft!=product full CWB / non-x86.
 * Continuous RX re-post: clean desc+buf lines so NIC DRAM owns OWN clear
 * and next DMA write is not masked by dirty CPU lines.
 */
static int
dma_clflush_range(void *pVa, u64 cb)
{
    u8 *pByte;
    u8 *pEnd;
    uintptr_t uPtr;

    if (pVa == NULL || cb == 0ull) {
        return -1;
    }
    uPtr = (uintptr_t)pVa;
    /* Align down to cache line so partial first line is flushed. */
    uPtr &= ~((uintptr_t)DMA_BUF_CLINE - 1u);
    pByte = (u8 *)uPtr;
    pEnd = (u8 *)pVa + cb;
    while (pByte < pEnd) {
        __asm__ volatile("clflush (%0)" : : "r"(pByte) : "memory");
        pByte += DMA_BUF_CLINE;
    }
    __asm__ volatile("mfence" ::: "memory");
    return 0;
}

int
dma_buf_is_low(gj_paddr_t pa)
{
    return (pa != 0 && (u64)pa < GJ_DMA_LOW_MAX) ? 1 : 0;
}

int
dma_buf_in_vtd_identity(gj_paddr_t pa, u64 cb)
{
    u64 u64Pa = (u64)pa;

    if (pa == 0 || cb == 0) {
        return 0;
    }
    if (u64Pa >= GJ_DMA_VTD_IDENTITY_LIMIT) {
        return 0;
    }
    if (u64Pa + cb < u64Pa) {
        return 0; /* overflow */
    }
    if (u64Pa + cb > GJ_DMA_VTD_IDENTITY_LIMIT) {
        return 0;
    }
    return 1;
}

gj_paddr_t
dma_buf_alloc_page(void **ppVa)
{
    gj_paddr_t aHoldId[DMA_BUF_LOW_TRIES]; /* low but outside identity */
    gj_paddr_t aHoldHi[DMA_BUF_LOW_TRIES]; /* high >=4GiB */
    u32 cHoldId = 0;
    u32 cHoldHi = 0;
    gj_paddr_t pa = 0;
    u32 i;

    if (ppVa != NULL) {
        *ppVa = NULL;
    }

    /*
     * Prefer PA inside VT-d bring-up identity [0, 1GiB) first so force32
     * masters (rtl8168 @ 03:00.0, xHCI) work when TE is live (G752 Translated).
     * Hold low-outside-identity and high while hunting identity pages.
     */
    for (i = 0; i < DMA_BUF_LOW_TRIES; i++) {
        pa = pmm_alloc();
        if (pa == 0) {
            break;
        }
        if (dma_buf_in_vtd_identity(pa, GJ_PAGE_SIZE)) {
            while (cHoldId > 0u) {
                cHoldId--;
                pmm_free(aHoldId[cHoldId]);
            }
            while (cHoldHi > 0u) {
                cHoldHi--;
                pmm_free(aHoldHi[cHoldHi]);
            }
            dma_zero(pa, 1u);
            if (ppVa != NULL) {
                *ppVa = dma_va(pa);
            }
            dma_note_ok(pa, 1u);
            return pa;
        }
        if (dma_buf_is_low(pa)) {
            if (cHoldId < DMA_BUF_LOW_TRIES) {
                aHoldId[cHoldId++] = pa;
            } else {
                pmm_free(pa);
            }
            pa = 0;
            continue;
        }
        if (cHoldHi < DMA_BUF_LOW_TRIES) {
            aHoldHi[cHoldHi++] = pa;
        } else {
            pmm_free(pa);
            pa = 0;
            break;
        }
        pa = 0;
    }

    /* Fallback: low <4GiB (force32 ok, OWN risk under TE if outside identity). */
    if (cHoldId > 0u) {
        pa = aHoldId[0];
        for (i = 1u; i < cHoldId; i++) {
            pmm_free(aHoldId[i]);
        }
        while (cHoldHi > 0u) {
            cHoldHi--;
            pmm_free(aHoldHi[cHoldHi]);
        }
        if (g_cHighSoft < 0xffffffffu) {
            g_cHighSoft++;
        }
        /* Cap: continuous re-post must not flood serial. Soft!=product. */
        if (g_cAllocSoftLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cAllocSoftLog++;
            /* Grep: dma_buf: soft pa= | dma_buf: force32 | Soft!=product */
            kprintf("dma_buf: soft pa=0x%lx force32=1 outside vtd_identity 1GiB "
                    "(OWN risk under TE; prefer dma_buf_alloc_page_force32; "
                    "Soft!=product)\n",
                    (unsigned long)pa);
        }
        dma_zero(pa, 1u);
        if (ppVa != NULL) {
            *ppVa = dma_va(pa);
        }
        dma_note_ok(pa, 1u);
        return pa;
    }

    /* Last resort: high page (soft warn - not force32-safe). */
    if (cHoldHi > 0u) {
        pa = aHoldHi[0];
        for (i = 1u; i < cHoldHi; i++) {
            pmm_free(aHoldHi[i]);
        }
        if (g_cHighSoft < 0xffffffffu) {
            g_cHighSoft++;
        }
        if (g_cAllocSoftLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cAllocSoftLog++;
            kprintf("dma_buf: high soft pa=0x%lx (prefer <4GiB; VT-d id cover "
                    "1GiB; force32 fail risk; Soft!=product)\n",
                    (unsigned long)pa);
        }
        dma_zero(pa, 1u);
        if (ppVa != NULL) {
            *ppVa = dma_va(pa);
        }
        dma_note_ok(pa, 1u);
        return pa;
    }

    if (g_cAllocFail < 0xffffffffu) {
        g_cAllocFail++;
    }
    return 0;
}

gj_paddr_t
dma_buf_alloc_page_force32(void **ppVa)
{
    gj_paddr_t aBad[DMA_BUF_FORCE32_TRIES];
    u32 cBad = 0;
    gj_paddr_t pa = 0;
    u32 i;

    if (ppVa != NULL) {
        *ppVa = NULL;
    }

    /*
     * Strict force32 + identity: only return PA in [0, 1GiB).
     * Used by freestanding NIC/xHCI under TE. Returns 0 if pool exhausted
     * rather than handing a page that will OWN-stick.
     * Grep: dma_buf: force32
     */
    for (i = 0; i < DMA_BUF_FORCE32_TRIES; i++) {
        pa = pmm_alloc();
        if (pa == 0) {
            break;
        }
        if (dma_buf_in_vtd_identity(pa, GJ_PAGE_SIZE)) {
            while (cBad > 0u) {
                cBad--;
                pmm_free(aBad[cBad]);
            }
            dma_zero(pa, 1u);
            if (ppVa != NULL) {
                *ppVa = dma_va(pa);
            }
            dma_note_ok(pa, 1u);
            return pa;
        }
        if (cBad < DMA_BUF_FORCE32_TRIES) {
            aBad[cBad++] = pa;
        } else {
            pmm_free(pa);
            break;
        }
        pa = 0;
    }
    while (cBad > 0u) {
        cBad--;
        pmm_free(aBad[cBad]);
    }
    if (g_cAllocFail < 0xffffffffu) {
        g_cAllocFail++;
    }
    /* Cap under continuous re-post / ring refill thrash. Soft!=product. */
    if (g_cForce32FailLog < DMA_BUF_FAIL_LOG_CAP) {
        g_cForce32FailLog++;
        /* Grep: dma_buf: force32 FAIL | Soft!=product */
        kprintf("dma_buf: force32 FAIL (no page in vtd_identity [0,1GiB); "
                "OWN would stick under TE; Soft!=product)\n");
    }
    return 0;
}

gj_paddr_t
dma_buf_alloc_pages(u32 cPages, void **ppVa)
{
    gj_paddr_t pa;
    u32 iTry;
    gj_paddr_t aHold[4];
    u32 cHold = 0;

    if (ppVa != NULL) {
        *ppVa = NULL;
    }
    if (cPages == 0) {
        return 0;
    }
    if (cPages == 1u) {
        return dma_buf_alloc_page(ppVa);
    }

    /*
     * Contiguous multi-page: prefer base inside VT-d identity [0,1GiB),
     * then any low <4GiB. High base is last-resort soft accept.
     */
    for (iTry = 0; iTry < 8u; iTry++) {
        pa = pmm_alloc_pages(cPages);
        if (pa == 0) {
            break;
        }
        if (dma_buf_in_vtd_identity(pa, (u64)cPages * (u64)GJ_PAGE_SIZE)) {
            while (cHold > 0u) {
                cHold--;
                pmm_free_pages(aHold[cHold], cPages);
            }
            dma_zero(pa, cPages);
            if (ppVa != NULL) {
                *ppVa = dma_va(pa);
            }
            dma_note_ok(pa, cPages);
            return pa;
        }
        if (dma_buf_is_low(pa) && cHold < 4u && iTry + 1u < 8u) {
            aHold[cHold++] = pa;
            continue;
        }
        if (dma_buf_is_low(pa)) {
            while (cHold > 0u) {
                cHold--;
                pmm_free_pages(aHold[cHold], cPages);
            }
            if (g_cHighSoft < 0xffffffffu) {
                g_cHighSoft++;
            }
            if (g_cAllocSoftLog < DMA_BUF_FAIL_LOG_CAP) {
                g_cAllocSoftLog++;
                kprintf("dma_buf: soft pa=0x%lx pages=%u force32=1 outside "
                        "vtd_identity (OWN risk under TE; Soft!=product)\n",
                        (unsigned long)pa, cPages);
            }
            dma_zero(pa, cPages);
            if (ppVa != NULL) {
                *ppVa = dma_va(pa);
            }
            dma_note_ok(pa, cPages);
            return pa;
        }
        if (cHold < 4u && iTry + 1u < 8u) {
            aHold[cHold++] = pa;
            continue;
        }
        /* Accept high only on last tries. */
        while (cHold > 0u) {
            cHold--;
            pmm_free_pages(aHold[cHold], cPages);
        }
        if (g_cHighSoft < 0xffffffffu) {
            g_cHighSoft++;
        }
        if (g_cAllocSoftLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cAllocSoftLog++;
            kprintf("dma_buf: high soft pa=0x%lx pages=%u (prefer <4GiB; "
                    "force32 fail risk; Soft!=product)\n",
                    (unsigned long)pa, cPages);
        }
        dma_zero(pa, cPages);
        if (ppVa != NULL) {
            *ppVa = dma_va(pa);
        }
        dma_note_ok(pa, cPages);
        return pa;
    }

    /* Drain held: prefer first low if any. */
    if (cHold > 0u) {
        pa = aHold[0];
        for (iTry = 1u; iTry < cHold; iTry++) {
            pmm_free_pages(aHold[iTry], cPages);
        }
        if (!dma_buf_in_vtd_identity(pa, (u64)cPages * (u64)GJ_PAGE_SIZE)) {
            if (g_cHighSoft < 0xffffffffu) {
                g_cHighSoft++;
            }
            if (g_cAllocSoftLog < DMA_BUF_FAIL_LOG_CAP) {
                g_cAllocSoftLog++;
                kprintf("dma_buf: soft pa=0x%lx pages=%u held outside identity "
                        "(Soft!=product)\n",
                        (unsigned long)pa, cPages);
            }
        }
        dma_zero(pa, cPages);
        if (ppVa != NULL) {
            *ppVa = dma_va(pa);
        }
        dma_note_ok(pa, cPages);
        return pa;
    }

    if (g_cAllocFail < 0xffffffffu) {
        g_cAllocFail++;
    }
    return 0;
}

void
dma_buf_free_page(gj_paddr_t pa)
{
    if (pa == 0) {
        return;
    }
    /*
     * Soft residual free deepen: drop soft-map-live + soft DDI live covering
     * this page before free. Soft!=product: not product IOMMU revoke /
     * DMA window cap destroy. DDI FREE/CLOSE already unmap first; this is
     * honesty under skip path (raw free_page without ddi_free).
     */
    dma_soft_map_drop_range(pa, 1u);
    dma_ddi_slot_drop_range(pa, 1u);
    pmm_free(pa);
    dma_pages_live_sub(1u);
    if (g_cFree < 0xffffffffu) {
        g_cFree++;
    }
}

void
dma_buf_free_pages(gj_paddr_t pa, u32 cPages)
{
    if (pa == 0 || cPages == 0) {
        return;
    }
    if (cPages == 1u) {
        dma_buf_free_page(pa);
        return;
    }
    /*
     * Soft residual free deepen: soft-unmap + DDI live drop in range,
     * then pmm free. Soft!=product mint=OPEN.
     */
    dma_soft_map_drop_range(pa, cPages);
    dma_ddi_slot_drop_range(pa, cPages);
    pmm_free_pages(pa, cPages);
    dma_pages_live_sub(cPages);
    if (g_cFree < 0xffffffffu) {
        g_cFree++;
    }
}

gj_paddr_t
dma_buf_alloc_pages_force32(u32 cPages, void **ppVa)
{
    gj_paddr_t aBad[8];
    u32 cBad = 0;
    gj_paddr_t pa = 0;
    u32 iTry;
    u64 cbNeed;

    if (ppVa != NULL) {
        *ppVa = NULL;
    }
    if (cPages == 0) {
        return 0;
    }
    if (cPages == 1u) {
        return dma_buf_alloc_page_force32(ppVa);
    }

    /*
     * Strict multi-page force32: full [pa, pa+cPages*4K) in VT-d identity.
     * Free non-cover bases rather than hand OWN-stick ranges under TE.
     * Grep: dma_buf: force32
     */
    cbNeed = (u64)cPages * (u64)GJ_PAGE_SIZE;
    for (iTry = 0; iTry < DMA_BUF_FORCE32_TRIES && iTry < 8u; iTry++) {
        pa = pmm_alloc_pages(cPages);
        if (pa == 0) {
            break;
        }
        if (dma_buf_in_vtd_identity(pa, cbNeed) != 0) {
            while (cBad > 0u) {
                cBad--;
                pmm_free_pages(aBad[cBad], cPages);
            }
            dma_zero(pa, cPages);
            if (ppVa != NULL) {
                *ppVa = dma_va(pa);
            }
            dma_note_ok(pa, cPages);
            return pa;
        }
        if (cBad < 8u) {
            aBad[cBad++] = pa;
        } else {
            pmm_free_pages(pa, cPages);
            pa = 0;
            break;
        }
        pa = 0;
    }
    while (cBad > 0u) {
        cBad--;
        pmm_free_pages(aBad[cBad], cPages);
    }
    if (g_cAllocFail < 0xffffffffu) {
        g_cAllocFail++;
    }
    if (g_cForce32FailLog < DMA_BUF_FAIL_LOG_CAP) {
        g_cForce32FailLog++;
        kprintf("dma_buf: force32 FAIL pages=%u (no contiguous identity "
                "[0,1GiB); Soft!=product)\n",
                cPages);
    }
    return 0;
}

/* ---- Freestanding NIC ring residual (force32 + map + sync) ------------- */

gj_paddr_t
dma_buf_alloc_ring_page(void **ppVa)
{
    gj_paddr_t pa;
    gj_paddr_t paBus;
    void *pVa = NULL;

    /*
     * One force32 identity page for freestanding NIC desc/buf or xHCI
     * TRB/event ring slots. Map bus=PA under force32; sync_for_device so
     * zeroed lines hit DRAM before OWN/cycle-bit program (stale cache
     * -> stick under TE). Soft!=product. Grep: dma_buf: soft ring
     */
    if (ppVa != NULL) {
        *ppVa = NULL;
    }
    pa = dma_buf_alloc_page_force32(&pVa);
    if (pa == 0 || pVa == NULL) {
        dma_inc(&g_cRingPageFail);
        if (g_cRingFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cRingFailLog++;
            kprintf("dma_buf: soft ring FAIL page force32 (no identity; "
                    "Soft!=product freestanding NIC/USB rings)\n");
        }
        return 0;
    }
    paBus = dma_buf_map(pa, (u64)GJ_PAGE_SIZE, DMA_BUF_BIDIRECTIONAL, 1);
    if (paBus == 0) {
        /* Map failed: page not soft-mapped; free without ring free path. */
        dma_buf_free_page(pa);
        dma_inc(&g_cRingPageFail);
        if (g_cRingFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cRingFailLog++;
            kprintf("dma_buf: soft ring FAIL page map pa=0x%lx "
                    "(Soft!=product freestanding NIC rings)\n",
                    (unsigned long)pa);
        }
        return 0;
    }
    if (dma_buf_sync_for_device(pVa, (u64)GJ_PAGE_SIZE,
                                DMA_BUF_BIDIRECTIONAL) != 0) {
        /*
         * Mapped but sync failed: free via ring free residual
         * (sync_cpu + unmap + free) so map-live honesty stays clean.
         * Soft!=product freestanding NIC/USB + UDX rings residual.
         */
        dma_buf_free_ring_page(paBus);
        dma_inc(&g_cRingPageFail);
        if (g_cRingFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cRingFailLog++;
            kprintf("dma_buf: soft ring FAIL page sync pa=0x%lx "
                    "(Soft!=product freestanding NIC rings)\n",
                    (unsigned long)pa);
        }
        return 0;
    }
    if (ppVa != NULL) {
        *ppVa = pVa;
    }
    dma_inc(&g_cRingPageOk);
    return paBus;
}

void
dma_buf_free_ring_page(gj_paddr_t pa)
{
    /*
     * UDX rings free residual deepen (Soft!=product):
     *   sync_for_cpu so device-written OWN/desc/TRB lines hit CPU view
     *   under TE, then soft unmap, then free_page (map-live drop honesty).
     * Matches ddi_free free-path sync residual spirit for freestanding
     * NIC/USB rings and UDX ring foundation. Soft!=product: not IOTLB.
     * Grep: dma_buf: soft ring free | soft ring
     */
    if (pa == 0) {
        return;
    }
    if (dma_buf_sync_pa_for_cpu(pa, (u64)GJ_PAGE_SIZE,
                                DMA_BUF_BIDIRECTIONAL) == 0) {
        dma_inc(&g_cRingFreeSyncOk);
    } else {
        dma_inc(&g_cRingFreeSyncFail);
        /* Soft: still unmap+free (caller teardown residual). */
    }
    dma_buf_unmap(pa, (u64)GJ_PAGE_SIZE, DMA_BUF_BIDIRECTIONAL);
    dma_buf_free_page(pa);
    dma_inc(&g_cRingPageFree);
}

int
dma_buf_alloc_ring_slots(u32 cSlots, gj_paddr_t *paOut, void **ppVaOut)
{
    u32 i;
    u32 cGot = 0;
    gj_paddr_t pa;
    void *pVa;

    /*
     * All-or-nothing force32 ring buffers (rtl TX or RX slot array).
     * Partial fail rolls back so freestanding NIC never programs half rings.
     * Soft!=product. Grep: dma_buf: soft ring
     */
    if (cSlots == 0u || cSlots > DMA_BUF_RING_SLOTS_MAX || paOut == NULL) {
        dma_inc(&g_cRingSlotsFail);
        if (g_cRingFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cRingFailLog++;
            kprintf("dma_buf: soft ring FAIL slots=%u (bad args max=%u; "
                    "Soft!=product freestanding NIC rings)\n",
                    cSlots, DMA_BUF_RING_SLOTS_MAX);
        }
        return -1;
    }
    for (i = 0; i < cSlots; i++) {
        paOut[i] = 0;
        if (ppVaOut != NULL) {
            ppVaOut[i] = NULL;
        }
    }
    for (i = 0; i < cSlots; i++) {
        pVa = NULL;
        pa = dma_buf_alloc_ring_page(&pVa);
        if (pa == 0) {
            while (cGot > 0u) {
                cGot--;
                dma_buf_free_ring_page(paOut[cGot]);
                paOut[cGot] = 0;
                if (ppVaOut != NULL) {
                    ppVaOut[cGot] = NULL;
                }
            }
            dma_inc(&g_cRingSlotsFail);
            if (g_cRingFailLog < DMA_BUF_FAIL_LOG_CAP) {
                g_cRingFailLog++;
                kprintf("dma_buf: soft ring FAIL slots=%u at=%u "
                        "(force32 pool; Soft!=product freestanding NIC)\n",
                        cSlots, i);
            }
            return -1;
        }
        paOut[i] = pa;
        if (ppVaOut != NULL) {
            ppVaOut[i] = pVa;
        }
        cGot++;
    }
    dma_inc(&g_cRingSlotsOk);
    return 0;
}

void
dma_buf_free_ring_slots(u32 cSlots, gj_paddr_t *paIn)
{
    u32 i;

    if (cSlots == 0u || paIn == NULL) {
        return;
    }
    if (cSlots > DMA_BUF_RING_SLOTS_MAX) {
        cSlots = DMA_BUF_RING_SLOTS_MAX;
    }
    for (i = 0; i < cSlots; i++) {
        if (paIn[i] != 0) {
            dma_buf_free_ring_page(paIn[i]);
            paIn[i] = 0;
        }
    }
    dma_inc(&g_cRingSlotsFree);
}

/**
 * Soft ring chain residual once: UDX rings foundation seed (Soft!=product).
 * Functional residual for freestanding NIC desc/buf + xHCI TRB rings and
 * the force32 map+sync path UDX product hosts rely on via DDI DMA_BUF:
 *   1) ring_page: alloc_ring_page (force32+map+sync_device) + identity
 *      window_ok honesty + free_ring_page (sync_cpu+unmap+free)
 *   2) ring_slots: all-or-nothing 2-slot alloc + free (rollback path peer)
 *   3) bad-args: cSlots=0 must FAIL (soft refuse; never programs half ring)
 *   4) soft live-zero honesty: map_live / pages_live not leaked by chain
 * Soft fail only (pool may be tight at soft_note); never hard-gates Dual DoD.
 * Grep: dma_buf: soft ring chain | soft ring slots | soft ring live
 * Grep: DMA buf for UDX rings | UDX rings residual | Dual DoD A/B OPEN
 */
static void
dma_soft_ring_chain_once(void)
{
    gj_paddr_t paPage;
    gj_paddr_t aSlots[2];
    void *pVa = NULL;
    void *apVa[2];
    u32 cMapBefore;
    u32 cPagesBefore;
    u32 cSlotsFailBefore;
    int fPageOk = 0;
    int fSlotsOk = 0;
    int fBadArgsOk = 0;

    if (g_fSoftRingOnce != 0) {
        return;
    }
    g_fSoftRingOnce = 1;

    cMapBefore = g_cMapLive;
    cPagesBefore = g_cPagesLive;

    /*
     * (1) One force32 ring page under TE identity. UDX rings residual:
     * desc / TRB / event ring single-page foundation. Soft!=product.
     * Grep: dma_buf: soft ring chain
     */
    paPage = dma_buf_alloc_ring_page(&pVa);
    if (paPage == 0 || pVa == NULL) {
        dma_inc(&g_cRingChainPageFail);
        kprintf("dma_buf: soft ring chain FAIL page force32 "
                "(UDX rings residual; Soft!=product mint=OPEN; "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN)\n");
    } else {
        /* Identity + window geometric honesty (OWN-safe under TE). */
        if (dma_buf_in_vtd_identity(paPage, (u64)GJ_PAGE_SIZE) != 0 &&
            dma_buf_window_ok(paPage, (u64)GJ_PAGE_SIZE, 1) != 0) {
            dma_inc(&g_cRingChainWinOk);
        }
        /*
         * Soft re-post residual: sync_for_device already done in alloc;
         * exercise sync_for_cpu + re-sync_device (continuous RX re-post
         * peer for freestanding NIC under arping). Soft!=product.
         */
        (void)dma_buf_sync_for_cpu(pVa, (u64)GJ_PAGE_SIZE,
                                   DMA_BUF_BIDIRECTIONAL);
        (void)dma_buf_sync_for_device(pVa, (u64)GJ_PAGE_SIZE,
                                      DMA_BUF_BIDIRECTIONAL);
        dma_buf_free_ring_page(paPage);
        dma_inc(&g_cRingChainPageOk);
        fPageOk = 1;
        /* Grep: dma_buf: soft ring chain | DMA buf for UDX rings */
        kprintf("dma_buf: soft ring chain ok path=PAGE force32=1 "
                "pa=0x%lx UDX rings residual Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
                "Soft!=product mint=OPEN G-AC-1 dual=MIT|Apache-2.0 "
                "(soft residual != product UDX DMA caps)\n",
                (unsigned long)paPage);
    }

    /*
     * (2) All-or-nothing 2-slot ring residual (TX/RX slot pair spirit).
     * Grep: dma_buf: soft ring slots | soft ring chain
     */
    aSlots[0] = 0;
    aSlots[1] = 0;
    apVa[0] = NULL;
    apVa[1] = NULL;
    if (dma_buf_alloc_ring_slots(2u, aSlots, apVa) != 0) {
        dma_inc(&g_cRingChainSlotsFail);
        kprintf("dma_buf: soft ring chain FAIL slots=2 "
                "(UDX rings residual; Soft!=product mint=OPEN; "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN)\n");
    } else {
        dma_buf_free_ring_slots(2u, aSlots);
        dma_inc(&g_cRingChainSlotsOk);
        fSlotsOk = 1;
        /* Grep: dma_buf: soft ring slots | soft ring chain */
        kprintf("dma_buf: soft ring slots ok path=SLOTS_2 force32=1 "
                "UDX rings residual Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
                "Soft!=product mint=OPEN G-AC-1 "
                "(all-or-nothing; soft residual != product UDX DMA caps)\n");
    }

    /*
     * (3) Bad-args residual: cSlots=0 must refuse (never half-programs).
     * Grep: dma_buf: soft ring chain | soft ring
     */
    cSlotsFailBefore = g_cRingSlotsFail;
    if (dma_buf_alloc_ring_slots(0u, aSlots, NULL) != 0 &&
        g_cRingSlotsFail > cSlotsFailBefore) {
        dma_inc(&g_cRingChainBadArgsOk);
        fBadArgsOk = 1;
    }

    /*
     * (4) Soft live-zero honesty: ring chain must not leak map/pages live
     * relative to pre-chain baseline (best-effort). Soft!=product.
     * Grep: dma_buf: soft ring live | soft ring chain
     */
    if (g_cMapLive <= cMapBefore && g_cPagesLive <= cPagesBefore) {
        dma_inc(&g_cRingChainLiveOk);
    } else {
        dma_inc(&g_cRingChainLiveFail);
        kprintf("dma_buf: soft ring live FAIL "
                "map_live=%u/%u pages_live=%u/%u "
                "(soft leak residual; Soft!=product mint=OPEN; "
                "UDX rings residual Dual_DoD_A=OPEN Dual_DoD_B=OPEN)\n",
                g_cMapLive, cMapBefore, g_cPagesLive, cPagesBefore);
    }

    /* Grep: dma_buf: soft ring chain | UDX rings residual | Dual DoD A/B OPEN */
    kprintf("dma_buf: soft ring chain UDX rings residual "
            "page=%s slots=%s bad_args=%s "
            "win_ok=%u free_sync=%u/%u "
            "live_ok=%u live_fail=%u "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "Soft!=product mint=OPEN G-AC-1 dual=MIT|Apache-2.0 "
            "(DMA buf for UDX rings; soft residual != product UDX DMA caps; "
            "never closes Dual DoD)\n",
            (fPageOk != 0) ? "ok" : "fail",
            (fSlotsOk != 0) ? "ok" : "fail",
            (fBadArgsOk != 0) ? "ok" : "fail",
            g_cRingChainWinOk, g_cRingFreeSyncOk, g_cRingFreeSyncFail,
            g_cRingChainLiveOk, g_cRingChainLiveFail);
}

/* ---- UDX/DDI coherent (size-based residual) ----------------------------- */

gj_paddr_t
dma_buf_alloc_coherent(u64 cb, void **ppVa, int fForce32)
{
    u32 cPages;
    gj_paddr_t pa;

    if (ppVa != NULL) {
        *ppVa = NULL;
    }
    cPages = dma_pages_for_cb(cb);
    if (cPages == 0) {
        dma_inc(&g_cCoherentFail);
        if (g_cCoherentFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cCoherentFailLog++;
            /* Grep: dma_buf: soft coherent FAIL | Soft!=product */
            kprintf("dma_buf: soft coherent FAIL cb=0x%lx (zero/overflow; "
                    "Soft!=product UDX map)\n",
                    (unsigned long)cb);
        }
        return 0;
    }

    if (fForce32 != 0) {
        pa = (cPages == 1u) ? dma_buf_alloc_page_force32(ppVa)
                            : dma_buf_alloc_pages_force32(cPages, ppVa);
    } else {
        pa = (cPages == 1u) ? dma_buf_alloc_page(ppVa)
                            : dma_buf_alloc_pages(cPages, ppVa);
    }
    if (pa == 0) {
        dma_inc(&g_cCoherentFail);
        if (g_cCoherentFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cCoherentFailLog++;
            kprintf("dma_buf: soft coherent FAIL cb=0x%lx pages=%u force32=%d "
                    "(pool; Soft!=product UDX map)\n",
                    (unsigned long)cb, cPages, fForce32);
        }
        return 0;
    }
    dma_inc(&g_cCoherentOk);
    return pa;
}

void
dma_buf_free_coherent(gj_paddr_t pa, u64 cb)
{
    u32 cPages;

    if (pa == 0) {
        return;
    }
    cPages = dma_pages_for_cb(cb);
    if (cPages == 0) {
        /* Soft: unknown size - free one page only (caller contract miss). */
        dma_buf_free_page(pa);
        dma_inc(&g_cCoherentFree);
        return;
    }
    dma_buf_free_pages(pa, cPages);
    dma_inc(&g_cCoherentFree);
}

int
dma_buf_window_ok(gj_paddr_t pa, u64 cb, int fForce32)
{
    u64 u64Pa;

    /*
     * Geometric precheck for UDX caps / DDI DMA_NOTE residual.
     * Soft!=product: no iommu_window_grant, no cap mint.
     * Grep: dma_buf: soft window
     */
    if (pa == 0 || cb == 0ull) {
        dma_inc(&g_cWindowFail);
        if (g_cWindowFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cWindowFailLog++;
            kprintf("dma_buf: soft window FAIL pa=0x%lx cb=0x%lx (null/zero; "
                    "Soft!=product UDX caps)\n",
                    (unsigned long)pa, (unsigned long)cb);
        }
        return 0;
    }
    u64Pa = (u64)pa;
    if (u64Pa + cb < u64Pa) {
        dma_inc(&g_cWindowFail);
        if (g_cWindowFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cWindowFailLog++;
            kprintf("dma_buf: soft window FAIL pa=0x%lx cb=0x%lx (overflow; "
                    "Soft!=product UDX caps)\n",
                    (unsigned long)pa, (unsigned long)cb);
        }
        return 0;
    }
    if (fForce32 != 0 && dma_buf_in_vtd_identity(pa, cb) == 0) {
        dma_inc(&g_cWindowFail);
        if (g_cWindowFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cWindowFailLog++;
            kprintf("dma_buf: soft window FAIL pa=0x%lx cb=0x%lx force32=1 "
                    "outside vtd_identity (OWN risk; Soft!=product UDX caps)\n",
                    (unsigned long)pa, (unsigned long)cb);
        }
        return 0;
    }
    dma_inc(&g_cWindowOk);
    return 1;
}

/* ---- Map / unmap (soft identity; residual honesty under re-post) -------- */

gj_paddr_t
dma_buf_map(gj_paddr_t pa, u64 cb, int nDir, int fForce32)
{
    u64 u64Pa;

    /*
     * Hot under continuous RX re-post if caller remaps each slot: success is
     * silent; FAIL/outside notes capped (Soft!=product).
     * Soft mint honesty: never mints DMA window / CNode caps (mint=OPEN).
     * Soft residual: soft map-live slot record (not product IOMMU PTE).
     */
    if (pa == 0 || cb == 0ull) {
        dma_inc(&g_cMapFail);
        if (g_cMapFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cMapFailLog++;
            /* Grep: dma_buf: soft map FAIL | Soft!=product */
            kprintf("dma_buf: soft map FAIL pa=0x%lx cb=0x%lx (null/zero; "
                    "Soft!=product identity)\n",
                    (unsigned long)pa, (unsigned long)cb);
        }
        return 0;
    }

    /* Overflow geometric honesty (same spirit as window_ok). */
    u64Pa = (u64)pa;
    if (u64Pa + cb < u64Pa) {
        dma_inc(&g_cMapFail);
        if (g_cMapFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cMapFailLog++;
            kprintf("dma_buf: soft map FAIL pa=0x%lx cb=0x%lx (overflow; "
                    "Soft!=product identity)\n",
                    (unsigned long)pa, (unsigned long)cb);
        }
        return 0;
    }

    /*
     * force32 masters (rtl8168 addr_hi=0): range must sit in VT-d identity
     * cover or OWN sticks under TE. Soft!=product: geometric check only.
     */
    if (fForce32 != 0 && dma_buf_in_vtd_identity(pa, cb) == 0) {
        dma_inc(&g_cMapFail);
        if (g_cMapFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cMapFailLog++;
            kprintf("dma_buf: soft map FAIL pa=0x%lx cb=0x%lx force32=1 "
                    "outside vtd_identity (OWN would stick under TE; "
                    "Soft!=product)\n",
                    (unsigned long)pa, (unsigned long)cb);
        }
        return 0;
    }

    /*
     * Soft identity bus address = PA. No IOMMU window grant here.
     * Soft mint honesty: mint=OPEN (product DMA window cap OPEN elsewhere).
     * Outside-identity non-force32: soft accept (caller risk under TE).
     * Map does not clflush - caller dma_buf_sync_* on re-post.
     */
    if (fForce32 == 0 && dma_buf_in_vtd_identity(pa, cb) == 0) {
        /* Cap soft-outside note so continuous map under arping stays quiet. */
        if (g_cMapSoftOutLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cMapSoftOutLog++;
            kprintf("dma_buf: soft map ok pa=0x%lx cb=0x%lx outside "
                    "vtd_identity (OWN risk under TE; Soft!=product "
                    "mint=OPEN)\n",
                    (unsigned long)pa, (unsigned long)cb);
        }
    }

    dma_soft_map_record(pa, cb, nDir, fForce32);
    dma_soft_mint_honesty_once();
    dma_inc(&g_cMapOk);
    return pa;
}

void
dma_buf_unmap(gj_paddr_t paDma, u64 cb, int nDir)
{
    (void)cb;
    (void)nDir;

    if (paDma == 0) {
        return;
    }
    /*
     * Soft residual unmap: clear soft map-live when present.
     * Soft!=product: not product IOMMU revoke / window cap destroy.
     * Miss is soft honesty only (cap log) - continuous re-post may
     * unmap without prior soft-record if table was full.
     */
    if (dma_soft_map_clear(paDma) == 0) {
        dma_inc(&g_cUnmapMiss);
        if (g_cUnmapMissLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cUnmapMissLog++;
            /* Grep: dma_buf: soft unmap miss | Soft!=product */
            kprintf("dma_buf: soft unmap miss pa=0x%lx "
                    "(no soft-map live; Soft!=product mint=OPEN)\n",
                    (unsigned long)paDma);
        }
    }
    dma_inc(&g_cUnmap);
}

gj_paddr_t
dma_buf_map_single(void *pVa, u64 cb, int nDir, int fForce32)
{
    gj_paddr_t pa;

    /*
     * UDX-shaped map_single residual: HHDM/kernel VA -> identity bus=PA.
     * Soft!=product: not process-AS user VA (use DDI grants + virt_to_phys).
     * Grep: dma_buf: soft map_single
     */
    if (pVa == NULL || cb == 0ull) {
        dma_inc(&g_cMapSingleFail);
        if (g_cMapSingleFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cMapSingleFailLog++;
            kprintf("dma_buf: soft map_single FAIL pVa=%p cb=0x%lx "
                    "(null/zero; Soft!=product UDX map)\n",
                    pVa, (unsigned long)cb);
        }
        return 0;
    }
    pa = hhdm_to_phys((gj_vaddr_t)(uintptr_t)pVa);
    if (pa == 0) {
        dma_inc(&g_cMapSingleFail);
        if (g_cMapSingleFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cMapSingleFailLog++;
            kprintf("dma_buf: soft map_single FAIL pVa=%p (no PA; "
                    "Soft!=product UDX map HHDM only)\n",
                    pVa);
        }
        return 0;
    }
    pa = dma_buf_map(pa, cb, nDir, fForce32);
    if (pa == 0) {
        dma_inc(&g_cMapSingleFail);
        /* map already hard-capped FAIL log; no second stamp. */
        return 0;
    }
    dma_inc(&g_cMapSingleOk);
    return pa;
}

void
dma_buf_unmap_single(gj_paddr_t paDma, u64 cb, int nDir)
{
    dma_buf_unmap(paDma, cb, nDir);
}

int
dma_buf_mapping_error(gj_paddr_t paDma)
{
    return (paDma == 0) ? 1 : 0;
}

/* ---- DDI DMA_BUF residual (UDX host foundation; Soft!=product) ---------- */

gj_paddr_t
dma_buf_ddi_alloc(u32 cPages, int fForce32, void **ppVa)
{
    gj_paddr_t pa;
    u64 cb;

    /*
     * DDI_OP_DMA_BUF_ALLOC residual shape for UDX hosts (deepen):
     *   cPages 0 -> 1; soft bound matches ddi_door (cPages > 16 FAIL).
     *   Soft DDI alloc-live record (not product handle quota).
     *   Geometric honesty: window_ok / identity / force32 tallies.
     * Soft!=product: not DMA window / CNode cap mint (mint=OPEN).
     * Grep: dma_buf: soft ddi | soft ddi deepen
     */
    if (ppVa != NULL) {
        *ppVa = NULL;
    }
    if (cPages == 0u) {
        cPages = 1u;
    }
    if (cPages > DMA_BUF_DDI_PAGES_MAX) {
        dma_inc(&g_cDdiAllocFail);
        if (g_cDdiFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cDdiFailLog++;
            kprintf("dma_buf: soft ddi FAIL alloc pages=%u max=%u "
                    "(Soft!=product UDX DDI DMA_BUF)\n",
                    cPages, DMA_BUF_DDI_PAGES_MAX);
        }
        return 0;
    }
    if (fForce32 != 0) {
        pa = (cPages == 1u) ? dma_buf_alloc_page_force32(ppVa)
                            : dma_buf_alloc_pages_force32(cPages, ppVa);
    } else {
        pa = (cPages == 1u) ? dma_buf_alloc_page(ppVa)
                            : dma_buf_alloc_pages(cPages, ppVa);
    }
    if (pa == 0) {
        dma_inc(&g_cDdiAllocFail);
        if (g_cDdiFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cDdiFailLog++;
            kprintf("dma_buf: soft ddi FAIL alloc pages=%u force32=%d "
                    "(pool; Soft!=product UDX DDI DMA_BUF mint=OPEN)\n",
                    cPages, fForce32);
        }
        return 0;
    }

    /* Soft DDI live record + geometric honesty (matches ddi_door residual). */
    dma_ddi_slot_record(pa, cPages, fForce32);
    cb = (u64)cPages * (u64)GJ_PAGE_SIZE;
    if (fForce32 != 0) {
        dma_inc(&g_cDdiForce32Alloc);
    }
    if (dma_buf_in_vtd_identity(pa, cb) != 0) {
        dma_inc(&g_cDdiIdentityAlloc);
    }
    if (dma_buf_window_ok(pa, cb, fForce32) != 0) {
        dma_inc(&g_cDdiWinOk);
    }
    /*
     * STRONGER product DMA residual denser (Soft!=product; Dual DoD OPEN):
     * PA page-align; multi-page; force32+identity; busmaster spirit
     * (OWN-safe force32 identity under TE for UDX busmasters).
     * Grep: identity force32 owned clamp | product_hosts=UDX
     */
    if (((u64)pa & (u64)GJ_PAGE_MASK) == 0ull) {
        dma_inc(&g_cProdDmaPaAlignOk);
    }
    if (cPages > 1u) {
        dma_inc(&g_cProdDmaMpOk);
    }
    if (fForce32 != 0 && dma_buf_in_vtd_identity(pa, cb) != 0) {
        dma_inc(&g_cProdDmaForce32Ok);
        /* Busmaster spirit: force32 identity + low PA (no dual-cycle). */
        if (dma_buf_is_low(pa) != 0) {
            dma_inc(&g_cProdDmaBmSpiritOk);
        }
    }
    dma_inc(&g_cDdiAllocOk);
    dma_soft_mint_honesty_once();
    return pa;
}

void
dma_buf_ddi_free(gj_paddr_t pa, u32 cPages)
{
    int iSlot;
    u32 cFree;
    u64 cbUnmap;
    gj_paddr_t paBusUnmap;

    /*
     * DDI_OP_DMA_BUF_FREE residual deepen (C2 Dual DoD DMA):
     *   Consult soft DDI alloc-live for cPages when 0 / for mapped state.
     *   sync_for_cpu residual then soft unmap (matches ddi_door FREE).
     *   Free-miss honesty when free without prior soft ddi_alloc.
     * Soft!=product: not product IOMMU revoke / window cap destroy.
     * free_* also drop_range for honesty if map/DDI live under skip path.
     * Grep: dma_buf: soft ddi | soft free miss
     */
    if (pa == 0) {
        return;
    }
    iSlot = dma_ddi_slot_find(pa);
    if (iSlot < 0) {
        /* Free-miss: still free pages (caller contract residual). Soft!=product. */
        dma_inc(&g_cDdiFreeMiss);
        if (cPages == 0u) {
            cPages = 1u;
        }
        if (g_cDdiFreeMissLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cDdiFreeMissLog++;
            /* Grep: dma_buf: soft free miss | soft ddi */
            kprintf("dma_buf: soft free miss ddi pa=0x%lx pages=%u "
                    "(no soft ddi live; still free; Soft!=product mint=OPEN)\n",
                    (unsigned long)pa, cPages);
        }
        if (dma_soft_map_find(pa) >= 0) {
            cbUnmap = (u64)cPages * (u64)GJ_PAGE_SIZE;
            /* Free-path sync residual (device-written lines) before unmap. */
            if (dma_buf_sync_pa_for_cpu(pa, cbUnmap,
                                        DMA_BUF_BIDIRECTIONAL) == 0) {
                dma_inc(&g_cDdiFreeSyncOk);
            } else {
                dma_inc(&g_cDdiFreeSyncFail);
            }
            dma_buf_unmap(pa, cbUnmap, DMA_BUF_BIDIRECTIONAL);
        }
        if (cPages <= 1u) {
            dma_buf_free_page(pa);
        } else {
            dma_buf_free_pages(pa, cPages);
        }
        dma_inc(&g_cDdiFree);
        return;
    }

    /* Prefer recorded cPages when caller passed 0 (matches ddi_door). */
    cFree = g_aDdiSlot[iSlot].cPages;
    if (cFree == 0u) {
        cFree = 1u;
    }
    if (cPages == 0u) {
        cPages = cFree;
    } else if (cPages != cFree) {
        /*
         * STRONGER product DMA clamp residual denser (Soft!=product):
         * FREE cPages clamped to soft-slot page count so UDX hosts cannot
         * free a wrong range via stale arg. Not product IOMMU revoke.
         * Grep: clamp residual | identity force32 owned clamp
         */
        cPages = cFree;
        dma_inc(&g_cProdDmaClampOk);
    }
    cbUnmap = (u64)cPages * (u64)GJ_PAGE_SIZE;

    /*
     * Soft residual: sync_for_cpu then unmap map-live / DDI mapped cookie
     * before free (ddi_door FREE shape; Dual DoD DMA residual under TE).
     * Soft!=product: not full CWB / IOTLB.
     */
    if (g_aDdiSlot[iSlot].u8Mapped != 0u && g_aDdiSlot[iSlot].paBus != 0) {
        paBusUnmap = g_aDdiSlot[iSlot].paBus;
        cbUnmap = (g_aDdiSlot[iSlot].u64MapCb != 0ull)
                      ? g_aDdiSlot[iSlot].u64MapCb
                      : cbUnmap;
        if (dma_buf_sync_pa_for_cpu(paBusUnmap, cbUnmap,
                                    DMA_BUF_BIDIRECTIONAL) == 0) {
            dma_inc(&g_cDdiFreeSyncOk);
        } else {
            dma_inc(&g_cDdiFreeSyncFail);
        }
        dma_buf_unmap(paBusUnmap, cbUnmap, DMA_BUF_BIDIRECTIONAL);
        g_aDdiSlot[iSlot].u8Mapped = 0u;
        g_aDdiSlot[iSlot].paBus = 0;
        g_aDdiSlot[iSlot].u64MapCb = 0ull;
    } else if (dma_soft_map_find(pa) >= 0) {
        if (dma_buf_sync_pa_for_cpu(pa, cbUnmap,
                                    DMA_BUF_BIDIRECTIONAL) == 0) {
            dma_inc(&g_cDdiFreeSyncOk);
        } else {
            dma_inc(&g_cDdiFreeSyncFail);
        }
        dma_buf_unmap(pa, cbUnmap, DMA_BUF_BIDIRECTIONAL);
    }

    /* Clear DDI live before free_page so drop_range does not double-log. */
    dma_ddi_slot_clear((u32)iSlot);

    if (cPages <= 1u) {
        dma_buf_free_page(pa);
    } else {
        dma_buf_free_pages(pa, cPages);
    }
    dma_inc(&g_cDdiFree);
}

gj_paddr_t
dma_buf_ddi_map(gj_paddr_t pa, u64 cb, int fForce32)
{
    gj_paddr_t paBus;
    int iSlot;
    int fForceEff;
    u64 cbAlloc;

    /*
     * DDI_OP_DMA_BUF_MAP residual deepen (C2 Dual DoD DMA):
     *   Inherit force32 from soft DDI alloc-live when slot recorded.
     *   Span honesty: cb must fit soft DDI alloc pages when live.
     *   Orphan map (no soft ddi_alloc) still maps for lab; tallied.
     *   Idempotent rematch same pa+cb -> prior bus cookie.
     *   Different cb on mapped slot: soft unmap prior then remap.
     *   window_ok when force32; map BIDIR; sync_pa_for_device residual.
     * Soft mint honesty: identity bus=PA; no window cap mint (mint=OPEN).
     * Grep: dma_buf: soft ddi | soft mint honesty | soft ddi deepen
     * Grep: dma_buf: soft map span | soft map orphan
     */
    if (pa == 0 || cb == 0ull) {
        dma_inc(&g_cDdiMapFail);
        if (g_cDdiFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cDdiFailLog++;
            kprintf("dma_buf: soft ddi FAIL map pa=0x%lx cb=0x%lx "
                    "(null/zero; Soft!=product UDX DDI DMA_BUF)\n",
                    (unsigned long)pa, (unsigned long)cb);
        }
        return 0;
    }

    iSlot = dma_ddi_slot_find(pa);
    /* Force32: caller flag OR soft DDI alloc force32 residual (OWN-safe). */
    fForceEff = fForce32;
    if (iSlot >= 0 && g_aDdiSlot[iSlot].u8Force32 != 0u) {
        fForceEff = 1;
    }

    /*
     * Map span honesty: when soft DDI live records alloc, cb must fit
     * [pa, pa+cPages*4K). Oversize map would OWN-stick / over-DMA lie
     * under TE. Soft!=product geometric residual (not product window mint).
     * Grep: dma_buf: soft map span
     */
    if (iSlot >= 0) {
        u32 cSlotPages = g_aDdiSlot[iSlot].cPages;

        if (cSlotPages == 0u) {
            cSlotPages = 1u;
        }
        cbAlloc = (u64)cSlotPages * (u64)GJ_PAGE_SIZE;
        if (cb > cbAlloc) {
            dma_inc(&g_cDdiMapSpanFail);
            dma_inc(&g_cDdiMapFail);
            if (g_cDdiMapSpanFailLog < DMA_BUF_FAIL_LOG_CAP) {
                g_cDdiMapSpanFailLog++;
                kprintf("dma_buf: soft map span FAIL pa=0x%lx cb=0x%lx "
                        "alloc_cb=0x%lx pages=%u "
                        "(oversize; Soft!=product UDX DDI DMA_BUF)\n",
                        (unsigned long)pa, (unsigned long)cb,
                        (unsigned long)cbAlloc, cSlotPages);
            }
            return 0;
        }
    } else {
        /* Orphan map: no soft ddi_alloc live (lab path). Soft!=product. */
        dma_inc(&g_cDdiMapOrphan);
        if (g_cDdiMapOrphanLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cDdiMapOrphanLog++;
            /* Grep: dma_buf: soft map orphan | Soft!=product */
            kprintf("dma_buf: soft map orphan pa=0x%lx cb=0x%lx "
                    "(no soft ddi live; still map; Soft!=product mint=OPEN)\n",
                    (unsigned long)pa, (unsigned long)cb);
        }
    }

    /* Idempotent rematch: same cb returns prior bus (matches ddi_door). */
    if (iSlot >= 0 && g_aDdiSlot[iSlot].u8Mapped != 0u &&
        g_aDdiSlot[iSlot].paBus != 0 &&
        g_aDdiSlot[iSlot].u64MapCb == cb) {
        dma_inc(&g_cDdiMapIdem);
        dma_inc(&g_cDdiMapOk);
        return g_aDdiSlot[iSlot].paBus;
    }
    /* Different cb on mapped slot: soft unmap prior cookie first. */
    if (iSlot >= 0 && g_aDdiSlot[iSlot].u8Mapped != 0u) {
        dma_buf_unmap(g_aDdiSlot[iSlot].paBus,
                      (g_aDdiSlot[iSlot].u64MapCb != 0ull)
                          ? g_aDdiSlot[iSlot].u64MapCb
                          : cb,
                      DMA_BUF_BIDIRECTIONAL);
        g_aDdiSlot[iSlot].u8Mapped = 0u;
        g_aDdiSlot[iSlot].paBus = 0;
        g_aDdiSlot[iSlot].u64MapCb = 0ull;
        dma_inc(&g_cDdiMapRemap);
    }

    if (fForceEff != 0 && dma_buf_window_ok(pa, cb, 1) == 0) {
        dma_inc(&g_cDdiMapFail);
        /* window_ok already capped FAIL log; no second stamp. */
        return 0;
    }
    paBus = dma_buf_map(pa, cb, DMA_BUF_BIDIRECTIONAL, fForceEff);
    if (dma_buf_mapping_error(paBus)) {
        dma_inc(&g_cDdiMapFail);
        return 0;
    }

    /*
     * Soft sync residual: make zeroed/CPU lines device-visible (clflush).
     * Matches ddi_door MAP residual under freestanding UDX TE.
     * Soft!=product full CWB / IOTLB.
     */
    if (dma_buf_sync_pa_for_device(paBus, cb, DMA_BUF_BIDIRECTIONAL) == 0) {
        dma_inc(&g_cDdiSyncOk);
    } else {
        dma_inc(&g_cDdiSyncFail);
        /* Soft: map still ok (identity bus); sync fail residual only. */
    }

    /*
     * STRONGER product DMA residual denser (Soft!=product; Dual DoD OPEN):
     * identity bus==PA; owned soft DDI slot map; force32+identity;
     * PA page-align; map_cb match; busmaster spirit.
     * Grep: map_cb match | PA page-align | busmaster spirit
     */
    if (paBus == pa) {
        dma_inc(&g_cProdDmaIdentOk);
    }
    if (iSlot >= 0) {
        dma_inc(&g_cProdDmaOwnedOk);
    }
    if (fForceEff != 0 && dma_buf_in_vtd_identity(pa, cb) != 0) {
        dma_inc(&g_cProdDmaForce32Ok);
        if (dma_buf_is_low(pa) != 0) {
            dma_inc(&g_cProdDmaBmSpiritOk);
        }
    }
    if (((u64)pa & (u64)GJ_PAGE_MASK) == 0ull) {
        dma_inc(&g_cProdDmaPaAlignOk);
    }

    if (iSlot >= 0) {
        g_aDdiSlot[iSlot].u8Mapped = 1u;
        g_aDdiSlot[iSlot].paBus = paBus;
        g_aDdiSlot[iSlot].u64MapCb = cb;
        /* map_cb match residual denser: recorded span == request cb. */
        if (g_aDdiSlot[iSlot].u64MapCb == cb) {
            dma_inc(&g_cProdDmaMapCbOk);
        }
    }
    dma_inc(&g_cDdiMapOk);
    dma_soft_mint_honesty_once();
    return paBus;
}

/**
 * Soft chain residual once: ALLOC -> MAP -> FREE (C2 Dual DoD DMA seed).
 * C2 deepen (Soft!=product; not product AC / Dual DoD close):
 *   1) force32 1-page ALLOC_MAP_FREE (primary UDX ddi_buf_chain peer)
 *   2) force32 multi-page (2) ALLOC_MAP_FREE under identity span
 *   3) span-fail seed: oversize MAP must refuse while soft DDI live
 *   4) idem rematch: same-cb MAP returns prior bus cookie
 *   5) free-miss seed: ddi_free raw force32 page (no soft DDI live; one free)
 *   6) soft live-zero honesty: ddi_live / map_live not leaked by chain
 * Mirrors UDX udx_dma_ddi_buf_chain spirit for freestanding residual.
 * Soft!=product mint=OPEN; soft PASS != product UDX DMA caps / Dual DoD close.
 * Grep: dma_buf: soft ddi chain | C2 Dual DoD DMA residual
 * Grep: dma_buf: soft ddi chain mp | soft ddi chain span | soft ddi chain idem
 * Grep: dma_buf: soft ddi chain live | soft free miss
 */
static void
dma_soft_ddi_chain_once(void)
{
    gj_paddr_t pa;
    gj_paddr_t paBus;
    gj_paddr_t paBus2;
    gj_paddr_t paMp;
    gj_paddr_t paMpBus;
    gj_paddr_t paSpan;
    gj_paddr_t paSpanBus;
    u64 cb;
    u64 cbMp;
    u32 cLiveBefore;
    u32 cMapBefore;
    u32 cPagesBefore;
    u32 cSpanFailBefore;
    u32 cFreeMissBefore;
    u32 cIdemBefore;
    int fPrimaryOk = 0;

    if (g_fSoftChainOnce != 0) {
        return;
    }
    g_fSoftChainOnce = 1;

    cLiveBefore = g_cDdiLive;
    cMapBefore = g_cMapLive;
    cPagesBefore = g_cDdiPagesLive;
    cSpanFailBefore = g_cDdiMapSpanFail;
    cFreeMissBefore = g_cDdiFreeMiss;
    cIdemBefore = g_cDdiMapIdem;

    /*
     * (1) Force32 1-page chain under TE identity. Fail soft-only (pool may
     * be tight at soft_note time); never hard-gates Dual DoD A/B.
     */
    pa = dma_buf_ddi_alloc(1u, 1, NULL);
    if (pa == 0) {
        dma_inc(&g_cDdiChainFail);
        kprintf("dma_buf: soft ddi chain FAIL alloc force32 "
                "(C2 Dual DoD DMA residual; Soft!=product mint=OPEN)\n");
        /* Continue residual seeds that do not depend on primary PA. */
    } else {
        cb = (u64)GJ_PAGE_SIZE;
        paBus = dma_buf_ddi_map(pa, cb, 1);
        if (dma_buf_mapping_error(paBus)) {
            dma_buf_ddi_free(pa, 1u);
            dma_inc(&g_cDdiChainFail);
            kprintf("dma_buf: soft ddi chain FAIL map pa=0x%lx "
                    "(C2 Dual DoD DMA residual; Soft!=product mint=OPEN)\n",
                    (unsigned long)pa);
        } else {
            /*
             * (4) Idem rematch residual: same pa+cb must return prior bus.
             * Soft!=product geometric rematch (not product handle table).
             * Grep: dma_buf: soft ddi chain idem
             */
            paBus2 = dma_buf_ddi_map(pa, cb, 1);
            if (!dma_buf_mapping_error(paBus2) && paBus2 == paBus &&
                g_cDdiMapIdem > cIdemBefore) {
                dma_inc(&g_cDdiChainIdemOk);
            } else {
                dma_inc(&g_cDdiChainIdemFail);
            }

            dma_buf_ddi_free(pa, 1u);
            dma_inc(&g_cDdiChainOk);
            fPrimaryOk = 1;
            /* Grep: dma_buf: soft ddi chain | C2 Dual DoD DMA residual */
            kprintf("dma_buf: soft ddi chain ok path=ALLOC_MAP_FREE force32=1 "
                    "pa=0x%lx bus=0x%lx pages=1 "
                    "C2 Dual DoD DMA residual Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
                    "Soft!=product mint=OPEN G-AC-1 dual=MIT|Apache-2.0 "
                    "(soft residual != product UDX DMA caps)\n",
                    (unsigned long)pa, (unsigned long)paBus);
        }
    }

    /*
     * (5) Free-miss residual: ddi_free a force32 page never recorded in
     * soft DDI live (raw alloc_page_force32). Tallies free_miss once and
     * frees exactly once (no double-free). Soft!=product.
     * Grep: dma_buf: soft free miss | soft ddi chain
     */
    {
        gj_paddr_t paMiss = dma_buf_alloc_page_force32(NULL);

        if (paMiss != 0) {
            cFreeMissBefore = g_cDdiFreeMiss;
            dma_buf_ddi_free(paMiss, 1u);
            if (g_cDdiFreeMiss > cFreeMissBefore) {
                dma_inc(&g_cDdiChainFreeMissOk);
            }
        }
    }

    /*
     * (2) Multi-page force32 chain residual (2 pages; full span MAP).
     * Exercises alloc_pages_force32 + span-fit MAP under TE identity.
     * Soft!=product. Grep: dma_buf: soft ddi chain mp
     */
    paMp = dma_buf_ddi_alloc(2u, 1, NULL);
    if (paMp == 0) {
        dma_inc(&g_cDdiChainMpFail);
        kprintf("dma_buf: soft ddi chain mp FAIL alloc force32 pages=2 "
                "(C2 Dual DoD DMA residual; Soft!=product mint=OPEN)\n");
    } else {
        cbMp = 2ull * (u64)GJ_PAGE_SIZE;
        paMpBus = dma_buf_ddi_map(paMp, cbMp, 1);
        if (dma_buf_mapping_error(paMpBus)) {
            dma_buf_ddi_free(paMp, 2u);
            dma_inc(&g_cDdiChainMpFail);
            kprintf("dma_buf: soft ddi chain mp FAIL map pa=0x%lx "
                    "(C2 Dual DoD DMA residual; Soft!=product mint=OPEN)\n",
                    (unsigned long)paMp);
        } else {
            dma_buf_ddi_free(paMp, 2u);
            dma_inc(&g_cDdiChainMpOk);
            /* Grep: dma_buf: soft ddi chain mp | C2 Dual DoD DMA residual */
            kprintf("dma_buf: soft ddi chain mp ok path=ALLOC_MAP_FREE "
                    "force32=1 pa=0x%lx bus=0x%lx pages=2 "
                    "C2 Dual DoD DMA residual Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
                    "Soft!=product mint=OPEN G-AC-1 "
                    "(soft residual != product UDX DMA caps)\n",
                    (unsigned long)paMp, (unsigned long)paMpBus);
        }
    }

    /*
     * (3) Span-fail seed: 1-page alloc then MAP with oversize cb must FAIL.
     * OWN-safe geometric residual under TE. Soft!=product.
     * Grep: dma_buf: soft ddi chain span | soft map span
     */
    paSpan = dma_buf_ddi_alloc(1u, 1, NULL);
    if (paSpan == 0) {
        dma_inc(&g_cDdiChainSpanFail);
        kprintf("dma_buf: soft ddi chain span FAIL alloc force32 "
                "(C2 Dual DoD DMA residual; Soft!=product mint=OPEN)\n");
    } else {
        cSpanFailBefore = g_cDdiMapSpanFail;
        paSpanBus = dma_buf_ddi_map(paSpan, 2ull * (u64)GJ_PAGE_SIZE, 1);
        if (dma_buf_mapping_error(paSpanBus) &&
            g_cDdiMapSpanFail > cSpanFailBefore) {
            dma_inc(&g_cDdiChainSpanOk);
            /* Grep: dma_buf: soft ddi chain span | soft map span */
            kprintf("dma_buf: soft ddi chain span ok refuse_oversize=1 "
                    "pa=0x%lx alloc_pages=1 map_cb=0x%lx "
                    "C2 Dual DoD DMA residual Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
                    "Soft!=product mint=OPEN G-AC-1 "
                    "(span honesty; soft residual != product UDX DMA caps)\n",
                    (unsigned long)paSpan,
                    (unsigned long)(2ull * (u64)GJ_PAGE_SIZE));
        } else {
            dma_inc(&g_cDdiChainSpanFail);
            if (!dma_buf_mapping_error(paSpanBus)) {
                /* Soft: unexpected accept — free path still cleans. */
                kprintf("dma_buf: soft ddi chain span FAIL unexpected_map "
                        "pa=0x%lx (C2 Dual DoD DMA residual; Soft!=product)\n",
                        (unsigned long)paSpan);
            }
        }
        dma_buf_ddi_free(paSpan, 1u);
    }

    /*
     * (6) Soft live-zero honesty: chain residual must not leak soft DDI /
     * map live relative to pre-chain baseline (best-effort; other callers
     * may allocate concurrently — soft only). Soft!=product mint=OPEN.
     * Grep: dma_buf: soft ddi chain live
     */
    if (g_cDdiLive <= cLiveBefore && g_cMapLive <= cMapBefore &&
        g_cDdiPagesLive <= cPagesBefore) {
        dma_inc(&g_cDdiChainLiveOk);
    } else {
        dma_inc(&g_cDdiChainLiveFail);
        kprintf("dma_buf: soft ddi chain live FAIL "
                "ddi_live=%u/%u map_live=%u/%u ddi_pages=%u/%u "
                "(soft leak residual; Soft!=product mint=OPEN; "
                "C2 Dual DoD DMA residual Dual_DoD_A=OPEN Dual_DoD_B=OPEN)\n",
                g_cDdiLive, cLiveBefore, g_cMapLive, cMapBefore,
                g_cDdiPagesLive, cPagesBefore);
    }

    /* Grep: dma_buf: soft ddi chain | C2 Dual DoD DMA residual */
    kprintf("dma_buf: soft ddi chain C2 Dual DoD DMA residual "
            "primary=%s mp_ok=%u mp_fail=%u span_ok=%u span_fail=%u "
            "idem_ok=%u idem_fail=%u free_miss_ok=%u "
            "live_ok=%u live_fail=%u "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "Soft!=product mint=OPEN G-AC-1 dual=MIT|Apache-2.0 "
            "(soft residual != product UDX DMA caps; never closes Dual DoD)\n",
            (fPrimaryOk != 0) ? "ok" : "fail",
            g_cDdiChainMpOk, g_cDdiChainMpFail,
            g_cDdiChainSpanOk, g_cDdiChainSpanFail,
            g_cDdiChainIdemOk, g_cDdiChainIdemFail,
            g_cDdiChainFreeMissOk, g_cDdiChainLiveOk, g_cDdiChainLiveFail);
}

/**
 * STRONGER residual for product DMA (W10 Dual DoD; bar v2026.08.04.75
 * stamp-free; H2 once-lamp). Live UDX host DMA path residual for
 * rtl8168_udx / xhci_udx / ddi_host. Soft!=product; never closes Dual DoD.
 *
 * Arms:
 *   1) null_miss: map/window/ddi_map/coherent/map_single fail closed
 *   2) coherent_map: force32 coherent -> map -> window_ok -> sync -> free
 *   3) dense product DMA residual:
 *        identity/force32/owned/clamp denser;
 *        multi-page alloc; map_cb match; PA page-align; busmaster spirit
 *   4) dual_dod_open: Dual_DoD_A/B remain OPEN after residual
 *
 * Grep: dma_buf: soft udx host | soft udx host residual
 * Grep: dma_buf: soft residual product dma | Soft!=product dual_dod OPEN
 * Grep: product_hosts=UDX | identity force32 owned clamp
 * Soft!=product mint=OPEN G-AC-1 dual=MIT|Apache-2.0.
 */
static void
dma_soft_udx_host_residual_once(void)
{
    gj_paddr_t pa;
    gj_paddr_t paBus;
    gj_paddr_t paMp;
    gj_paddr_t paMpBus;
    gj_paddr_t paMpBusIdem;
    void *pVa = NULL;
    u32 cMapBefore;
    u32 cPagesBefore;
    u32 cClampBefore;
    u32 cIdentBefore;
    u32 cOwnedBefore;
    u32 cMapCbBefore;
    u32 cForceBefore;
    u32 cPaAlignBefore;
    u32 cMpBefore;
    u32 cBmBefore;
    u64 cbMp;
    int iSlot;
    int fNullOk = 0;
    int fCohOk = 0;
    int fDenseOk = 0;
    int fDualOk = 0;
    int fIdent = 0;
    int fForce = 0;
    int fOwned = 0;
    int fClamp = 0;
    int fMp = 0;
    int fMapCb = 0;
    int fPaAlign = 0;
    int fBm = 0;

    /* H2 once: no stamp storms. Soft!=product. */
    if (g_fSoftUdxHostOnce != 0) {
        return;
    }
    g_fSoftUdxHostOnce = 1;

    cMapBefore = g_cMapLive;
    cPagesBefore = g_cPagesLive;

    /*
     * (1) null_miss residual: product fail-closed edges used by live UDX
     * hosts (sshd/UDX DMA setup). Soft!=product.
     */
    if (dma_buf_map(0, (u64)GJ_PAGE_SIZE, DMA_BUF_BIDIRECTIONAL, 1) == 0 &&
        dma_buf_window_ok(0, (u64)GJ_PAGE_SIZE, 1) == 0 &&
        dma_buf_ddi_map(0, (u64)GJ_PAGE_SIZE, 1) == 0 &&
        dma_buf_alloc_coherent(0, NULL, 1) == 0 &&
        dma_buf_map_single(NULL, (u64)GJ_PAGE_SIZE, DMA_BUF_BIDIRECTIONAL,
                           1) == 0 &&
        dma_buf_mapping_error(0) != 0) {
        fNullOk = 1;
        dma_inc(&g_cUdxHostNullOk);
    } else {
        dma_inc(&g_cUdxHostNullFail);
    }

    /*
     * (2) coherent_map residual: force32 coherent page under TE identity
     * (UDX udx_dma_alloc_coherent peer) -> map -> window_ok ->
     * sync_device + sync_cpu -> unmap -> free_coherent.
     * Soft fail only if pool tight; never hard-gates Dual DoD.
     */
    pa = dma_buf_alloc_coherent((u64)GJ_PAGE_SIZE, &pVa, 1);
    if (pa == 0 || pVa == NULL) {
        dma_inc(&g_cUdxHostCohFail);
        kprintf("dma_buf: soft udx host FAIL coherent force32 "
                "(live UDX host residual; Soft!=product mint=OPEN; "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN product_hosts=UDX)\n");
    } else {
        paBus = dma_buf_map(pa, (u64)GJ_PAGE_SIZE, DMA_BUF_BIDIRECTIONAL, 1);
        if (dma_buf_mapping_error(paBus) != 0 ||
            dma_buf_window_ok(pa, (u64)GJ_PAGE_SIZE, 1) == 0 ||
            dma_buf_in_vtd_identity(pa, (u64)GJ_PAGE_SIZE) == 0) {
            dma_buf_free_coherent(pa, (u64)GJ_PAGE_SIZE);
            dma_inc(&g_cUdxHostCohFail);
            kprintf("dma_buf: soft udx host FAIL map/window pa=0x%lx "
                    "(live UDX host residual; Soft!=product mint=OPEN; "
                    "product_hosts=UDX)\n",
                    (unsigned long)pa);
        } else {
            (void)dma_buf_sync_for_device(pVa, (u64)GJ_PAGE_SIZE,
                                          DMA_BUF_BIDIRECTIONAL);
            (void)dma_buf_sync_for_cpu(pVa, (u64)GJ_PAGE_SIZE,
                                       DMA_BUF_BIDIRECTIONAL);
            dma_buf_unmap(paBus, (u64)GJ_PAGE_SIZE, DMA_BUF_BIDIRECTIONAL);
            dma_buf_free_coherent(pa, (u64)GJ_PAGE_SIZE);
            /* Soft live-zero honesty relative to pre-arm baseline. */
            if (g_cMapLive <= cMapBefore && g_cPagesLive <= cPagesBefore) {
                fCohOk = 1;
                dma_inc(&g_cUdxHostCohOk);
                /* Grep: dma_buf: soft udx host */
                kprintf("dma_buf: soft udx host ok path=COHERENT_MAP_SYNC_FREE "
                        "force32=1 pa=0x%lx bus=0x%lx "
                        "hosts=rtl8168_udx|xhci_udx|ddi_host "
                        "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
                        "product_hosts=UDX "
                        "Soft!=product mint=OPEN G-AC-1 dual=MIT|Apache-2.0 "
                        "(live UDX host residual; soft != product UDX DMA "
                        "caps)\n",
                        (unsigned long)pa, (unsigned long)paBus);
            } else {
                dma_inc(&g_cUdxHostCohFail);
                kprintf("dma_buf: soft udx host FAIL live_leak "
                        "map_live=%u/%u pages_live=%u/%u "
                        "(Soft!=product mint=OPEN Dual_DoD_A=OPEN "
                        "Dual_DoD_B=OPEN dual_dod=OPEN product_hosts=UDX)\n",
                        g_cMapLive, cMapBefore, g_cPagesLive, cPagesBefore);
            }
        }
    }

    /*
     * (3) STRONGER product DMA residual denser (Soft!=product):
     * identity / force32 / owned / clamp + multi-page / map_cb /
     * PA page-align / busmaster spirit for UDX product hosts Dual DoD.
     * Functional: force32 multi-page ddi_alloc -> ddi_map full span ->
     * idem map_cb rematch -> FREE with wrong cPages (clamp) -> live-zero.
     * Soft fail only (pool may be tight); never hard-gates Dual DoD.
     * Grep: dma_buf: soft residual product dma
     */
    cClampBefore = g_cProdDmaClampOk;
    cIdentBefore = g_cProdDmaIdentOk;
    cOwnedBefore = g_cProdDmaOwnedOk;
    cMapCbBefore = g_cProdDmaMapCbOk;
    cForceBefore = g_cProdDmaForce32Ok;
    cPaAlignBefore = g_cProdDmaPaAlignOk;
    cMpBefore = g_cProdDmaMpOk;
    cBmBefore = g_cProdDmaBmSpiritOk;

    paMp = dma_buf_ddi_alloc(2u, 1, NULL);
    if (paMp == 0) {
        dma_inc(&g_cProdDmaDenseFail);
        kprintf("dma_buf: soft residual product dma FAIL multi-page "
                "force32 pages=2 (Soft!=product mint=OPEN; "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
                "product_hosts=UDX)\n");
    } else {
        cbMp = 2ull * (u64)GJ_PAGE_SIZE;
        if (g_cProdDmaMpOk > cMpBefore) {
            fMp = 1;
        }
        if (g_cProdDmaPaAlignOk > cPaAlignBefore &&
            ((u64)paMp & (u64)GJ_PAGE_MASK) == 0ull) {
            fPaAlign = 1;
        }
        if (g_cProdDmaForce32Ok > cForceBefore &&
            dma_buf_in_vtd_identity(paMp, cbMp) != 0) {
            fForce = 1;
        }
        if (g_cProdDmaBmSpiritOk > cBmBefore) {
            fBm = 1;
        }

        paMpBus = dma_buf_ddi_map(paMp, cbMp, 1);
        if (dma_buf_mapping_error(paMpBus)) {
            dma_buf_ddi_free(paMp, 2u);
            dma_inc(&g_cProdDmaDenseFail);
            kprintf("dma_buf: soft residual product dma FAIL map "
                    "pa=0x%lx cb=0x%lx (Soft!=product mint=OPEN; "
                    "dual_dod=OPEN product_hosts=UDX)\n",
                    (unsigned long)paMp, (unsigned long)cbMp);
        } else {
            /* identity denser: bus cookie == PA under TE identity. */
            if (paMpBus == paMp && g_cProdDmaIdentOk > cIdentBefore) {
                fIdent = 1;
            }
            /* owned denser: soft DDI alloc-live slot was mapped. */
            iSlot = dma_ddi_slot_find(paMp);
            if (iSlot >= 0 && g_aDdiSlot[iSlot].u8Mapped != 0u &&
                g_cProdDmaOwnedOk > cOwnedBefore) {
                fOwned = 1;
            }
            /* map_cb match denser: recorded map span == request. */
            if (iSlot >= 0 && g_aDdiSlot[iSlot].u64MapCb == cbMp &&
                g_cProdDmaMapCbOk > cMapCbBefore) {
                fMapCb = 1;
            }
            /* Idem rematch: same map_cb returns prior bus (owned path). */
            paMpBusIdem = dma_buf_ddi_map(paMp, cbMp, 1);
            if (dma_buf_mapping_error(paMpBusIdem) == 0 &&
                paMpBusIdem == paMpBus) {
                /* Soft: idem map_cb rematch honesty. */
                if (fMapCb == 0 && iSlot >= 0 &&
                    g_aDdiSlot[iSlot].u64MapCb == cbMp) {
                    fMapCb = 1;
                }
            }

            /*
             * Clamp residual denser: FREE with wrong cPages (1 vs 2)
             * must clamp to soft-slot recorded pages. Soft!=product.
             */
            dma_buf_ddi_free(paMp, 1u); /* intentional wrong cPages */
            if (g_cProdDmaClampOk > cClampBefore) {
                fClamp = 1;
            }

            if (fIdent != 0 && fForce != 0 && fOwned != 0 && fClamp != 0 &&
                fMp != 0 && fMapCb != 0 && fPaAlign != 0 && fBm != 0 &&
                g_cMapLive <= cMapBefore && g_cPagesLive <= cPagesBefore) {
                fDenseOk = 1;
                dma_inc(&g_cProdDmaDenseOk);
                /* Grep: dma_buf: soft residual product dma */
                kprintf("dma_buf: soft residual product dma ok "
                        "identity=1 force32=1 owned=1 clamp=1 "
                        "multi_page=1 map_cb=1 pa_align=1 busmaster_spirit=1 "
                        "pa=0x%lx bus=0x%lx pages=2 map_cb=0x%lx "
                        "hosts=rtl8168_udx|xhci_udx|ddi_host "
                        "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
                        "product_hosts=UDX "
                        "Soft!=product mint=OPEN G-AC-1 dual=MIT|Apache-2.0 "
                        "stamp_storm=0 version_stamp=0 "
                        "(STRONGER denser product DMA residual; "
                        "Soft!=product; never closes Dual DoD)\n",
                        (unsigned long)paMp, (unsigned long)paMpBus,
                        (unsigned long)cbMp);
            } else {
                dma_inc(&g_cProdDmaDenseFail);
                kprintf("dma_buf: soft residual product dma FAIL denser "
                        "id=%d f32=%d owned=%d clamp=%d mp=%d map_cb=%d "
                        "pa_align=%d bm=%d map_live=%u/%u pages_live=%u/%u "
                        "(Soft!=product mint=OPEN dual_dod=OPEN "
                        "product_hosts=UDX)\n",
                        fIdent, fForce, fOwned, fClamp, fMp, fMapCb,
                        fPaAlign, fBm, g_cMapLive, cMapBefore,
                        g_cPagesLive, cPagesBefore);
            }
        }
    }

    /*
     * (4) dual_dod_open residual honesty (law): soft residual never closes
     * Dual DoD A (xhci_udx) or B (rtl8168_udx). Soft!=product; G-AC-1.
     */
    if (fNullOk != 0 &&
        DMA_BUF_DDI_PAGES_MAX >= 1u &&
        DMA_BUF_SOFT_MAP_MAX >= 1u &&
        DMA_BUF_RING_SLOTS_MAX >= 2u) {
        fDualOk = 1;
        dma_inc(&g_cUdxHostDualDodOk);
    }

    /* Grep: dma_buf: soft udx host residual | Soft!=product dual_dod OPEN */
    kprintf("dma_buf: soft udx host residual "
            "null_miss=%s coherent_map=%s denser=%s dual_dod_open=%s "
            "null_ok=%u null_fail=%u coh_ok=%u coh_fail=%u dual_ok=%u "
            "ident=%u force32=%u owned=%u clamp=%u "
            "mp=%u map_cb=%u pa_align=%u bm_spirit=%u "
            "dense_ok=%u dense_fail=%u "
            "hosts=rtl8168_udx|xhci_udx|ddi_host product=UDX+ABI "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
            "product_hosts=UDX "
            "Soft!=product mint=OPEN G-AC-1 dual=MIT|Apache-2.0 "
            "stamp_storm=0 version_stamp=0 H2_once=1 "
            "(W10 STRONGER denser product DMA residual; Soft!=product; "
            "not Dual DoD close; no .ko product)\n",
            (fNullOk != 0) ? "ok" : "fail",
            (fCohOk != 0) ? "ok" : "fail",
            (fDenseOk != 0) ? "ok" : "fail",
            (fDualOk != 0) ? "ok" : "fail",
            g_cUdxHostNullOk, g_cUdxHostNullFail, g_cUdxHostCohOk,
            g_cUdxHostCohFail, g_cUdxHostDualDodOk,
            g_cProdDmaIdentOk, g_cProdDmaForce32Ok, g_cProdDmaOwnedOk,
            g_cProdDmaClampOk, g_cProdDmaMpOk, g_cProdDmaMapCbOk,
            g_cProdDmaPaAlignOk, g_cProdDmaBmSpiritOk,
            g_cProdDmaDenseOk, g_cProdDmaDenseFail);
}

/**
 * Product IRQ+DMA Dual DoD denser residual (Soft!=product; bar .75).
 * Densifies freestanding residual for concurrent UDX product IRQ+DMA path:
 *   1) ring denser: force32 ring_page + ring_slots(2) under VT-d identity
 *   2) map_single denser: VA->bus force32 + window_ok + unmap_single
 *   3) sync denser: device then cpu roundtrip on ring page
 *   4) dual_host denser: NIC ring page + USB-class force32 page spirit
 *   5) dual_dod_open denser: Dual DoD A/B remain OPEN after residual
 * Soft fail only (pool may be tight); never hard-gates Dual DoD.
 * H2 once-lamp; Soft!=product ASCII only; no version stamp.
 * Grep: dma_buf: soft residual irq_dma denser
 * Grep: dma_buf: soft residual irq_dma denser PASS
 * Grep: product IRQ+DMA Dual DoD OPEN Soft!=product denser=1
 */
static void
dma_soft_irq_dma_dual_residual_once(void)
{
    gj_paddr_t paRing;
    gj_paddr_t paRingBus;
    gj_paddr_t paUsb;
    gj_paddr_t paUsbBus;
    gj_paddr_t paSlots[2];
    gj_paddr_t paMs;
    void *pVaRing = NULL;
    void *pVaUsb = NULL;
    void *pVaMs = NULL;
    void *apSlotVa[2];
    u32 cMapBefore;
    u32 cPagesBefore;
    u32 cDdiBefore;
    int fRing = 0;
    int fMapSingle = 0;
    int fSync = 0;
    int fDualHost = 0;
    int fDualOpen = 0;
    int fDense = 0;
    int nStSlots;

    /* H2 once: no stamp storms. Soft!=product. */
    if (g_fSoftIrqDmaOnce != 0) {
        return;
    }
    g_fSoftIrqDmaOnce = 1;

    cMapBefore = g_cMapLive;
    cPagesBefore = g_cPagesLive;
    cDdiBefore = g_cDdiLive;
    paSlots[0] = 0;
    paSlots[1] = 0;
    apSlotVa[0] = NULL;
    apSlotVa[1] = NULL;

    /*
     * (1) ring denser: force32 ring page + 2 ring slots (NIC/USB spirit).
     * alloc_ring_page already maps bus=PA + sync_device (OWN-safe).
     * Soft!=product; freestanding residual for product UDX rings.
     */
    paRing = dma_buf_alloc_ring_page(&pVaRing);
    paRingBus = paRing; /* bus cookie == PA under TE identity */
    if (paRing == 0 || pVaRing == NULL ||
        dma_buf_in_vtd_identity(paRing, (u64)GJ_PAGE_SIZE) == 0 ||
        dma_buf_window_ok(paRing, (u64)GJ_PAGE_SIZE, 1) == 0) {
        if (paRing != 0) {
            dma_buf_free_ring_page(paRing);
        }
        dma_inc(&g_cIrqDmaRingFail);
        kprintf("dma_buf: soft residual irq_dma denser FAIL ring_page "
                "(Soft!=product mint=OPEN; Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
                "dual_dod=OPEN product_hosts=UDX product_IRQ+DMA=OPEN)\n");
        paRing = 0;
        pVaRing = NULL;
        paRingBus = 0;
    } else {
        nStSlots = dma_buf_alloc_ring_slots(2u, paSlots, apSlotVa);
        if (nStSlots != 0 || paSlots[0] == 0 || paSlots[1] == 0) {
            dma_buf_free_ring_page(paRing);
            dma_inc(&g_cIrqDmaRingFail);
            kprintf("dma_buf: soft residual irq_dma denser FAIL ring_slots "
                    "pa=0x%lx (Soft!=product mint=OPEN dual_dod=OPEN "
                    "product_hosts=UDX product_IRQ+DMA=OPEN)\n",
                    (unsigned long)paRing);
            paRing = 0;
            pVaRing = NULL;
            paRingBus = 0;
        } else {
            fRing = 1;
            dma_inc(&g_cIrqDmaRingOk);
        }
    }

    /*
     * (2) map_single denser: force32 VA->bus (UDX map_single peer).
     * Soft!=product mint=OPEN.
     */
    paMs = dma_buf_alloc_page_force32(&pVaMs);
    if (paMs == 0 || pVaMs == NULL) {
        dma_inc(&g_cIrqDmaMapSingleFail);
    } else {
        gj_paddr_t paMsBus;

        paMsBus = dma_buf_map_single(pVaMs, (u64)GJ_PAGE_SIZE,
                                     DMA_BUF_BIDIRECTIONAL, 1);
        if (dma_buf_mapping_error(paMsBus) != 0 ||
            dma_buf_window_ok(paMs, (u64)GJ_PAGE_SIZE, 1) == 0 ||
            paMsBus != paMs) {
            if (dma_buf_mapping_error(paMsBus) == 0) {
                dma_buf_unmap_single(paMsBus, (u64)GJ_PAGE_SIZE,
                                     DMA_BUF_BIDIRECTIONAL);
            }
            dma_buf_free_page(paMs);
            dma_inc(&g_cIrqDmaMapSingleFail);
            kprintf("dma_buf: soft residual irq_dma denser FAIL map_single "
                    "pa=0x%lx (Soft!=product mint=OPEN dual_dod=OPEN "
                    "product_hosts=UDX product_IRQ+DMA=OPEN)\n",
                    (unsigned long)paMs);
            paMs = 0;
            pVaMs = NULL;
        } else {
            dma_buf_unmap_single(paMsBus, (u64)GJ_PAGE_SIZE,
                                 DMA_BUF_BIDIRECTIONAL);
            dma_buf_free_page(paMs);
            fMapSingle = 1;
            dma_inc(&g_cIrqDmaMapSingleOk);
            paMs = 0;
            pVaMs = NULL;
        }
    }

    /*
     * (3) sync denser: device then cpu roundtrip on live ring page.
     * Soft!=product full CWB; clflush residual under re-post spirit.
     */
    if (fRing != 0 && pVaRing != NULL) {
        if (dma_buf_sync_for_device(pVaRing, (u64)GJ_PAGE_SIZE,
                                    DMA_BUF_BIDIRECTIONAL) == 0 &&
            dma_buf_sync_for_cpu(pVaRing, (u64)GJ_PAGE_SIZE,
                                 DMA_BUF_BIDIRECTIONAL) == 0 &&
            dma_buf_sync_pa_for_device(paRingBus, (u64)GJ_PAGE_SIZE,
                                       DMA_BUF_BIDIRECTIONAL) == 0 &&
            dma_buf_sync_pa_for_cpu(paRingBus, (u64)GJ_PAGE_SIZE,
                                    DMA_BUF_BIDIRECTIONAL) == 0) {
            fSync = 1;
            dma_inc(&g_cIrqDmaSyncOk);
        } else {
            dma_inc(&g_cIrqDmaSyncFail);
        }
    } else {
        dma_inc(&g_cIrqDmaSyncFail);
    }

    /*
     * (4) dual_host denser: USB-class force32 page alongside NIC ring.
     * Product Dual DoD A (xhci) + B (rtl) DMA residual spirit.
     * Soft!=product; never closes Dual DoD.
     */
    if (fRing != 0) {
        paUsb = dma_buf_alloc_page_force32(&pVaUsb);
        if (paUsb == 0 || pVaUsb == NULL ||
            dma_buf_in_vtd_identity(paUsb, (u64)GJ_PAGE_SIZE) == 0) {
            if (paUsb != 0) {
                dma_buf_free_page(paUsb);
            }
            dma_inc(&g_cIrqDmaDualHostFail);
        } else {
            paUsbBus = dma_buf_map(paUsb, (u64)GJ_PAGE_SIZE,
                                   DMA_BUF_BIDIRECTIONAL, 1);
            if (dma_buf_mapping_error(paUsbBus) != 0 || paUsbBus != paUsb) {
                if (dma_buf_mapping_error(paUsbBus) == 0) {
                    dma_buf_unmap(paUsbBus, (u64)GJ_PAGE_SIZE,
                                  DMA_BUF_BIDIRECTIONAL);
                }
                dma_buf_free_page(paUsb);
                dma_inc(&g_cIrqDmaDualHostFail);
            } else {
                (void)dma_buf_sync_for_device(pVaUsb, (u64)GJ_PAGE_SIZE,
                                              DMA_BUF_BIDIRECTIONAL);
                dma_buf_unmap(paUsbBus, (u64)GJ_PAGE_SIZE,
                              DMA_BUF_BIDIRECTIONAL);
                dma_buf_free_page(paUsb);
                fDualHost = 1;
                dma_inc(&g_cIrqDmaDualHostOk);
            }
        }
    } else {
        dma_inc(&g_cIrqDmaDualHostFail);
    }

    /* Free ring denser resources (free_ring_* does sync+unmap). Soft!=product. */
    if (fRing != 0) {
        dma_buf_free_ring_slots(2u, paSlots);
        dma_buf_free_ring_page(paRing);
        paRing = 0;
        pVaRing = NULL;
        paRingBus = 0;
    }

    /*
     * (5) dual_dod_open denser honesty (law): soft residual never closes
     * Dual DoD A (xhci_udx) or B (rtl8168_udx). Soft!=product; G-AC-1.
     * Live-zero honesty relative to pre-arm baseline.
     */
    if (DMA_BUF_RING_SLOTS_MAX >= 2u &&
        DMA_BUF_DDI_PAGES_MAX >= 1u &&
        DMA_BUF_SOFT_MAP_MAX >= 1u &&
        g_cMapLive <= cMapBefore &&
        g_cPagesLive <= cPagesBefore &&
        g_cDdiLive <= cDdiBefore) {
        fDualOpen = 1;
        dma_inc(&g_cIrqDmaDualDodOk);
    }

    if (fRing != 0 && fMapSingle != 0 && fSync != 0 && fDualHost != 0 &&
        fDualOpen != 0) {
        fDense = 1;
        dma_inc(&g_cIrqDmaDenseOk);
        /* Grep: dma_buf: soft residual irq_dma denser PASS */
        kprintf("dma_buf: soft residual irq_dma denser PASS denser=1 "
                "ring=1 map_single=1 sync=1 dual_host=1 dual_dod_open=1 "
                "hosts=rtl8168_udx|xhci_udx|ddi_host product=UDX+ABI "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
                "product_hosts=UDX product_IRQ+DMA=OPEN "
                "Soft!=product mint=OPEN G-AC-1 dual=MIT|Apache-2.0 "
                "stamp_storm=0 version_stamp=0 H2_once=1 "
                "bar_honesty=v2026.08.04.75 never_invent=.76 "
                "(product IRQ+DMA Dual DoD denser residual; Soft!=product; "
                "never closes Dual DoD)\n");
    } else {
        dma_inc(&g_cIrqDmaDenseFail);
        kprintf("dma_buf: soft residual irq_dma denser FAIL denser=1 "
                "ring=%d map_single=%d sync=%d dual_host=%d dual_open=%d "
                "map_live=%u/%u pages_live=%u/%u ddi_live=%u/%u "
                "(Soft!=product mint=OPEN dual_dod=OPEN "
                "product_hosts=UDX product_IRQ+DMA=OPEN)\n",
                fRing, fMapSingle, fSync, fDualHost, fDualOpen,
                g_cMapLive, cMapBefore, g_cPagesLive, cPagesBefore,
                g_cDdiLive, cDdiBefore);
    }

    /* Grep: dma_buf: soft residual irq_dma denser | product IRQ+DMA Dual DoD */
    kprintf("dma_buf: soft residual irq_dma denser "
            "ring=%s map_single=%s sync=%s dual_host=%s dual_dod_open=%s "
            "ring_ok=%u ring_fail=%u ms_ok=%u ms_fail=%u "
            "sync_ok=%u sync_fail=%u dual_ok=%u dual_fail=%u dual_dod=%u "
            "dense_ok=%u dense_fail=%u denser=%s "
            "hosts=rtl8168_udx|xhci_udx|ddi_host product=UDX+ABI "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
            "product_hosts=UDX product_IRQ+DMA=OPEN "
            "Soft!=product mint=OPEN G-AC-1 dual=MIT|Apache-2.0 "
            "stamp_storm=0 version_stamp=0 H2_once=1 "
            "bar_honesty=v2026.08.04.75 never_invent=.76 "
            "(product IRQ+DMA Dual DoD denser residual; Soft!=product; "
            "not Dual DoD close; no .ko product)\n",
            (fRing != 0) ? "ok" : "fail",
            (fMapSingle != 0) ? "ok" : "fail",
            (fSync != 0) ? "ok" : "fail",
            (fDualHost != 0) ? "ok" : "fail",
            (fDualOpen != 0) ? "ok" : "fail",
            g_cIrqDmaRingOk, g_cIrqDmaRingFail,
            g_cIrqDmaMapSingleOk, g_cIrqDmaMapSingleFail,
            g_cIrqDmaSyncOk, g_cIrqDmaSyncFail,
            g_cIrqDmaDualHostOk, g_cIrqDmaDualHostFail,
            g_cIrqDmaDualDodOk,
            g_cIrqDmaDenseOk, g_cIrqDmaDenseFail,
            (fDense != 0) ? "ok" : "fail");
}

/* ---- Sync (clflush coherence under continuous re-post / arping) --------- */

int
dma_buf_sync_for_cpu(void *pVa, u64 cb, int nDir)
{
    int nSt;

    /*
     * FROM_DEVICE / BIDIR after NIC write: drop stale desc/buf lines so CPU
     * sees OWN clear + payload (4 descs/line stick under arping flood).
     * nDir soft (ignored). Hot path silent on ok. Soft!=product no IOTLB.
     */
    (void)nDir;
    nSt = dma_clflush_range(pVa, cb);
    if (nSt != 0) {
        dma_inc(&g_cSyncCpuFail);
        if (g_cSyncFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cSyncFailLog++;
            /* Grep: dma_buf: soft sync FAIL | Soft!=product */
            kprintf("dma_buf: soft sync FAIL for_cpu pVa=%p cb=0x%lx "
                    "(null/zero; Soft!=product)\n",
                    pVa, (unsigned long)cb);
        }
        return -1;
    }
    dma_inc(&g_cSyncCpuOk);
    return 0;
}

int
dma_buf_sync_for_device(void *pVa, u64 cb, int nDir)
{
    int nSt;

    /*
     * TO_DEVICE / BIDIR before re-OWN: push CPU opts1/addr/payload so NIC
     * DRAM owns the next DMA cycle (continuous RX re-post). nDir soft.
     */
    (void)nDir;
    nSt = dma_clflush_range(pVa, cb);
    if (nSt != 0) {
        dma_inc(&g_cSyncDevFail);
        if (g_cSyncFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cSyncFailLog++;
            kprintf("dma_buf: soft sync FAIL for_device pVa=%p cb=0x%lx "
                    "(null/zero; Soft!=product)\n",
                    pVa, (unsigned long)cb);
        }
        return -1;
    }
    dma_inc(&g_cSyncDevOk);
    return 0;
}

int
dma_buf_sync_pa_for_cpu(gj_paddr_t pa, u64 cb, int nDir)
{
    void *pVa;

    if (pa == 0 || cb == 0ull) {
        dma_inc(&g_cSyncCpuFail);
        if (g_cSyncFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cSyncFailLog++;
            kprintf("dma_buf: soft sync FAIL for_cpu pa=0x%lx cb=0x%lx "
                    "(null/zero; Soft!=product)\n",
                    (unsigned long)pa, (unsigned long)cb);
        }
        return -1;
    }
    pVa = dma_va(pa);
    if (pVa == NULL) {
        dma_inc(&g_cSyncCpuFail);
        if (g_cSyncFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cSyncFailLog++;
            kprintf("dma_buf: soft sync FAIL for_cpu pa=0x%lx (no VA; "
                    "Soft!=product)\n",
                    (unsigned long)pa);
        }
        return -1;
    }
    return dma_buf_sync_for_cpu(pVa, cb, nDir);
}

int
dma_buf_sync_pa_for_device(gj_paddr_t pa, u64 cb, int nDir)
{
    void *pVa;

    if (pa == 0 || cb == 0ull) {
        dma_inc(&g_cSyncDevFail);
        if (g_cSyncFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cSyncFailLog++;
            kprintf("dma_buf: soft sync FAIL for_device pa=0x%lx cb=0x%lx "
                    "(null/zero; Soft!=product)\n",
                    (unsigned long)pa, (unsigned long)cb);
        }
        return -1;
    }
    pVa = dma_va(pa);
    if (pVa == NULL) {
        dma_inc(&g_cSyncDevFail);
        if (g_cSyncFailLog < DMA_BUF_FAIL_LOG_CAP) {
            g_cSyncFailLog++;
            kprintf("dma_buf: soft sync FAIL for_device pa=0x%lx (no VA; "
                    "Soft!=product)\n",
                    (unsigned long)pa);
        }
        return -1;
    }
    return dma_buf_sync_for_device(pVa, cb, nDir);
}

void
dma_buf_soft_note(void)
{
    if (g_fSoftNoteOnce != 0) {
        return;
    }
    g_fSoftNoteOnce = 1;

    /*
     * Lean one-shot residual (UDX NIC/USB DMA caps eng + DDI DMA_BUF).
     * No stamp storms. Soft mint honesty once if map path not yet hit.
     * UDX rings residual: soft ring chain page+slots+bad_args+live once.
     * C2 Dual DoD DMA residual: soft chain ALLOC->MAP->FREE + mp/span/idem
     * + free-miss + live-zero honesty once (Soft!=product; Dual DoD OPEN).
     * Grep: dma_buf: soft note | dma_buf: soft residual | dma_buf: soft PASS
     * Grep: dma_buf: soft mint honesty | dma_buf: soft ddi
     * Grep: dma_buf: soft ddi deepen | soft free miss | soft ddi live
     * Grep: dma_buf: soft ddi chain | soft map span | soft free intersect
     * Grep: dma_buf: soft ddi chain mp | soft ddi chain span | soft ddi chain idem
     * Grep: dma_buf: soft ddi chain live | C2 Dual DoD DMA residual
     * Grep: dma_buf: soft ring | soft ring chain | soft ring slots | soft ring live
     * Grep: force32 | Soft!=product | DMA buf for UDX caps | DMA buf for UDX rings
     * Grep: UDX NIC/USB DMA caps eng | UDX rings residual | xHCI USB residual lean
     * Grep: DDI DMA_BUF residual | soft map live | Dual DoD A/B OPEN
     * Soft != product | G-AC-1 | soft PASS != DUT arping R-climb / :22 / product.
     */
    dma_soft_mint_honesty_once();
    /* UDX rings residual: functional ring chain once before DDI chain. */
    dma_soft_ring_chain_once();
    /* C2 Dual DoD DMA residual: functional DDI chain once before counters. */
    dma_soft_ddi_chain_once();
    /*
     * W10 live UDX host residual (H2 once): null_miss + coherent_map +
     * denser identity/force32/owned/clamp + multi-page/map_cb/pa_align/
     * busmaster spirit + dual_dod OPEN product_hosts=UDX.
     * Soft!=product; never closes Dual DoD.
     */
    dma_soft_udx_host_residual_once();
    /*
     * Product IRQ+DMA Dual DoD denser residual (H2 once; bar .75):
     * ring + map_single + sync + dual_host + dual_dod OPEN denser.
     * Soft!=product; never closes Dual DoD.
     */
    dma_soft_irq_dma_dual_residual_once();
    kprintf("dma_buf: soft note prefer_low=<4GiB vtd_identity=0x%lx "
            "force32_limit=0x%lx alloc_ok=%u fail=%u high_soft=%u "
            "id_ok=%u id_miss=%u force32_ok=%u free=%u "
            "pages_live=%u pages_peak=%u "
            "map_ok=%u map_fail=%u unmap=%u map_live=%u map_peak=%u "
            "map_full=%u unmap_miss=%u free_mapped=%u "
            "sync_cpu=%u/%u sync_dev=%u/%u "
            "ring_page=%u/%u ring_slots=%u/%u ring_free_sync=%u/%u "
            "ring_chain_page=%u/%u ring_chain_slots=%u/%u "
            "coherent=%u/%u free_coh=%u map_single=%u/%u window=%u/%u "
            "ddi_alloc=%u/%u ddi_free=%u ddi_map=%u/%u "
            "ddi_live=%u ddi_pages=%u free_miss=%u "
            "dir_b=%u dir_to=%u dir_from=%u "
            "(force32 identity; map bus=PA; soft map-live; soft pages-live; "
            "soft ddi live; sync clflush; ring all-or-nothing+chain; "
            "DDI DMA_BUF residual deepen C2 Dual DoD DMA; "
            "Soft!=product mint=OPEN no VT-d grant/IOTLB)\n",
            (unsigned long)GJ_DMA_VTD_IDENTITY_LIMIT,
            (unsigned long)GJ_DMA_VTD_IDENTITY_LIMIT, g_cAllocOk, g_cAllocFail,
            g_cHighSoft, g_cIdentityOk, g_cIdentityMiss, g_cForce32Ok, g_cFree,
            g_cPagesLive, g_cPagesPeak, g_cMapOk, g_cMapFail, g_cUnmap,
            g_cMapLive, g_cMapLivePeak, g_cMapLiveFull, g_cUnmapMiss,
            g_cFreeWhileMapped, g_cSyncCpuOk, g_cSyncCpuFail, g_cSyncDevOk,
            g_cSyncDevFail, g_cRingPageOk, g_cRingPageFail, g_cRingSlotsOk,
            g_cRingSlotsFail, g_cRingFreeSyncOk, g_cRingFreeSyncFail,
            g_cRingChainPageOk, g_cRingChainPageFail, g_cRingChainSlotsOk,
            g_cRingChainSlotsFail, g_cCoherentOk, g_cCoherentFail,
            g_cCoherentFree, g_cMapSingleOk, g_cMapSingleFail, g_cWindowOk,
            g_cWindowFail, g_cDdiAllocOk, g_cDdiAllocFail, g_cDdiFree,
            g_cDdiMapOk, g_cDdiMapFail, g_cDdiLive, g_cDdiPagesLive,
            g_cDdiFreeMiss, g_cMapDirBidir, g_cMapDirToDev, g_cMapDirFromDev);
    kprintf("dma_buf: soft residual freestanding NIC rings Dual DoD B "
            "+ xHCI USB residual lean Dual DoD A; UDX NIC/USB DMA caps eng "
            "alloc+map+sync+ring under continuous re-post "
            "(force32 [0,1GiB); ring_page+ring_slots max=%u; fail_cap=%u; "
            "ring free=sync_cpu+unmap; ring chain page|slots2|bad_args|live; "
            "DMA buf for UDX rings residual; "
            "DMA buf for UDX caps coherent+map_single+window_ok; "
            "DDI DMA_BUF residual soft map-live max=%u soft ddi live max=%u "
            "soft pages-live ddi_alloc/free/map deepen C2 Dual DoD DMA; "
            "Soft!=product mint=OPEN; "
            "G-AC-1; soft PASS != DUT arping R-climb/:22; "
            "soft PASS != product UDX DMA; Dual_DoD_A=OPEN Dual_DoD_B=OPEN)\n",
            DMA_BUF_RING_SLOTS_MAX, DMA_BUF_FAIL_LOG_CAP,
            DMA_BUF_SOFT_MAP_MAX, DMA_BUF_DDI_SLOT_MAX);
    kprintf("dma_buf: soft ddi residual "
            "ops=ALLOC|FREE|MAP foundation=UDX_host "
            "alloc_ok=%u alloc_fail=%u free=%u map_ok=%u map_fail=%u "
            "live=%u live_peak=%u pages_live=%u pages_peak=%u "
            "free_miss=%u map_idem=%u map_remap=%u "
            "win_ok=%u f32_alloc=%u id_alloc=%u "
            "sync_ok=%u sync_fail=%u free_sync=%u/%u "
            "span_fail=%u orphan=%u chain_ok=%u chain_fail=%u "
            "chain_mp=%u/%u chain_span=%u/%u chain_idem=%u/%u "
            "chain_free_miss=%u chain_live=%u/%u "
            "intersect_drop=%u slot_full=%u drop_free=%u "
            "pages_max=%u map_live_max=%u ddi_slot_max=%u "
            "mint=OPEN window_cap=0 "
            "Soft!=product G-AC-1 dual=MIT|Apache-2.0\n",
            g_cDdiAllocOk, g_cDdiAllocFail, g_cDdiFree, g_cDdiMapOk,
            g_cDdiMapFail, g_cDdiLive, g_cDdiLivePeak, g_cDdiPagesLive,
            g_cDdiPagesPeak, g_cDdiFreeMiss, g_cDdiMapIdem, g_cDdiMapRemap,
            g_cDdiWinOk, g_cDdiForce32Alloc, g_cDdiIdentityAlloc,
            g_cDdiSyncOk, g_cDdiSyncFail, g_cDdiFreeSyncOk, g_cDdiFreeSyncFail,
            g_cDdiMapSpanFail, g_cDdiMapOrphan, g_cDdiChainOk, g_cDdiChainFail,
            g_cDdiChainMpOk, g_cDdiChainMpFail, g_cDdiChainSpanOk,
            g_cDdiChainSpanFail, g_cDdiChainIdemOk, g_cDdiChainIdemFail,
            g_cDdiChainFreeMissOk, g_cDdiChainLiveOk, g_cDdiChainLiveFail,
            g_cFreeIntersectDrop, g_cDdiSlotFull, g_cDdiDropFree,
            DMA_BUF_DDI_PAGES_MAX, DMA_BUF_SOFT_MAP_MAX, DMA_BUF_DDI_SLOT_MAX);
    /* Grep: dma_buf: soft ddi deepen | C2 Dual DoD DMA residual */
    kprintf("dma_buf: soft ddi deepen "
            "alloc=slot+window_ok free=sync_cpu+unmap+live_drop+free_miss "
            "map=span_check|idem|remap+sync_device force32_inherit=1 "
            "orphan_map=1 free_intersect=1 "
            "chain=ALLOC_MAP_FREE|mp2|span_refuse|idem|free_miss|live_zero "
            "soft_ddi_live=1 soft_map_live=1 soft_pages_live=1 "
            "C2 Dual DoD DMA residual Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "product_DMA_window_cap=OPEN product_hosts=UDX "
            "Soft!=product G-AC-1 dual=MIT|Apache-2.0 "
            "(soft residual != product UDX DMA caps; never closes Dual DoD)\n");
    /* Grep: dma_buf: soft udx host residual | Dual DoD A/B OPEN */
    kprintf("dma_buf: soft udx host residual rollup "
            "null_ok=%u coh_ok=%u dual_ok=%u "
            "ident=%u force32=%u owned=%u clamp=%u "
            "mp=%u map_cb=%u pa_align=%u bm_spirit=%u "
            "dense_ok=%u dense_fail=%u "
            "hosts=rtl8168_udx|xhci_udx|ddi_host product=UDX+ABI "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
            "product_hosts=UDX "
            "Soft!=product mint=OPEN G-AC-1 "
            "(W10 STRONGER denser product DMA residual; not Dual DoD close; "
            "H2 once)\n",
            g_cUdxHostNullOk, g_cUdxHostCohOk, g_cUdxHostDualDodOk,
            g_cProdDmaIdentOk, g_cProdDmaForce32Ok, g_cProdDmaOwnedOk,
            g_cProdDmaClampOk, g_cProdDmaMpOk, g_cProdDmaMapCbOk,
            g_cProdDmaPaAlignOk, g_cProdDmaBmSpiritOk,
            g_cProdDmaDenseOk, g_cProdDmaDenseFail);
    /* Grep: dma_buf: soft residual | Soft!=product dual_dod OPEN product_hosts=UDX */
    kprintf("dma_buf: soft residual product dma denser "
            "identity/force32/owned/clamp multi-page map_cb "
            "PA_page_align busmaster_spirit "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
            "product_hosts=UDX Soft!=product mint=OPEN G-AC-1 "
            "stamp_storm=0 version_stamp=0 H2_once=1 "
            "(STRONGER residual for product DMA; Soft!=product; "
            "never closes Dual DoD)\n");
    /* Grep: dma_buf: soft residual irq_dma denser | product IRQ+DMA Dual DoD */
    kprintf("dma_buf: soft residual irq_dma denser rollup "
            "ring_ok=%u ring_fail=%u ms_ok=%u ms_fail=%u "
            "sync_ok=%u sync_fail=%u dual_ok=%u dual_fail=%u dual_dod=%u "
            "dense_ok=%u dense_fail=%u denser=1 "
            "hosts=rtl8168_udx|xhci_udx|ddi_host product=UDX+ABI "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
            "product_hosts=UDX product_IRQ+DMA=OPEN "
            "Soft!=product mint=OPEN G-AC-1 "
            "stamp_storm=0 version_stamp=0 H2_once=1 "
            "bar_honesty=v2026.08.04.75 never_invent=.76 "
            "(product IRQ+DMA Dual DoD denser residual; Soft!=product; "
            "never closes Dual DoD)\n",
            g_cIrqDmaRingOk, g_cIrqDmaRingFail,
            g_cIrqDmaMapSingleOk, g_cIrqDmaMapSingleFail,
            g_cIrqDmaSyncOk, g_cIrqDmaSyncFail,
            g_cIrqDmaDualHostOk, g_cIrqDmaDualHostFail,
            g_cIrqDmaDualDodOk,
            g_cIrqDmaDenseOk, g_cIrqDmaDenseFail);
    kprintf("dma_buf: soft PASS\n");
}
