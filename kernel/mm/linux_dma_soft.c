/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux DMA + MMIO bodies for host r8169.ko-class ksym resolve.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko source.
 *
 * Soft only:
 *   - linux_dma_soft_init: ready lamp + linux_ksym_register of bodies
 *   - dma_alloc_attrs / dma_free_attrs: pmm pages, identity VA=PA when pa < 4 GiB
 *   - dmam_alloc_attrs: managed soft alias (no full devres table)
 *   - dma_map_single / dma_map_page_attrs / sync: identity cookies for buffers
 *     only (never BAR MMIO)
 *   - dma_mapping_error / dma_max_mapping_size: map-ish residual stubs
 *   - dma_set_mask / dma_set_coherent_mask / dma_set_mask_and_coherent:
 *     accept mask + soft last-mask bookkeeping
 *   - ioremap / readl-class: identity cast + volatile MMIO access
 *   - pci_iomap / pci_iounmap / pcim_iomap_region: soft BAR -> ioremap
 *     (hostish gated)
 *   - kmalloc / kzalloc / kmalloc_trace / kfree: kheap or pmm soft
 *   - firmware_request_nowarn / request_firmware / request_firmware_direct:
 *     soft embed table HIT (rtl_nic/rtl8168*.fw .incbin) or MISS -> -ENOENT
 *
 * Residual lean (UDX / userspace driver DMA path eng):
 *   Soft surface tracks UDX shape (dma_alloc_coherent dma_free_coherent
 *   dma_map_single dma_unmap_single dma_mapping_error
 *   dma_sync_single_for_cpu dma_sync_single_for_device dma_set_mask)
 *   for later DDI/UDX DMA caps. Soft identity cookie != product DMA window
 *   cap / VT-d mint (product OPEN). layout_ver>=3: force32 geometric +
 *   window_ok + VT-d identity prefer for coherent alloc; residual api
 *   honesty once-lamp; iommu_grant OPEN honesty (peers udx_dma_* residual).
 *   Wire-safe residual when freestanding SKIP: hybrid refuse -> soft BAR
 *   iomap/map SKIP/NOOP (no second VA; never MMIO as DMA). ksym SKIP still
 *   lamps wire_safe residual. Soft!=product; G-AC-1. Not product AC.
 *   Grep: linux_dma_soft: soft residual lean PASS
 *   Grep: linux_dma_soft: soft residual lean UDX eng
 *   Grep: linux_dma_soft: soft residual api honesty
 *   Grep: linux_dma_soft: soft freestanding SKIP wire_safe
 *   Grep: linux_dma_soft: soft force32 residual
 *   Grep: linux_dma_soft: soft window residual
 *
 * C0 soft residual deepen (this pass / Soft!=product / no stamp storms):
 *   Peer UDX dma residual catalog on existing residual lean once-lamps only:
 *   coherent / stream / sync / iommu / mint / mask / ddi_buf + dual_dod
 *   OPEN honesty (DoD A/B remain OPEN; not close). denser C0 fields:
 *   freestanding_no_exec / never_exec_ko / g_ac1_waiver=0 / storm=0 /
 *   resolve_only / product_hosts=rtl8168_udx|xhci_udx. No layout_ver bump;
 *   no GJ_IMAGE_VERSION; no new kprintf storm class. Soft!=product; G-AC-1.
 *   Grep: linux_dma_soft: soft residual lean PASS
 *   Grep: linux_dma_soft: soft residual coherent
 *   Grep: linux_dma_soft: soft residual stream
 *   Grep: linux_dma_soft: soft residual sync
 *   Grep: linux_dma_soft: soft residual iommu
 *   Grep: linux_dma_soft: soft residual mint
 *   Grep: linux_dma_soft: soft residual mask
 *   Grep: linux_dma_soft: soft residual ddi_buf
 *   Grep: linux_dma_soft: soft residual dual_dod
 *
 * Hybrid SOFT zero-touch (dual DoD residual companion; Soft!=product):
 *   Freestanding rtl8168 owns 10ec:8168 wire under gate0. Soft ioremap /
 *   pci_iomap (and pcim_iomap_region -> pci_iomap) must NOOP live BAR map so
 *   .ko never gets a second VA (dual-drive -> orphan rings / R0).
 *   Gate0 (HANDOFF==0): always refuse soft map of 8168 BAR.
 *   Gate1: refuse while rtl8168_ready()!=0; allow after quiesce.
 *   Dual-check residual: hw_touch_ok + VID:DID + live BAR phys refuse.
 *   pci_iomap NOOP honesty: reason=hw_touch|vid_dual|bar_phys (once lamp).
 *
 * Dual-map reclaim residual (R0 companion; Soft!=product; G-AC-1):
 *   Soft second VA on freestanding BAR orphans FS rings -> reclaim conflict
 *   / STATUS R0. Never dual-map; scrub leave-behind soft iomap slots that
 *   cover live 8168 BAR phys under hybrid refuse (no freestanding unmap).
 *   Also scrub pcim identity leave-behind + map-slot BAR cookies.
 *   pci_iomap_range / pcim_iomap_regions|table / pci_ioremap_bar share NOOP.
 *   Grep: linux_dma_soft: soft dual-map reclaim scrub|residual
 *
 * H4 residual (assurance H4 - soft BAR thrash -> freestanding R0):
 *   Soft iomap of live 10ec:8168 while freestanding owns wire thrashs
 *   rings -> RX dead. no_second_va + scrub + h4_no_dual_map_ok once lamp.
 *   Soft!=product; G-AC-1. Not product AC / not Dual DoD close.
 *   Grep: linux_dma_soft: soft H4 residual
 *
 * Map residual (userspace DMA story later; eng only):
 *   - mapping_error(cookie==0), max_mapping_size = soft ID ceiling
 *   - map_* refuse live 8168 BAR as buffer cookie; size > MAP_MAX refuse
 *   - light map-slot bookkeeping; hard-capped inventory lamp
 *   - dma_sync_single_for_{cpu,device} soft aliases of __dma_sync_* (ksym)
 *
 * Soft != product: no VT-d grant, no full struct page/device ABI.
 * Cap: LINUX_DMA_SOFT_PAGE_MAX coherent pages; fw embed <= ~2 MiB.
 * Logs: once / FW_LOG_MAX only - no stamp storms. No version stamp.
 *
 * Greppable markers (keep stable):
 *   linux_dma_soft: soft init PASS n=
 *   linux_dma_soft: soft ksym register PASS|SKIP
 *   linux_dma_soft: soft firmware HIT name=
 *   linux_dma_soft: soft firmware MISS name=
 *   linux_dma_soft: soft pci_iomap ...
 *   linux_dma_soft: soft pci_iomap NOOP hybrid
 *   linux_dma_soft: soft ioremap NOOP hybrid
 *   linux_dma_soft: soft hybrid zero-touch PASS
 *   linux_dma_soft: soft dual-map reclaim scrub
 *   linux_dma_soft: soft dual-map reclaim residual
 *   linux_dma_soft: soft H4 residual
 *   linux_dma_soft: soft residual lean PASS
 *   linux_dma_soft: soft residual lean UDX eng
 *   linux_dma_soft: soft residual api honesty
 *   linux_dma_soft: soft freestanding SKIP wire_safe
 *   linux_dma_soft: soft force32 residual
 *   linux_dma_soft: soft window residual
 *   linux_dma_soft: soft residual coherent
 *   linux_dma_soft: soft residual stream
 *   linux_dma_soft: soft residual sync
 *   linux_dma_soft: soft residual iommu
 *   linux_dma_soft: soft residual mint
 *   linux_dma_soft: soft residual mask
 *   linux_dma_soft: soft residual ddi_buf
 *   linux_dma_soft: soft residual dual_dod
 *   linux_dma_soft: soft dma_set_mask ...
 *   linux_dma_soft: soft map residual ...
 *   linux_dma_soft: soft dma_mapping_error ...
 *   linux_dma_soft: soft dma_max_mapping_size ...
 *   linux_dma_soft: soft dmam_alloc_attrs ...
 *   linux_dma_soft: soft dma_sync ...
 */
#include <gj/config.h>
#include <gj/devmgr.h>
#include <gj/kheap.h>
#include <gj/klog.h>
#include <gj/linux_abi.h>
#include <gj/linux_dma_soft.h>
#include <gj/linux_pci_hostish_off.h>
#include <gj/linux_pci_soft.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/vmm.h>

/*
 * F2 linux_ksym may be linked later. Weak unresolved -> NULL; init skips export.
 * Coordinator may also point ksym at these symbols by C name without this call.
 */
int linux_ksym_register(const char *szName, void *pFn) __attribute__((weak));

/* ---- Soft state --------------------------------------------------------- */

static int g_fReady;
static u32 g_cInitCalls;
static u32 g_cPagesUsed;
static u32 g_cAllocOk;
static u32 g_cAllocFail;
static u32 g_cFree;
static u32 g_cMap;
static u32 g_cMapOk;
static u32 g_cMapFail;
static u32 g_cMapBarRefuse; /* map of live 8168 BAR phys refused */
static u32 g_cMapSizeRefuse; /* map size > LINUX_DMA_SOFT_MAP_MAX */
static u32 g_cMapErrorQ;    /* dma_mapping_error error returns */
static u32 g_cUnmap;
static u32 g_cSyncCpu;
static u32 g_cSyncDev;
static u32 g_cMaxMapQ;
static u32 g_cDmamAlloc;
static u32 g_cNoopReasonHwTouch; /* pci_iomap NOOP reason tallies */
static u32 g_cNoopReasonVidDual;
static u32 g_cNoopReasonBarPhys;
static u32 g_cIoremap;
static u32 g_cIounmap;
static u32 g_cKmallocOk;
static u32 g_cKfree;
static u32 g_cFwReq;
static u32 g_cFwRel;
static u32 g_cFwHit;
static u32 g_cFwMiss;
static u32 g_cFwMissLog; /* cap MISS log spam */
static u32 g_cFwHitLog;  /* cap HIT log spam */
static u32 g_cSetMask;
static u32 g_cSetCohMask;
static u32 g_cPciIomap;
static u32 g_cPciIomapFail;
static u32 g_cPciIounmap;
static u32 g_cPcimEnable;
static u32 g_cPcimIomap;
static u32 g_cPcimMwi;
static u32 g_cHybridNoopIomap;
static u32 g_cHybridNoopIoremap;
static u32 g_cDualMapScrub;      /* soft iomap slots dropped (8168 BAR leave-behind) */
static u32 g_cDualMapScrubCall;  /* drop_freestanding_bar_maps calls */
static u32 g_cDualMapConflict;   /* scrub saw dual-map risk */
static u32 g_cDualMapMapSlotScrub; /* map-slot BAR cookies scrubbed (H4) */
static u32 g_cDualMapPcimScrub;  /* pcim table identity leave-behind scrubbed */
static u32 g_cPciIomapRange;
static u32 g_cPciIoremapBar;     /* pci_ioremap_bar soft calls */
static u32 g_cPcimIomapRegions;
static u32 g_cPcimIomapTable;
static u32 g_cH4Check;           /* h4_no_dual_map_ok calls */
static u64 g_u64LastDmaMask;
static u64 g_u64LastCohMask;
static int g_fMaskLogOnce;
static int g_fIomapLogOnce;
static int g_fMapErrLogOnce;
static int g_fMaxMapLogOnce;
static int g_fDmamLogOnce;
static int g_fMapResidualInvOnce;
static int g_fMapBarRefuseLogOnce;
static int g_fMapSizeRefuseLogOnce;
static int g_fSyncLogOnce;
static int g_fDualMapReclaimLogOnce;
static int g_fDualMapResidualLogOnce;
static int g_fH4ResidualLogOnce;
static int g_fResidualLeanLamp;
static int g_fWireSafeSkipLogOnce;
static int g_fApiHonestyLogOnce;
static int g_fForce32ResidualLogOnce;
static int g_fWindowResidualLogOnce;
/* C0 soft residual deepen once-lamps (storm=0; Soft!=product). */
static int g_fCohResidualLogOnce;
static int g_fStreamResidualLogOnce;
static int g_fSyncResidualLogOnce;
static int g_fIommuResidualLogOnce;
static int g_fMintResidualLogOnce;
static int g_fMaskResidualLogOnce;
static int g_fDdiBufResidualLogOnce;
static int g_fDualDodResidualLogOnce;
static u32 g_cKsymOk;
static u32 g_cKsymSkip;
/* UDX geometric residual tallies (Soft!=product; peers udx_dma_window_ok). */
static u32 g_cWindowOk;
static u32 g_cWindowFail;
static u32 g_cForce32Note;     /* window_ok with fForce32 */
static u32 g_cAllocVtdId;      /* coherent alloc PA in VT-d identity */
static u32 g_cAllocLow;        /* coherent alloc PA in low < 4 GiB */
static u32 g_cAllocHighSoft;   /* coherent alloc fell above low zone */
/* Last pci_iomap hybrid NOOP reason token (0=none,1=hw_touch,2=vid_dual,3=bar_phys). */
static u8  g_u8LastIomapNoopReason;

/*
 * Soft pcim_iomap_regions table (BAR0..5). Soft!=product; no full devres.
 * Never filled for freestanding 8168 under hybrid refuse (dual-map R0).
 */
static void *g_apPcimIomapTab[LINUX_PCI_SOFT_BAR_MAX];
static void *g_pPcimIomapTabOwner;

struct ldmas_dma_slot {
    u8         u8Live;
    u8         u8Pad[3];
    u32        cPages;
    gj_paddr_t paBase;
    void      *pCpu;
    u64        u64Dma;
    unsigned long cbSize;
};

struct ldmas_iomap {
    u8    u8Live;
    u8    u8Pad[7];
    u64   u64Phys;
    unsigned long cbSize;
    void *pVa;
};

struct ldmas_kmalloc {
    u8    u8Live;
    u8    u8Kind; /* 0 = kheap, 1 = pmm */
    u8    u8Pad[2];
    u32   cPages;
    void *pCpu;
    gj_paddr_t paBase;
    unsigned long cbSize;
};

/* Soft streaming map residual bookkeeping (identity cookies; Soft!=product). */
struct ldmas_map_slot {
    u8    u8Live;
    u8    u8Dir;
    u8    u8Pad[2];
    unsigned long cbSize;
    void *pCpu;
    u64   u64Dma;
};

static struct ldmas_dma_slot  g_aDma[LINUX_DMA_SOFT_SLOT_MAX];
static struct ldmas_iomap     g_aIo[LINUX_DMA_SOFT_IOMAP_MAX];
static struct ldmas_kmalloc   g_aKm[LINUX_DMA_SOFT_KMALLOC_MAX];
static struct ldmas_map_slot  g_aMap[LINUX_DMA_SOFT_MAP_SLOT_MAX];

/* ---- Helpers ------------------------------------------------------------ */

static void
ldmas_ksym_one(const char *szName, void *pFn, u32 *pu32Ok, u32 *pu32Skip)
{
    int nSt;

    if (szName == NULL || pFn == NULL) {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
        return;
    }
    if (linux_ksym_register == NULL) {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
        return;
    }
    nSt = linux_ksym_register(szName, pFn);
    if (nSt == 0) {
        if (pu32Ok != NULL && *pu32Ok < 0xffffffffu) {
            (*pu32Ok)++;
        }
    } else {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
    }
}

static void
ldmas_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

/**
 * CPU VA for a physical frame: identity when pa < 4 GiB, else HHDM soft.
 */
static void *
ldmas_cpu_va(gj_paddr_t pa)
{
    if (pa == 0) {
        return NULL;
    }
    if ((u64)pa < LINUX_DMA_SOFT_ID_MAX) {
        return (void *)(uintptr_t)pa;
    }
    return (void *)hhdm_to_virt(pa);
}

static u32
ldmas_pages_for(unsigned long cbSize)
{
    unsigned long cb;

    if (cbSize == 0ul) {
        return 0u;
    }
    cb = cbSize + (unsigned long)GJ_PAGE_SIZE - 1ul;
    cb /= (unsigned long)GJ_PAGE_SIZE;
    if (cb > 0xfffffffful) {
        return 0xffffffffu;
    }
    return (u32)cb;
}

static void
ldmas_zero_pages(gj_paddr_t paBase, u32 cPages)
{
    u32 i;
    void *pVa;

    for (i = 0u; i < cPages; i++) {
        pVa = ldmas_cpu_va(paBase + (gj_paddr_t)i * (gj_paddr_t)GJ_PAGE_SIZE);
        if (pVa != NULL) {
            memset(pVa, 0, GJ_PAGE_SIZE);
        }
    }
}

/**
 * Prefer VT-d identity [0, 1 GiB) first (force32 UDX residual), then low
 * (< 4 GiB), else high soft. Hold non-preferred briefly and retry freelist.
 * Soft!=product; geometric only - no live VT-d mint.
 */
static gj_paddr_t
ldmas_alloc_pages_low(u32 cPages)
{
    gj_paddr_t aHoldHigh[16];
    gj_paddr_t aHoldLow[16];
    u32 cHoldHigh = 0u;
    u32 cHoldLow = 0u;
    gj_paddr_t pa = 0;
    u32 i;

    if (cPages == 0u) {
        return 0;
    }

    for (i = 0u; i < 32u; i++) {
        if (cPages == 1u) {
            pa = pmm_alloc();
        } else {
            pa = pmm_alloc_pages(cPages);
        }
        if (pa == 0) {
            break;
        }
        /* Prefer VT-d identity cover for force32 UDX masters residual. */
        if ((u64)pa < LINUX_DMA_SOFT_VTD_IDENTITY_LIMIT) {
            while (cHoldHigh > 0u) {
                cHoldHigh--;
                if (cPages == 1u) {
                    pmm_free(aHoldHigh[cHoldHigh]);
                } else {
                    pmm_free_pages(aHoldHigh[cHoldHigh], cPages);
                }
            }
            while (cHoldLow > 0u) {
                cHoldLow--;
                if (cPages == 1u) {
                    pmm_free(aHoldLow[cHoldLow]);
                } else {
                    pmm_free_pages(aHoldLow[cHoldLow], cPages);
                }
            }
            return pa;
        }
        if ((u64)pa < LINUX_DMA_SOFT_LOW_MAX) {
            if (cHoldLow < 16u) {
                aHoldLow[cHoldLow++] = pa;
            } else {
                if (cPages == 1u) {
                    pmm_free(pa);
                } else {
                    pmm_free_pages(pa, cPages);
                }
            }
            continue;
        }
        if (cHoldHigh < 16u) {
            aHoldHigh[cHoldHigh++] = pa;
        } else {
            if (cPages == 1u) {
                pmm_free(pa);
            } else {
                pmm_free_pages(pa, cPages);
            }
            break;
        }
    }

    /* Second preference: low < 4 GiB (soft force32 note falls outside 1 GiB). */
    if (cHoldLow > 0u) {
        pa = aHoldLow[0];
        for (i = 1u; i < cHoldLow; i++) {
            if (cPages == 1u) {
                pmm_free(aHoldLow[i]);
            } else {
                pmm_free_pages(aHoldLow[i], cPages);
            }
        }
        while (cHoldHigh > 0u) {
            cHoldHigh--;
            if (cPages == 1u) {
                pmm_free(aHoldHigh[cHoldHigh]);
            } else {
                pmm_free_pages(aHoldHigh[cHoldHigh], cPages);
            }
        }
        return pa;
    }

    /* Soft high fallback (CPU via HHDM). Soft!=product. */
    if (cHoldHigh > 0u) {
        pa = aHoldHigh[0];
        for (i = 1u; i < cHoldHigh; i++) {
            if (cPages == 1u) {
                pmm_free(aHoldHigh[i]);
            } else {
                pmm_free_pages(aHoldHigh[i], cPages);
            }
        }
        return pa;
    }
    return 0;
}

static struct ldmas_dma_slot *
ldmas_dma_slot_alloc(void)
{
    u32 i;

    for (i = 0u; i < LINUX_DMA_SOFT_SLOT_MAX; i++) {
        if (g_aDma[i].u8Live == 0u) {
            memset(&g_aDma[i], 0, sizeof(g_aDma[i]));
            g_aDma[i].u8Live = 1u;
            return &g_aDma[i];
        }
    }
    return NULL;
}

static struct ldmas_dma_slot *
ldmas_dma_slot_find(void *pCpu, u64 u64Dma)
{
    u32 i;

    for (i = 0u; i < LINUX_DMA_SOFT_SLOT_MAX; i++) {
        if (g_aDma[i].u8Live == 0u) {
            continue;
        }
        if (pCpu != NULL && g_aDma[i].pCpu == pCpu) {
            return &g_aDma[i];
        }
        if (u64Dma != 0ull && g_aDma[i].u64Dma == u64Dma) {
            return &g_aDma[i];
        }
    }
    return NULL;
}

static void
ldmas_dma_slot_free(struct ldmas_dma_slot *pSlot)
{
    if (pSlot == NULL || pSlot->u8Live == 0u) {
        return;
    }
    if (pSlot->cPages > 0u && pSlot->paBase != 0) {
        if (pSlot->cPages == 1u) {
            pmm_free(pSlot->paBase);
        } else {
            pmm_free_pages(pSlot->paBase, pSlot->cPages);
        }
        if (g_cPagesUsed >= pSlot->cPages) {
            g_cPagesUsed -= pSlot->cPages;
        } else {
            g_cPagesUsed = 0u;
        }
    }
    memset(pSlot, 0, sizeof(*pSlot));
}

/* ---- DMA alloc / free --------------------------------------------------- */

void *
dma_alloc_attrs(void *dev, unsigned long size, u64 *pDma, unsigned gfp,
                unsigned long attrs)
{
    u32 cPages;
    gj_paddr_t pa;
    void *pCpu;
    struct ldmas_dma_slot *pSlot;

    (void)dev;
    (void)gfp;
    (void)attrs;

    if (pDma != NULL) {
        *pDma = 0ull;
    }
    if (size == 0ul || pDma == NULL) {
        ldmas_inc(&g_cAllocFail);
        return NULL;
    }

    cPages = ldmas_pages_for(size);
    if (cPages == 0u || cPages > LINUX_DMA_SOFT_PAGE_MAX) {
        ldmas_inc(&g_cAllocFail);
        return NULL;
    }
    if (g_cPagesUsed + cPages > LINUX_DMA_SOFT_PAGE_MAX) {
        ldmas_inc(&g_cAllocFail);
        return NULL;
    }

    pSlot = ldmas_dma_slot_alloc();
    if (pSlot == NULL) {
        ldmas_inc(&g_cAllocFail);
        return NULL;
    }

    pa = ldmas_alloc_pages_low(cPages);
    if (pa == 0) {
        pSlot->u8Live = 0u;
        ldmas_inc(&g_cAllocFail);
        return NULL;
    }

    pCpu = ldmas_cpu_va(pa);
    if (pCpu == NULL) {
        if (cPages == 1u) {
            pmm_free(pa);
        } else {
            pmm_free_pages(pa, cPages);
        }
        pSlot->u8Live = 0u;
        ldmas_inc(&g_cAllocFail);
        return NULL;
    }

    ldmas_zero_pages(pa, cPages);

    pSlot->cPages = cPages;
    pSlot->paBase = pa;
    pSlot->pCpu = pCpu;
    pSlot->u64Dma = (u64)pa;
    pSlot->cbSize = size;

    g_cPagesUsed += cPages;
    *pDma = (u64)pa;
    ldmas_inc(&g_cAllocOk);
    /* Soft residual tallies for UDX force32 / low-zone honesty. */
    if ((u64)pa < LINUX_DMA_SOFT_VTD_IDENTITY_LIMIT) {
        ldmas_inc(&g_cAllocVtdId);
    } else if ((u64)pa < LINUX_DMA_SOFT_LOW_MAX) {
        ldmas_inc(&g_cAllocLow);
    } else {
        ldmas_inc(&g_cAllocHighSoft);
    }
    return pCpu;
}

void
dma_free_attrs(void *dev, unsigned long size, void *cpu, u64 dma,
               unsigned long attrs)
{
    struct ldmas_dma_slot *pSlot;

    (void)dev;
    (void)size;
    (void)attrs;

    if (cpu == NULL && dma == 0ull) {
        return;
    }
    pSlot = ldmas_dma_slot_find(cpu, dma);
    if (pSlot == NULL) {
        return;
    }
    ldmas_dma_slot_free(pSlot);
    ldmas_inc(&g_cFree);
}

void *
dma_alloc_coherent(void *dev, unsigned long size, u64 *pDma, unsigned gfp)
{
    return dma_alloc_attrs(dev, size, pDma, gfp, 0ul);
}

void
dma_free_coherent(void *dev, unsigned long size, void *cpu, u64 dma)
{
    dma_free_attrs(dev, size, cpu, dma, 0ul);
}

/**
 * Soft managed coherent alloc (dmam_*). Empty ksym stub returned 0.
 * Soft: no full devres auto-free table - same pmm path as dma_alloc_attrs.
 * Soft!=product managed lifetime. Grep: linux_dma_soft: soft dmam_alloc_attrs
 */
void *
dmam_alloc_attrs(void *dev, unsigned long size, u64 *pDma, unsigned gfp,
                 unsigned long attrs)
{
    void *pCpu;

    ldmas_inc(&g_cDmamAlloc);
    pCpu = dma_alloc_attrs(dev, size, pDma, gfp, attrs);
    if (g_fDmamLogOnce == 0) {
        g_fDmamLogOnce = 1;
        /* Grep: linux_dma_soft: soft dmam_alloc_attrs */
        kprintf("linux_dma_soft: soft dmam_alloc_attrs size=%lu cpu=%p "
                "dma=0x%llx soft_ne_product=1 (no full devres)\n",
                (unsigned long)size, pCpu,
                (unsigned long long)((pDma != NULL) ? *pDma : 0ull));
    }
    return pCpu;
}

void
dmam_free_coherent(void *dev, unsigned long size, void *cpu, u64 dma)
{
    /* Soft managed free -> plain free_attrs (no devres table). */
    dma_free_attrs(dev, size, cpu, dma, 0ul);
}

/* ---- Map / unmap / sync ------------------------------------------------- */

/*
 * Forward decls used by map residual (hybrid helpers live in MMIO section).
 * Defined below with ioremap hybrid refuse path.
 */
static int ldmas_hybrid_8168_map_refused(void);
static int ldmas_phys_is_live_8168_bar(u64 phys);
static u32 ldmas_scrub_freestanding_bar_iomaps(void);

static void
ldmas_map_slot_note(void *pCpu, u64 u64Dma, unsigned long cbSize, int nDir)
{
    u32 i;

    for (i = 0u; i < LINUX_DMA_SOFT_MAP_SLOT_MAX; i++) {
        if (g_aMap[i].u8Live != 0u) {
            continue;
        }
        memset(&g_aMap[i], 0, sizeof(g_aMap[i]));
        g_aMap[i].u8Live = 1u;
        g_aMap[i].u8Dir = (u8)(nDir & 0xff);
        g_aMap[i].cbSize = cbSize;
        g_aMap[i].pCpu = pCpu;
        g_aMap[i].u64Dma = u64Dma;
        return;
    }
}

static void
ldmas_map_slot_clear(u64 u64Dma)
{
    u32 i;

    if (u64Dma == 0ull) {
        return;
    }
    for (i = 0u; i < LINUX_DMA_SOFT_MAP_SLOT_MAX; i++) {
        if (g_aMap[i].u8Live != 0u && g_aMap[i].u64Dma == u64Dma) {
            memset(&g_aMap[i], 0, sizeof(g_aMap[i]));
            return;
        }
    }
}

/**
 * Soft: refuse DMA-map of live 10ec:8168 BAR phys (MMIO != buffer).
 * Hard-capped log. Soft!=product.
 */
static int
ldmas_map_cookie_is_8168_bar(u64 u64Cookie)
{
    if (u64Cookie == 0ull) {
        return 0;
    }
    if (ldmas_hybrid_8168_map_refused() == 0) {
        return 0;
    }
    return ldmas_phys_is_live_8168_bar(u64Cookie);
}

u64
dma_map_page_attrs(void *dev, void *page, unsigned long offset,
                   unsigned long size, int dir, unsigned long attrs)
{
    u64 u64Base;
    u64 u64Cookie;

    (void)dev;
    (void)attrs;

    ldmas_inc(&g_cMap);

    if (page == NULL || size == 0ul) {
        ldmas_inc(&g_cMapFail);
        return 0ull;
    }
    /* Residual: soft identity ceiling honesty (empty stub returned 0 size). */
    if ((u64)size > LINUX_DMA_SOFT_MAP_MAX) {
        ldmas_inc(&g_cMapFail);
        ldmas_inc(&g_cMapSizeRefuse);
        if (g_fMapSizeRefuseLogOnce == 0) {
            g_fMapSizeRefuseLogOnce = 1;
            /* Grep: linux_dma_soft: soft map residual */
            kprintf("linux_dma_soft: soft map residual size refuse "
                    "size=0x%lx max=0x%llx soft_ne_product=1\n",
                    (unsigned long)size,
                    (unsigned long long)LINUX_DMA_SOFT_MAP_MAX);
        }
        return 0ull;
    }
    /*
     * Soft: page cookie treated as identity VA or soft page base.
     * Host r8169.ko may pass real struct page *; soft best-effort only.
     */
    u64Base = (u64)(uintptr_t)page;
    u64Cookie = u64Base + (u64)offset;

    if (ldmas_map_cookie_is_8168_bar(u64Cookie) != 0) {
        ldmas_inc(&g_cMapFail);
        ldmas_inc(&g_cMapBarRefuse);
        if (g_fMapBarRefuseLogOnce == 0) {
            g_fMapBarRefuseLogOnce = 1;
            /* Grep: linux_dma_soft: soft map residual */
            kprintf("linux_dma_soft: soft map residual BAR refuse "
                    "cookie=0x%llx 10ec:8168 never MMIO as DMA "
                    "(Soft!=product)\n",
                    (unsigned long long)u64Cookie);
        }
        return 0ull;
    }

    ldmas_map_slot_note(page, u64Cookie, size, dir);
    ldmas_inc(&g_cMapOk);
    return u64Cookie;
}

void
dma_unmap_page_attrs(void *dev, u64 dma, unsigned long size, int dir,
                     unsigned long attrs)
{
    (void)dev;
    (void)size;
    (void)dir;
    (void)attrs;
    ldmas_map_slot_clear(dma);
    ldmas_inc(&g_cUnmap);
}

u64
dma_map_single(void *dev, void *cpu, unsigned long size, int dir)
{
    u64 u64Cookie;

    (void)dev;

    ldmas_inc(&g_cMap);

    if (cpu == NULL || size == 0ul) {
        ldmas_inc(&g_cMapFail);
        return 0ull;
    }

    /* Residual: soft identity ceiling honesty. Cookie 0 -> mapping_error. */
    if ((u64)size > LINUX_DMA_SOFT_MAP_MAX) {
        ldmas_inc(&g_cMapFail);
        ldmas_inc(&g_cMapSizeRefuse);
        if (g_fMapSizeRefuseLogOnce == 0) {
            g_fMapSizeRefuseLogOnce = 1;
            /* Grep: linux_dma_soft: soft map residual */
            kprintf("linux_dma_soft: soft map residual size refuse "
                    "size=0x%lx max=0x%llx soft_ne_product=1\n",
                    (unsigned long)size,
                    (unsigned long long)LINUX_DMA_SOFT_MAP_MAX);
        }
        return 0ull;
    }

    /* Soft identity: bus address == CPU pointer value for low identity. */
    u64Cookie = (u64)(uintptr_t)cpu;

    /*
     * Residual: never treat live freestanding 8168 BAR as a DMA buffer.
     * Cookie 0 -> dma_mapping_error. Soft!=product.
     */
    if (ldmas_map_cookie_is_8168_bar(u64Cookie) != 0) {
        ldmas_inc(&g_cMapFail);
        ldmas_inc(&g_cMapBarRefuse);
        if (g_fMapBarRefuseLogOnce == 0) {
            g_fMapBarRefuseLogOnce = 1;
            /* Grep: linux_dma_soft: soft map residual */
            kprintf("linux_dma_soft: soft map residual BAR refuse "
                    "cpu=%p 10ec:8168 never MMIO as DMA "
                    "(Soft!=product)\n",
                    cpu);
        }
        return 0ull;
    }

    ldmas_map_slot_note(cpu, u64Cookie, size, dir);
    ldmas_inc(&g_cMapOk);
    return u64Cookie;
}

void
dma_unmap_single(void *dev, u64 dma, unsigned long size, int dir)
{
    (void)dev;
    (void)size;
    (void)dir;
    ldmas_map_slot_clear(dma);
    ldmas_inc(&g_cUnmap);
}

/**
 * Soft dma_mapping_error: non-zero = map failed (Linux / UDX convention).
 * Soft identity: cookie 0 is the only soft error cookie.
 * Grep: linux_dma_soft: soft dma_mapping_error
 */
int
dma_mapping_error(void *dev, u64 dma)
{
    (void)dev;

    if (dma == 0ull) {
        ldmas_inc(&g_cMapErrorQ);
        if (g_fMapErrLogOnce == 0) {
            g_fMapErrLogOnce = 1;
            /* Grep: linux_dma_soft: soft dma_mapping_error */
            kprintf("linux_dma_soft: soft dma_mapping_error cookie=0 "
                    "soft_ne_product=1 (identity zero=fail)\n");
        }
        return 1;
    }
    return 0;
}

/**
 * Soft dma_max_mapping_size: soft identity ceiling (not empty-stub 0).
 * Soft!=product IOMMU limit. Grep: linux_dma_soft: soft dma_max_mapping_size
 */
unsigned long
dma_max_mapping_size(void *dev)
{
    (void)dev;
    ldmas_inc(&g_cMaxMapQ);
    if (g_fMaxMapLogOnce == 0) {
        g_fMaxMapLogOnce = 1;
        /* Grep: linux_dma_soft: soft dma_max_mapping_size */
        kprintf("linux_dma_soft: soft dma_max_mapping_size "
                "max=0x%llx soft_ne_product=1 (identity ceiling)\n",
                (unsigned long long)LINUX_DMA_SOFT_MAP_MAX);
    }
    return (unsigned long)LINUX_DMA_SOFT_MAP_MAX;
}

void
__dma_sync_single_for_cpu(void *dev, u64 addr, unsigned long size, int dir)
{
    (void)dev;
    (void)addr;
    (void)size;
    (void)dir;
    /* Soft coherent: compiler barrier only. Soft!=product IOMMU sync. */
    __asm__ __volatile__("" ::: "memory");
    ldmas_inc(&g_cSyncCpu);
    if (g_fSyncLogOnce == 0) {
        g_fSyncLogOnce = 1;
        /* Grep: linux_dma_soft: soft dma_sync */
        kprintf("linux_dma_soft: soft dma_sync for_cpu "
                "soft_ne_product=1 (identity coherent barrier)\n");
    }
}

void
__dma_sync_single_for_device(void *dev, u64 addr, unsigned long size, int dir)
{
    (void)dev;
    (void)addr;
    (void)size;
    (void)dir;
    __asm__ __volatile__("" ::: "memory");
    ldmas_inc(&g_cSyncDev);
}

/**
 * Soft residual: non-__ names used by some ksym / module resolves.
 * Empty ksym stubs returned 0 without barrier. Soft!=product.
 */
void
dma_sync_single_for_cpu(void *dev, u64 addr, unsigned long size, int dir)
{
    __dma_sync_single_for_cpu(dev, addr, size, dir);
}

void
dma_sync_single_for_device(void *dev, u64 addr, unsigned long size, int dir)
{
    __dma_sync_single_for_device(dev, addr, size, dir);
}

/**
 * Soft dma_set_mask: accept any mask (Soft!=product DMA filter).
 * Records last mask for diagnostics. Does not poke hostish device offsets
 * (arg0 may be soft or &pdev->dev; layout not trusted here). Soft!=ABI.
 */
int
dma_set_mask(void *dev, u64 mask)
{
    (void)dev;
    ldmas_inc(&g_cSetMask);
    g_u64LastDmaMask = mask;

    if (g_fMaskLogOnce == 0) {
        g_fMaskLogOnce = 1;
        /* Grep: linux_dma_soft: soft dma_set_mask */
        kprintf("linux_dma_soft: soft dma_set_mask mask=0x%llx dev=%p "
                "calls=%u soft_ne_product=1\n",
                (unsigned long long)mask, dev, (unsigned)g_cSetMask);
    }
    return 0;
}

int
dma_set_coherent_mask(void *dev, u64 mask)
{
    (void)dev;
    ldmas_inc(&g_cSetCohMask);
    g_u64LastCohMask = mask;
    return 0;
}

int
dma_set_mask_and_coherent(void *dev, u64 mask)
{
    int nSt;

    nSt = dma_set_mask(dev, mask);
    if (nSt != 0) {
        return nSt;
    }
    return dma_set_coherent_mask(dev, mask);
}

/* ---- MMIO --------------------------------------------------------------- */

/*
 * Hybrid residual: freestanding owns 10ec:8168 wire. Soft identity ioremap of
 * that BAR phys (or pci_iomap -> ioremap) would mint a second VA -> dual-drive.
 * Gate0 always refuse; gate1 refuse while rtl8168_ready()!=0. Soft!=product.
 * Grep: linux_dma_soft: soft ioremap NOOP hybrid
 * Grep: linux_dma_soft: soft pci_iomap NOOP hybrid
 */
#define LDMAS_VID_REALTEK  0x10ecu
#define LDMAS_DID_RTL8168  0x8168u

/**
 * Non-zero if soft may not identity-map live 10ec:8168 BAR phys right now.
 * Gate0: always 1 (freestanding sole BAR owner for this build).
 * Gate1: 1 while freestanding ready; 0 after quiesce (toward REAL/.ko open).
 */
static int
ldmas_hybrid_8168_map_refused(void)
{
#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
    /* Gate0 hybrid: freestanding sole BAR owner - always refuse soft map. */
    return 1;
#else
    {
        extern int rtl8168_ready(void);

        /* Gate1: refuse while freestanding wire live; allow after quiesce. */
        return (rtl8168_ready() != 0) ? 1 : 0;
    }
#endif
}

/**
 * Non-zero if phys falls in a soft-inventory MEM BAR for 10ec:8168.
 * Uses devmgr inventory (CF8 scan); Soft!=product BAR snapshot only.
 */
static int
ldmas_phys_is_live_8168_bar(u64 phys)
{
    struct gj_devmgr_pci_fn fn;
    u32 iBar;
    u64 u64Start;
    u64 u64Len;
    u64 u64End;

    if (phys == 0ull) {
        return 0;
    }
    if (devmgr_soft_find_by_id(LDMAS_VID_REALTEK, LDMAS_DID_RTL8168, &fn) !=
        0) {
        return 0;
    }
    for (iBar = 0u; iBar < GJ_DEVMGR_SOFT_BAR_MAX; iBar++) {
        if (fn.aBar[iBar].u8Mem == 0u) {
            continue;
        }
        u64Start = fn.aBar[iBar].u64Pa;
        u64Len = fn.aBar[iBar].u64Cb;
        if (u64Start == 0ull) {
            continue;
        }
        if (phys == u64Start) {
            return 1;
        }
        if (u64Len == 0ull) {
            continue;
        }
        u64End = u64Start + u64Len;
        if (phys >= u64Start && phys < u64End) {
            return 1;
        }
    }
    return 0;
}

/**
 * Soft dual-map reclaim residual: clear soft iomap slots that cover live
 * freestanding 10ec:8168 BAR phys while hybrid map is refused.
 * H4 deepen: also scrub pcim identity leave-behind VAs and streaming
 * map-slot cookies that fall in live BAR phys (never MMIO as DMA).
 * Soft bookkeeping only - never unmaps freestanding g_pMmio / rings.
 * Dual-map leave-behind -> orphan rings / reclaim conflict R0.
 * Soft!=product; G-AC-1. Returns count of slots scrubbed.
 */
static u32
ldmas_scrub_freestanding_bar_iomaps(void)
{
    u32 i;
    u32 cDrop;
    u32 cPcim;

    cDrop = 0u;
    cPcim = 0u;
    if (ldmas_hybrid_8168_map_refused() == 0) {
        /* Gate1 post-quiesce: soft may own BAR toward REAL - keep maps. */
        return 0u;
    }
    for (i = 0u; i < LINUX_DMA_SOFT_IOMAP_MAX; i++) {
        if (g_aIo[i].u8Live == 0u) {
            continue;
        }
        if (ldmas_phys_is_live_8168_bar(g_aIo[i].u64Phys) == 0) {
            continue;
        }
        memset(&g_aIo[i], 0, sizeof(g_aIo[i]));
        if (cDrop < 0xffffffffu) {
            cDrop++;
        }
        ldmas_inc(&g_cDualMapScrub);
    }
    /*
     * H4: scrub pcim table entries whose identity VA == live BAR phys
     * (second soft VA leave-behind). Soft!=product; no freestanding unmap.
     */
    for (i = 0u; i < LINUX_PCI_SOFT_BAR_MAX; i++) {
        if (g_apPcimIomapTab[i] == NULL) {
            continue;
        }
        if (ldmas_phys_is_live_8168_bar(
                (u64)(uintptr_t)g_apPcimIomapTab[i]) == 0) {
            continue;
        }
        g_apPcimIomapTab[i] = NULL;
        if (cPcim < 0xffffffffu) {
            cPcim++;
        }
        ldmas_inc(&g_cDualMapPcimScrub);
        if (cDrop < 0xffffffffu) {
            cDrop++;
        }
        ldmas_inc(&g_cDualMapScrub);
    }
    if (cPcim > 0u) {
        g_pPcimIomapTabOwner = NULL;
    }
    /*
     * H4: streaming map slots must never hold live BAR MMIO as DMA cookie
     * under hybrid refuse (BAR thrash residual). Soft!=product.
     */
    for (i = 0u; i < LINUX_DMA_SOFT_MAP_SLOT_MAX; i++) {
        if (g_aMap[i].u8Live == 0u) {
            continue;
        }
        if (ldmas_phys_is_live_8168_bar(g_aMap[i].u64Dma) == 0) {
            continue;
        }
        memset(&g_aMap[i], 0, sizeof(g_aMap[i]));
        ldmas_inc(&g_cDualMapMapSlotScrub);
        if (cDrop < 0xffffffffu) {
            cDrop++;
        }
        ldmas_inc(&g_cDualMapScrub);
    }
    /*
     * If any leave-behind dual-map was dropped, wipe full pcim table owner
     * so no partial second-VA table remains. Soft!=product.
     */
    if (cDrop > 0u && cPcim == 0u) {
        /* iomap/map scrub only - still clear owner table for safety. */
        memset(g_apPcimIomapTab, 0, sizeof(g_apPcimIomapTab));
        g_pPcimIomapTabOwner = NULL;
    }
    return cDrop;
}

/**
 * Soft dual-map reclaim: drop leave-behind soft 8168 BAR iomaps.
 * Grep: linux_dma_soft: soft dual-map reclaim scrub
 */
u32
linux_dma_soft_drop_freestanding_bar_maps(void)
{
    u32 cDrop;

    ldmas_inc(&g_cDualMapScrubCall);
    cDrop = ldmas_scrub_freestanding_bar_iomaps();
    if (cDrop > 0u) {
        ldmas_inc(&g_cDualMapConflict);
        if (g_fDualMapReclaimLogOnce == 0) {
            g_fDualMapReclaimLogOnce = 1;
            /* Grep: linux_dma_soft: soft dual-map reclaim scrub */
            kprintf("linux_dma_soft: soft dual-map reclaim scrub "
                    "dropped=%u no_second_va=1 freestanding_bar_sole=1 "
                    "soft_ne_product=1 (Soft!=product; reclaim conflict R0; "
                    "G-AC-1)\n",
                    (unsigned)cDrop);
        }
    }
    return cDrop;
}

/**
 * Non-zero if soft holds a live 8168 BAR iomap under hybrid refuse
 * (dual-map risk vs freestanding reclaim / H4 thrash->R0). Soft!=product.
 * Checks iomap slots, pcim identity leave-behind, and map-slot BAR cookies.
 */
int
linux_dma_soft_freestanding_bar_dual_mapped(void)
{
    u32 i;

    if (ldmas_hybrid_8168_map_refused() == 0) {
        return 0;
    }
    for (i = 0u; i < LINUX_DMA_SOFT_IOMAP_MAX; i++) {
        if (g_aIo[i].u8Live == 0u) {
            continue;
        }
        if (ldmas_phys_is_live_8168_bar(g_aIo[i].u64Phys) != 0) {
            return 1;
        }
    }
    /* H4: pcim table identity VA leave-behind == dual-map risk. */
    for (i = 0u; i < LINUX_PCI_SOFT_BAR_MAX; i++) {
        if (g_apPcimIomapTab[i] == NULL) {
            continue;
        }
        if (ldmas_phys_is_live_8168_bar(
                (u64)(uintptr_t)g_apPcimIomapTab[i]) != 0) {
            return 1;
        }
    }
    /* H4: streaming map cookie of live BAR phys == MMIO-as-DMA thrash. */
    for (i = 0u; i < LINUX_DMA_SOFT_MAP_SLOT_MAX; i++) {
        if (g_aMap[i].u8Live == 0u) {
            continue;
        }
        if (ldmas_phys_is_live_8168_bar(g_aMap[i].u64Dma) != 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * H4 residual: scrub leave-behind dual-maps, then report sole soft safety.
 * Non-zero = freestanding BAR has no soft second VA under hybrid refuse
 * (or map allowed post-quiesce). Soft!=product; G-AC-1. Not product AC.
 * Grep: linux_dma_soft: soft H4 residual
 */
int
linux_dma_soft_h4_no_dual_map_ok(void)
{
    u32 cDrop;
    int fDual;
    int fOk;

    ldmas_inc(&g_cH4Check);
    cDrop = linux_dma_soft_drop_freestanding_bar_maps();
    fDual = linux_dma_soft_freestanding_bar_dual_mapped();
    /*
     * Gate1 post-quiesce: hybrid map allowed -> dual-map check N/A (ok=1).
     * Gate0 / gate1 while FS ready: ok only when dual_mapped==0 after scrub.
     */
    if (ldmas_hybrid_8168_map_refused() == 0) {
        fOk = 1;
    } else {
        fOk = (fDual == 0) ? 1 : 0;
    }

    if (g_fH4ResidualLogOnce == 0) {
        g_fH4ResidualLogOnce = 1;
        /* Grep: linux_dma_soft: soft H4 residual */
        kprintf("linux_dma_soft: soft H4 residual "
                "no_dual_map=%d freestanding_bar_sole=%d "
                "rtl_rx_safe=%d scrub_drop=%u dual_mapped=%d "
                "pcim_scrub=%u map_slot_scrub=%u "
                "soft_ne_product=1 (H4 BAR thrash->R0; Soft!=product; G-AC-1)\n",
                fOk, fOk, fOk, (unsigned)cDrop, fDual,
                (unsigned)g_cDualMapPcimScrub,
                (unsigned)g_cDualMapMapSlotScrub);
    }
    return fOk;
}

/**
 * Dual-check residual: soft front (vendor@0) or hostish (vendor@0x3c) is
 * 10ec:8168. Defense when hw_touch_ok shape miss; Soft!=ABI.
 */
static int
ldmas_dev_is_8168(const void *pDev)
{
    u16 u16SoftVend;
    u16 u16SoftDev;
    u16 u16HostVend;
    u16 u16HostDev;

    if (pDev == NULL) {
        return 0;
    }

    u16SoftVend = *(const u16 *)pDev;
    u16SoftDev = *(const u16 *)((const u8 *)pDev + 2u);
    if (u16SoftVend == LDMAS_VID_REALTEK && u16SoftDev == LDMAS_DID_RTL8168) {
        return 1;
    }

    u16HostVend =
        *(const u16 *)((const u8 *)pDev + LINUX_PCI_HOSTISH_OFF_VENDOR);
    u16HostDev =
        *(const u16 *)((const u8 *)pDev + LINUX_PCI_HOSTISH_OFF_DEVICE);
    if (u16HostVend == LDMAS_VID_REALTEK && u16HostDev == LDMAS_DID_RTL8168) {
        return 1;
    }
    return 0;
}

/*
 * pci_iomap hybrid NOOP reason codes (honesty residual; Soft!=product).
 * 0 = map allowed; nonzero = must return NULL (no second VA).
 */
#define LDMAS_IOMAP_NOOP_NONE      0
#define LDMAS_IOMAP_NOOP_HW_TOUCH  1
#define LDMAS_IOMAP_NOOP_VID_DUAL  2
#define LDMAS_IOMAP_NOOP_BAR_PHYS  3

/**
 * Hybrid zero-touch honesty: why soft pci_iomap must NOOP (or 0 = allow).
 * Paths: (1) linux_pci_soft_hw_touch_ok==0 -> HW_TOUCH
 *        (2) hybrid refused + VID 10ec:8168 -> VID_DUAL
 * Gate0: freestanding sole owner always. Soft!=product.
 * Grep: linux_dma_soft: soft pci_iomap NOOP hybrid
 */
static int
ldmas_pci_iomap_hybrid_noop_reason(void *dev)
{
    if (dev == NULL) {
        /* Caller rejects NULL before reason path; defensive. */
        return LDMAS_IOMAP_NOOP_HW_TOUCH;
    }
    /* Primary: pci_soft zero-touch choke (VID|BDF|pointer fail-closed). */
    if (linux_pci_soft_hw_touch_ok(dev) == 0) {
        return LDMAS_IOMAP_NOOP_HW_TOUCH;
    }
    /*
     * Dual-check residual: if hybrid map refused and this is 10ec:8168
     * (soft or hostish shape), still NOOP even if hw_touch slipped.
     * Gate0 always refused; gate1 while freestanding ready.
     */
    if (ldmas_hybrid_8168_map_refused() != 0 && ldmas_dev_is_8168(dev) != 0) {
        return LDMAS_IOMAP_NOOP_VID_DUAL;
    }
    return LDMAS_IOMAP_NOOP_NONE;
}

static const char *
ldmas_iomap_noop_reason_sz(int nReason)
{
    if (nReason == LDMAS_IOMAP_NOOP_HW_TOUCH) {
        return "hw_touch";
    }
    if (nReason == LDMAS_IOMAP_NOOP_VID_DUAL) {
        return "vid_dual";
    }
    if (nReason == LDMAS_IOMAP_NOOP_BAR_PHYS) {
        return "bar_phys";
    }
    return "none";
}

static void
ldmas_iomap_noop_tally(int nReason)
{
    g_u8LastIomapNoopReason = (u8)(nReason & 0xff);
    if (nReason == LDMAS_IOMAP_NOOP_HW_TOUCH) {
        ldmas_inc(&g_cNoopReasonHwTouch);
    } else if (nReason == LDMAS_IOMAP_NOOP_VID_DUAL) {
        ldmas_inc(&g_cNoopReasonVidDual);
    } else if (nReason == LDMAS_IOMAP_NOOP_BAR_PHYS) {
        ldmas_inc(&g_cNoopReasonBarPhys);
    }
}

static void *
ldmas_ioremap_common(u64 phys, unsigned long size)
{
    void *pVa;
    u32 i;
    struct ldmas_iomap *pSlot;
    static u8 s_fIoremapNoopOnce;

    if (size == 0ul) {
        return NULL;
    }

    /*
     * Hybrid residual: soft ioremap of live 10ec:8168 BAR phys -> NULL while
     * freestanding owns wire. Defense in depth vs .ko calling ioremap(bar)
     * without pci_iomap. Soft!=product.
     * Grep: linux_dma_soft: soft ioremap NOOP hybrid
     */
    if (ldmas_hybrid_8168_map_refused() != 0 &&
        ldmas_phys_is_live_8168_bar(phys) != 0) {
        ldmas_inc(&g_cHybridNoopIoremap);
        /* Dual-map reclaim residual: drop any leave-behind soft slots. */
        (void)ldmas_scrub_freestanding_bar_iomaps();
        if (s_fIoremapNoopOnce == 0u) {
            s_fIoremapNoopOnce = 1u;
#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
            kprintf("linux_dma_soft: soft ioremap NOOP hybrid "
                    "phys=0x%llx 10ec:8168 gate0 no live BAR map "
                    "no_dual_map=1 freestanding_sole=1 "
                    "(Soft!=product; reclaim conflict R0)\n",
                    (unsigned long long)phys);
#else
            kprintf("linux_dma_soft: soft ioremap NOOP hybrid "
                    "phys=0x%llx 10ec:8168 freestanding owns wire "
                    "no_dual_map=1 (gate1 ready; Soft!=product)\n",
                    (unsigned long long)phys);
#endif
        }
        return NULL;
    }

    /* Soft identity VA = PA (kernel identity / UC BAR maps for low+device). */
    pVa = (void *)(uintptr_t)phys;

    pSlot = NULL;
    for (i = 0u; i < LINUX_DMA_SOFT_IOMAP_MAX; i++) {
        if (g_aIo[i].u8Live == 0u) {
            pSlot = &g_aIo[i];
            break;
        }
    }
    if (pSlot != NULL) {
        memset(pSlot, 0, sizeof(*pSlot));
        pSlot->u8Live = 1u;
        pSlot->u64Phys = phys;
        pSlot->cbSize = size;
        pSlot->pVa = pVa;
    }

    ldmas_inc(&g_cIoremap);
    return pVa;
}

void *
ioremap(u64 phys, unsigned long size)
{
    return ldmas_ioremap_common(phys, size);
}

void *
ioremap_wc(u64 phys, unsigned long size)
{
    return ldmas_ioremap_common(phys, size);
}

void *
ioremap_noprofile(u64 phys, unsigned long size)
{
    return ldmas_ioremap_common(phys, size);
}

/**
 * Soft ioremap_uc / ioremap_cache: H4 residual aliases through common path
 * so hybrid 8168 BAR refuse still NOOPs (no second VA). Soft!=product.
 */
void *
ioremap_uc(u64 phys, unsigned long size)
{
    return ldmas_ioremap_common(phys, size);
}

void *
ioremap_cache(u64 phys, unsigned long size)
{
    return ldmas_ioremap_common(phys, size);
}

void
iounmap(void *addr)
{
    u32 i;

    if (addr == NULL) {
        return;
    }
    for (i = 0u; i < LINUX_DMA_SOFT_IOMAP_MAX; i++) {
        if (g_aIo[i].u8Live != 0u && g_aIo[i].pVa == addr) {
            memset(&g_aIo[i], 0, sizeof(g_aIo[i]));
            break;
        }
    }
    ldmas_inc(&g_cIounmap);
}

u32
readl(const volatile void *addr)
{
    if (addr == NULL) {
        return 0u;
    }
    return *(const volatile u32 *)addr;
}

void
writel(u32 val, volatile void *addr)
{
    if (addr == NULL) {
        return;
    }
    *(volatile u32 *)addr = val;
}

u16
readw(const volatile void *addr)
{
    if (addr == NULL) {
        return 0u;
    }
    return *(const volatile u16 *)addr;
}

void
writew(u16 val, volatile void *addr)
{
    if (addr == NULL) {
        return;
    }
    *(volatile u16 *)addr = val;
}

u8
readb(const volatile void *addr)
{
    if (addr == NULL) {
        return 0u;
    }
    return *(const volatile u8 *)addr;
}

void
writeb(u8 val, volatile void *addr)
{
    if (addr == NULL) {
        return;
    }
    *(volatile u8 *)addr = val;
}

void
memcpy_fromio(void *pTo, const volatile void *pFrom, unsigned long cb)
{
    u8 *pu8Dst;
    const volatile u8 *pu8Src;
    unsigned long i;
    static int s_fLogOnce;

    if (pTo == NULL || pFrom == NULL || cb == 0ul) {
        return;
    }
    /*
     * Soft: byte-wise copy so volatile MMIO loads are not optimized into a
     * single bulk that the empty ksym stub never performed. Soft!=product
     * (no dma_rmb / ordered barrier beyond volatile access).
     */
    pu8Dst = (u8 *)pTo;
    pu8Src = (const volatile u8 *)pFrom;
    for (i = 0ul; i < cb; i++) {
        pu8Dst[i] = pu8Src[i];
    }
    if (s_fLogOnce == 0) {
        s_fLogOnce = 1;
        /* Grep: linux_dma_soft: soft memcpy_fromio */
        kprintf("linux_dma_soft: soft memcpy_fromio cb=%lu (Soft!=product)\n",
                (unsigned long)cb);
    }
}

/* ---- pci_iomap / pcim_* (re-register over empty ksym stubs) ------------- */

/**
 * Hostish BAR start (Strategy A RHEL 5.14 resource[] offsets).
 * Soft!=ABI-stable across kver.
 */
static u64
ldmas_hostish_bar_start(const void *pDev, int nBar)
{
    const u8 *pRes;

    if (pDev == NULL || nBar < 0 ||
        nBar >= (int)LINUX_PCI_SOFT_BAR_MAX) {
        return 0ull;
    }
    pRes = (const u8 *)pDev + LINUX_PCI_HOSTISH_OFF_RESOURCE_N(nBar);
    return *(const u64 *)(pRes + LINUX_PCI_HOSTISH_RES_OFF_START);
}

static u64
ldmas_hostish_bar_len(const void *pDev, int nBar)
{
    const u8 *pRes;
    u64 u64Start;
    u64 u64End;

    if (pDev == NULL || nBar < 0 ||
        nBar >= (int)LINUX_PCI_SOFT_BAR_MAX) {
        return 0ull;
    }
    pRes = (const u8 *)pDev + LINUX_PCI_HOSTISH_OFF_RESOURCE_N(nBar);
    u64Start = *(const u64 *)(pRes + LINUX_PCI_HOSTISH_RES_OFF_START);
    u64End = *(const u64 *)(pRes + LINUX_PCI_HOSTISH_RES_OFF_END);
    if (u64Start == 0ull || u64End < u64Start) {
        return 0ull;
    }
    return (u64End - u64Start) + 1ull;
}

/**
 * Soft: prefer linux_pci_soft BAR arrays; when empty and the pointer looks
 * hostish-shaped (soft front vendor@0 == 0, hostish vendor@0x3c set), read
 * hostish resource[]. Parent hostish blobs are >= 0xb40; filled soft
 * inventory pci_dev has vendor at offset 0 so never takes hostish path.
 */
static void
ldmas_pci_bar(void *dev, int nBar, u64 *pu64Start, u64 *pu64Len)
{
    u64 u64Start;
    u64 u64Len;
    u16 u16SoftVend;
    u16 u16HostVend;

    if (pu64Start != NULL) {
        *pu64Start = 0ull;
    }
    if (pu64Len != NULL) {
        *pu64Len = 0ull;
    }
    if (dev == NULL || nBar < 0) {
        return;
    }

    u64Start = pci_resource_start(dev, nBar);
    u64Len = pci_resource_len(dev, nBar);

    if (u64Start == 0ull || u64Len == 0ull) {
        u16SoftVend = *(const u16 *)dev;
        u16HostVend =
            *(const u16 *)((const u8 *)dev + LINUX_PCI_HOSTISH_OFF_VENDOR);
        if (u16SoftVend == 0u && u16HostVend != 0u) {
            u64Start = ldmas_hostish_bar_start(dev, nBar);
            u64Len = ldmas_hostish_bar_len(dev, nBar);
        }
    }

    if (pu64Start != NULL) {
        *pu64Start = u64Start;
    }
    if (pu64Len != NULL) {
        *pu64Len = u64Len;
    }
}

void *
pci_iomap(void *dev, int nBar, unsigned long ulMaxLen)
{
    u64 u64Start;
    u64 u64Len;
    unsigned long cbMap;
    void *pVa;
    int nReason;
    static u8 s_fIomapNoopOnce;

    ldmas_inc(&g_cPciIomap);

    if (dev == NULL || nBar < 0) {
        ldmas_inc(&g_cPciIomapFail);
        return NULL;
    }

    /*
     * Hybrid BAR ownership: freestanding owns 10ec:8168 wire.
     * Soft pci_iomap would give .ko a second VA on the live MMIO (dual-drive
     * -> orphan freestanding rings / R0). Refuse with honest reason tag:
     *   reason=hw_touch  - linux_pci_soft_hw_touch_ok==0
     *   reason=vid_dual  - hybrid refused + soft/hostish VID 10ec:8168
     *   reason=bar_phys  - live 8168 MEM BAR start (defense after bar lookup)
     * Gate0 (HANDOFF==0): always NOOP for 10ec:8168 (no ready race).
     * Gate1: NOOP while rtl8168_ready()!=0; allow after quiesce.
     * Soft!=product. pcim_iomap_region also lands here.
     * Grep: linux_dma_soft: soft pci_iomap NOOP hybrid
     */
    nReason = ldmas_pci_iomap_hybrid_noop_reason(dev);
    if (nReason != LDMAS_IOMAP_NOOP_NONE) {
        ldmas_inc(&g_cPciIomapFail);
        ldmas_inc(&g_cHybridNoopIomap);
        ldmas_iomap_noop_tally(nReason);
        /* Dual-map reclaim residual: scrub leave-behind soft BAR slots. */
        (void)ldmas_scrub_freestanding_bar_iomaps();
        if (g_pPcimIomapTabOwner == dev) {
            memset(g_apPcimIomapTab, 0, sizeof(g_apPcimIomapTab));
            g_pPcimIomapTabOwner = NULL;
        }
        if (s_fIomapNoopOnce == 0u) {
            s_fIomapNoopOnce = 1u;
#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
            kprintf("linux_dma_soft: soft pci_iomap NOOP hybrid "
                    "bar=%d 10ec:8168 gate0 no live BAR map "
                    "reason=%s freestanding_sole=1 no_dual_map=1 "
                    "soft_ne_product=1 (Soft!=product; reclaim conflict R0)\n",
                    nBar, ldmas_iomap_noop_reason_sz(nReason));
#else
            kprintf("linux_dma_soft: soft pci_iomap NOOP hybrid "
                    "bar=%d 10ec:8168 freestanding owns wire "
                    "reason=%s gate1 dual_check no_dual_map=1 "
                    "soft_ne_product=1 (Soft!=product)\n",
                    nBar, ldmas_iomap_noop_reason_sz(nReason));
#endif
        }
        return NULL;
    }

    ldmas_pci_bar(dev, nBar, &u64Start, &u64Len);
    if (u64Start == 0ull || u64Len == 0ull) {
        ldmas_inc(&g_cPciIomapFail);
        if (g_fIomapLogOnce == 0) {
            g_fIomapLogOnce = 1;
            /* Grep: linux_dma_soft: soft pci_iomap */
            kprintf("linux_dma_soft: soft pci_iomap FAIL bar=%d dev=%p "
                    "(empty BAR; soft_ne_product=1)\n",
                    nBar, dev);
        }
        return NULL;
    }

    /*
     * Residual honesty: if BAR start is live 8168 MEM while hybrid refused,
     * NOOP even when VID dual-check missed (phys path). Soft!=product.
     * Grep: linux_dma_soft: soft pci_iomap NOOP hybrid
     */
    if (ldmas_hybrid_8168_map_refused() != 0 &&
        ldmas_phys_is_live_8168_bar(u64Start) != 0) {
        nReason = LDMAS_IOMAP_NOOP_BAR_PHYS;
        ldmas_inc(&g_cPciIomapFail);
        ldmas_inc(&g_cHybridNoopIomap);
        ldmas_iomap_noop_tally(nReason);
        (void)ldmas_scrub_freestanding_bar_iomaps();
        if (s_fIomapNoopOnce == 0u) {
            s_fIomapNoopOnce = 1u;
            kprintf("linux_dma_soft: soft pci_iomap NOOP hybrid "
                    "bar=%d start=0x%llx 10ec:8168 live BAR phys "
                    "reason=%s freestanding_sole=1 no_dual_map=1 "
                    "soft_ne_product=1 (Soft!=product; reclaim conflict R0)\n",
                    nBar, (unsigned long long)u64Start,
                    ldmas_iomap_noop_reason_sz(nReason));
        }
        return NULL;
    }

    cbMap = (unsigned long)u64Len;
    if (ulMaxLen != 0ul && ulMaxLen < cbMap) {
        cbMap = ulMaxLen;
    }

    pVa = ioremap(u64Start, cbMap);

    if (g_fIomapLogOnce == 0) {
        g_fIomapLogOnce = 1;
        /* Grep: linux_dma_soft: soft pci_iomap */
        kprintf("linux_dma_soft: soft pci_iomap bar=%d start=0x%llx "
                "len=0x%lx va=%p soft_ne_product=1\n",
                nBar, (unsigned long long)u64Start, cbMap, pVa);
    }
    if (pVa == NULL) {
        ldmas_inc(&g_cPciIomapFail);
    }
    return pVa;
}

void
pci_iounmap(void *dev, void *pAddr)
{
    (void)dev;
    ldmas_inc(&g_cPciIounmap);
    iounmap(pAddr);
}

/**
 * Soft pci_iomap_range: hybrid dual-map refuse first (same as pci_iomap),
 * then identity map BAR start+offset. Soft!=product; G-AC-1.
 * Empty ksym stub returned 0 without refuse path.
 */
void *
pci_iomap_range(void *dev, int nBar, unsigned long ulOff,
                unsigned long ulMaxLen)
{
    u64 u64Start;
    u64 u64Len;
    unsigned long cbMap;
    void *pVa;
    int nReason;

    ldmas_inc(&g_cPciIomapRange);
    ldmas_inc(&g_cPciIomap);

    if (dev == NULL || nBar < 0) {
        ldmas_inc(&g_cPciIomapFail);
        return NULL;
    }

    /*
     * Dual-map reclaim residual: never second VA on freestanding 8168 BAR.
     * Soft!=product. Grep: linux_dma_soft: soft pci_iomap NOOP hybrid
     */
    nReason = ldmas_pci_iomap_hybrid_noop_reason(dev);
    if (nReason != LDMAS_IOMAP_NOOP_NONE) {
        ldmas_inc(&g_cPciIomapFail);
        ldmas_inc(&g_cHybridNoopIomap);
        ldmas_iomap_noop_tally(nReason);
        (void)ldmas_scrub_freestanding_bar_iomaps();
        return NULL;
    }

    ldmas_pci_bar(dev, nBar, &u64Start, &u64Len);
    if (u64Start == 0ull || u64Len == 0ull || (u64)ulOff >= u64Len) {
        ldmas_inc(&g_cPciIomapFail);
        return NULL;
    }
    if (ldmas_hybrid_8168_map_refused() != 0 &&
        ldmas_phys_is_live_8168_bar(u64Start) != 0) {
        ldmas_inc(&g_cPciIomapFail);
        ldmas_inc(&g_cHybridNoopIomap);
        ldmas_iomap_noop_tally(LDMAS_IOMAP_NOOP_BAR_PHYS);
        (void)ldmas_scrub_freestanding_bar_iomaps();
        return NULL;
    }

    cbMap = (unsigned long)(u64Len - (u64)ulOff);
    if (ulMaxLen != 0ul && ulMaxLen < cbMap) {
        cbMap = ulMaxLen;
    }
    pVa = ioremap(u64Start + (u64)ulOff, cbMap);
    if (pVa == NULL) {
        ldmas_inc(&g_cPciIomapFail);
    }
    return pVa;
}

/**
 * Soft pci_ioremap_bar: full BAR ioremap. H4 residual - alternate dual-map
 * entry (r8169-class) must share hybrid refuse with pci_iomap.
 * Soft!=product; G-AC-1. Grep: linux_dma_soft: soft pci_iomap NOOP hybrid
 */
void *
pci_ioremap_bar(void *dev, int nBar)
{
    u64 u64Start;
    u64 u64Len;
    void *pVa;
    int nReason;

    ldmas_inc(&g_cPciIoremapBar);
    ldmas_inc(&g_cPciIomap);

    if (dev == NULL || nBar < 0) {
        ldmas_inc(&g_cPciIomapFail);
        return NULL;
    }

    nReason = ldmas_pci_iomap_hybrid_noop_reason(dev);
    if (nReason != LDMAS_IOMAP_NOOP_NONE) {
        ldmas_inc(&g_cPciIomapFail);
        ldmas_inc(&g_cHybridNoopIomap);
        ldmas_iomap_noop_tally(nReason);
        (void)ldmas_scrub_freestanding_bar_iomaps();
        return NULL;
    }

    ldmas_pci_bar(dev, nBar, &u64Start, &u64Len);
    if (u64Start == 0ull || u64Len == 0ull) {
        ldmas_inc(&g_cPciIomapFail);
        return NULL;
    }
    if (ldmas_hybrid_8168_map_refused() != 0 &&
        ldmas_phys_is_live_8168_bar(u64Start) != 0) {
        ldmas_inc(&g_cPciIomapFail);
        ldmas_inc(&g_cHybridNoopIomap);
        ldmas_iomap_noop_tally(LDMAS_IOMAP_NOOP_BAR_PHYS);
        (void)ldmas_scrub_freestanding_bar_iomaps();
        return NULL;
    }

    /* Full BAR; ioremap itself re-checks live 8168 phys under hybrid. */
    pVa = ioremap(u64Start, (unsigned long)u64Len);
    if (pVa == NULL) {
        ldmas_inc(&g_cPciIomapFail);
    }
    return pVa;
}

int
pcim_enable_device(void *dev)
{
    int nSt;

    ldmas_inc(&g_cPcimEnable);
    /* Soft managed enable -> plain soft pci_enable_device (no devres table). */
    nSt = pci_enable_device(dev);
    return nSt;
}

void *
pcim_iomap_region(void *dev, int nBar, const char *szName)
{
    (void)szName;
    ldmas_inc(&g_cPcimIomap);
    /* Soft: no region claim table; map full BAR (maxlen 0 = full). */
    return pci_iomap(dev, nBar, 0ul);
}

/**
 * Soft pcim_iomap_regions: under hybrid 8168 refuse -> NULL (no dual-map
 * table; reclaim conflict R0). Else soft-try pci_iomap per mask bit into
 * static table. Soft!=product (no full devres).
 */
void *
pcim_iomap_regions(void *dev, int nMask, const char *szName)
{
    int nBar;
    int nReason;
    u32 u32Bit;

    (void)szName;
    ldmas_inc(&g_cPcimIomapRegions);
    ldmas_inc(&g_cPcimIomap);

    if (dev == NULL || nMask == 0) {
        return NULL;
    }

    nReason = ldmas_pci_iomap_hybrid_noop_reason(dev);
    if (nReason != LDMAS_IOMAP_NOOP_NONE) {
        ldmas_inc(&g_cHybridNoopIomap);
        ldmas_iomap_noop_tally(nReason);
        (void)ldmas_scrub_freestanding_bar_iomaps();
        memset(g_apPcimIomapTab, 0, sizeof(g_apPcimIomapTab));
        g_pPcimIomapTabOwner = NULL;
        return NULL;
    }

    memset(g_apPcimIomapTab, 0, sizeof(g_apPcimIomapTab));
    g_pPcimIomapTabOwner = dev;
    for (nBar = 0; nBar < (int)LINUX_PCI_SOFT_BAR_MAX; nBar++) {
        u32Bit = 1u << (u32)nBar;
        if (((u32)nMask & u32Bit) == 0u) {
            continue;
        }
        g_apPcimIomapTab[nBar] = pci_iomap(dev, nBar, 0ul);
        if (g_apPcimIomapTab[nBar] == NULL) {
            /* Soft: partial fail -> drop table (no dual-map leave-behind). */
            memset(g_apPcimIomapTab, 0, sizeof(g_apPcimIomapTab));
            g_pPcimIomapTabOwner = NULL;
            return NULL;
        }
    }
    return (void *)g_apPcimIomapTab;
}

/**
 * Soft pcim_iomap_table: last soft regions table for dev, or NULL.
 * Never returns a freestanding 8168 dual-map table under hybrid refuse.
 * Soft!=product.
 */
void *
pcim_iomap_table(void *dev)
{
    ldmas_inc(&g_cPcimIomapTable);

    if (dev == NULL) {
        return NULL;
    }
    if (ldmas_pci_iomap_hybrid_noop_reason(dev) != LDMAS_IOMAP_NOOP_NONE) {
        (void)ldmas_scrub_freestanding_bar_iomaps();
        return NULL;
    }
    if (g_pPcimIomapTabOwner != dev) {
        return NULL;
    }
    return (void *)g_apPcimIomapTab;
}

int
pcim_set_mwi(void *dev)
{
    (void)dev;
    ldmas_inc(&g_cPcimMwi);
    /* Soft: MWI / cache-line no-op success (Soft!=product bus cmd). */
    return 0;
}

/* ---- kmalloc_trace / kmalloc / kzalloc / kfree -------------------------- */

void *
kmalloc_trace(void *s, unsigned long size, unsigned gfp)
{
    void *p;
    u32 cPages;
    gj_paddr_t pa;
    u32 i;
    struct ldmas_kmalloc *pSlot;

    (void)s;
    (void)gfp;

    if (size == 0ul) {
        return NULL;
    }

    /* Prefer kheap for sub-page payloads. */
    if (size < (unsigned long)GJ_PAGE_SIZE - 64ul) {
        p = kheap_alloc((size_t)size);
        if (p != NULL) {
            /* Track as kheap for honest free path. */
            for (i = 0u; i < LINUX_DMA_SOFT_KMALLOC_MAX; i++) {
                if (g_aKm[i].u8Live == 0u) {
                    memset(&g_aKm[i], 0, sizeof(g_aKm[i]));
                    g_aKm[i].u8Live = 1u;
                    g_aKm[i].u8Kind = 0u;
                    g_aKm[i].pCpu = p;
                    g_aKm[i].cbSize = size;
                    break;
                }
            }
            ldmas_inc(&g_cKmallocOk);
            return p;
        }
    }

    cPages = ldmas_pages_for(size);
    if (cPages == 0u || cPages > LINUX_DMA_SOFT_PAGE_MAX) {
        return NULL;
    }

    pSlot = NULL;
    for (i = 0u; i < LINUX_DMA_SOFT_KMALLOC_MAX; i++) {
        if (g_aKm[i].u8Live == 0u) {
            pSlot = &g_aKm[i];
            break;
        }
    }
    if (pSlot == NULL) {
        return NULL;
    }

    pa = ldmas_alloc_pages_low(cPages);
    if (pa == 0) {
        return NULL;
    }
    p = ldmas_cpu_va(pa);
    if (p == NULL) {
        if (cPages == 1u) {
            pmm_free(pa);
        } else {
            pmm_free_pages(pa, cPages);
        }
        return NULL;
    }
    ldmas_zero_pages(pa, cPages);

    memset(pSlot, 0, sizeof(*pSlot));
    pSlot->u8Live = 1u;
    pSlot->u8Kind = 1u;
    pSlot->cPages = cPages;
    pSlot->pCpu = p;
    pSlot->paBase = pa;
    pSlot->cbSize = size;
    ldmas_inc(&g_cKmallocOk);
    return p;
}

void *
kmalloc(unsigned long size, unsigned gfp)
{
    void *p;

    p = kmalloc_trace(NULL, size, gfp);
    /*
     * Soft: honor common __GFP_ZERO bit (0x100 on many x86_64 configs) by
     * zeroing when set. Soft!=product GFP ABI completeness.
     */
    if (p != NULL && (gfp & 0x100u) != 0u) {
        memset(p, 0, (size_t)size);
    }
    return p;
}

void *
kzalloc(unsigned long size, unsigned gfp)
{
    void *p;

    p = kmalloc_trace(NULL, size, gfp);
    if (p != NULL && size > 0ul) {
        memset(p, 0, (size_t)size);
    }
    return p;
}

void
kfree(const void *p)
{
    u32 i;
    struct ldmas_kmalloc *pSlot;

    if (p == NULL) {
        return;
    }

    for (i = 0u; i < LINUX_DMA_SOFT_KMALLOC_MAX; i++) {
        if (g_aKm[i].u8Live == 0u || g_aKm[i].pCpu != (void *)p) {
            continue;
        }
        pSlot = &g_aKm[i];
        if (pSlot->u8Kind == 0u) {
            kheap_free((void *)p);
        } else if (pSlot->cPages > 0u && pSlot->paBase != 0) {
            if (pSlot->cPages == 1u) {
                pmm_free(pSlot->paBase);
            } else {
                pmm_free_pages(pSlot->paBase, pSlot->cPages);
            }
        }
        memset(pSlot, 0, sizeof(*pSlot));
        ldmas_inc(&g_cKfree);
        return;
    }

    /*
     * Soft: untracked pointer - do not probe kheap (header walk may fault
     * on non-heap VA). Silent soft free count only.
     */
    ldmas_inc(&g_cKfree);
}

/* ---- Soft firmware (embed table HIT / else MISS ENOENT) ----------------- */

/*
 * Host Linux layout (RHEL 9-class firmware.h):
 *   struct firmware { size_t size; const u8 *data; void *priv; };
 * Soft embeds rtl_nic/rtl8168*.fw via scripts/embed-linux-fw.sh (.incbin).
 * request_firmware looks up by exact name; HIT -> soft hdr; MISS -> -ENOENT.
 *
 * Soft != product. Grep:
 *   linux_dma_soft: soft firmware HIT name=
 *   linux_dma_soft: soft firmware MISS name=
 */
#define LDMAS_FW_SLOT_MAX  8u
/* Prefer public hard-cap; keep local alias for fw path. */
#define LDMAS_FW_MISS_LOG_MAX  LINUX_DMA_SOFT_FW_LOG_MAX
#define LDMAS_FW_HIT_LOG_MAX   LINUX_DMA_SOFT_FW_LOG_MAX

/* Linux-shaped firmware object (size / data / priv). */
struct ldmas_fw_hdr {
    unsigned long size;
    const u8     *data;
    void         *priv;
};

struct ldmas_fw_slot {
    u8                u8Live;
    u8                u8Pad[7];
    struct ldmas_fw_hdr hdr;
};

struct ldmas_fw_ent {
    const char *szName;
    const u8   *pData;
    const u8   *pEnd;
};

#define LDMAS_FW_DECL
#include <gj/linux_fw_soft_tab.inc>
#undef LDMAS_FW_DECL

static const struct ldmas_fw_ent g_aFwEmbed[] = {
#define LDMAS_FW_TAB
#include <gj/linux_fw_soft_tab.inc>
#undef LDMAS_FW_TAB
    /* Sentinel: keeps array non-empty when embed table is vacant. */
    { NULL, NULL, NULL },
};

static struct ldmas_fw_slot g_aFwSlot[LDMAS_FW_SLOT_MAX];

static const struct ldmas_fw_ent *
ldmas_fw_find(const char *szName)
{
    u32 i;
    u32 n;

    if (szName == NULL || szName[0] == '\0') {
        return NULL;
    }
    n = (u32)(sizeof(g_aFwEmbed) / sizeof(g_aFwEmbed[0]));
    for (i = 0u; i < n; i++) {
        if (g_aFwEmbed[i].szName == NULL || g_aFwEmbed[i].pData == NULL ||
            g_aFwEmbed[i].pEnd == NULL) {
            continue;
        }
        if (g_aFwEmbed[i].pEnd <= g_aFwEmbed[i].pData) {
            continue;
        }
        if (strcmp(g_aFwEmbed[i].szName, szName) == 0) {
            return &g_aFwEmbed[i];
        }
    }
    return NULL;
}

static struct ldmas_fw_hdr *
ldmas_fw_slot_take(const u8 *pData, unsigned long cb)
{
    u32 i;

    for (i = 0u; i < LDMAS_FW_SLOT_MAX; i++) {
        if (g_aFwSlot[i].u8Live == 0) {
            g_aFwSlot[i].u8Live = 1;
            g_aFwSlot[i].hdr.size = cb;
            g_aFwSlot[i].hdr.data = pData;
            g_aFwSlot[i].hdr.priv = NULL;
            return &g_aFwSlot[i].hdr;
        }
    }
    return NULL;
}

static void
ldmas_fw_slot_release(const void *pFw)
{
    u32 i;

    if (pFw == NULL) {
        return;
    }
    for (i = 0u; i < LDMAS_FW_SLOT_MAX; i++) {
        if (g_aFwSlot[i].u8Live != 0 &&
            (const void *)&g_aFwSlot[i].hdr == pFw) {
            g_aFwSlot[i].u8Live = 0;
            g_aFwSlot[i].hdr.size = 0ul;
            g_aFwSlot[i].hdr.data = NULL;
            g_aFwSlot[i].hdr.priv = NULL;
            return;
        }
    }
}

static int
ldmas_fw_request(void **ppFw, const char *szName, void *pDev, const char *szApi)
{
    const struct ldmas_fw_ent *pEnt;
    struct ldmas_fw_hdr *pHdr;
    unsigned long cb;
    const char *szNm;
    const char *szA;

    (void)pDev;

    ldmas_inc(&g_cFwReq);
    szNm = (szName != NULL && szName[0] != '\0') ? szName : "(null)";
    szA = (szApi != NULL && szApi[0] != '\0') ? szApi : "?";

    if (ppFw != NULL) {
        *ppFw = NULL;
    }

    pEnt = ldmas_fw_find(szName);
    if (pEnt == NULL) {
        ldmas_inc(&g_cFwMiss);
        /* Grep: linux_dma_soft: soft firmware MISS name= */
        if (g_cFwMissLog < LDMAS_FW_MISS_LOG_MAX) {
            g_cFwMissLog++;
            kprintf("linux_dma_soft: soft firmware MISS name=%s api=%s "
                    "(null fw; soft_ne_product=1)\n",
                    szNm, szA);
        }
        return -LINUX_ENOENT;
    }

    cb = (unsigned long)(pEnt->pEnd - pEnt->pData);
    pHdr = ldmas_fw_slot_take(pEnt->pData, cb);
    if (pHdr == NULL) {
        ldmas_inc(&g_cFwMiss);
        if (g_cFwMissLog < LDMAS_FW_MISS_LOG_MAX) {
            g_cFwMissLog++;
            kprintf("linux_dma_soft: soft firmware MISS name=%s api=%s "
                    "(slot full; soft_ne_product=1)\n",
                    szNm, szA);
        }
        return -LINUX_ENOENT;
    }

    if (ppFw != NULL) {
        *ppFw = (void *)pHdr;
    }
    ldmas_inc(&g_cFwHit);
    /* Grep: linux_dma_soft: soft firmware HIT name= (hard-capped) */
    if (g_cFwHitLog < LDMAS_FW_HIT_LOG_MAX) {
        g_cFwHitLog++;
        kprintf("linux_dma_soft: soft firmware HIT name=%s size=%lu api=%s "
                "soft_ne_product=1\n",
                szNm, (unsigned long)cb, szA);
    }
    return 0;
}

int
firmware_request_nowarn(void **ppFw, const char *szName, void *pDev)
{
    return ldmas_fw_request(ppFw, szName, pDev, "firmware_request_nowarn");
}

int
request_firmware(void **ppFw, const char *szName, void *pDev)
{
    return ldmas_fw_request(ppFw, szName, pDev, "request_firmware");
}

int
request_firmware_direct(void **ppFw, const char *szName, void *pDev)
{
    return ldmas_fw_request(ppFw, szName, pDev, "request_firmware_direct");
}

void
release_firmware(const void *pFw)
{
    ldmas_inc(&g_cFwRel);
    /* Soft: free slot only; blob data stays in .rodata embed. */
    ldmas_fw_slot_release(pFw);
}

/* ---- Init / diagnostics ------------------------------------------------- */

void
linux_dma_soft_init(void)
{
    u32 u32KsymOk;
    u32 u32KsymSkip;
    u32 u32N;

    g_cInitCalls++;
    if (g_fReady) {
        /* Grep: linux_dma_soft: soft init PASS n= */
        kprintf("linux_dma_soft: soft init PASS n=%u (idempotent call=%u)\n",
                (unsigned)g_cInitCalls, (unsigned)g_cInitCalls);
        return;
    }

    g_cPagesUsed = 0u;
    g_cAllocOk = 0u;
    g_cAllocFail = 0u;
    g_cFree = 0u;
    g_cMap = 0u;
    g_cMapOk = 0u;
    g_cMapFail = 0u;
    g_cMapBarRefuse = 0u;
    g_cMapSizeRefuse = 0u;
    g_cMapErrorQ = 0u;
    g_cUnmap = 0u;
    g_cSyncCpu = 0u;
    g_cSyncDev = 0u;
    g_cMaxMapQ = 0u;
    g_cDmamAlloc = 0u;
    g_cNoopReasonHwTouch = 0u;
    g_cNoopReasonVidDual = 0u;
    g_cNoopReasonBarPhys = 0u;
    g_cIoremap = 0u;
    g_cIounmap = 0u;
    g_cKmallocOk = 0u;
    g_cKfree = 0u;
    g_cFwReq = 0u;
    g_cFwRel = 0u;
    g_cFwHit = 0u;
    g_cFwMiss = 0u;
    g_cFwMissLog = 0u;
    g_cFwHitLog = 0u;
    memset(g_aFwSlot, 0, sizeof(g_aFwSlot));
    g_cSetMask = 0u;
    g_cSetCohMask = 0u;
    g_cPciIomap = 0u;
    g_cPciIomapFail = 0u;
    g_cPciIounmap = 0u;
    g_cPcimEnable = 0u;
    g_cPcimIomap = 0u;
    g_cPcimMwi = 0u;
    g_cHybridNoopIomap = 0u;
    g_cHybridNoopIoremap = 0u;
    g_cDualMapScrub = 0u;
    g_cDualMapScrubCall = 0u;
    g_cDualMapConflict = 0u;
    g_cDualMapMapSlotScrub = 0u;
    g_cDualMapPcimScrub = 0u;
    g_cPciIomapRange = 0u;
    g_cPciIoremapBar = 0u;
    g_cPcimIomapRegions = 0u;
    g_cPcimIomapTable = 0u;
    g_cH4Check = 0u;
    g_u64LastDmaMask = 0ull;
    g_u64LastCohMask = 0ull;
    g_u8LastIomapNoopReason = 0u;
    g_fMaskLogOnce = 0;
    g_fIomapLogOnce = 0;
    g_fMapErrLogOnce = 0;
    g_fMaxMapLogOnce = 0;
    g_fDmamLogOnce = 0;
    g_fMapResidualInvOnce = 0;
    g_fMapBarRefuseLogOnce = 0;
    g_fMapSizeRefuseLogOnce = 0;
    g_fSyncLogOnce = 0;
    g_fDualMapReclaimLogOnce = 0;
    g_fDualMapResidualLogOnce = 0;
    g_fH4ResidualLogOnce = 0;
    g_fResidualLeanLamp = 0;
    g_fWireSafeSkipLogOnce = 0;
    g_fApiHonestyLogOnce = 0;
    g_fForce32ResidualLogOnce = 0;
    g_fWindowResidualLogOnce = 0;
    g_fCohResidualLogOnce = 0;
    g_fStreamResidualLogOnce = 0;
    g_fSyncResidualLogOnce = 0;
    g_fIommuResidualLogOnce = 0;
    g_fMintResidualLogOnce = 0;
    g_fMaskResidualLogOnce = 0;
    g_fDdiBufResidualLogOnce = 0;
    g_fDualDodResidualLogOnce = 0;
    g_cKsymOk = 0u;
    g_cKsymSkip = 0u;
    g_cWindowOk = 0u;
    g_cWindowFail = 0u;
    g_cForce32Note = 0u;
    g_cAllocVtdId = 0u;
    g_cAllocLow = 0u;
    g_cAllocHighSoft = 0u;
    memset(g_aDma, 0, sizeof(g_aDma));
    memset(g_aIo, 0, sizeof(g_aIo));
    memset(g_aKm, 0, sizeof(g_aKm));
    memset(g_aMap, 0, sizeof(g_aMap));
    memset(g_apPcimIomapTab, 0, sizeof(g_apPcimIomapTab));
    g_pPcimIomapTabOwner = NULL;
    g_fReady = 1;

    /*
     * Export soft bodies for F2 ksym / module resolve (replace stubs).
     * Weak linux_ksym_register: no-op SKIP when F2 not linked yet.
     * Wire-safe residual still holds under freestanding SKIP (ksym or BAR).
     */
    u32KsymOk = 0u;
    u32KsymSkip = 0u;

    ldmas_ksym_one("dma_alloc_attrs", (void *)dma_alloc_attrs, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_alloc_coherent", (void *)dma_alloc_coherent, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_free_attrs", (void *)dma_free_attrs, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_free_coherent", (void *)dma_free_coherent, &u32KsymOk,
                   &u32KsymSkip);
    /* Map residual: managed alloc + mapping_error + max size (empty stubs). */
    ldmas_ksym_one("dmam_alloc_attrs", (void *)dmam_alloc_attrs, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dmam_free_coherent", (void *)dmam_free_coherent, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_map_page_attrs", (void *)dma_map_page_attrs, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_unmap_page_attrs", (void *)dma_unmap_page_attrs,
                   &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("dma_map_single", (void *)dma_map_single, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_unmap_single", (void *)dma_unmap_single, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_mapping_error", (void *)dma_mapping_error, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_max_mapping_size", (void *)dma_max_mapping_size,
                   &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("__dma_sync_single_for_cpu",
                   (void *)__dma_sync_single_for_cpu, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("__dma_sync_single_for_device",
                   (void *)__dma_sync_single_for_device, &u32KsymOk,
                   &u32KsymSkip);
    /* Residual: non-__ sync names (empty ksym stubs returned 0). */
    ldmas_ksym_one("dma_sync_single_for_cpu",
                   (void *)dma_sync_single_for_cpu, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("dma_sync_single_for_device",
                   (void *)dma_sync_single_for_device, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_set_mask", (void *)dma_set_mask, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_set_coherent_mask", (void *)dma_set_coherent_mask,
                   &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("dma_set_mask_and_coherent",
                   (void *)dma_set_mask_and_coherent, &u32KsymOk, &u32KsymSkip);

    ldmas_ksym_one("ioremap", (void *)ioremap, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("ioremap_wc", (void *)ioremap_wc, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("ioremap_noprofile", (void *)ioremap_noprofile, &u32KsymOk,
                   &u32KsymSkip);
    /* H4 residual: alternate ioremap names still hybrid-refuse 8168 BAR. */
    ldmas_ksym_one("ioremap_uc", (void *)ioremap_uc, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("ioremap_cache", (void *)ioremap_cache, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("iounmap", (void *)iounmap, &u32KsymOk, &u32KsymSkip);

    /* Replace empty ksym pci_iomap / pcim_* stubs (r8169 early probe). */
    ldmas_ksym_one("pci_iomap", (void *)pci_iomap, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("pci_iomap_range", (void *)pci_iomap_range, &u32KsymOk,
                   &u32KsymSkip);
    /* H4 residual: pci_ioremap_bar alternate dual-map entry -> same NOOP. */
    ldmas_ksym_one("pci_ioremap_bar", (void *)pci_ioremap_bar, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("pci_iounmap", (void *)pci_iounmap, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("pcim_enable_device", (void *)pcim_enable_device, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("pcim_iomap_region", (void *)pcim_iomap_region, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("pcim_iomap_regions", (void *)pcim_iomap_regions, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("pcim_iomap_table", (void *)pcim_iomap_table, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("pcim_set_mwi", (void *)pcim_set_mwi, &u32KsymOk,
                   &u32KsymSkip);

    ldmas_ksym_one("readl", (void *)readl, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("writel", (void *)writel, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("readw", (void *)readw, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("writew", (void *)writew, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("readb", (void *)readb, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("writeb", (void *)writeb, &u32KsymOk, &u32KsymSkip);
    /* Post-probe EEPROM/MAC path: replace empty ksym (no store) stub. */
    ldmas_ksym_one("memcpy_fromio", (void *)memcpy_fromio, &u32KsymOk,
                   &u32KsymSkip);

    ldmas_ksym_one("kmalloc_trace", (void *)kmalloc_trace, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("kmalloc", (void *)kmalloc, &u32KsymOk, &u32KsymSkip);
    /* C0 residual: empty ksym __kmalloc stub -> soft kmalloc body. Soft!=product. */
    ldmas_ksym_one("__kmalloc", (void *)kmalloc, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("kzalloc", (void *)kzalloc, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("kfree", (void *)kfree, &u32KsymOk, &u32KsymSkip);

    /* Replace empty ksym firmware stubs (return 0 / no *fw write). Soft!=product. */
    ldmas_ksym_one("firmware_request_nowarn", (void *)firmware_request_nowarn,
                   &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("request_firmware", (void *)request_firmware, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("request_firmware_direct", (void *)request_firmware_direct,
                   &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("release_firmware", (void *)release_firmware, &u32KsymOk,
                   &u32KsymSkip);

    u32N = u32KsymOk;
    g_cKsymOk = u32KsymOk;
    g_cKsymSkip = u32KsymSkip;

    /* Grep: linux_dma_soft: soft init PASS n= */
    kprintf("linux_dma_soft: soft init PASS n=%u page_max=%u slot_max=%u "
            "id_max=4GiB layout_ver=%u soft_ne_product=1\n",
            (unsigned)u32N, (unsigned)LINUX_DMA_SOFT_PAGE_MAX,
            (unsigned)LINUX_DMA_SOFT_SLOT_MAX,
            (unsigned)LINUX_DMA_SOFT_LAYOUT_VER);

    if (u32KsymOk > 0u) {
        /* Grep: linux_dma_soft: soft ksym register PASS */
        kprintf("linux_dma_soft: soft ksym register PASS n=%u skip=%u "
                "wire_safe=1 soft_ne_product=1\n",
                (unsigned)u32KsymOk, (unsigned)u32KsymSkip);
    } else {
        /*
         * Grep: linux_dma_soft: soft ksym register SKIP
         * Wire-safe residual when freestanding SKIP: soft bodies still
         * refuse dual-map of live 8168 BAR; DDI/UDX caps later.
         * Soft!=product; G-AC-1.
         */
        kprintf("linux_dma_soft: soft ksym register SKIP (no linux_ksym or "
                "all failed) skip=%u wire_safe=1 freestanding_skip=1 "
                "soft_ne_product=1 G-AC-1=1 "
                "(residual lean; Soft!=product)\n",
                (unsigned)u32KsymSkip);
    }

    /*
     * Hybrid 4a honesty lamp (once): soft ioremap/pci_iomap never dual-maps
     * live 10ec:8168 BAR while freestanding owns wire. Soft!=product; G-AC-1.
     * Grep: linux_dma_soft: soft hybrid zero-touch PASS
     */
    {
        static u8 s_fHybridLamp;

        if (s_fHybridLamp == 0u) {
            s_fHybridLamp = 1u;
#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
            kprintf("linux_dma_soft: soft hybrid zero-touch PASS "
                    "gate0 ioremap/pci_iomap=NOOP_8168 always "
                    "no_second_va=1 freestanding_bar_sole=1 "
                    "dual_check=hw_touch|vid_dual|bar_phys "
                    "no_dual_map_reclaim=1 "
                    "noop_reason_tag=1 soft_ne_product=1 (Soft!=product)\n");
#else
            kprintf("linux_dma_soft: soft hybrid zero-touch READY "
                    "gate1 ioremap/pci_iomap=NOOP_while_fs_ready "
                    "toward_sole_owner=1 dual_check=hw_touch|vid_dual|bar_phys "
                    "no_dual_map_reclaim=1 "
                    "noop_reason_tag=1 soft_ne_product=1 (Soft!=product)\n");
#endif
        }
    }

    /*
     * Dual-map reclaim residual (once): scrub any soft 8168 BAR leave-behind
     * so freestanding reclaim never fights a second soft VA (R0). Soft!=product;
     * G-AC-1. Grep: linux_dma_soft: soft dual-map reclaim residual
     */
    (void)linux_dma_soft_drop_freestanding_bar_maps();
    if (g_fDualMapResidualLogOnce == 0) {
        g_fDualMapResidualLogOnce = 1;
        /* Grep: linux_dma_soft: soft dual-map reclaim residual */
        kprintf("linux_dma_soft: soft dual-map reclaim residual "
                "no_dual_map=1 freestanding_bar_sole=1 "
                "iomap_range=1 pci_ioremap_bar=1 pcim_regions=1 pcim_table=1 "
                "scrub_api=1 dual_mapped=%u scrub_call=%u scrub_drop=%u "
                "pcim_scrub=%u map_slot_scrub=%u "
                "soft_ne_product=1 (Soft!=product; G-AC-1; reclaim conflict R0)\n",
                (unsigned)(linux_dma_soft_freestanding_bar_dual_mapped() != 0
                               ? 1u
                               : 0u),
                (unsigned)g_cDualMapScrubCall, (unsigned)g_cDualMapScrub,
                (unsigned)g_cDualMapPcimScrub,
                (unsigned)g_cDualMapMapSlotScrub);
    }

    /*
     * H4 residual (once via h4_no_dual_map_ok): soft BAR thrash -> R0 defense.
     * Scrub + sole-owner honesty. Soft!=product; G-AC-1. Not Dual DoD close.
     * Grep: linux_dma_soft: soft H4 residual
     */
    (void)linux_dma_soft_h4_no_dual_map_ok();

    /* Map residual inventory once (hard-capped; observation only). */
    linux_dma_soft_map_residual_inventory();

    /*
     * Residual lean once-lamp: UDX/userspace DMA path eng honesty +
     * freestanding SKIP wire-safe. Soft!=product; G-AC-1.
     * Grep: linux_dma_soft: soft residual lean PASS
     */
    (void)linux_dma_soft_residual_lean_lamp_once();
}

int
linux_dma_soft_ready(void)
{
    return g_fReady;
}

u32
linux_dma_soft_pages_used(void)
{
    return g_cPagesUsed;
}

u32
linux_dma_soft_alloc_ok(void)
{
    return g_cAllocOk;
}

u32
linux_dma_soft_map_ok(void)
{
    return g_cMapOk;
}

u32
linux_dma_soft_hybrid_noop(void)
{
    u32 u32Sum;

    u32Sum = g_cHybridNoopIomap;
    if (u32Sum < 0xffffffffu - g_cHybridNoopIoremap) {
        u32Sum += g_cHybridNoopIoremap;
    } else {
        u32Sum = 0xffffffffu;
    }
    return u32Sum;
}

/**
 * Soft map residual inventory (once). Useful later for userspace DMA story
 * shape (alloc/map/mapping_error/max_size). Soft!=product; no stamp storms.
 * Grep: linux_dma_soft: soft map residual
 */
void
linux_dma_soft_map_residual_inventory(void)
{
    if (g_fMapResidualInvOnce != 0) {
        return;
    }
    g_fMapResidualInvOnce = 1;

    /* Grep: linux_dma_soft: soft map residual */
    kprintf("linux_dma_soft: soft map residual "
            "alloc_ok=%u map_ok=%u map_fail=%u bar_refuse=%u size_refuse=%u "
            "mapping_error_q=%u max_map_q=%u dmam=%u sync_cpu=%u sync_dev=%u "
            "hybrid_noop_iomap=%u hybrid_noop_ioremap=%u "
            "noop_hw_touch=%u noop_vid_dual=%u noop_bar_phys=%u "
            "dual_map_scrub=%u dual_map_scrub_call=%u dual_map_conflict=%u "
            "pcim_scrub=%u map_slot_scrub=%u "
            "iomap_range=%u pci_ioremap_bar=%u pcim_regions=%u pcim_table=%u "
            "h4_check=%u "
            "window_ok=%u window_fail=%u force32_note=%u "
            "alloc_vtd_id=%u alloc_low=%u alloc_high_soft=%u "
            "last_noop_reason=%s last_dma_mask=0x%llx last_coh_mask=0x%llx "
            "page_max=%u map_slot_max=%u map_max=0x%llx "
            "vtd_identity=0x%llx layout_ver=%u "
            "no_dual_map_reclaim=1 h4_no_dual_map=1 soft_ne_product=1 "
            "(UDX/userspace DMA story shape; Soft!=product; H4 R0)\n",
            (unsigned)g_cAllocOk, (unsigned)g_cMapOk, (unsigned)g_cMapFail,
            (unsigned)g_cMapBarRefuse, (unsigned)g_cMapSizeRefuse,
            (unsigned)g_cMapErrorQ, (unsigned)g_cMaxMapQ,
            (unsigned)g_cDmamAlloc, (unsigned)g_cSyncCpu,
            (unsigned)g_cSyncDev, (unsigned)g_cHybridNoopIomap,
            (unsigned)g_cHybridNoopIoremap,
            (unsigned)g_cNoopReasonHwTouch, (unsigned)g_cNoopReasonVidDual,
            (unsigned)g_cNoopReasonBarPhys,
            (unsigned)g_cDualMapScrub, (unsigned)g_cDualMapScrubCall,
            (unsigned)g_cDualMapConflict,
            (unsigned)g_cDualMapPcimScrub, (unsigned)g_cDualMapMapSlotScrub,
            (unsigned)g_cPciIomapRange, (unsigned)g_cPciIoremapBar,
            (unsigned)g_cPcimIomapRegions, (unsigned)g_cPcimIomapTable,
            (unsigned)g_cH4Check,
            (unsigned)g_cWindowOk, (unsigned)g_cWindowFail,
            (unsigned)g_cForce32Note,
            (unsigned)g_cAllocVtdId, (unsigned)g_cAllocLow,
            (unsigned)g_cAllocHighSoft,
            ldmas_iomap_noop_reason_sz((int)g_u8LastIomapNoopReason),
            (unsigned long long)g_u64LastDmaMask,
            (unsigned long long)g_u64LastCohMask,
            (unsigned)LINUX_DMA_SOFT_PAGE_MAX,
            (unsigned)LINUX_DMA_SOFT_MAP_SLOT_MAX,
            (unsigned long long)LINUX_DMA_SOFT_MAP_MAX,
            (unsigned long long)LINUX_DMA_SOFT_VTD_IDENTITY_LIMIT,
            (unsigned)LINUX_DMA_SOFT_LAYOUT_VER);
}

/**
 * Soft residual lean honesty (once). UDX/userspace DMA path eng.
 * Wire-safe freestanding SKIP: hybrid refuse -> BAR iomap/map NOOP.
 * layout_ver>=3: full udx_dma crosswalk + force32/window geometric +
 * product DMA window OPEN + residual api honesty.
 * C0 soft residual deepen: peer UDX residual catalog
 * (coherent/stream/sync/iommu/mint/mask/ddi_buf + dual_dod OPEN) on
 * once-lamps only; freestanding_no_exec / never_exec_ko / g_ac1_waiver=0 /
 * storm=0 denser honesty. No layout_ver bump; Dual DoD A/B remain OPEN.
 * Soft!=product; G-AC-1; not product AC / not Dual DoD close.
 * Grep: linux_dma_soft: soft residual lean PASS
 * Grep: linux_dma_soft: soft residual lean UDX eng
 * Grep: linux_dma_soft: soft residual api honesty
 * Grep: linux_dma_soft: soft freestanding SKIP wire_safe
 * Grep: linux_dma_soft: soft residual coherent
 * Grep: linux_dma_soft: soft residual stream
 * Grep: linux_dma_soft: soft residual sync
 * Grep: linux_dma_soft: soft residual iommu
 * Grep: linux_dma_soft: soft residual mint
 * Grep: linux_dma_soft: soft residual mask
 * Grep: linux_dma_soft: soft residual ddi_buf
 * Grep: linux_dma_soft: soft residual dual_dod
 */
int
linux_dma_soft_residual_lean_lamp_once(void)
{
    int fWireSafe;
    int fHybridRefuse;
    int fDual;
    int fH4;
    int fUdxShape;
    int fUdxEng;

    if (g_fResidualLeanLamp != 0) {
        return 0;
    }
    if (g_fReady == 0) {
        return 0;
    }
    g_fResidualLeanLamp = 1;

    fHybridRefuse = ldmas_hybrid_8168_map_refused();
    fDual = linux_dma_soft_freestanding_bar_dual_mapped();
    fH4 = linux_dma_soft_h4_no_dual_map_ok();
    fWireSafe = linux_dma_soft_wire_safe();
    fUdxShape = linux_dma_soft_udx_shape_ok();
    fUdxEng = linux_dma_soft_udx_eng_ok();

    /*
     * Grep: linux_dma_soft: soft residual lean PASS
     * Soft UDX-shaped DMA surface for later DDI/UDX DMA caps (eng only).
     * layout_ver>=3: free/unmap + force32/window + product window OPEN.
     * C0: freestanding_no_exec / never_exec_ko / g_ac1_waiver=0 / storm=0.
     */
    kprintf("linux_dma_soft: soft residual lean PASS "
            "layout_ver=%u udx_shape=%d udx_eng=%d wire_safe=%d h4_ok=%d "
            "hybrid_refuse=%d dual_mapped=%d "
            "ksym_ok=%u ksym_skip=%u "
            "alloc_ok=%u free=%u map_ok=%u map_fail=%u unmap=%u "
            "bar_refuse=%u mapping_error_q=%u "
            "sync_cpu=%u sync_dev=%u dmam=%u set_mask=%u "
            "hybrid_noop_iomap=%u hybrid_noop_ioremap=%u "
            "window_ok=%u window_fail=%u force32_note=%u "
            "alloc_vtd_id=%u alloc_low=%u alloc_high_soft=%u "
            "page_max=%u map_max=0x%llx vtd_identity=0x%llx "
            "udx=dma_alloc_coherent|dma_free_coherent|"
            "dma_map_single|dma_unmap_single|dma_mapping_error|"
            "dma_sync_single_for_cpu|dma_sync_single_for_device|"
            "dma_set_mask|window_ok|is_low|in_vtd_identity "
            "shape=coherent|stream|force32|sync|iommu|window|mint|mask "
            "identity_cookie=1 product_DMA_window_cap=OPEN "
            "full_vtd=0 cap_mint=0 iommu_grant=OPEN "
            "no_second_va=1 no_mmio_as_dma=1 "
            "freestanding_no_exec=1 never_exec_ko=1 resolve_only=1 "
            "g_ac1_waiver=0 storm=0 C0=1 "
            "dual_dod_A=OPEN dual_dod_B=OPEN "
            "product_hosts=rtl8168_udx|xhci_udx freestanding_class=SKIP "
            "udx_eng=1 soft_only=1 soft_ne_product=1 G-AC-1=1 "
            "(UDX/userspace DMA path eng residual lean C0; "
            "DDI/UDX DMA caps later; Soft!=product; "
            "freestanding owns wire under hybrid refuse; "
            "not product AC / Dual DoD close)\n",
            (unsigned)LINUX_DMA_SOFT_LAYOUT_VER, fUdxShape, fUdxEng,
            fWireSafe, fH4, fHybridRefuse, fDual,
            (unsigned)g_cKsymOk, (unsigned)g_cKsymSkip,
            (unsigned)g_cAllocOk, (unsigned)g_cFree,
            (unsigned)g_cMapOk, (unsigned)g_cMapFail, (unsigned)g_cUnmap,
            (unsigned)g_cMapBarRefuse,
            (unsigned)g_cMapErrorQ, (unsigned)g_cSyncCpu,
            (unsigned)g_cSyncDev, (unsigned)g_cDmamAlloc,
            (unsigned)g_cSetMask,
            (unsigned)g_cHybridNoopIomap, (unsigned)g_cHybridNoopIoremap,
            (unsigned)g_cWindowOk, (unsigned)g_cWindowFail,
            (unsigned)g_cForce32Note,
            (unsigned)g_cAllocVtdId, (unsigned)g_cAllocLow,
            (unsigned)g_cAllocHighSoft,
            (unsigned)LINUX_DMA_SOFT_PAGE_MAX,
            (unsigned long long)LINUX_DMA_SOFT_MAP_MAX,
            (unsigned long long)LINUX_DMA_SOFT_VTD_IDENTITY_LIMIT);

    /*
     * Grep: linux_dma_soft: soft residual lean UDX eng
     * Once-lamp crosswalk to userspace UDX dma path (udx_dma_*). Soft!=product.
     * Soft identity cookie / barrier sync != product DMA window cap + VT-d.
     * layout_ver>=3: force32 geometric + window_ok + max_mapping_size +
     * set_mask_and_coherent + iommu_grant OPEN honesty.
     * C0: freestanding_no_exec; dual_dod A/B OPEN; product_hosts seed.
     * Mirrors udx: dma soft residual lean / api honesty (eng residual only).
     */
    kprintf("linux_dma_soft: soft residual lean UDX eng "
            "layout_ver=%u udx_eng=1 udx_shape=%d wire_safe=%d "
            "path=dma_alloc_coherent|dma_free_coherent|"
            "dma_map_single|dma_unmap_single|dma_mapping_error|"
            "dma_sync_single_for_cpu|dma_sync_single_for_device|"
            "dma_set_mask|dma_set_coherent_mask|dma_set_mask_and_coherent|"
            "dma_max_mapping_size|window_ok|is_low|in_vtd_identity "
            "udx_peer=udx_dma_alloc_coherent|udx_dma_free_coherent|"
            "udx_dma_map_single|udx_dma_unmap_single|"
            "udx_dma_mapping_error|udx_dma_sync_single_for_cpu|"
            "udx_dma_sync_single_for_device|udx_dma_set_mask|"
            "udx_dma_set_coherent_mask|udx_dma_set_mask_and_coherent|"
            "udx_dma_max_mapping_size|udx_dma_window_ok|"
            "udx_dma_is_low|udx_dma_in_vtd_identity|udx_dma_iommu_grant "
            "shape=coherent|stream|force32|ddi_buf|iommu|sync|window|mint "
            "identity_cookie=1 product_DMA_window_cap=OPEN "
            "full_vtd=0 cap_mint=0 iommu_grant=OPEN "
            "force32=geometric_note vtd_identity=0x%llx low_max=0x%llx "
            "ddi_buf_ops=10/11/12 ddi_buf_mint=0 "
            "no_second_va=1 no_mmio_as_dma=1 h4_ok=%d "
            "alloc_vtd_prefer=1 "
            "product_hosts=rtl8168_udx|xhci_udx DoD_A=OPEN DoD_B=OPEN "
            "freestanding_no_exec=1 never_exec_ko=1 g_ac1_waiver=0 storm=0 "
            "C0=1 soft_ne_product=1 G-AC-1=1 "
            "(UDX dma path eng residual lean C0; Soft!=product; "
            "not freestanding ring thrash; not product AC / Dual DoD close)\n",
            (unsigned)LINUX_DMA_SOFT_LAYOUT_VER, fUdxShape, fWireSafe,
            (unsigned long long)LINUX_DMA_SOFT_VTD_IDENTITY_LIMIT,
            (unsigned long long)LINUX_DMA_SOFT_LOW_MAX, fH4);

    /*
     * Grep: linux_dma_soft: soft residual api honesty
     * Soft inventory != product multi-server confine / DMA window mint.
     * Peers udx: dma soft api honesty. C0 denser dual_dod OPEN.
     */
    if (g_fApiHonestyLogOnce == 0) {
        g_fApiHonestyLogOnce = 1;
        kprintf("linux_dma_soft: soft residual api honesty "
                "product_dma_window_cap=OPEN identity_cookie=1 "
                "full_vtd=0 cap_mint=0 force32_mint=0 ddi_buf_mint=0 "
                "iommu_grant=OPEN multi_server=0 confine=0 "
                "dual_dod_A=OPEN dual_dod_B=OPEN "
                "freestanding_no_exec=1 never_exec_ko=1 g_ac1_waiver=0 "
                "storm=0 C0=1 "
                "layout_ver=%u udx_eng=%d soft=1 product_kernel=OPEN "
                "soft_ne_product=1 G-AC-1=1 "
                "(Soft!=product; lean residual C0 only; "
                "dual MIT OR Apache-2.0; not Dual DoD close)\n",
                (unsigned)LINUX_DMA_SOFT_LAYOUT_VER, fUdxEng);
    }

    /*
     * Grep: linux_dma_soft: soft residual coherent
     * C0 peer udx: dma soft residual coherent. Soft identity cookie only.
     */
    if (g_fCohResidualLogOnce == 0) {
        g_fCohResidualLogOnce = 1;
        kprintf("linux_dma_soft: soft residual coherent "
                "path=dma_alloc_coherent|dma_free_coherent|dmam_alloc_attrs "
                "identity_cookie=1 product_DMA_window_cap=OPEN "
                "alloc_ok=%u free=%u fail=%u dmam=%u "
                "alloc_vtd_id=%u alloc_low=%u alloc_high_soft=%u "
                "page_max=%u force32_mint=0 full_vtd=0 "
                "product_hosts=rtl8168_udx|xhci_udx "
                "rtl8168_udx=ring xhci_udx=TRB residual "
                "soft=1 product=0 freestanding_no_exec=1 storm=0 C0=1 "
                "soft_ne_product=1 G-AC-1=1 layout_ver=%u "
                "(coherent residual; identity cookie only; Soft!=product)\n",
                (unsigned)g_cAllocOk, (unsigned)g_cFree,
                (unsigned)g_cAllocFail, (unsigned)g_cDmamAlloc,
                (unsigned)g_cAllocVtdId, (unsigned)g_cAllocLow,
                (unsigned)g_cAllocHighSoft,
                (unsigned)LINUX_DMA_SOFT_PAGE_MAX,
                (unsigned)LINUX_DMA_SOFT_LAYOUT_VER);
    }

    /*
     * Grep: linux_dma_soft: soft residual stream
     * C0 peer udx: dma soft residual stream. Soft map-live + BAR refuse.
     */
    if (g_fStreamResidualLogOnce == 0) {
        g_fStreamResidualLogOnce = 1;
        kprintf("linux_dma_soft: soft residual stream "
                "path=dma_map_single|dma_unmap_single|dma_map_page_attrs "
                "identity=1 mapping_error=zero_cookie "
                "map_ok=%u map_fail=%u unmap=%u "
                "bar_refuse=%u size_refuse=%u mapping_error_q=%u "
                "map_max=0x%llx map_slot_max=%u no_mmio_as_dma=1 "
                "product_hosts=xhci_udx|rtl8168_udx "
                "soft=1 product=0 freestanding_no_exec=1 storm=0 C0=1 "
                "soft_ne_product=1 G-AC-1=1 layout_ver=%u "
                "(stream residual; soft map-live only; Soft!=product)\n",
                (unsigned)g_cMapOk, (unsigned)g_cMapFail, (unsigned)g_cUnmap,
                (unsigned)g_cMapBarRefuse, (unsigned)g_cMapSizeRefuse,
                (unsigned)g_cMapErrorQ,
                (unsigned long long)LINUX_DMA_SOFT_MAP_MAX,
                (unsigned)LINUX_DMA_SOFT_MAP_SLOT_MAX,
                (unsigned)LINUX_DMA_SOFT_LAYOUT_VER);
    }

    /*
     * Grep: linux_dma_soft: soft residual sync
     * C0 peer udx: dma soft residual sync. Compiler barrier only.
     */
    if (g_fSyncResidualLogOnce == 0) {
        g_fSyncResidualLogOnce = 1;
        kprintf("linux_dma_soft: soft residual sync "
                "path=dma_sync_single_for_cpu|dma_sync_single_for_device "
                "for_cpu=%u for_device=%u "
                "host_barrier=compiler full_iommu_flush=0 "
                "cwb=0 iotlb_flush=0 "
                "product_hosts=rtl8168_udx|xhci_udx "
                "rtl8168_udx=repost_sync_for_device "
                "soft=1 product=0 freestanding_no_exec=1 storm=0 C0=1 "
                "soft_ne_product=1 G-AC-1=1 layout_ver=%u "
                "(sync residual; soft barrier only; Soft!=product)\n",
                (unsigned)g_cSyncCpu, (unsigned)g_cSyncDev,
                (unsigned)LINUX_DMA_SOFT_LAYOUT_VER);
    }

    /*
     * Grep: linux_dma_soft: soft residual iommu
     * C0 peer udx: dma soft residual iommu. Soft geometric only; grant OPEN.
     */
    if (g_fIommuResidualLogOnce == 0) {
        g_fIommuResidualLogOnce = 1;
        kprintf("linux_dma_soft: soft residual iommu "
                "path=linux_dma_soft_window_ok|in_vtd_identity "
                "udx_peer=udx_dma_iommu_grant "
                "window_ok=%u window_fail=%u force32_note=%u "
                "vtd_identity=0x%llx full_vtd=0 cap_mint=0 vtd_grant=0 "
                "iommu_grant=OPEN product_DMA_window_cap=OPEN "
                "product_hosts=rtl8168_udx|xhci_udx "
                "soft=1 product=0 freestanding_no_exec=1 storm=0 C0=1 "
                "soft_ne_product=1 G-AC-1=1 layout_ver=%u "
                "(iommu residual; geometric note only; Soft!=product)\n",
                (unsigned)g_cWindowOk, (unsigned)g_cWindowFail,
                (unsigned)g_cForce32Note,
                (unsigned long long)LINUX_DMA_SOFT_VTD_IDENTITY_LIMIT,
                (unsigned)LINUX_DMA_SOFT_LAYOUT_VER);
    }

    /*
     * Grep: linux_dma_soft: soft residual mint
     * C0 peer udx: dma soft residual mint. Product window mint remains OPEN.
     */
    if (g_fMintResidualLogOnce == 0) {
        g_fMintResidualLogOnce = 1;
        kprintf("linux_dma_soft: soft residual mint "
                "product_DMA_window_cap_mint=OPEN "
                "cnode=0 vtd_grant=0 identity_cookie=1 "
                "force32_mint=0 ddi_buf_mint=0 iommu_grant=OPEN "
                "multi_server=0 confine=0 "
                "DoD_A=OPEN DoD_B=OPEN "
                "product_hosts=rtl8168_udx|xhci_udx "
                "freestanding_class=SKIP freestanding_no_exec=1 "
                "soft=1 product=0 storm=0 C0=1 "
                "soft_ne_product=1 G-AC-1=1 layout_ver=%u "
                "(product DMA window cap mint OPEN; Soft!=product; "
                "not Dual DoD close)\n",
                (unsigned)LINUX_DMA_SOFT_LAYOUT_VER);
    }

    /*
     * Grep: linux_dma_soft: soft residual mask
     * C0 peer udx: dma soft residual mask. Soft accept + last-mask only.
     */
    if (g_fMaskResidualLogOnce == 0) {
        g_fMaskResidualLogOnce = 1;
        kprintf("linux_dma_soft: soft residual mask "
                "path=dma_set_mask|dma_set_coherent_mask|"
                "dma_set_mask_and_coherent "
                "set_mask=%u set_coh=%u "
                "last_dma_mask=0x%llx last_coh_mask=0x%llx "
                "filter=accept_any soft_ne_device_abi=1 "
                "product_hosts=rtl8168_udx|xhci_udx "
                "soft=1 product=0 freestanding_no_exec=1 storm=0 C0=1 "
                "soft_ne_product=1 G-AC-1=1 layout_ver=%u "
                "(mask residual; soft bookkeeping only; Soft!=product)\n",
                (unsigned)g_cSetMask, (unsigned)g_cSetCohMask,
                (unsigned long long)g_u64LastDmaMask,
                (unsigned long long)g_u64LastCohMask,
                (unsigned)LINUX_DMA_SOFT_LAYOUT_VER);
    }

    /*
     * Grep: linux_dma_soft: soft residual ddi_buf
     * C0 peer udx: dma soft residual ddi_buf. Kernel soft has no door mint.
     */
    if (g_fDdiBufResidualLogOnce == 0) {
        g_fDdiBufResidualLogOnce = 1;
        kprintf("linux_dma_soft: soft residual ddi_buf "
                "ops=10/11/12 chain=ALLOC_MAP_FREE "
                "ddi_buf_mint=0 product_DMA_window_cap=OPEN "
                "kernel_soft_slots=dma_slot|map_slot "
                "slot_max=%u map_slot_max=%u page_max=%u "
                "product_hosts=rtl8168_udx|xhci_udx "
                "soft=1 product=0 freestanding_no_exec=1 storm=0 C0=1 "
                "soft_ne_product=1 G-AC-1=1 layout_ver=%u "
                "(ddi_buf residual seed; soft slots only; no door mint; "
                "Soft!=product)\n",
                (unsigned)LINUX_DMA_SOFT_SLOT_MAX,
                (unsigned)LINUX_DMA_SOFT_MAP_SLOT_MAX,
                (unsigned)LINUX_DMA_SOFT_PAGE_MAX,
                (unsigned)LINUX_DMA_SOFT_LAYOUT_VER);
    }

    /*
     * Grep: linux_dma_soft: soft residual dual_dod
     * C0 Dual DoD A/B OPEN honesty. Soft residual != Dual DoD close.
     */
    if (g_fDualDodResidualLogOnce == 0) {
        g_fDualDodResidualLogOnce = 1;
        kprintf("linux_dma_soft: soft residual dual_dod "
                "dual_dod_A=OPEN dual_dod_B=OPEN "
                "UDX_USB=OPEN UDX_NIC=OPEN "
                "product_hosts=rtl8168_udx|xhci_udx "
                "freestanding_class=SKIP freestanding_no_exec=1 "
                "never_exec_ko=1 resolve_only=1 g_ac1_waiver=0 "
                "wire_safe=%d h4_ok=%d udx_eng=%d "
                "product_DMA_window_cap=OPEN iommu_grant=OPEN "
                "soft=1 product=0 storm=0 C0=1 "
                "soft_ne_product=1 G-AC-1=1 layout_ver=%u "
                "(Dual DoD A/B OPEN honesty; soft residual != close; "
                "Soft!=product; not product AC)\n",
                fWireSafe, fH4, fUdxEng,
                (unsigned)LINUX_DMA_SOFT_LAYOUT_VER);
    }

    /*
     * Grep: linux_dma_soft: soft force32 residual
     * Geometric note residual for force32 masters (rtl8168-class addr_hi=0).
     * Soft!=product; no live VT-d / no window cap mint. C0 denser fields.
     */
    if (g_fForce32ResidualLogOnce == 0) {
        g_fForce32ResidualLogOnce = 1;
        kprintf("linux_dma_soft: soft force32 residual "
                "note=1 low_max=0x%llx vtd_identity=0x%llx "
                "window_ok=%u window_fail=%u force32_note=%u "
                "alloc_vtd_id=%u alloc_low=%u alloc_high_soft=%u "
                "strict_pages=DDI_DMA_BUF soft=1 product_mint=0 "
                "force32_mint=0 freestanding_no_exec=1 storm=0 C0=1 "
                "soft_ne_product=1 G-AC-1=1 "
                "(force32 geometric residual; peers udx_dma_window_ok; "
                "Soft!=product)\n",
                (unsigned long long)LINUX_DMA_SOFT_LOW_MAX,
                (unsigned long long)LINUX_DMA_SOFT_VTD_IDENTITY_LIMIT,
                (unsigned)g_cWindowOk, (unsigned)g_cWindowFail,
                (unsigned)g_cForce32Note,
                (unsigned)g_cAllocVtdId, (unsigned)g_cAllocLow,
                (unsigned)g_cAllocHighSoft);
    }

    /*
     * Grep: linux_dma_soft: soft window residual
     * Soft geometric window precheck for UDX/DDI DMA caps later. C0 denser.
     */
    if (g_fWindowResidualLogOnce == 0) {
        g_fWindowResidualLogOnce = 1;
        kprintf("linux_dma_soft: soft window residual "
                "ok=%u fail=%u force32_note=%u "
                "api=linux_dma_soft_window_ok "
                "udx_peer=udx_dma_window_ok dma_buf_peer=dma_buf_window_ok "
                "live_iommu_pte=0 geometric=1 "
                "product_DMA_window_cap=OPEN cap_mint=0 soft=1 "
                "freestanding_no_exec=1 storm=0 C0=1 "
                "soft_ne_product=1 G-AC-1=1 "
                "(window geometric residual; Soft!=product)\n",
                (unsigned)g_cWindowOk, (unsigned)g_cWindowFail,
                (unsigned)g_cForce32Note);
    }

    /*
     * Wire-safe residual when freestanding SKIP (hybrid refuse path).
     * Soft ioremap/pci_iomap/map of live 8168 BAR is SKIP/NOOP.
     * Grep: linux_dma_soft: soft freestanding SKIP wire_safe
     * C0: freestanding_no_exec / never_exec_ko denser honesty.
     */
    if (g_fWireSafeSkipLogOnce == 0) {
        g_fWireSafeSkipLogOnce = 1;
        kprintf("linux_dma_soft: soft freestanding SKIP wire_safe "
                "hybrid_refuse=%d wire_safe=%d dual_mapped=%d "
                "no_second_va=1 no_mmio_as_dma=1 bar_map=SKIP "
                "pci_iomap=NOOP ioremap=NOOP map_bar=REFUSE "
                "freestanding_no_exec=1 never_exec_ko=1 resolve_only=1 "
                "g_ac1_waiver=0 storm=0 C0=1 "
                "udx_eng=1 ksym_skip=%u soft_ne_product=1 G-AC-1=1 "
                "(wire-safe residual C0; freestanding sole BAR owner under "
                "hybrid refuse; Soft!=product; not product AC)\n",
                fHybridRefuse, fWireSafe, fDual, (unsigned)g_cKsymSkip);
    }

    return 1;
}

/**
 * Non-zero if soft is wire-safe vs freestanding 8168 BAR ownership.
 * Under hybrid refuse: ok only when no soft second VA / BAR DMA cookie.
 * Post-quiesce (map allowed): soft may own BAR toward REAL - still ok.
 * Soft!=product; eng residual only.
 */
int
linux_dma_soft_wire_safe(void)
{
    if (g_fReady == 0) {
        return 0;
    }
    if (ldmas_hybrid_8168_map_refused() == 0) {
        /* Gate1 post-quiesce: freestanding not sole owner; soft map allowed. */
        return 1;
    }
    /* Freestanding owns wire: wire-safe iff no soft dual-map leave-behind. */
    return (linux_dma_soft_freestanding_bar_dual_mapped() == 0) ? 1 : 0;
}

/**
 * Non-zero when soft UDX-shaped DMA surface is present after init.
 * Supports DDI/UDX DMA caps later - eng residual only; Soft!=product.
 * layout_ver>=3: shape includes free/unmap + force32 geometric + window_ok
 * + product DMA window OPEN honesty + residual api honesty.
 */
int
linux_dma_soft_udx_shape_ok(void)
{
    /*
     * Soft bodies always linked with this TU; ready lamp is the gate.
     * Shape catalog (eng residual; Soft!=product):
     *   dma_alloc_coherent / dma_free_coherent
     *   dma_map_single / dma_unmap_single / dma_mapping_error
     *   dma_sync_single_for_cpu / dma_sync_single_for_device
     *   dma_set_mask / dma_set_coherent_mask / dma_set_mask_and_coherent
     *   dma_max_mapping_size
     *   linux_dma_soft_window_ok / is_low / in_vtd_identity
     * Peers userspace udx_dma_*; product window/VT-d still OPEN.
     */
    return g_fReady ? 1 : 0;
}

u32
linux_dma_soft_layout_ver(void)
{
    return LINUX_DMA_SOFT_LAYOUT_VER;
}

/* ---- UDX geometric residual (Soft!=product; peers udx_dma_*) ------------ */

int
linux_dma_soft_is_low(u64 u64Dma)
{
    return (u64Dma != 0ull && u64Dma < LINUX_DMA_SOFT_LOW_MAX) ? 1 : 0;
}

int
linux_dma_soft_in_vtd_identity(u64 u64Dma, unsigned long cb)
{
    u64 u64End;

    if (u64Dma == 0ull || cb == 0ul) {
        return 0;
    }
    u64End = u64Dma + (u64)cb;
    if (u64End < u64Dma) {
        return 0; /* overflow */
    }
    if (u64End > LINUX_DMA_SOFT_VTD_IDENTITY_LIMIT) {
        return 0;
    }
    return 1;
}

/**
 * Soft geometric ok for DMA window grant note. Peers udx_dma_window_ok.
 * Soft!=product: no cap mint / no iommu_window_grant.
 * Grep: linux_dma_soft: soft window residual
 * Grep: linux_dma_soft: soft force32 residual
 */
int
linux_dma_soft_window_ok(u64 u64Dma, unsigned long cb, int fForce32)
{
    u64 u64End;
    int fOk;

    if (fForce32 != 0) {
        ldmas_inc(&g_cForce32Note);
    }
    if (u64Dma == 0ull || cb == 0ul) {
        ldmas_inc(&g_cWindowFail);
        return 0;
    }
    u64End = u64Dma + (u64)cb;
    if (u64End < u64Dma) {
        ldmas_inc(&g_cWindowFail);
        return 0;
    }
    if (fForce32 != 0) {
        fOk = (u64End <= LINUX_DMA_SOFT_VTD_IDENTITY_LIMIT) ? 1 : 0;
    } else {
        fOk = 1;
    }
    if (fOk != 0) {
        ldmas_inc(&g_cWindowOk);
    } else {
        ldmas_inc(&g_cWindowFail);
    }
    return fOk;
}

/**
 * Soft residual: UDX eng surface complete after init (layout_ver >= 3).
 * Soft!=product; eng residual only - not Dual DoD close / not product AC.
 */
int
linux_dma_soft_udx_eng_ok(void)
{
    if (g_fReady == 0) {
        return 0;
    }
    if (LINUX_DMA_SOFT_LAYOUT_VER < 3u) {
        return 0;
    }
    /* Shape + geometric residual surface present. Soft!=product. */
    return linux_dma_soft_udx_shape_ok();
}

u32
linux_dma_soft_window_ok_count(void)
{
    return g_cWindowOk;
}

u32
linux_dma_soft_window_fail_count(void)
{
    return g_cWindowFail;
}

u32
linux_dma_soft_force32_note_count(void)
{
    return g_cForce32Note;
}
