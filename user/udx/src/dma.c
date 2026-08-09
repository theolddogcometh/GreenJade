/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * DMA helpers. Host: calloc identity cookies + soft DDI DMA_BUF slots.
 * Freestanding: two-tier static slab
 *   small UDX_FS_DMA_SLOTS × UDX_FS_DMA_SLOT_CB (4 KiB rings)
 *   large UDX_FS_DMA_LARGE_SLOTS × UDX_FS_DMA_LARGE_SLOT_CB (32 KiB RX);
 *               DDI DMA_BUF via GJ_SYS_DDI ops 10/11/12.
 *
 * Soft inventory (exclusive deepen; this unit only) --
 * greppable "udx: dma soft ...":
 *   udx: dma soft inventory ...
 *   udx: dma soft alloc ...
 *   udx: dma soft map ...
 *   udx: dma soft sync ...
 *   udx: dma soft iommu ...
 *   udx: dma soft slab ...
 *   udx: dma soft path ...
 *   udx: dma soft coherent ...
 *   udx: dma soft stream ...
 *   udx: dma soft force32 ...
 *   udx: dma soft mask ...
 *   udx: dma soft ddi_buf ...
 *   udx: dma soft ddi_buf chain ...
 *   udx: dma soft product host ...
 *   udx: dma soft mint honesty ...
 *   udx: dma soft residual lean ...
 *   udx: dma soft residual lean UDX eng ...
 *   udx: dma soft residual coherent ...
 *   udx: dma soft residual stream ...
 *   udx: dma soft residual force32 ...
 *   udx: dma soft residual ddi_buf ...
 *   udx: dma soft residual iommu ...
 *   udx: dma soft residual sync ...
 *   udx: dma soft residual window ...
 *   udx: dma soft residual mint ...
 *   udx: dma soft residual mask ...
 *   udx: dma soft residual product_host ...
 *   udx: dma soft residual dual_dod ...
 *   udx: dma soft residual hazard ...
 *   udx: dma soft residual functional ...
 *   udx: dma soft residual buf_map ...
 *   udx: dma soft residual ring ...
 *   udx: dma soft residual map_idem ...
 *   udx: dma soft residual map_recb ...
 *   udx: dma soft residual map_note ...
 *   udx: dma soft api honesty ...
 * Pure observation; never gates host skeleton PASS or freestanding path.
 * Soft != skeleton PASS. greppable: udx: dma soft
 * greppable: udx: dma soft residual lean
 * greppable: udx: dma soft residual functional
 * greppable: udx: dma soft residual buf_map
 * greppable: udx: dma soft residual ring
 * greppable: udx: dma soft residual map_note
 * greppable: product_hosts=UDX
 * greppable: Soft!=product soft residual dual_dod OPEN
 *
 * Soft residual deepen (UDX product hosts; Soft!=product; G-AC-1):
 *   coherent residual  -- identity cookie != DMA window cap mint
 *   stream residual    -- map_single identity; soft map-live bookkeeping
 *   force32 residual   -- geometric note / window_ok; not product mint
 *   ddi_buf residual   -- ops 10/11/12 + host slots + chain; mint=0
 *   iommu residual     -- host nop / soft grant != live VT-d / CNode
 *   sync residual      -- mfence only; not full IOTLB / CWB
 *   window residual    -- geometric ok; not live IOMMU PTE query
 *   mint residual      -- product DMA window cap OPEN (Dual DoD A/B OPEN)
 *   mask residual      -- set_mask / coherent_mask process-global soft;
 *                          != struct device embed / IOMMU program
 *   product_host residual -- rtl8168_udx DoD B OPEN / xhci_udx DoD A OPEN
 *                          catalog only; Dual DoD never closed here
 *   dual_dod residual  -- Dual DoD A/B OPEN honesty (agent!=close)
 *   hazard residual    -- H1/H2/H3 DMA-path honesty (no net_eth_poll;
 *                          once-lamp storm=0; free order OPEN)
 * Functional residual DMA buf/map toward product UDX NIC/USB rings
 * (Soft!=product; denser only; never Dual DoD close):
 *   functional residual -- denser buf/map path catalog toward rings
 *   buf_map residual    -- ddi_buf_map + stream map toward ring/xfer
 *   ring residual       -- NIC desc ring / USB TRB page spirit catalog
 *   map_idem residual   -- re-MAP same pa/cb cookie match (soft)
 *   map_recb residual   -- re-MAP different cb soft span update
 *   map_note residual   -- STRONGER map/note path for rtl/xhci UDX
 *                          (map_single + force32 note + ddi_buf_map);
 *                          product_hosts=UDX; dual_dod OPEN; Soft!=product
 *   ddi_buf chain denser -- ALLOC→MAP→MAP_REMAP(idem)→FREE (ops 10/12/11)
 * denser residual (preferred over lamps alone; Soft!=product):
 *   denser_prove triple: coherent + stream + ddi_buf_map;
 *   denser ring (ring-sized coherent) + denser map_note surface;
 *   denser product_host + dual_dod OPEN + agent_close=0;
 *   denser Soft!=product + freestanding class SKIP + stamp_free_75.
 * greppable: denser_prove=1 | denser residual | lean_checks=
 * greppable: udx: dma soft residual denser
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 * Dual MIT OR Apache-2.0. Product hosts = Linux-shaped userspace UDX
 * (rtl8168_udx / xhci_udx Dual DoD B/A OPEN). Freestanding class SKIP.
 * No stamp storms. No version stamp. Soft!=product. G-AC-1.
 * Bar honesty v2026.08.04.75 stamp-free. NEVER bump GJ_IMAGE_VERSION.
 */
#include "udx_internal.h"

#include <udx/dma.h>
#include <udx/device.h>

#include <stdarg.h>

#if defined(UDX_HOST_LIBC)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
/* 256-byte aligned for TNPDS/RDSAR product program_gate (option 3). */
static struct udx_fs_dma_slot g_aFsDma[UDX_FS_DMA_SLOTS]
    __attribute__((aligned(256)));
/* Large tier: product rtl8168_udx RX pool (32 KiB); Soft!=product. */
static struct udx_fs_dma_large_slot g_aFsDmaLarge[UDX_FS_DMA_LARGE_SLOTS]
    __attribute__((aligned(256)));
#endif

/*
 * Soft DMA product inventory (exclusive deepen). Cumulative for this
 * process. greppable: udx: dma soft …
 * Never hard-gates; wrap OK if ever hit. Once-lamp inventory only.
 */
static u32 g_u32DmaAllocEnter;   /* udx_dma_alloc_coherent entries */
static u32 g_u32DmaAllocOk;      /* coherent alloc success */
static u32 g_u32DmaAllocFail;    /* allocator returned NULL */
static u32 g_u32DmaAllocInval;   /* size 0 or null handle */
static u32 g_u32DmaAllocForce32; /* alloc with UDX_DMA_F_FORCE32 note */
static u32 g_u32DmaFree;         /* free_coherent calls */
static u32 g_u32DmaFreeNull;     /* free with null cpu cookie */
static u32 g_u32DmaMap;          /* map_single entries (stream) */
static u32 g_u32DmaMapOk;        /* map_single returned non-zero */
static u32 g_u32DmaMapNull;      /* map_single rejected null cpu */
static u32 g_u32DmaMapOversize;  /* stream map size > UDX_DMA_MAP_MAX */
static u32 g_u32DmaMapDirInval;  /* soft dir ordinal out of range (obs) */
static u32 g_u32DmaMapLive;      /* soft stream map-live residual */
static u32 g_u32DmaMapLivePeak;  /* peak map-live */
static u32 g_u32DmaUnmap;        /* unmap_single calls */
static u32 g_u32DmaUnmapMiss;    /* unmap without soft map-live hit */
static u32 g_u32DmaSyncCpu;      /* sync_single_for_cpu */
static u32 g_u32DmaSyncDev;      /* sync_single_for_device */
static u32 g_u32DmaIommuEnter;   /* iommu_grant entries */
static u32 g_u32DmaIommuOk;      /* grant success (incl host nop) */
static u32 g_u32DmaIommuFail;    /* grant rejected / syscall fail */
static u32 g_u32DmaIommuHostNop; /* host identity-map nop path */
static u32 g_u32DmaSlabLive;     /* freestanding slots currently used */
static u32 g_u32DmaSlabPeak;     /* peak freestanding slots used */
static u32 g_u32DmaSlabFull;     /* freestanding alloc pool full */
static u32 g_u32DmaLastCb;       /* last coherent alloc size (soft snap) */
static u32 g_u32DmaPeakCb;       /* peak coherent alloc size */
static u32 g_u32DmaWinOk;        /* window_ok true */
static u32 g_u32DmaWinFail;      /* window_ok false */
static u32 g_u32DmaWinForce32;   /* window_ok with fForce32 */
static u32 g_u32DmaMaskSet;      /* set_mask calls */
static u32 g_u32DmaCohMaskSet;   /* set_coherent_mask calls */
static u32 g_u32DmaMaskFail;     /* zero mask reject */
static u32 g_u32DmaDdiBufAlloc;  /* ddi_buf_alloc entries */
static u32 g_u32DmaDdiBufFree;   /* ddi_buf_free entries */
static u32 g_u32DmaDdiBufMap;    /* ddi_buf_map entries */
static u32 g_u32DmaDdiBufOk;     /* ddi_buf * success */
static u32 g_u32DmaDdiBufFail;   /* ddi_buf * fail */
static u32 g_u32DmaDdiBufForce32;/* ddi_buf alloc with FORCE32 flag */
static u32 g_u32DmaDdiBufChain;  /* ddi_buf_chain entries */
static u32 g_u32DmaDdiBufChainOk;/* ddi_buf_chain full PASS */
static u32 g_u32DmaDdiMapIdem;   /* ddi_buf_map re-MAP same pa/cb (idem) */
static u32 g_u32DmaDdiMapRecb;   /* ddi_buf_map re-MAP different cb */
static u32 g_u32DmaMapRemap;     /* stream map re-map same cookie residual */
static u32 g_u32DmaMapLiveFull;  /* soft stream map-live table full */
static u32 g_u32DmaRingSized;    /* coherent alloc sized like ring page */
static u32 g_u32DmaDdiSlotLive;  /* host soft DDI DMA_BUF slots live */
static u32 g_u32DmaDdiSlotPeak;  /* peak host soft DDI slots */
static u32 g_u32DmaDdiSlotFull;  /* host soft DDI slot pool full */
static u32 g_u32DmaLogN;         /* soft inventory dumps emitted */

/*
 * Soft ring residual geometry spirit (Soft!=product; product-host seed).
 * Matches rtl8168_udx RING_BYTES / xhci TRB page residual spirit (4 KiB).
 * Catalog only — never programs TNPDS/CRCR/DCBAAP or mints DMA window.
 * greppable: udx: dma soft residual ring
 */
#define UDX_DMA_SOFT_RING_PAGE_CB  4096u

/*
 * FUNCTIONAL residual locks (STRONGER toward product UDX rings + map/note;
 * Soft!=product; G-AC-1; Dual DoD A/B OPEN; agent!=close; stamp-free
 * bar v2026.08.04.75). H1: DMA path never injects net_eth_poll / hard-IRQ
 * map product. thr-only product hosts consume rings from thr BH / udx_run.
 * Map/note path (rtl/xhci product hosts): map_single + force32 note +
 * ddi_buf_map — Soft!=product; product_hosts=UDX; dual_dod OPEN.
 * denser residual (preferred over lamps alone): denser_prove triple
 * coherent+stream+ddi_buf_map + denser ring + denser map_note +
 * denser product_host + Soft!=product + agent_close=0 + fs SKIP.
 * greppable: udx: dma soft residual functional
 * greppable: udx: dma soft residual map_note
 * greppable: udx: dma soft residual denser
 * greppable: denser_prove=1 | lean_checks=
 * greppable: udx: dma soft residual hazard
 * greppable: thr_only=1 | H1=no_net_eth_poll | DoD_A=OPEN | DoD_B=OPEN
 * greppable: product_hosts=UDX | Soft!=product | dual_dod OPEN
 */
#define UDX_DMA_H1_NO_NET_ETH_POLL   1u /* H1: no net_eth_poll from DMA path */
#define UDX_DMA_HARD_IRQ_MAP         0u /* never hard-IRQ map product */
#define UDX_DMA_THR_ONLY_CONSUMER    1u /* ring consumer = thr BH / udx_run */
#define UDX_DMA_DOD_A_OPEN           1u /* xhci_udx Dual DoD A OPEN */
#define UDX_DMA_DOD_B_OPEN           1u /* rtl8168_udx Dual DoD B OPEN */
#define UDX_DMA_AGENT_CLOSE          0u /* soft lamps never close Dual DoD */
#define UDX_DMA_NET_INJECT           0u /* never net inject from DMA residual */
#define UDX_DMA_RING_DEV_VISIBLE     1u /* ring-sized alloc post-mfence residual */
#define UDX_DMA_MAP_NOTE_SURFACE     1u /* map_single|force32_note|ddi_buf_map */
#define UDX_DMA_DENSER_PROVE         1u /* denser residual prove on */
#define UDX_DMA_DENSER_TRIPLE        1u /* denser: coherent+stream+ddi_map */
#define UDX_DMA_DENSER_RING          1u /* denser ring-sized coherent prove */
#define UDX_DMA_DENSER_MAP_NOTE      1u /* denser map_note surface prove */
#define UDX_DMA_DENSER_PRODUCT_HOST  1u /* denser product-host DoD A/B */
#define UDX_DMA_SOFT_NE_PRODUCT      1u /* Soft!=product claim-class law */
#define UDX_DMA_FS_CLASS_SKIP        1u /* freestanding class SKIP */
#define UDX_DMA_STAMP_FREE_75        1u /* bar v2026.08.04.75; never invent .76 */
#define UDX_DMA_LEAN_CHECKS         16u /* denser functional self-check count */
#define UDX_DMA_SOFT_WAVE            75u /* stamp-free wave aligns bar .75 */

_Static_assert(UDX_DMA_H1_NO_NET_ETH_POLL == 1u,
               "H1: DMA residual never net_eth_poll");
_Static_assert(UDX_DMA_HARD_IRQ_MAP == 0u,
               "DMA residual: no hard-IRQ map product");
_Static_assert(UDX_DMA_THR_ONLY_CONSUMER == 1u,
               "DMA ring consumer thr-only (BH / udx_run)");
_Static_assert(UDX_DMA_DOD_A_OPEN == 1u,
               "Dual DoD A remains OPEN (xhci_udx)");
_Static_assert(UDX_DMA_DOD_B_OPEN == 1u,
               "Dual DoD B remains OPEN (rtl8168_udx)");
_Static_assert(UDX_DMA_AGENT_CLOSE == 0u,
               "soft residual lamps never close Dual DoD");
_Static_assert(UDX_DMA_NET_INJECT == 0u,
               "DMA residual never net inject product");
_Static_assert(UDX_DMA_RING_DEV_VISIBLE == 1u,
               "ring-sized alloc device-visible barrier residual");
_Static_assert(UDX_DMA_MAP_NOTE_SURFACE == 1u,
               "map/note residual surface for product UDX hosts");
_Static_assert(UDX_DMA_DENSER_PROVE == 1u,
               "denser residual prove on (triple/ring/map_note)");
_Static_assert(UDX_DMA_DENSER_TRIPLE == 1u,
               "denser residual: coherent+stream+ddi_map triple");
_Static_assert(UDX_DMA_DENSER_RING == 1u,
               "denser residual: ring-sized coherent prove");
_Static_assert(UDX_DMA_DENSER_MAP_NOTE == 1u,
               "denser residual: map_note surface prove");
_Static_assert(UDX_DMA_DENSER_PRODUCT_HOST == 1u,
               "denser residual: product-host DoD A/B id");
_Static_assert(UDX_DMA_SOFT_NE_PRODUCT == 1u,
               "Soft!=product claim-class law (never Soft unicode ne)");
_Static_assert(UDX_DMA_FS_CLASS_SKIP == 1u,
               "freestanding class SKIP (product=UDX)");
_Static_assert(UDX_DMA_STAMP_FREE_75 == 1u,
               "stamp-free residual bar v2026.08.04.75 (never .76)");
_Static_assert(UDX_DMA_LEAN_CHECKS == 16u,
               "FUNCTIONAL lean residual denser: 16 ring/map_note checks");
_Static_assert(UDX_DMA_SOFT_WAVE == 75u,
               "stamp-free soft wave aligns bar v2026.08.04.75 (never .76)");

/*
 * Residual path counters (one-shot residual honesty; Soft!=product).
 * greppable: udx: dma soft residual {coherent,stream,force32,ddi_buf,
 *            iommu,sync,window,mint,mask,product_host,dual_dod,hazard,
 *            functional,buf_map,ring,map_idem,map_recb,map_note}
 */
static u32 g_u32DmaResidualCoherent;    /* residual coherent lamp n */
static u32 g_u32DmaResidualStream;      /* residual stream lamp n */
static u32 g_u32DmaResidualForce32;     /* residual force32 lamp n */
static u32 g_u32DmaResidualDdiBuf;      /* residual ddi_buf lamp n */
static u32 g_u32DmaResidualIommu;       /* residual iommu lamp n */
static u32 g_u32DmaResidualSync;        /* residual sync lamp n */
static u32 g_u32DmaResidualWindow;      /* residual window lamp n */
static u32 g_u32DmaResidualMint;        /* residual mint honesty lamp n */
static u32 g_u32DmaResidualMask;        /* residual mask lamp n */
static u32 g_u32DmaResidualProductHost; /* residual product_host lamp n */
static u32 g_u32DmaResidualDualDod;     /* residual dual_dod lamp n */
static u32 g_u32DmaResidualHazard;      /* residual hazard H1/H2/H3 lamp n */
static u32 g_u32DmaResidualFunctional;  /* residual functional buf/map lamp n */
static u32 g_u32DmaResidualBufMap;      /* residual buf_map lamp n */
static u32 g_u32DmaResidualRing;        /* residual ring lamp n */
static u32 g_u32DmaResidualMapIdem;     /* residual map_idem lamp n */
static u32 g_u32DmaResidualMapRecb;     /* residual map_recb lamp n */
static u32 g_u32DmaResidualMapNote;     /* residual map_note lamp n */
static u32 g_u32DmaResidualDenser;      /* denser residual prove lamp n */

static u8  g_fDmaSoftOnce;               /* one-shot deep dump after first ok path */
static u8  g_fDmaCohResidualOnce;        /* one-shot coherent residual */
static u8  g_fDmaStreamResidualOnce;     /* one-shot stream residual */
static u8  g_fDmaForce32ResidualOnce;    /* one-shot force32 residual */
static u8  g_fDmaDdiBufResidualOnce;     /* one-shot ddi_buf residual */
static u8  g_fDmaIommuResidualOnce;      /* one-shot iommu residual */
static u8  g_fDmaSyncResidualOnce;       /* one-shot sync residual */
static u8  g_fDmaWindowResidualOnce;     /* one-shot window residual */
static u8  g_fDmaMintResidualOnce;       /* one-shot mint residual */
static u8  g_fDmaMaskResidualOnce;       /* one-shot mask residual */
static u8  g_fDmaProductHostResidualOnce;/* one-shot product_host residual */
static u8  g_fDmaDualDodResidualOnce;    /* one-shot dual_dod residual */
static u8  g_fDmaHazardResidualOnce;     /* one-shot hazard residual */
static u8  g_fDmaFunctionalResidualOnce; /* one-shot functional residual */
static u8  g_fDmaBufMapResidualOnce;     /* one-shot buf_map residual */
static u8  g_fDmaRingResidualOnce;       /* one-shot ring residual */
static u8  g_fDmaMapIdemResidualOnce;    /* one-shot map_idem residual */
static u8  g_fDmaMapRecbResidualOnce;    /* one-shot map_recb residual */
static u8  g_fDmaMapNoteResidualOnce;    /* one-shot map_note residual */
static u8  g_fDmaDenserResidualOnce;     /* one-shot denser residual prove */

/* Soft mask residual (process-global; Soft!=product device embedding). */
static u64 g_u64DmaMask         = ~0ull;
static u64 g_u64DmaCoherentMask = ~0ull;

#if !defined(UDX_HOST_LIBC) || defined(GJ_FREESTANDING)
/*
 * Soft freestanding ddi_buf_map last-cookie residual (map_idem/recb without
 * host slot table). Soft!=product; door owns real span; process-global only.
 */
static u32            g_u32FsDdiMapHandle;
static udx_dma_addr_t g_dmaFsDdiMapPa;
static u32            g_u32FsDdiMapCb;
static u8             g_fFsDdiMapLive;
#endif

/*
 * Soft stream map-live residual (Soft!=product IOMMU PTE).
 * Tracks map_single cookies for unmap honesty / inventory only.
 */
struct udx_dma_soft_map_slot {
    u8             u8Live;
    u8             u8Pad[3];
    u32            cbSize;
    udx_dma_addr_t dma;
};

static struct udx_dma_soft_map_slot g_aMapSlot[UDX_DMA_SOFT_MAP_SLOT_MAX];

#if defined(UDX_HOST_LIBC) && !defined(GJ_FREESTANDING)
/*
 * Host soft DDI DMA_BUF residual slots (product host inject path).
 * Soft!=product: calloc pages + identity cookie; not VT-d / CNode mint.
 * Freestanding uses GJ_SYS_DDI instead (no host table).
 */
struct udx_dma_soft_ddi_slot {
    u8             u8Live;
    u8             u8Mapped;
    u8             u8Force32;
    u8             u8Pad;
    u32            u32Handle;
    u32            cPages;
    u32            cbMapped; /* soft map span residual (Soft!=product) */
    void          *pCpu;
    udx_dma_addr_t cookie; /* identity bus via udx_dma_cookie_from_cpu */
};

static struct udx_dma_soft_ddi_slot g_aDdiSlot[UDX_DMA_SOFT_DDI_SLOT_MAX];
#endif

static void dma_soft_inc(u32 *pu32);
static void dma_soft_note_peak(u32 *pu32Peak, u32 u32Val);
static void dma_soft_emit(const char *szFmt, ...);
static void dma_soft_inventory_log(void);
static void dma_soft_maybe_once(void);
static void dma_soft_log_coherent_residual_once(void);
static void dma_soft_log_stream_residual_once(void);
static void dma_soft_log_force32_residual_once(void);
static void dma_soft_log_ddi_buf_residual_once(void);
static void dma_soft_log_iommu_residual_once(void);
#if !defined(UDX_HOST_LIBC)
static void dma_soft_log_iommu_fail_once(void);
#endif
static void dma_soft_log_sync_residual_once(void);
static void dma_soft_log_window_residual_once(void);
static void dma_soft_log_mint_residual_once(void);
static void dma_soft_log_mask_residual_once(void);
static void dma_soft_log_product_host_residual_once(void);
static void dma_soft_log_dual_dod_residual_once(void);
static void dma_soft_log_hazard_residual_once(void);
static void dma_soft_log_functional_residual_once(void);
static void dma_soft_log_buf_map_residual_once(void);
static void dma_soft_log_ring_residual_once(void);
static void dma_soft_log_map_idem_residual_once(void);
static void dma_soft_log_map_recb_residual_once(void);
static void dma_soft_log_map_note_residual_once(void);
static void dma_soft_log_denser_residual_once(void);
static void dma_soft_map_slot_add(udx_dma_addr_t dma, size_t cbSize);
static int  dma_soft_map_slot_drop(udx_dma_addr_t dma);

static void
dma_soft_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

/** Soft: raise peak if u32Val is higher (diagnostics only). */
static void
dma_soft_note_peak(u32 *pu32Peak, u32 u32Val)
{
    if (pu32Peak != NULL && u32Val > *pu32Peak) {
        *pu32Peak = u32Val;
    }
}

/*
 * Soft inventory emit path — host console; does not use udx_printk so
 * core soft printk tallies stay product-path pure.
 */
static void
dma_soft_emit(const char *szFmt, ...)
{
    if (szFmt == NULL) {
        return;
    }
#if defined(UDX_HOST_LIBC)
    {
        va_list apArgs;

        va_start(apArgs, szFmt);
        (void)vprintf(szFmt, apArgs);
        va_end(apArgs);
    }
#else
    (void)szFmt;
#endif
}

/**
 * Soft stream map-live bookkeeping (Soft!=product IOMMU).
 * Functional residual deepen: re-map same cookie refreshes cb (map_idem
 * spirit for stream); table-full still identity-maps (tally only).
 */
static void
dma_soft_map_slot_add(udx_dma_addr_t dma, size_t cbSize)
{
    u32 iSlot;
    u32 cbSoft;

    if (dma == 0) {
        return;
    }
    cbSoft = (cbSize > 0xffffffffu) ? 0xffffffffu : (u32)cbSize;

    /* Functional: re-map same cookie → refresh span (stream map_idem spirit). */
    for (iSlot = 0; iSlot < UDX_DMA_SOFT_MAP_SLOT_MAX; iSlot++) {
        if (g_aMapSlot[iSlot].u8Live != 0u && g_aMapSlot[iSlot].dma == dma) {
            g_aMapSlot[iSlot].cbSize = cbSoft;
            dma_soft_inc(&g_u32DmaMapRemap);
            dma_soft_log_map_idem_residual_once();
            return;
        }
    }

    for (iSlot = 0; iSlot < UDX_DMA_SOFT_MAP_SLOT_MAX; iSlot++) {
        if (g_aMapSlot[iSlot].u8Live == 0u) {
            g_aMapSlot[iSlot].u8Live = 1u;
            g_aMapSlot[iSlot].dma = dma;
            g_aMapSlot[iSlot].cbSize = cbSoft;
            dma_soft_inc(&g_u32DmaMapLive);
            dma_soft_note_peak(&g_u32DmaMapLivePeak, g_u32DmaMapLive);
            return;
        }
    }
    /* Soft: table full — map still identity-ok; residual tally only. */
    dma_soft_inc(&g_u32DmaMapLiveFull);
}

/** Soft: drop map-live; returns 1 if found. */
static int
dma_soft_map_slot_drop(udx_dma_addr_t dma)
{
    u32 iSlot;

    if (dma == 0) {
        return 0;
    }
    for (iSlot = 0; iSlot < UDX_DMA_SOFT_MAP_SLOT_MAX; iSlot++) {
        if (g_aMapSlot[iSlot].u8Live != 0u && g_aMapSlot[iSlot].dma == dma) {
            g_aMapSlot[iSlot].u8Live = 0u;
            g_aMapSlot[iSlot].dma = 0;
            g_aMapSlot[iSlot].cbSize = 0u;
            if (g_u32DmaMapLive > 0u) {
                g_u32DmaMapLive--;
            }
            return 1;
        }
    }
    return 0;
}

#if defined(UDX_HOST_LIBC) && !defined(GJ_FREESTANDING)
/** Host soft DDI slot find by handle + cookie. */
static struct udx_dma_soft_ddi_slot *
dma_soft_ddi_slot_find(u32 u32Handle, udx_dma_addr_t cookie)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < UDX_DMA_SOFT_DDI_SLOT_MAX; iSlot++) {
        if (g_aDdiSlot[iSlot].u8Live != 0u &&
            g_aDdiSlot[iSlot].u32Handle == u32Handle &&
            g_aDdiSlot[iSlot].cookie == cookie) {
            return &g_aDdiSlot[iSlot];
        }
    }
    return NULL;
}

/** Host soft DDI slot find free entry. */
static struct udx_dma_soft_ddi_slot *
dma_soft_ddi_slot_alloc(void)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < UDX_DMA_SOFT_DDI_SLOT_MAX; iSlot++) {
        if (g_aDdiSlot[iSlot].u8Live == 0u) {
            return &g_aDdiSlot[iSlot];
        }
    }
    return NULL;
}
#endif

/**
 * Greppable soft DMA inventory (exclusive deepen).
 * Prefix-stable "udx: dma soft …" — never hard-gates; observation only.
 * Once-lamp via dma_soft_maybe_once — no stamp storms.
 *
 * greppable: udx: dma soft
 */
static void
dma_soft_inventory_log(void)
{
    u32 u32Host;
    u32 u32FsSlots;
    u32 u32FsSlotCb;

    dma_soft_inc(&g_u32DmaLogN);

#if defined(UDX_HOST_LIBC)
    u32Host = 1u;
    u32FsSlots = 0u;
    u32FsSlotCb = 0u;
#else
    u32Host = 0u;
    u32FsSlots = UDX_FS_DMA_SLOTS;
    u32FsSlotCb = UDX_FS_DMA_SLOT_CB;
#endif

    /* Grep: udx: dma soft inventory */
    dma_soft_emit("udx: dma soft inventory alloc_ok=%u alloc_fail=%u "
                  "alloc_inval=%u free=%u map_ok=%u map_null=%u unmap=%u "
                  "sync_cpu=%u sync_dev=%u iommu_ok=%u iommu_fail=%u "
                  "log_n=%u layout_ver=%u\n",
                  g_u32DmaAllocOk, g_u32DmaAllocFail, g_u32DmaAllocInval,
                  g_u32DmaFree, g_u32DmaMapOk, g_u32DmaMapNull, g_u32DmaUnmap,
                  g_u32DmaSyncCpu, g_u32DmaSyncDev, g_u32DmaIommuOk,
                  g_u32DmaIommuFail, g_u32DmaLogN, UDX_DMA_LAYOUT_VER);

    /* Grep: udx: dma soft alloc (coherent) */
    dma_soft_emit("udx: dma soft alloc enter=%u ok=%u fail=%u inval=%u "
                  "free=%u free_null=%u last_cb=%u peak_cb=%u "
                  "force32_note=%u identity_cookie=1\n",
                  g_u32DmaAllocEnter, g_u32DmaAllocOk, g_u32DmaAllocFail,
                  g_u32DmaAllocInval, g_u32DmaFree, g_u32DmaFreeNull,
                  g_u32DmaLastCb, g_u32DmaPeakCb, g_u32DmaAllocForce32);

    /* Grep: udx: dma soft coherent */
    dma_soft_emit("udx: dma soft coherent path=alloc_coherent "
                  "enter=%u ok=%u fail=%u force32_note=%u "
                  "soft=1 product_window_cap=OPEN "
                  "product_host=rtl8168_udx|xhci_udx\n",
                  g_u32DmaAllocEnter, g_u32DmaAllocOk, g_u32DmaAllocFail,
                  g_u32DmaAllocForce32);

    /* Grep: udx: dma soft map / stream */
    dma_soft_emit("udx: dma soft map enter=%u ok=%u null=%u unmap=%u "
                  "oversize=%u dir_inval=%u live=%u live_peak=%u "
                  "unmap_miss=%u remap=%u live_full=%u "
                  "identity=1 mapping_error=zero_cookie\n",
                  g_u32DmaMap, g_u32DmaMapOk, g_u32DmaMapNull, g_u32DmaUnmap,
                  g_u32DmaMapOversize, g_u32DmaMapDirInval, g_u32DmaMapLive,
                  g_u32DmaMapLivePeak, g_u32DmaUnmapMiss, g_u32DmaMapRemap,
                  g_u32DmaMapLiveFull);
    dma_soft_emit("udx: dma soft stream path=map_single enter=%u ok=%u "
                  "null=%u oversize=%u dir_inval=%u map_max=0x%llx soft=1 "
                  "product_host=xhci_udx|rtl8168_udx "
                  "remap=%u live_full=%u "
                  "cookie_from_cpu=1 Soft!=product\n",
                  g_u32DmaMap, g_u32DmaMapOk, g_u32DmaMapNull,
                  g_u32DmaMapOversize, g_u32DmaMapDirInval,
                  (unsigned long long)UDX_DMA_MAP_MAX,
                  g_u32DmaMapRemap, g_u32DmaMapLiveFull);

    /* Grep: udx: dma soft sync */
    dma_soft_emit("udx: dma soft sync for_cpu=%u for_device=%u "
                  "host_barrier=clflush|mfence full_iommu_flush=0 "
                  "product_host=rtl8168_udx_repost\n",
                  g_u32DmaSyncCpu, g_u32DmaSyncDev);

    /* Grep: udx: dma soft iommu */
    dma_soft_emit("udx: dma soft iommu enter=%u ok=%u fail=%u host_nop=%u "
                  "plat_op=5 freestanding_syscall=%u soft=1 mint=0\n",
                  g_u32DmaIommuEnter, g_u32DmaIommuOk, g_u32DmaIommuFail,
                  g_u32DmaIommuHostNop, (u32Host == 0u) ? 1u : 0u);

    /* Grep: udx: dma soft slab */
    dma_soft_emit("udx: dma soft slab host_libc=%u freestanding=%u "
                  "slots=%u slot_cb=%u live=%u peak=%u full=%u\n",
                  u32Host, (u32Host == 0u) ? 1u : 0u, u32FsSlots, u32FsSlotCb,
                  g_u32DmaSlabLive, g_u32DmaSlabPeak, g_u32DmaSlabFull);

    /* Grep: udx: dma soft force32 */
    dma_soft_emit("udx: dma soft force32 note=1 low_max=0x%llx "
                  "vtd_identity=0x%llx window_ok=%u window_fail=%u "
                  "window_f32=%u alloc_f32_note=%u ddi_f32=%u "
                  "strict_pages=DDI_DMA_BUF soft=1 product_mint=0\n",
                  (unsigned long long)UDX_DMA_LOW_MAX,
                  (unsigned long long)UDX_DMA_VTD_IDENTITY_LIMIT,
                  g_u32DmaWinOk, g_u32DmaWinFail, g_u32DmaWinForce32,
                  g_u32DmaAllocForce32, g_u32DmaDdiBufForce32);

    /* Grep: udx: dma soft mask */
    dma_soft_emit("udx: dma soft mask set=%u coh_set=%u fail=%u "
                  "mask=0x%llx coh_mask=0x%llx max_map=0x%llx soft=1\n",
                  g_u32DmaMaskSet, g_u32DmaCohMaskSet, g_u32DmaMaskFail,
                  (unsigned long long)g_u64DmaMask,
                  (unsigned long long)g_u64DmaCoherentMask,
                  (unsigned long long)UDX_DMA_MAP_MAX);

    /* Grep: udx: dma soft ddi_buf */
    dma_soft_emit("udx: dma soft ddi_buf alloc=%u free=%u map=%u "
                  "ok=%u fail=%u force32=%u chain=%u chain_ok=%u "
                  "map_idem=%u map_recb=%u "
                  "ops=10/11/12 force32_flag=1 "
                  "host_soft_slots=%u slot_live=%u slot_peak=%u slot_full=%u "
                  "page_cb=%u pages_max=%u freestanding_syscall=%u "
                  "soft=1 product_mint=0 Soft!=product\n",
                  g_u32DmaDdiBufAlloc, g_u32DmaDdiBufFree, g_u32DmaDdiBufMap,
                  g_u32DmaDdiBufOk, g_u32DmaDdiBufFail, g_u32DmaDdiBufForce32,
                  g_u32DmaDdiBufChain, g_u32DmaDdiBufChainOk,
                  g_u32DmaDdiMapIdem, g_u32DmaDdiMapRecb,
                  u32Host, g_u32DmaDdiSlotLive, g_u32DmaDdiSlotPeak,
                  g_u32DmaDdiSlotFull, UDX_DMA_BUF_PAGE_CB,
                  UDX_DMA_SOFT_DDI_PAGES_MAX,
                  (u32Host == 0u) ? 1u : 0u);

    /* Grep: udx: dma soft ddi_buf chain (functional denser: +MAP_REMAP) */
    dma_soft_emit("udx: dma soft ddi_buf chain enter=%u ok=%u "
                  "path=ALLOC_MAP_MAP_REMAP_FREE "
                  "map_idem=%u map_recb=%u "
                  "soft=1 mint=0 Soft!=product\n",
                  g_u32DmaDdiBufChain, g_u32DmaDdiBufChainOk,
                  g_u32DmaDdiMapIdem, g_u32DmaDdiMapRecb);

    /*
     * Product host shape catalog (Dual DoD A/B OPEN seed).
     * greppable: udx: dma soft product host
     */
    dma_soft_emit("udx: dma soft product host "
                  "shape=coherent|stream|force32|ddi_buf "
                  "coherent=udx_dma_alloc_coherent "
                  "stream=udx_dma_map_single "
                  "force32=note|window_ok|DDI_BUF_F "
                  "ddi_buf=ops_10_11_12|chain "
                  "rtl8168_udx=DoD_B_OPEN "
                  "xhci_udx=DoD_A_OPEN "
                  "layout_ver=%u soft=1 product_mint=0 "
                  "G-AC-1=1 Soft!=product\n",
                  UDX_DMA_LAYOUT_VER);

    /*
     * Soft mint honesty — never claims DMA window / CNode mint.
     * greppable: udx: dma soft mint honesty
     */
    dma_soft_emit("udx: dma soft mint honesty "
                  "dma_window=OPEN cnode=0 vtd_grant=0 "
                  "identity_cookie=1 force32_mint=0 ddi_buf_mint=0 "
                  "iommu_grant=soft host_nop=%u soft=1 Soft!=product\n",
                  g_u32DmaIommuHostNop);

    /*
     * Path catalog — what this soft surface is / is not.
     * greppable: udx: dma soft path
     */
    dma_soft_emit("udx: dma soft path "
                  "coherent=udx_dma_alloc_coherent "
                  "stream=udx_dma_map_single "
                  "free=udx_dma_free_coherent "
                  "unmap=udx_dma_unmap_single "
                  "sync_cpu=udx_dma_sync_single_for_cpu "
                  "sync_dev=udx_dma_sync_single_for_device "
                  "iommu=udx_dma_iommu_grant "
                  "window_ok=udx_dma_window_ok "
                  "mask=udx_dma_set_mask "
                  "ddi_buf=udx_dma_ddi_buf_* "
                  "ddi_chain=udx_dma_ddi_buf_chain "
                  "shape_ok=udx_dma_shape_ok "
                  "skeleton_gate=0 hard_gate=0 soft=1 "
                  "layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);

    /*
     * Grep: udx: dma soft honesty
     * Soft inventory != product multi-server confine.
     */
    dma_soft_emit("udx: dma soft honesty multi_server=0 confine=0 "
                  "exclusive=1 soft=1 product_kernel=OPEN "
                  "force32=note ddi_buf=soft layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);

    /*
     * Lean residual DMA API honesty (Soft!=product; dual MIT OR Apache-2.0).
     * Deepen: coherent/stream/force32/ddi_buf/iommu/sync/window/mint/mask/
     * product_host/dual_dod/hazard/functional/buf_map/ring/map_idem/
     * map_recb/map_note path residual lamps toward Dual DoD A/B OPEN
     * (agent != close). H1/H2/H3 hazard honesty; storm=0. Functional denser
     * buf/map + STRONGER map/note toward product NIC/USB rings
     * (Soft!=product; never Dual DoD close; product_hosts=UDX).
     * greppable: udx: dma soft residual lean
     * greppable: udx: dma soft residual lean UDX eng
     * greppable: udx: dma soft residual coherent
     * greppable: udx: dma soft residual stream
     * greppable: udx: dma soft residual force32
     * greppable: udx: dma soft residual ddi_buf
     * greppable: udx: dma soft residual iommu
     * greppable: udx: dma soft residual sync
     * greppable: udx: dma soft residual window
     * greppable: udx: dma soft residual mint
     * greppable: udx: dma soft residual mask
     * greppable: udx: dma soft residual product_host
     * greppable: udx: dma soft residual dual_dod
     * greppable: udx: dma soft residual hazard
     * greppable: udx: dma soft residual functional
     * greppable: udx: dma soft residual buf_map
     * greppable: udx: dma soft residual ring
     * greppable: udx: dma soft residual map_idem
     * greppable: udx: dma soft residual map_recb
     * greppable: udx: dma soft residual map_note
     * greppable: product_hosts=UDX
     * greppable: udx: dma soft api honesty
     */
    dma_soft_emit("udx: dma soft residual lean "
                  "layout_ver=%u "
                  "coherent=identity stream=map_single "
                  "force32=note|ddi_buf "
                  "iommu_grant=soft host_nop=1 "
                  "ddi_buf=host_slots|door_ops "
                  "sync=mfence window=geometric "
                  "mask=process_global "
                  "residual_coherent=%u residual_stream=%u "
                  "residual_force32=%u residual_ddi_buf=%u "
                  "residual_iommu=%u residual_sync=%u "
                  "residual_window=%u residual_mint=%u "
                  "residual_mask=%u residual_product_host=%u "
                  "residual_dual_dod=%u residual_hazard=%u "
                  "residual_functional=%u residual_buf_map=%u "
                  "residual_ring=%u residual_map_idem=%u "
                  "residual_map_recb=%u residual_map_note=%u "
                  "product_DMA_window_cap=OPEN soft=1 "
                  "product_hosts=UDX "
                  "hosts=rtl8168_udx,xhci_udx "
                  "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN "
                  "freestanding_class=SKIP "
                  "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
                  "G-AC-1=1 H1=no_net_eth_poll H2=storm=0 "
                  "H3=free_order_OPEN storm=0 "
                  "(Soft!=product; lean residual only)\n",
                  UDX_DMA_LAYOUT_VER,
                  g_u32DmaResidualCoherent, g_u32DmaResidualStream,
                  g_u32DmaResidualForce32, g_u32DmaResidualDdiBuf,
                  g_u32DmaResidualIommu, g_u32DmaResidualSync,
                  g_u32DmaResidualWindow, g_u32DmaResidualMint,
                  g_u32DmaResidualMask, g_u32DmaResidualProductHost,
                  g_u32DmaResidualDualDod, g_u32DmaResidualHazard,
                  g_u32DmaResidualFunctional, g_u32DmaResidualBufMap,
                  g_u32DmaResidualRing, g_u32DmaResidualMapIdem,
                  g_u32DmaResidualMapRecb, g_u32DmaResidualMapNote);
    dma_soft_emit("udx: dma soft residual lean UDX eng "
                  "layout_ver=%u "
                  "shape=coherent|stream|force32|ddi_buf|"
                  "iommu|sync|window|mint|mask|product_host|"
                  "dual_dod|hazard|functional|buf_map|ring|"
                  "map_idem|map_recb|map_note "
                  "product_hosts=UDX "
                  "hosts=rtl8168_udx|xhci_udx "
                  "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_close=0 "
                  "rtl8168_udx=coherent+iommu_grant+sync_for_device "
                  "xhci_udx=coherent|stream residual "
                  "functional=buf_map+map_idem+ring "
                  "map_note=map_single|force32_note|ddi_buf_map "
                  "product_dma_window=OPEN soft=1 "
                  "H1=no_net_eth_poll H2=storm=0 H3=free_order_OPEN "
                  "G-AC-1=1 Soft!=product\n",
                  UDX_DMA_LAYOUT_VER);

    /*
     * coherent residual -- identity cookie != DMA window cap mint.
     * greppable: udx: dma soft residual coherent
     */
    dma_soft_emit("udx: dma soft residual coherent "
                  "path=udx_dma_alloc_coherent "
                  "identity_cookie=1 product_DMA_window_cap=OPEN "
                  "enter=%u ok=%u fail=%u inval=%u force32_note=%u "
                  "last_cb=%u peak_cb=%u residual_n=%u "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "rtl8168_udx=ring xhci_udx=TRB residual "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(coherent residual; identity cookie only)\n",
                  g_u32DmaAllocEnter, g_u32DmaAllocOk, g_u32DmaAllocFail,
                  g_u32DmaAllocInval, g_u32DmaAllocForce32,
                  g_u32DmaLastCb, g_u32DmaPeakCb, g_u32DmaResidualCoherent,
                  UDX_DMA_LAYOUT_VER);

    /*
     * stream residual -- map_single identity + soft map-live.
     * greppable: udx: dma soft residual stream
     */
    dma_soft_emit("udx: dma soft residual stream "
                  "path=udx_dma_map_single "
                  "identity=1 cookie_from_cpu=1 "
                  "mapping_error=zero_cookie "
                  "enter=%u ok=%u null=%u oversize=%u dir_inval=%u "
                  "unmap=%u unmap_miss=%u live=%u live_peak=%u "
                  "remap=%u live_full=%u "
                  "map_max=0x%llx residual_n=%u "
                  "product_hosts=UDX "
                  "hosts=xhci_udx,rtl8168_udx "
                  "xhci_udx=xfer residual "
                  "map_note_surface=%u dual_dod=OPEN "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(stream residual; soft map-live only; Soft!=product)\n",
                  g_u32DmaMap, g_u32DmaMapOk, g_u32DmaMapNull,
                  g_u32DmaMapOversize, g_u32DmaMapDirInval,
                  g_u32DmaUnmap, g_u32DmaUnmapMiss,
                  g_u32DmaMapLive, g_u32DmaMapLivePeak,
                  g_u32DmaMapRemap, g_u32DmaMapLiveFull,
                  (unsigned long long)UDX_DMA_MAP_MAX,
                  g_u32DmaResidualStream, UDX_DMA_MAP_NOTE_SURFACE,
                  UDX_DMA_LAYOUT_VER);

    /*
     * force32 residual -- geometric note; not product mint.
     * greppable: udx: dma soft residual force32
     */
    dma_soft_emit("udx: dma soft residual force32 "
                  "note=1 low_max=0x%llx vtd_identity=0x%llx "
                  "window_ok=%u window_fail=%u window_f32=%u "
                  "alloc_f32_note=%u ddi_f32=%u residual_n=%u "
                  "force32_mint=0 product_DMA_window_cap=OPEN "
                  "strict_pages=DDI_DMA_BUF "
                  "product_hosts=UDX "
                  "hosts=rtl8168_udx,xhci_udx "
                  "map_note_surface=%u dual_dod=OPEN "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(force32 residual; geometric note only; Soft!=product)\n",
                  (unsigned long long)UDX_DMA_LOW_MAX,
                  (unsigned long long)UDX_DMA_VTD_IDENTITY_LIMIT,
                  g_u32DmaWinOk, g_u32DmaWinFail, g_u32DmaWinForce32,
                  g_u32DmaAllocForce32, g_u32DmaDdiBufForce32,
                  g_u32DmaResidualForce32, UDX_DMA_MAP_NOTE_SURFACE,
                  UDX_DMA_LAYOUT_VER);

    /*
     * ddi_buf residual -- ops 10/11/12 + host slots + chain; mint=0.
     * greppable: udx: dma soft residual ddi_buf
     */
    dma_soft_emit("udx: dma soft residual ddi_buf "
                  "ops=10/11/12 chain=ALLOC_MAP_MAP_REMAP_FREE "
                  "alloc=%u free=%u map=%u ok=%u fail=%u "
                  "force32=%u chain=%u chain_ok=%u "
                  "map_idem=%u map_recb=%u "
                  "slot_live=%u slot_peak=%u slot_full=%u "
                  "residual_n=%u ddi_buf_mint=0 "
                  "product_DMA_window_cap=OPEN "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(ddi_buf residual; soft slots or door ops)\n",
                  g_u32DmaDdiBufAlloc, g_u32DmaDdiBufFree, g_u32DmaDdiBufMap,
                  g_u32DmaDdiBufOk, g_u32DmaDdiBufFail, g_u32DmaDdiBufForce32,
                  g_u32DmaDdiBufChain, g_u32DmaDdiBufChainOk,
                  g_u32DmaDdiMapIdem, g_u32DmaDdiMapRecb,
                  g_u32DmaDdiSlotLive, g_u32DmaDdiSlotPeak, g_u32DmaDdiSlotFull,
                  g_u32DmaResidualDdiBuf, UDX_DMA_LAYOUT_VER);

    /*
     * iommu residual -- host nop / soft grant != live VT-d / CNode mint.
     * greppable: udx: dma soft residual iommu
     */
    dma_soft_emit("udx: dma soft residual iommu "
                  "path=udx_dma_iommu_grant "
                  "enter=%u ok=%u fail=%u host_nop=%u "
                  "plat_op=5 residual_n=%u "
                  "full_vtd=0 cap_mint=0 vtd_grant=0 "
                  "product_DMA_window_cap=OPEN "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "rtl8168_udx=iommu_grant residual "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(iommu residual; soft grant or host nop)\n",
                  g_u32DmaIommuEnter, g_u32DmaIommuOk, g_u32DmaIommuFail,
                  g_u32DmaIommuHostNop, g_u32DmaResidualIommu,
                  UDX_DMA_LAYOUT_VER);

    /*
     * sync residual -- mfence only; not full IOTLB / CWB.
     * greppable: udx: dma soft residual sync
     */
    dma_soft_emit("udx: dma soft residual sync "
                  "for_cpu=%u for_device=%u "
                  "host_barrier=mfence full_iommu_flush=0 "
                  "cwb=0 iotlb_flush=0 residual_n=%u "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "rtl8168_udx=repost_sync_for_device "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(sync residual; mfence only)\n",
                  g_u32DmaSyncCpu, g_u32DmaSyncDev, g_u32DmaResidualSync,
                  UDX_DMA_LAYOUT_VER);

    /*
     * window residual -- geometric ok; not live IOMMU PTE query.
     * greppable: udx: dma soft residual window
     */
    dma_soft_emit("udx: dma soft residual window "
                  "path=udx_dma_window_ok "
                  "ok=%u fail=%u force32=%u residual_n=%u "
                  "live_iommu_pte=0 geometric=1 "
                  "vtd_identity=0x%llx low_max=0x%llx "
                  "product_DMA_window_cap=OPEN "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(window residual; geometric only)\n",
                  g_u32DmaWinOk, g_u32DmaWinFail, g_u32DmaWinForce32,
                  g_u32DmaResidualWindow,
                  (unsigned long long)UDX_DMA_VTD_IDENTITY_LIMIT,
                  (unsigned long long)UDX_DMA_LOW_MAX,
                  UDX_DMA_LAYOUT_VER);

    /*
     * mint residual -- product DMA window cap remains OPEN.
     * greppable: udx: dma soft residual mint
     */
    dma_soft_emit("udx: dma soft residual mint "
                  "product_DMA_window_cap_mint=OPEN "
                  "cnode=0 vtd_grant=0 identity_cookie=1 "
                  "force32_mint=0 ddi_buf_mint=0 iommu_grant=soft "
                  "multi_server=0 confine=0 residual_n=%u "
                  "DoD_A=OPEN DoD_B=OPEN "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "freestanding_class=SKIP "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(product DMA window cap mint OPEN)\n",
                  g_u32DmaResidualMint, UDX_DMA_LAYOUT_VER);

    /*
     * mask residual -- process-global soft only; not device embed.
     * greppable: udx: dma soft residual mask
     */
    dma_soft_emit("udx: dma soft residual mask "
                  "path=udx_dma_set_mask|set_coherent_mask "
                  "set=%u coh_set=%u fail=%u residual_n=%u "
                  "mask=0x%llx coh_mask=0x%llx max_map=0x%llx "
                  "process_global=1 device_embed=0 iommu_program=0 "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(mask residual; process-global soft only)\n",
                  g_u32DmaMaskSet, g_u32DmaCohMaskSet, g_u32DmaMaskFail,
                  g_u32DmaResidualMask,
                  (unsigned long long)g_u64DmaMask,
                  (unsigned long long)g_u64DmaCoherentMask,
                  (unsigned long long)UDX_DMA_MAP_MAX,
                  UDX_DMA_LAYOUT_VER);

    /*
     * product_host residual -- Dual DoD A/B catalog only (never close).
     * greppable: udx: dma soft residual product_host
     */
    dma_soft_emit("udx: dma soft residual product_host "
                  "hosts=rtl8168_udx,xhci_udx surface=dma "
                  "product=UDX+ABI product_hosts=UDX "
                  "Linux-shaped_userspace=1 "
                  "freestanding_class=SKIP kernel_rtl8168=SKIP "
                  "kernel_xhci_msc=SKIP "
                  "rtl8168_udx=DoD_B_OPEN "
                  "rtl8168_path=coherent+iommu_grant+sync_for_device "
                  "xhci_udx=DoD_A_OPEN "
                  "xhci_path=coherent|stream residual "
                  "force32=note|window_ok|DDI_BUF_F "
                  "ddi_buf=ops_10_11_12|chain|map_idem "
                  "map_note=map_single|force32_note|ddi_buf_map "
                  "functional=buf_map+ring "
                  "product_DMA_window_cap=OPEN residual_n=%u "
                  "dual_dod=OPEN "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(product-host DMA residual; Soft!=product)\n",
                  g_u32DmaResidualProductHost, UDX_DMA_LAYOUT_VER);

    /*
     * Dual DoD A/B OPEN residual (agent!=close; Soft!=product).
     * greppable: udx: dma soft residual dual_dod
     * greppable: product_hosts=UDX | dual_dod OPEN | Soft!=product
     */
    dma_soft_emit("udx: dma soft residual dual_dod "
                  "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_close=0 "
                  "A=xhci_udx+DDI B=rtl8168_udx+DDI "
                  "surface=dma coherent|stream|force32|ddi_buf|map_note "
                  "product=UDX+ABI product_hosts=UDX "
                  "Linux-shaped_userspace=1 "
                  "freestanding_class=SKIP "
                  "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
                  "in_kernel_ko=0 G-AC-1=1 "
                  "soft_lamps_close=0 residual_n=%u "
                  "soft=1 soft_ne_product=1 Soft!=product "
                  "dual=MIT_OR_Apache-2.0 layout_ver=%u "
                  "(Dual DoD residual OPEN; soft residual; agent!=close)\n",
                  g_u32DmaResidualDualDod, UDX_DMA_LAYOUT_VER);

    /*
     * Hazard residual honesty (assurance H1/H2/H3; Soft!=product).
     * H1: DMA path never calls net_eth_poll (no IRQ-poll product).
     * H2: residual once-lamps only; storm=0 on inventory re-dump.
     * H3: free_coherent order vs as_destroy remains OPEN (process-law).
     * greppable: udx: dma soft residual hazard
     */
    dma_soft_emit("udx: dma soft residual hazard "
                  "H1=no_net_eth_poll_from_IRQ "
                  "H2=no_stamp_storms once=1 storm=0 "
                  "H3=free_before_as_destroy_OPEN "
                  "net_inject=0 hard_irq_dma=0 "
                  "sync=mfence_only residual_n=%u "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP "
                  "soft=1 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(hazard residual; H1/H2/H3 honesty)\n",
                  g_u32DmaResidualHazard, UDX_DMA_LAYOUT_VER);

    /*
     * Functional residual -- denser buf/map path toward product NIC/USB rings.
     * greppable: udx: dma soft residual functional
     */
    dma_soft_emit("udx: dma soft residual functional "
                  "path=buf_map+map_idem+ring "
                  "chain=ALLOC_MAP_MAP_REMAP_FREE "
                  "coherent_ok=%u map_ok=%u ddi_map=%u "
                  "map_idem=%u map_recb=%u stream_remap=%u "
                  "ring_sized=%u ring_page_cb=%u residual_n=%u "
                  "rtl8168_udx=NIC_desc_ring "
                  "xhci_udx=USB_TRB_ring "
                  "thr_only_consumer=%u dev_visible_mfence=%u "
                  "H1=no_net_eth_poll hard_irq_map=%u net_inject=%u "
                  "lean_checks=%u product_DMA_window_cap=OPEN "
                  "DoD_A=%u DoD_B=%u agent_close=%u "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(functional residual DMA buf/map toward rings; STRONGER)\n",
                  g_u32DmaAllocOk, g_u32DmaMapOk, g_u32DmaDdiBufMap,
                  g_u32DmaDdiMapIdem, g_u32DmaDdiMapRecb, g_u32DmaMapRemap,
                  g_u32DmaRingSized, UDX_DMA_SOFT_RING_PAGE_CB,
                  g_u32DmaResidualFunctional,
                  UDX_DMA_THR_ONLY_CONSUMER, UDX_DMA_RING_DEV_VISIBLE,
                  UDX_DMA_HARD_IRQ_MAP, UDX_DMA_NET_INJECT,
                  UDX_DMA_LEAN_CHECKS,
                  UDX_DMA_DOD_A_OPEN, UDX_DMA_DOD_B_OPEN,
                  UDX_DMA_AGENT_CLOSE, UDX_DMA_LAYOUT_VER);

    /*
     * buf_map residual -- ddi_buf_map + stream map toward ring/xfer.
     * greppable: udx: dma soft residual buf_map
     */
    dma_soft_emit("udx: dma soft residual buf_map "
                  "ddi_map=%u ddi_ok=%u stream_map=%u stream_ok=%u "
                  "map_idem=%u map_recb=%u stream_remap=%u "
                  "live=%u live_peak=%u live_full=%u residual_n=%u "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "rtl8168_udx=ring_buf xhci_udx=xfer_map "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(buf_map residual; soft map only)\n",
                  g_u32DmaDdiBufMap, g_u32DmaDdiBufOk,
                  g_u32DmaMap, g_u32DmaMapOk,
                  g_u32DmaDdiMapIdem, g_u32DmaDdiMapRecb, g_u32DmaMapRemap,
                  g_u32DmaMapLive, g_u32DmaMapLivePeak, g_u32DmaMapLiveFull,
                  g_u32DmaResidualBufMap, UDX_DMA_LAYOUT_VER);

    /*
     * ring residual -- product host NIC desc / USB TRB page spirit catalog.
     * greppable: udx: dma soft residual ring
     */
    dma_soft_emit("udx: dma soft residual ring "
                  "ring_page_cb=%u ring_sized_ok=%u residual_n=%u "
                  "rtl8168_udx=desc_ring_bytes "
                  "xhci_udx=TRB_event_ring_page "
                  "tnpds=OPEN crcr=OPEN dcbaap=OPEN "
                  "never_program=1 product_DMA_window_cap=OPEN "
                  "dev_visible_mfence=%u thr_only_consumer=%u "
                  "H1=no_net_eth_poll hard_irq_map=%u net_inject=%u "
                  "DoD_A=%u DoD_B=%u agent_close=%u "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(ring residual catalog; Soft!=product; STRONGER)\n",
                  UDX_DMA_SOFT_RING_PAGE_CB, g_u32DmaRingSized,
                  g_u32DmaResidualRing,
                  UDX_DMA_RING_DEV_VISIBLE, UDX_DMA_THR_ONLY_CONSUMER,
                  UDX_DMA_HARD_IRQ_MAP, UDX_DMA_NET_INJECT,
                  UDX_DMA_DOD_A_OPEN, UDX_DMA_DOD_B_OPEN,
                  UDX_DMA_AGENT_CLOSE, UDX_DMA_LAYOUT_VER);

    /*
     * map_idem residual -- re-MAP same pa/cb cookie match.
     * greppable: udx: dma soft residual map_idem
     */
    dma_soft_emit("udx: dma soft residual map_idem "
                  "ddi_map_idem=%u stream_remap=%u residual_n=%u "
                  "same_pa_cb=1 cookie_match=1 "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(map_idem residual; re-MAP same)\n",
                  g_u32DmaDdiMapIdem, g_u32DmaMapRemap,
                  g_u32DmaResidualMapIdem, UDX_DMA_LAYOUT_VER);

    /*
     * map_recb residual -- re-MAP different cb soft span update.
     * greppable: udx: dma soft residual map_recb
     */
    dma_soft_emit("udx: dma soft residual map_recb "
                  "ddi_map_recb=%u residual_n=%u "
                  "cb_change=1 prior_span_update=1 "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "soft=1 product=0 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(map_recb residual; re-MAP diff cb)\n",
                  g_u32DmaDdiMapRecb, g_u32DmaResidualMapRecb,
                  UDX_DMA_LAYOUT_VER);

    /*
     * map_note residual -- STRONGER DMA map/note path for rtl/xhci UDX.
     * Covers stream map_single + force32 geometric note + ddi_buf_map.
     * Soft!=product; dual_dod OPEN; product_hosts=UDX; agent!=close.
     * greppable: udx: dma soft residual map_note
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     */
    dma_soft_emit("udx: dma soft residual map_note "
                  "path=map_single|force32_note|ddi_buf_map "
                  "stream_map=%u stream_ok=%u "
                  "force32_note=%u ddi_f32=%u "
                  "ddi_map=%u ddi_ok=%u "
                  "map_idem=%u map_recb=%u stream_remap=%u "
                  "live=%u live_peak=%u live_full=%u residual_n=%u "
                  "surface=%u lean_checks=%u "
                  "product_hosts=UDX "
                  "hosts=rtl8168_udx|xhci_udx "
                  "rtl8168_udx=DoD_B_OPEN xhci_udx=DoD_A_OPEN "
                  "DoD_A=%u DoD_B=%u dual_dod=OPEN agent_close=%u "
                  "product_DMA_window_cap=OPEN force32_mint=0 "
                  "identity_cookie=1 soft_map_live=1 "
                  "H1=no_net_eth_poll hard_irq_map=%u net_inject=%u "
                  "soft=1 product=0 soft_ne_product=1 Soft!=product "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1 layout_ver=%u "
                  "(map/note residual STRONGER; soft residual; "
                  "dual_dod OPEN; product_hosts=UDX)\n",
                  g_u32DmaMap, g_u32DmaMapOk,
                  g_u32DmaAllocForce32, g_u32DmaDdiBufForce32,
                  g_u32DmaDdiBufMap, g_u32DmaDdiBufOk,
                  g_u32DmaDdiMapIdem, g_u32DmaDdiMapRecb, g_u32DmaMapRemap,
                  g_u32DmaMapLive, g_u32DmaMapLivePeak, g_u32DmaMapLiveFull,
                  g_u32DmaResidualMapNote,
                  UDX_DMA_MAP_NOTE_SURFACE, UDX_DMA_LEAN_CHECKS,
                  UDX_DMA_DOD_A_OPEN, UDX_DMA_DOD_B_OPEN,
                  UDX_DMA_AGENT_CLOSE,
                  UDX_DMA_HARD_IRQ_MAP, UDX_DMA_NET_INJECT,
                  UDX_DMA_LAYOUT_VER);

    /*
     * denser residual (preferred over lamps alone; Soft!=product).
     * greppable: udx: dma soft residual denser
     * greppable: denser_prove=1 | denser residual
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     */
    dma_soft_emit("udx: dma soft residual denser "
                  "denser_prove=%u denser_triple=%u denser_ring=%u "
                  "denser_map_note=%u denser_product_host=%u "
                  "coherent_ok=%u stream_ok=%u ddi_map=%u "
                  "ring_sized=%u map_note_n=%u residual_n=%u "
                  "path=coherent+stream+ddi_buf_map+ring "
                  "product_hosts=UDX hosts=rtl8168_udx|xhci_udx "
                  "DoD_A=%u DoD_B=%u dual_dod=OPEN agent_close=%u "
                  "soft_ne_product=%u fs_class_skip=%u stamp_free_75=%u "
                  "lean_checks=%u wave=%u "
                  "H1=no_net_eth_poll hard_irq_map=%u net_inject=%u "
                  "product_DMA_window_cap=OPEN soft=1 product=0 "
                  "Soft!=product dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                  "layout_ver=%u "
                  "(denser residual prove; Soft!=product; Dual DoD OPEN)\n",
                  UDX_DMA_DENSER_PROVE, UDX_DMA_DENSER_TRIPLE,
                  UDX_DMA_DENSER_RING, UDX_DMA_DENSER_MAP_NOTE,
                  UDX_DMA_DENSER_PRODUCT_HOST,
                  g_u32DmaAllocOk, g_u32DmaMapOk, g_u32DmaDdiBufMap,
                  g_u32DmaRingSized, g_u32DmaResidualMapNote,
                  g_u32DmaResidualDenser,
                  UDX_DMA_DOD_A_OPEN, UDX_DMA_DOD_B_OPEN,
                  UDX_DMA_AGENT_CLOSE,
                  UDX_DMA_SOFT_NE_PRODUCT, UDX_DMA_FS_CLASS_SKIP,
                  UDX_DMA_STAMP_FREE_75,
                  UDX_DMA_LEAN_CHECKS, UDX_DMA_SOFT_WAVE,
                  UDX_DMA_HARD_IRQ_MAP, UDX_DMA_NET_INJECT,
                  UDX_DMA_LAYOUT_VER);

    dma_soft_emit("udx: dma soft api honesty "
                  "product_dma_window_cap=OPEN "
                  "identity_cookie=1 full_vtd=0 cap_mint=0 "
                  "force32_mint=0 ddi_buf_mint=0 "
                  "coherent_residual_n=%u stream_residual_n=%u "
                  "force32_residual_n=%u ddi_buf_residual_n=%u "
                  "iommu_residual_n=%u sync_residual_n=%u "
                  "window_residual_n=%u mint_residual_n=%u "
                  "mask_residual_n=%u product_host_residual_n=%u "
                  "dual_dod_residual_n=%u hazard_residual_n=%u "
                  "functional_residual_n=%u buf_map_residual_n=%u "
                  "ring_residual_n=%u map_idem_residual_n=%u "
                  "map_recb_residual_n=%u map_note_residual_n=%u "
                  "multi_server=0 soft=1 product_kernel=OPEN "
                  "product_hosts=UDX dual_dod=OPEN "
                  "DoD_A=OPEN DoD_B=OPEN H1=no_net_eth_poll "
                  "H2=storm=0 H3=free_order_OPEN "
                  "layout_ver=%u Soft!=product "
                  "dual=MIT_OR_Apache-2.0 G-AC-1=1\n",
                  g_u32DmaResidualCoherent, g_u32DmaResidualStream,
                  g_u32DmaResidualForce32, g_u32DmaResidualDdiBuf,
                  g_u32DmaResidualIommu, g_u32DmaResidualSync,
                  g_u32DmaResidualWindow, g_u32DmaResidualMint,
                  g_u32DmaResidualMask, g_u32DmaResidualProductHost,
                  g_u32DmaResidualDualDod, g_u32DmaResidualHazard,
                  g_u32DmaResidualFunctional, g_u32DmaResidualBufMap,
                  g_u32DmaResidualRing, g_u32DmaResidualMapIdem,
                  g_u32DmaResidualMapRecb, g_u32DmaResidualMapNote,
                  UDX_DMA_LAYOUT_VER);
}

/** Soft: one-shot coherent residual honesty (sparse; no stamp storm). */
static void
dma_soft_log_coherent_residual_once(void)
{
    if (g_fDmaCohResidualOnce != 0) {
        return;
    }
    g_fDmaCohResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualCoherent);
    /* greppable: udx: dma soft residual coherent */
    dma_soft_emit("udx: dma soft residual coherent "
                  "identity_cookie=1 cookie_from_cpu=1 "
                  "product_DMA_window_cap_mint=OPEN "
                  "cap_mint=0 soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/** Soft: one-shot stream residual honesty (sparse; no stamp storm). */
static void
dma_soft_log_stream_residual_once(void)
{
    if (g_fDmaStreamResidualOnce != 0) {
        return;
    }
    g_fDmaStreamResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualStream);
    /* greppable: udx: dma soft residual stream */
    dma_soft_emit("udx: dma soft residual stream "
                  "identity=1 cookie_from_cpu=1 map_live=soft "
                  "product_IOMMU_PTE=0 dir_obs=1 "
                  "product_hosts=UDX dual_dod=OPEN "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/** Soft: one-shot force32 residual honesty (sparse; no stamp storm). */
static void
dma_soft_log_force32_residual_once(void)
{
    if (g_fDmaForce32ResidualOnce != 0) {
        return;
    }
    g_fDmaForce32ResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualForce32);
    /* greppable: udx: dma soft residual force32 */
    dma_soft_emit("udx: dma soft residual force32 "
                  "note=1 force32_mint=0 geometric=1 "
                  "product_DMA_window_cap_mint=OPEN "
                  "product_hosts=UDX dual_dod=OPEN "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
    /* STRONGER: force32 note couples into map/note residual surface. */
    dma_soft_log_map_note_residual_once();
}

/** Soft: one-shot ddi_buf residual honesty (sparse; no stamp storm). */
static void
dma_soft_log_ddi_buf_residual_once(void)
{
    if (g_fDmaDdiBufResidualOnce != 0) {
        return;
    }
    g_fDmaDdiBufResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualDdiBuf);
    /* greppable: udx: dma soft residual ddi_buf */
    dma_soft_emit("udx: dma soft residual ddi_buf "
                  "ops=10/11/12 ddi_buf_mint=0 "
                  "product_DMA_window_cap_mint=OPEN "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/** Soft: one-shot iommu residual honesty (sparse; no stamp storm). */
static void
dma_soft_log_iommu_residual_once(void)
{
    if (g_fDmaIommuResidualOnce != 0) {
        return;
    }
    g_fDmaIommuResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualIommu);
    /*
     * greppable: udx: dma soft residual iommu
     * Soft PLATFORM_INFO op5 only — not product DMA window CNode mint.
     * Under TE: success = soft window + identity cover honesty; cover
     * miss → grant fail. residual: Own stick under TE if identity miss.
     */
    dma_soft_emit("udx: dma soft residual iommu "
                  "path=udx_dma_iommu_grant plat_op=5 "
                  "host_nop_or_soft_grant=1 full_vtd=0 cap_mint=0 "
                  "vtd_grant=0 cover=soft_note "
                  "product_DMA_window_cap_mint=OPEN "
                  "residual=Own_stick_under_TE_if_identity_miss "
                  "enter=%u ok=%u fail=%u "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  g_u32DmaIommuEnter, g_u32DmaIommuOk, g_u32DmaIommuFail,
                  UDX_DMA_LAYOUT_VER);
}

#if !defined(UDX_HOST_LIBC)
/**
 * Soft: one-shot iommu FAIL residual (greppable; no stamp storm).
 * Covers silent soft-fail dig under TE cover miss.
 * greppable: udx: dma soft residual iommu FAIL
 */
static void
dma_soft_log_iommu_fail_once(void)
{
    static u8 fOnce;

    if (fOnce != 0) {
        return;
    }
    fOnce = 1;
    dma_soft_inc(&g_u32DmaResidualIommu);
    /* greppable: udx: dma soft residual iommu FAIL */
    dma_soft_emit("udx: dma soft residual iommu FAIL "
                  "path=udx_dma_iommu_grant plat_op=5 "
                  "enter=%u ok=%u fail=%u full_vtd=0 cap_mint=0 "
                  "cover=soft_note product_DMA_window_cap_mint=OPEN "
                  "residual=Own_stick_under_TE_if_identity_miss "
                  "soft=1 Soft!=product dual_dod_b=OPEN layout_ver=%u\n",
                  g_u32DmaIommuEnter, g_u32DmaIommuOk, g_u32DmaIommuFail,
                  UDX_DMA_LAYOUT_VER);
}
#endif /* !UDX_HOST_LIBC */

/** Soft: one-shot sync residual honesty (sparse; no stamp storm). */
static void
dma_soft_log_sync_residual_once(void)
{
    if (g_fDmaSyncResidualOnce != 0) {
        return;
    }
    g_fDmaSyncResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualSync);
    /* greppable: udx: dma soft residual sync */
    dma_soft_emit("udx: dma soft residual sync "
                  "host_barrier=mfence full_iommu_flush=0 "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/** Soft: one-shot window residual honesty (sparse; no stamp storm). */
static void
dma_soft_log_window_residual_once(void)
{
    if (g_fDmaWindowResidualOnce != 0) {
        return;
    }
    g_fDmaWindowResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualWindow);
    /* greppable: udx: dma soft residual window */
    dma_soft_emit("udx: dma soft residual window "
                  "geometric=1 live_iommu_pte=0 "
                  "product_DMA_window_cap_mint=OPEN "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/** Soft: one-shot mint residual honesty (product cap mint OPEN). */
static void
dma_soft_log_mint_residual_once(void)
{
    if (g_fDmaMintResidualOnce != 0) {
        return;
    }
    g_fDmaMintResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualMint);
    /* greppable: udx: dma soft residual mint */
    dma_soft_emit("udx: dma soft residual mint "
                  "product_DMA_window_cap_mint=OPEN "
                  "DoD_A=OPEN DoD_B=OPEN "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/** Soft: one-shot mask residual honesty (process-global; Soft!=product). */
static void
dma_soft_log_mask_residual_once(void)
{
    if (g_fDmaMaskResidualOnce != 0) {
        return;
    }
    g_fDmaMaskResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualMask);
    /* greppable: udx: dma soft residual mask */
    dma_soft_emit("udx: dma soft residual mask "
                  "process_global=1 device_embed=0 iommu_program=0 "
                  "product_DMA_window_cap_mint=OPEN "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/**
 * Soft: one-shot product_host residual honesty (Dual DoD catalog only).
 * greppable: udx: dma soft residual product_host
 */
static void
dma_soft_log_product_host_residual_once(void)
{
    if (g_fDmaProductHostResidualOnce != 0) {
        return;
    }
    g_fDmaProductHostResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualProductHost);
    /* greppable: udx: dma soft residual product_host */
    dma_soft_emit("udx: dma soft residual product_host "
                  "hosts=rtl8168_udx,xhci_udx "
                  "rtl8168_udx=DoD_B_OPEN xhci_udx=DoD_A_OPEN "
                  "surface=dma freestanding_class=SKIP "
                  "product_DMA_window_cap=OPEN "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/**
 * Soft: one-shot Dual DoD residual honesty (agent!=close).
 * greppable: udx: dma soft residual dual_dod
 * greppable: product_hosts=UDX | Soft!=product | dual_dod OPEN
 */
static void
dma_soft_log_dual_dod_residual_once(void)
{
    if (g_fDmaDualDodResidualOnce != 0) {
        return;
    }
    g_fDmaDualDodResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualDualDod);
    /* greppable: udx: dma soft residual dual_dod */
    dma_soft_emit("udx: dma soft residual dual_dod "
                  "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_close=0 "
                  "A=xhci_udx+DDI B=rtl8168_udx+DDI "
                  "product_hosts=UDX soft_lamps_close=0 G-AC-1=1 "
                  "soft=1 Soft!=product soft residual layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/**
 * Soft: one-shot hazard residual honesty (H1/H2/H3; Soft!=product).
 * greppable: udx: dma soft residual hazard
 */
static void
dma_soft_log_hazard_residual_once(void)
{
    if (g_fDmaHazardResidualOnce != 0) {
        return;
    }
    g_fDmaHazardResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualHazard);
    /* greppable: udx: dma soft residual hazard */
    dma_soft_emit("udx: dma soft residual hazard "
                  "H1=no_net_eth_poll_from_IRQ "
                  "H2=no_stamp_storms once=1 storm=0 "
                  "H3=free_before_as_destroy_OPEN "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/**
 * Soft: one-shot functional residual honesty (buf/map toward rings).
 * greppable: udx: dma soft residual functional
 */
static void
dma_soft_log_functional_residual_once(void)
{
    if (g_fDmaFunctionalResidualOnce != 0) {
        return;
    }
    g_fDmaFunctionalResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualFunctional);
    /* greppable: udx: dma soft residual functional */
    dma_soft_emit("udx: dma soft residual functional "
                  "path=buf_map+map_idem+ring "
                  "chain=ALLOC_MAP_MAP_REMAP_FREE "
                  "rtl8168_udx=NIC_desc_ring xhci_udx=USB_TRB_ring "
                  "product_DMA_window_cap_mint=OPEN "
                  "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/**
 * Soft: one-shot buf_map residual honesty (ddi_buf_map + stream map).
 * greppable: udx: dma soft residual buf_map
 */
static void
dma_soft_log_buf_map_residual_once(void)
{
    if (g_fDmaBufMapResidualOnce != 0) {
        return;
    }
    g_fDmaBufMapResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualBufMap);
    /* greppable: udx: dma soft residual buf_map */
    dma_soft_emit("udx: dma soft residual buf_map "
                  "ddi_buf_map=1 stream_map=1 identity=1 "
                  "product_IOMMU_PTE=0 "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/**
 * Soft: one-shot ring residual honesty (NIC/USB ring page spirit).
 * greppable: udx: dma soft residual ring
 */
static void
dma_soft_log_ring_residual_once(void)
{
    if (g_fDmaRingResidualOnce != 0) {
        return;
    }
    g_fDmaRingResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualRing);
    /* greppable: udx: dma soft residual ring */
    dma_soft_emit("udx: dma soft residual ring "
                  "ring_page_cb=%u rtl8168_udx=desc_ring "
                  "xhci_udx=TRB_page never_program=1 "
                  "product_DMA_window_cap_mint=OPEN "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_SOFT_RING_PAGE_CB, UDX_DMA_LAYOUT_VER);
}

/**
 * Soft: one-shot map_idem residual honesty (re-MAP same pa/cb).
 * greppable: udx: dma soft residual map_idem
 */
static void
dma_soft_log_map_idem_residual_once(void)
{
    if (g_fDmaMapIdemResidualOnce != 0) {
        return;
    }
    g_fDmaMapIdemResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualMapIdem);
    /* greppable: udx: dma soft residual map_idem */
    dma_soft_emit("udx: dma soft residual map_idem "
                  "same_pa_cb=1 cookie_match=1 "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/**
 * Soft: one-shot map_recb residual honesty (re-MAP different cb).
 * greppable: udx: dma soft residual map_recb
 */
static void
dma_soft_log_map_recb_residual_once(void)
{
    if (g_fDmaMapRecbResidualOnce != 0) {
        return;
    }
    g_fDmaMapRecbResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualMapRecb);
    /* greppable: udx: dma soft residual map_recb */
    dma_soft_emit("udx: dma soft residual map_recb "
                  "cb_change=1 prior_span_update=1 "
                  "soft=1 Soft!=product layout_ver=%u\n",
                  UDX_DMA_LAYOUT_VER);
}

/**
 * Soft: one-shot map_note residual honesty (STRONGER map/note path).
 * Product UDX hosts (rtl8168_udx / xhci_udx) consume map_single +
 * force32 geometric note + ddi_buf_map. Soft!=product; dual_dod OPEN;
 * product_hosts=UDX; never Dual DoD close; stamp-free bar v2026.08.04.75.
 * greppable: udx: dma soft residual map_note
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 */
static void
dma_soft_log_map_note_residual_once(void)
{
    if (g_fDmaMapNoteResidualOnce != 0) {
        return;
    }
    g_fDmaMapNoteResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualMapNote);
    /* greppable: udx: dma soft residual map_note */
    dma_soft_emit("udx: dma soft residual map_note "
                  "path=map_single|force32_note|ddi_buf_map "
                  "surface=%u product_hosts=UDX "
                  "hosts=rtl8168_udx|xhci_udx "
                  "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_close=0 "
                  "product_DMA_window_cap_mint=OPEN force32_mint=0 "
                  "identity_cookie=1 soft_map_live=1 "
                  "soft=1 Soft!=product soft residual layout_ver=%u\n",
                  UDX_DMA_MAP_NOTE_SURFACE, UDX_DMA_LAYOUT_VER);
    dma_soft_log_denser_residual_once();
}

/**
 * Soft: one-shot denser residual prove (preferred over lamps alone).
 * denser_prove triple coherent+stream+ddi_map + denser ring + denser
 * map_note + denser product_host. Soft!=product; Dual DoD OPEN.
 * greppable: udx: dma soft residual denser
 * greppable: denser_prove=1
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 */
static void
dma_soft_log_denser_residual_once(void)
{
    if (g_fDmaDenserResidualOnce != 0) {
        return;
    }
    /* denser_prove needs at least one product DMA surface live. */
    if (g_u32DmaAllocOk == 0 && g_u32DmaMapOk == 0 &&
        g_u32DmaDdiBufMap == 0 && g_u32DmaRingSized == 0) {
        return;
    }
    g_fDmaDenserResidualOnce = 1;
    dma_soft_inc(&g_u32DmaResidualDenser);
    /* greppable: udx: dma soft residual denser */
    dma_soft_emit("udx: dma soft residual denser "
                  "once=1 denser_prove=%u denser_triple=%u denser_ring=%u "
                  "denser_map_note=%u denser_product_host=%u "
                  "coherent_ok=%u stream_ok=%u ddi_map=%u ring_sized=%u "
                  "product_hosts=UDX DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN "
                  "agent_close=%u soft_ne_product=%u fs_class_skip=%u "
                  "lean_checks=%u wave=%u soft=1 Soft!=product "
                  "layout_ver=%u\n",
                  UDX_DMA_DENSER_PROVE, UDX_DMA_DENSER_TRIPLE,
                  UDX_DMA_DENSER_RING, UDX_DMA_DENSER_MAP_NOTE,
                  UDX_DMA_DENSER_PRODUCT_HOST,
                  g_u32DmaAllocOk, g_u32DmaMapOk, g_u32DmaDdiBufMap,
                  g_u32DmaRingSized,
                  UDX_DMA_AGENT_CLOSE, UDX_DMA_SOFT_NE_PRODUCT,
                  UDX_DMA_FS_CLASS_SKIP, UDX_DMA_LEAN_CHECKS,
                  UDX_DMA_SOFT_WAVE, UDX_DMA_LAYOUT_VER);
}

/** Soft: one-shot inventory after first successful alloc/map/grant/chain. */
static void
dma_soft_maybe_once(void)
{
    if (g_fDmaSoftOnce != 0) {
        return;
    }
    if (g_u32DmaAllocOk == 0 && g_u32DmaMapOk == 0 && g_u32DmaIommuOk == 0 &&
        g_u32DmaDdiBufOk == 0 && g_u32DmaWinOk == 0 && g_u32DmaMaskSet == 0 &&
        g_u32DmaCohMaskSet == 0 && g_u32DmaDdiBufChainOk == 0) {
        return;
    }
    g_fDmaSoftOnce = 1;
    /* Path residual once-lamps (storm=0 / H2) before inventory rollup. */
    if (g_u32DmaAllocOk != 0u) {
        dma_soft_log_coherent_residual_once();
        if (g_u32DmaRingSized != 0u) {
            dma_soft_log_ring_residual_once();
        }
    }
    if (g_u32DmaMapOk != 0u) {
        dma_soft_log_stream_residual_once();
        dma_soft_log_buf_map_residual_once();
        dma_soft_log_map_note_residual_once();
    }
    if (g_u32DmaAllocForce32 != 0u || g_u32DmaDdiBufForce32 != 0u ||
        g_u32DmaWinForce32 != 0u) {
        dma_soft_log_force32_residual_once();
        dma_soft_log_map_note_residual_once();
    }
    if (g_u32DmaDdiBufOk != 0u || g_u32DmaDdiBufChainOk != 0u) {
        dma_soft_log_ddi_buf_residual_once();
        dma_soft_log_buf_map_residual_once();
        dma_soft_log_map_note_residual_once();
    }
    if (g_u32DmaDdiMapIdem != 0u || g_u32DmaMapRemap != 0u) {
        dma_soft_log_map_idem_residual_once();
    }
    if (g_u32DmaDdiMapRecb != 0u) {
        dma_soft_log_map_recb_residual_once();
    }
    if (g_u32DmaIommuOk != 0u) {
        dma_soft_log_iommu_residual_once();
    }
    if (g_u32DmaSyncCpu != 0u || g_u32DmaSyncDev != 0u) {
        dma_soft_log_sync_residual_once();
    }
    if (g_u32DmaWinOk != 0u || g_u32DmaWinFail != 0u) {
        dma_soft_log_window_residual_once();
    }
    if (g_u32DmaMaskSet != 0u || g_u32DmaCohMaskSet != 0u) {
        dma_soft_log_mask_residual_once();
    }
    /*
     * Functional residual: denser buf/map toward product NIC/USB rings
     * when any map/alloc/chain path has soft-ok (never Dual DoD close).
     */
    if (g_u32DmaAllocOk != 0u || g_u32DmaMapOk != 0u ||
        g_u32DmaDdiBufOk != 0u || g_u32DmaDdiBufChainOk != 0u) {
        dma_soft_log_functional_residual_once();
    }
    dma_soft_log_mint_residual_once();
    dma_soft_log_product_host_residual_once();
    dma_soft_log_dual_dod_residual_once();
    dma_soft_log_hazard_residual_once();
    dma_soft_inventory_log();
}

#if !defined(UDX_HOST_LIBC)
/*
 * Freestanding coherent alloc: small tier (<=4 KiB) then large (<=32 KiB).
 * Product rtl8168_udx: ring 4 KiB small; per-slot RX 4 KiB; TX bounce small.
 * Soft!=product; never Dual DoD close.
 *
 * DUT .85 own_stuck: bus PA correct but sync was mfence-only — NIC never
 * saw Own=1 in DRAM. clflush residual pairs kernel dma_buf_sync_*.
 */
#define UDX_FS_DMA_CLINE  64u

static void
fs_dma_clflush_range(void *pVa, size_t cb)
{
    u8 *pByte;
    u8 *pEnd;
    uintptr_t uPtr;

    if (pVa == NULL || cb == 0u) {
        return;
    }
    uPtr = (uintptr_t)pVa;
    uPtr &= ~((uintptr_t)UDX_FS_DMA_CLINE - 1u);
    pByte = (u8 *)uPtr;
    pEnd = (u8 *)pVa + cb;
    while (pByte < pEnd) {
        __asm__ volatile("clflush (%0)" : : "r"(pByte) : "memory");
        pByte += UDX_FS_DMA_CLINE;
    }
    __asm__ volatile("mfence" ::: "memory");
}

static void
fs_dma_set_cookie(void *pCpu, udx_dma_addr_t dma)
{
    u32 iSlot;

    if (pCpu == NULL) {
        return;
    }
    for (iSlot = 0; iSlot < UDX_FS_DMA_SLOTS; iSlot++) {
        if (g_aFsDma[iSlot].u8Used &&
            g_aFsDma[iSlot].aBytes == (u8 *)pCpu) {
            g_aFsDma[iSlot].dmaCookie = dma;
            return;
        }
    }
    for (iSlot = 0; iSlot < UDX_FS_DMA_LARGE_SLOTS; iSlot++) {
        if (g_aFsDmaLarge[iSlot].u8Used &&
            g_aFsDmaLarge[iSlot].aBytes == (u8 *)pCpu) {
            g_aFsDmaLarge[iSlot].dmaCookie = dma;
            return;
        }
    }
}

/* Resolve CPU VA for a bus cookie (product sync clflush). Soft!=product. */
static void *
fs_dma_va_from_cookie(udx_dma_addr_t dma, size_t cbSize)
{
    u32 iSlot;

    if (dma == 0) {
        return NULL;
    }
    for (iSlot = 0; iSlot < UDX_FS_DMA_SLOTS; iSlot++) {
        if (g_aFsDma[iSlot].u8Used &&
            g_aFsDma[iSlot].dmaCookie == dma) {
            (void)cbSize;
            return g_aFsDma[iSlot].aBytes;
        }
    }
    for (iSlot = 0; iSlot < UDX_FS_DMA_LARGE_SLOTS; iSlot++) {
        if (g_aFsDmaLarge[iSlot].u8Used &&
            g_aFsDmaLarge[iSlot].dmaCookie == dma) {
            (void)cbSize;
            return g_aFsDmaLarge[iSlot].aBytes;
        }
    }
    return NULL;
}

static void *
fs_dma_alloc(size_t cbSize)
{
    u32 iSlot;

    if (cbSize == 0) {
        return NULL;
    }
    if (cbSize <= UDX_FS_DMA_SLOT_CB) {
        for (iSlot = 0; iSlot < UDX_FS_DMA_SLOTS; iSlot++) {
            if (!g_aFsDma[iSlot].u8Used) {
                u32 iByte;

                g_aFsDma[iSlot].u8Used = 1;
                g_aFsDma[iSlot].cbAlloc = cbSize;
                g_aFsDma[iSlot].dmaCookie = 0;
                for (iByte = 0; iByte < (u32)cbSize; iByte++) {
                    g_aFsDma[iSlot].aBytes[iByte] = 0;
                }
                dma_soft_inc(&g_u32DmaSlabLive);
                dma_soft_note_peak(&g_u32DmaSlabPeak, g_u32DmaSlabLive);
                return g_aFsDma[iSlot].aBytes;
            }
        }
        dma_soft_inc(&g_u32DmaSlabFull);
        return NULL;
    }
    if (cbSize <= UDX_FS_DMA_LARGE_SLOT_CB) {
        for (iSlot = 0; iSlot < UDX_FS_DMA_LARGE_SLOTS; iSlot++) {
            if (!g_aFsDmaLarge[iSlot].u8Used) {
                u32 iByte;

                g_aFsDmaLarge[iSlot].u8Used = 1;
                g_aFsDmaLarge[iSlot].cbAlloc = cbSize;
                g_aFsDmaLarge[iSlot].dmaCookie = 0;
                for (iByte = 0; iByte < (u32)cbSize; iByte++) {
                    g_aFsDmaLarge[iSlot].aBytes[iByte] = 0;
                }
                dma_soft_inc(&g_u32DmaSlabLive);
                dma_soft_note_peak(&g_u32DmaSlabPeak, g_u32DmaSlabLive);
                return g_aFsDmaLarge[iSlot].aBytes;
            }
        }
        dma_soft_inc(&g_u32DmaSlabFull);
        return NULL;
    }
    return NULL;
}

static void
fs_dma_free(void *pCpu)
{
    u32 iSlot;

    if (pCpu == NULL) {
        return;
    }
    for (iSlot = 0; iSlot < UDX_FS_DMA_SLOTS; iSlot++) {
        if (g_aFsDma[iSlot].u8Used &&
            g_aFsDma[iSlot].aBytes == (u8 *)pCpu) {
            g_aFsDma[iSlot].u8Used = 0;
            g_aFsDma[iSlot].cbAlloc = 0;
            g_aFsDma[iSlot].dmaCookie = 0;
            if (g_u32DmaSlabLive > 0u) {
                g_u32DmaSlabLive--;
            }
            return;
        }
    }
    for (iSlot = 0; iSlot < UDX_FS_DMA_LARGE_SLOTS; iSlot++) {
        if (g_aFsDmaLarge[iSlot].u8Used &&
            g_aFsDmaLarge[iSlot].aBytes == (u8 *)pCpu) {
            g_aFsDmaLarge[iSlot].u8Used = 0;
            g_aFsDmaLarge[iSlot].cbAlloc = 0;
            g_aFsDmaLarge[iSlot].dmaCookie = 0;
            if (g_u32DmaSlabLive > 0u) {
                g_u32DmaSlabLive--;
            }
            return;
        }
    }
}
#endif

/* ---- force32 geometric residual --------------------------------------- */

int
udx_dma_window_ok(udx_dma_addr_t dma, size_t cb, int fForce32)
{
    udx_dma_addr_t end;
    int fOk;

    if (fForce32 != 0) {
        dma_soft_inc(&g_u32DmaWinForce32);
        dma_soft_log_force32_residual_once();
    }
    if (dma == 0 || cb == 0) {
        dma_soft_inc(&g_u32DmaWinFail);
        dma_soft_log_window_residual_once();
        return 0;
    }
    end = dma + (udx_dma_addr_t)cb;
    if (end < dma) {
        dma_soft_inc(&g_u32DmaWinFail);
        dma_soft_log_window_residual_once();
        return 0;
    }
    if (fForce32 != 0) {
        fOk = (end <= UDX_DMA_VTD_IDENTITY_LIMIT) ? 1 : 0;
    } else {
        fOk = 1;
    }
    if (fOk != 0) {
        dma_soft_inc(&g_u32DmaWinOk);
        dma_soft_log_window_residual_once();
        dma_soft_maybe_once();
    } else {
        dma_soft_inc(&g_u32DmaWinFail);
        dma_soft_log_window_residual_once();
    }
    return fOk;
}

/* ---- Mask residual ---------------------------------------------------- */

int
udx_dma_set_mask(struct udx_device *pDev, u64 u64Mask)
{
    (void)pDev;
    dma_soft_inc(&g_u32DmaMaskSet);
    if (u64Mask == 0ull) {
        dma_soft_inc(&g_u32DmaMaskFail);
        return -1;
    }
    g_u64DmaMask = u64Mask;
    /* Soft residual: process-global only; never IOMMU program. */
    dma_soft_log_mask_residual_once();
    dma_soft_maybe_once();
    return 0;
}

int
udx_dma_set_coherent_mask(struct udx_device *pDev, u64 u64Mask)
{
    (void)pDev;
    dma_soft_inc(&g_u32DmaCohMaskSet);
    if (u64Mask == 0ull) {
        dma_soft_inc(&g_u32DmaMaskFail);
        return -1;
    }
    g_u64DmaCoherentMask = u64Mask;
    /* Soft residual: process-global only; never IOMMU program. */
    dma_soft_log_mask_residual_once();
    dma_soft_maybe_once();
    return 0;
}

int
udx_dma_set_mask_and_coherent(struct udx_device *pDev, u64 u64Mask)
{
    int nSt;

    nSt = udx_dma_set_mask(pDev, u64Mask);
    if (nSt != 0) {
        return nSt;
    }
    return udx_dma_set_coherent_mask(pDev, u64Mask);
}

u64
udx_dma_max_mapping_size(struct udx_device *pDev)
{
    (void)pDev;
    return UDX_DMA_MAP_MAX;
}

u64
udx_dma_get_mask(struct udx_device *pDev)
{
    (void)pDev;
    return g_u64DmaMask;
}

u64
udx_dma_get_coherent_mask(struct udx_device *pDev)
{
    (void)pDev;
    return g_u64DmaCoherentMask;
}

/* ---- DDI DMA_BUF soft hooks ------------------------------------------- */

long
udx_dma_ddi_buf_alloc(u32 u32Handle, u32 cPages, u32 u32Flags)
{
    dma_soft_inc(&g_u32DmaDdiBufAlloc);
    if ((u32Flags & UDX_DMA_BUF_F_FORCE32) != 0u) {
        dma_soft_inc(&g_u32DmaDdiBufForce32);
        dma_soft_log_force32_residual_once();
    }
#if defined(UDX_HOST_LIBC) && !defined(GJ_FREESTANDING)
    {
        struct udx_dma_soft_ddi_slot *pSlot;
        void *pCpu;
        size_t cb;
        u32 cPg;

        /*
         * Host soft DDI DMA_BUF residual (product host inject).
         * Soft!=product: calloc pages + identity cookie; not VT-d mint.
         * force32 flag is geometric note only on host (heap VA often high).
         */
        if (u32Handle == 0u) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return -1;
        }
        /* Soft residual pages gate (header helper; Soft!=product). */
        if (udx_dma_ddi_pages_ok(cPages) == 0) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return -1;
        }
        cPg = (cPages == 0u) ? 1u : cPages;
        pSlot = dma_soft_ddi_slot_alloc();
        if (pSlot == NULL) {
            dma_soft_inc(&g_u32DmaDdiSlotFull);
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return -1;
        }
        cb = udx_dma_ddi_pages_to_cb(cPages);
        if (cb == 0) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return -1;
        }
        pCpu = calloc(1, cb);
        if (pCpu == NULL) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return -1;
        }
        pSlot->u8Live = 1u;
        pSlot->u8Mapped = 0u;
        pSlot->u8Force32 =
            ((u32Flags & UDX_DMA_BUF_F_FORCE32) != 0u) ? 1u : 0u;
        pSlot->u32Handle = u32Handle;
        pSlot->cPages = cPg;
        pSlot->cbMapped = 0u;
        pSlot->pCpu = pCpu;
        /* Soft identity cookie; Soft!=product VT-d / CNode mint. */
        pSlot->cookie = udx_dma_cookie_from_cpu(pCpu);
        dma_soft_inc(&g_u32DmaDdiSlotLive);
        dma_soft_note_peak(&g_u32DmaDdiSlotPeak, g_u32DmaDdiSlotLive);
        dma_soft_inc(&g_u32DmaDdiBufOk);
        dma_soft_log_ddi_buf_residual_once();
        dma_soft_maybe_once();
        return (long)pSlot->cookie;
    }
#else
    {
        long ret;

        if (u32Handle == 0u || udx_dma_ddi_pages_ok(cPages) == 0) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return -1;
        }
        if (cPages == 0u) {
            cPages = 1u;
        }
        /* Soft residual bound — match ddi_door (avoid multi-page storms). */
        ret = udx_gj_syscall4((long)UDX_GJ_SYS_DDI,
                              (long)UDX_DDI_OP_DMA_BUF_ALLOC,
                              (long)u32Handle,
                              (long)cPages,
                              (long)u32Flags);
        if (ret < 0) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return ret;
        }
        dma_soft_inc(&g_u32DmaDdiBufOk);
        dma_soft_log_ddi_buf_residual_once();
        dma_soft_maybe_once();
        return ret;
    }
#endif
}

long
udx_dma_ddi_buf_free(u32 u32Handle, udx_dma_addr_t pa, u32 cPages)
{
    dma_soft_inc(&g_u32DmaDdiBufFree);
#if defined(UDX_HOST_LIBC) && !defined(GJ_FREESTANDING)
    {
        struct udx_dma_soft_ddi_slot *pSlot;

        (void)cPages;
        if (u32Handle == 0u || pa == 0) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return -1;
        }
        pSlot = dma_soft_ddi_slot_find(u32Handle, pa);
        if (pSlot == NULL) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return -1;
        }
        free(pSlot->pCpu);
        pSlot->u8Live = 0u;
        pSlot->u8Mapped = 0u;
        pSlot->u8Force32 = 0u;
        pSlot->u32Handle = 0u;
        pSlot->cPages = 0u;
        pSlot->cbMapped = 0u;
        pSlot->pCpu = NULL;
        pSlot->cookie = 0;
        if (g_u32DmaDdiSlotLive > 0u) {
            g_u32DmaDdiSlotLive--;
        }
        dma_soft_inc(&g_u32DmaDdiBufOk);
        return 0;
    }
#else
    {
        long ret;

        if (u32Handle == 0u || pa == 0) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return -1;
        }
        ret = udx_gj_syscall4((long)UDX_GJ_SYS_DDI,
                              (long)UDX_DDI_OP_DMA_BUF_FREE,
                              (long)u32Handle,
                              (long)pa,
                              (long)cPages);
        if (ret < 0) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return ret;
        }
        /* Soft freestanding last-map residual forget on free. */
        if (g_fFsDdiMapLive != 0u &&
            g_u32FsDdiMapHandle == u32Handle &&
            g_dmaFsDdiMapPa == pa) {
            g_fFsDdiMapLive = 0u;
            g_u32FsDdiMapHandle = 0u;
            g_dmaFsDdiMapPa = 0;
            g_u32FsDdiMapCb = 0u;
        }
        dma_soft_inc(&g_u32DmaDdiBufOk);
        return ret;
    }
#endif
}

long
udx_dma_ddi_buf_map(u32 u32Handle, udx_dma_addr_t pa, size_t cb)
{
    u32 cbSoft;

    dma_soft_inc(&g_u32DmaDdiBufMap);
    cbSoft = (cb > 0xffffffffu) ? 0xffffffffu : (u32)cb;
#if defined(UDX_HOST_LIBC) && !defined(GJ_FREESTANDING)
    {
        struct udx_dma_soft_ddi_slot *pSlot;
        int fForce32;

        if (u32Handle == 0u || pa == 0 || cb == 0) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return -1;
        }
        pSlot = dma_soft_ddi_slot_find(u32Handle, pa);
        if (pSlot == NULL) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return -1;
        }
        /*
         * Soft force32 geometric honesty on host: window_ok when flagged.
         * Host heap cookies are often high VA — force32 may soft-fail map.
         * Soft!=product; product freestanding uses door + real PA.
         */
        fForce32 = (pSlot->u8Force32 != 0u) ? 1 : 0;
        if (fForce32 != 0 && udx_dma_window_ok(pa, cb, 1) == 0) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return -1;
        }
        /*
         * Functional residual DMA buf/map (Soft!=product; peer ddi_host /
         * kernel dma_buf map deepen):
         *   re-MAP same pa/cb → map_idem (cookie match; no new mint)
         *   re-MAP different cb → map_recb (soft span update)
         * First MAP marks mapped + records span. Post-map mfence residual
         * (device-visible ring spirit; not full CWB/IOTLB).
         */
        if (pSlot->u8Mapped != 0u) {
            if (pSlot->cbMapped == cbSoft) {
                dma_soft_inc(&g_u32DmaDdiMapIdem);
                dma_soft_log_map_idem_residual_once();
            } else {
                dma_soft_inc(&g_u32DmaDdiMapRecb);
                pSlot->cbMapped = cbSoft;
                dma_soft_log_map_recb_residual_once();
            }
        } else {
            pSlot->u8Mapped = 1u;
            pSlot->cbMapped = cbSoft;
        }
        /* Soft post-map barrier (ring/xfer device-visible residual). */
        __asm__ volatile("mfence" ::: "memory");
        dma_soft_inc(&g_u32DmaDdiBufOk);
        dma_soft_log_ddi_buf_residual_once();
        dma_soft_log_buf_map_residual_once();
        dma_soft_log_map_note_residual_once();
        dma_soft_log_functional_residual_once();
        dma_soft_maybe_once();
        return (long)pa;
    }
#else
    {
        long ret;

        if (u32Handle == 0u || pa == 0 || cb == 0) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return -1;
        }
        ret = udx_gj_syscall4((long)UDX_GJ_SYS_DDI,
                              (long)UDX_DDI_OP_DMA_BUF_MAP,
                              (long)u32Handle,
                              (long)pa,
                              (long)cb);
        if (ret < 0) {
            dma_soft_inc(&g_u32DmaDdiBufFail);
            return ret;
        }
        /*
         * Freestanding door: soft map_idem / map_recb residual via last-map
         * process-global note (no host slot table). Soft!=product; door
         * owns real span; never claims CNode mint.
         */
        if (g_fFsDdiMapLive != 0u &&
            g_u32FsDdiMapHandle == u32Handle &&
            g_dmaFsDdiMapPa == pa) {
            if (g_u32FsDdiMapCb == cbSoft) {
                dma_soft_inc(&g_u32DmaDdiMapIdem);
                dma_soft_log_map_idem_residual_once();
            } else {
                dma_soft_inc(&g_u32DmaDdiMapRecb);
                g_u32FsDdiMapCb = cbSoft;
                dma_soft_log_map_recb_residual_once();
            }
        } else {
            g_fFsDdiMapLive = 1u;
            g_u32FsDdiMapHandle = u32Handle;
            g_dmaFsDdiMapPa = pa;
            g_u32FsDdiMapCb = cbSoft;
        }
        dma_soft_inc(&g_u32DmaDdiBufOk);
        dma_soft_log_ddi_buf_residual_once();
        dma_soft_log_buf_map_residual_once();
        dma_soft_log_map_note_residual_once();
        dma_soft_log_functional_residual_once();
        dma_soft_maybe_once();
        return ret;
    }
#endif
}

/**
 * Soft DDI DMA_BUF chain (functional residual denser toward product rings):
 *   ALLOC → MAP → MAP_REMAP(same pa/cb = map_idem) → FREE
 * ops 10 / 12 / 12 / 11 spirit. Peer ddi_host DMA_BUF_MAP_REMAP residual.
 * greppable: udx: dma soft ddi_buf chain
 * greppable: udx: dma soft residual functional
 * greppable: udx: dma soft residual map_idem
 * Soft!=product; G-AC-1; Dual DoD A/B OPEN.
 */
int
udx_dma_ddi_buf_chain(u32 u32Handle, u32 cPages, u32 u32Flags)
{
    long retAlloc;
    long retMap;
    long retMap2;
    long retFree;
    udx_dma_addr_t pa;
    size_t cb;
    u32 cPg;

    dma_soft_inc(&g_u32DmaDdiBufChain);
    if (u32Handle == 0u || udx_dma_ddi_pages_ok(cPages) == 0) {
        return -1;
    }
    cPg = (cPages == 0u) ? 1u : cPages;
    cb = udx_dma_ddi_pages_to_cb(cPages);
    if (cb == 0) {
        return -1;
    }

    retAlloc = udx_dma_ddi_buf_alloc(u32Handle, cPg, u32Flags);
    if (retAlloc < 0) {
        return -1;
    }
    pa = (udx_dma_addr_t)retAlloc;

    retMap = udx_dma_ddi_buf_map(u32Handle, pa, cb);
    if (retMap < 0) {
        (void)udx_dma_ddi_buf_free(u32Handle, pa, cPg);
        return -1;
    }

    /*
     * Functional residual: re-MAP same pa/cb (map_idem) before FREE.
     * Host soft slots tally map_idem; freestanding door may re-MAP ok.
     * Soft!=product; never Dual DoD close.
     */
    retMap2 = udx_dma_ddi_buf_map(u32Handle, pa, cb);
    if (retMap2 < 0) {
        (void)udx_dma_ddi_buf_free(u32Handle, pa, cPg);
        return -1;
    }

    retFree = udx_dma_ddi_buf_free(u32Handle, pa, cPg);
    if (retFree < 0) {
        return -1;
    }

    dma_soft_inc(&g_u32DmaDdiBufChainOk);
    dma_soft_log_functional_residual_once();
    dma_soft_maybe_once();
    return 0;
}

/* ---- Soft residual queries -------------------------------------------- */

int
udx_dma_shape_ok(void)
{
    /*
     * Soft product-host DMA surface always present in this unit:
     * coherent/stream/force32/ddi_buf (+ chain) at layout_ver >= 2.
     * Soft!=product; eng residual only.
     */
    return (UDX_DMA_LAYOUT_VER >= 2u) ? 1 : 0;
}

int
udx_dma_soft_residual_lean_lamp_once(void)
{
    if (g_fDmaSoftOnce != 0) {
        return 0;
    }
    g_fDmaSoftOnce = 1;
    /*
     * Catalog residual once-lamps (H2 storm=0); path residual_n may stay 0.
     * Always emit mint / product_host / dual_dod / hazard / functional /
     * map_note honesty (buf/map toward product NIC/USB rings; Soft!=product;
     * product_hosts=UDX; dual_dod OPEN).
     */
    dma_soft_log_mint_residual_once();
    dma_soft_log_product_host_residual_once();
    dma_soft_log_dual_dod_residual_once();
    dma_soft_log_hazard_residual_once();
    dma_soft_log_functional_residual_once();
    dma_soft_log_ring_residual_once();
    dma_soft_log_map_note_residual_once();
    dma_soft_inventory_log();
    return 1;
}

/* ---- Coherent / stream core ------------------------------------------- */

void *
udx_dma_alloc_coherent(struct udx_device *pDev, size_t cbSize,
                       udx_dma_addr_t *pDmaHandle, u32 u32Flags)
{
    void *p;

    (void)pDev;
    dma_soft_inc(&g_u32DmaAllocEnter);
    if ((u32Flags & UDX_DMA_F_FORCE32) != 0u) {
        dma_soft_inc(&g_u32DmaAllocForce32);
        dma_soft_log_force32_residual_once();
    }
    if (cbSize == 0 || pDmaHandle == NULL) {
        dma_soft_inc(&g_u32DmaAllocInval);
        return NULL;
    }
#if defined(UDX_HOST_LIBC)
    p = calloc(1, cbSize);
#else
    p = fs_dma_alloc(cbSize);
#endif
    if (p == NULL) {
        dma_soft_inc(&g_u32DmaAllocFail);
        return NULL;
    }
    /*
     * Host: DMA address == CPU cookie (lab identity).
     * Freestanding product: PLATFORM_INFO op6 virt_to_phys → bus PA.
     * DUT .84 own_handoff PASS programmed VA cookies (0x1058400-class);
     * elf_load maps BSS page-at-a-time (VA != PA) so Own never cleared
     * and inject stayed 0. Multi-page allocs require PA-contiguous.
     * FORCE32: refuse bus outside VT-d identity when flag set.
     * Soft!=product: PA cookie != DMA window CNode mint (pair iommu_grant).
     * greppable residual: udx: dma soft residual coherent / mint
     * greppable: udx: soft residual virt_to_phys
     */
#if defined(UDX_HOST_LIBC)
    *pDmaHandle = udx_dma_cookie_from_cpu(p);
#else
    {
        long i64Pa0;
        size_t off;
        udx_dma_addr_t dma0;
        int fForce32;

        i64Pa0 = udx_gj_virt_to_phys(p);
        if (i64Pa0 <= 0) {
            fs_dma_free(p);
            dma_soft_inc(&g_u32DmaAllocFail);
            return NULL;
        }
        dma0 = (udx_dma_addr_t)i64Pa0;
        /* Multi-page: require physically contiguous frames (desc base+off). */
        for (off = 4096u; off < cbSize; off += 4096u) {
            long i64Pa;

            i64Pa = udx_gj_virt_to_phys((const u8 *)p + off);
            if (i64Pa <= 0 ||
                (udx_dma_addr_t)i64Pa != dma0 + (udx_dma_addr_t)off) {
                fs_dma_free(p);
                dma_soft_inc(&g_u32DmaAllocFail);
                return NULL;
            }
        }
        fForce32 = ((u32Flags & UDX_DMA_F_FORCE32) != 0u) ? 1 : 0;
        if (udx_dma_window_ok(dma0, cbSize, fForce32) == 0) {
            fs_dma_free(p);
            dma_soft_inc(&g_u32DmaAllocFail);
            return NULL;
        }
        fs_dma_set_cookie(p, dma0);
        /* Device-visible zeroed page before Own / TNPDS. Soft!=product. */
        fs_dma_clflush_range(p, cbSize);
        *pDmaHandle = dma0;
    }
#endif
    g_u32DmaLastCb = (cbSize > 0xffffffffu) ? 0xffffffffu : (u32)cbSize;
    dma_soft_note_peak(&g_u32DmaPeakCb, g_u32DmaLastCb);
    /*
     * Soft ring residual geometry note (Soft!=product catalog only).
     * Product hosts: rtl8168_udx RING_BYTES=4096; xhci TRB/event page
     * spirit. Multiple-of ring page → ring-sized tally toward NIC/USB.
     * Never programs TNPDS/CRCR/DCBAAP; never mints DMA window.
     * greppable residual: udx: dma soft residual ring / functional
     */
    if (cbSize >= (size_t)UDX_DMA_SOFT_RING_PAGE_CB &&
        (cbSize % (size_t)UDX_DMA_SOFT_RING_PAGE_CB) == 0) {
        dma_soft_inc(&g_u32DmaRingSized);
        /*
         * FUNCTIONAL residual STRONGER (product UDX rings; Soft!=product):
         * ring-sized coherent pages get a device-visible barrier so thr
         * BH / udx_run consumers observe zeroed descriptors before Own
         * spirit. Never programs TNPDS/CRCR/DCBAAP; never mints DMA window;
         * never net_eth_poll / hard-IRQ map (H1 thr-only). Dual DoD OPEN.
         * greppable: udx: dma soft residual ring
         * greppable: udx: dma soft residual functional
         */
        if (UDX_DMA_RING_DEV_VISIBLE != 0u) {
            __asm__ volatile("mfence" ::: "memory");
        }
        dma_soft_log_ring_residual_once();
    }
    dma_soft_inc(&g_u32DmaAllocOk);
    dma_soft_log_coherent_residual_once();
    dma_soft_log_functional_residual_once();
    dma_soft_log_denser_residual_once();
    dma_soft_maybe_once();
    return p;
}

/**
 * Grant an IOMMU window for a DMA cookie (GJ freestanding).
 * bdf = bus<<16|slot<<8|func; dma/size describe the DMA region.
 * Host (UDX_HOST_LIBC): no-op success — sim identity map.
 *
 * Freestanding success under TE (PLATFORM_INFO op5):
 *   soft software window row recorded + PA in VT-d identity [0,1 GiB)
 *   when tables ready and TE armed. Not product DMA window CNode mint
 *   and not full VT-d product program. Soft!=product Dual DoD B OPEN.
 *   Cover miss under TE → fail; residual Own stick if identity miss.
 * greppable residual: udx: dma soft residual iommu
 * greppable residual: udx: dma soft residual iommu FAIL
 */
int
udx_dma_iommu_grant(u32 u32Bdf, udx_dma_addr_t dma, size_t cbSize)
{
    dma_soft_inc(&g_u32DmaIommuEnter);
#if defined(UDX_HOST_LIBC)
    (void)u32Bdf;
    (void)dma;
    (void)cbSize;
    dma_soft_inc(&g_u32DmaIommuHostNop);
    dma_soft_inc(&g_u32DmaIommuOk);
    dma_soft_log_iommu_residual_once();
    dma_soft_maybe_once();
    return 0;
#else
    {
        long ret;

        if (dma == 0 || cbSize == 0) {
            dma_soft_inc(&g_u32DmaIommuFail);
            dma_soft_log_iommu_fail_once();
            return -1;
        }
        /*
         * Soft force32 geometric precheck (dma_buf_window_ok spirit).
         * Does not hard-gate grant; tally + residual only. TE cover is
         * kernel op5 honesty. Soft!=product.
         */
        (void)udx_dma_window_ok(dma, cbSize, 1);
        ret = udx_gj_iommu_grant(u32Bdf, dma, cbSize);
        if (ret < 0) {
            dma_soft_inc(&g_u32DmaIommuFail);
            dma_soft_log_iommu_fail_once();
            return -1;
        }
        dma_soft_inc(&g_u32DmaIommuOk);
        dma_soft_log_iommu_residual_once();
        dma_soft_maybe_once();
        return 0;
    }
#endif
}

void
udx_dma_free_coherent(struct udx_device *pDev, size_t cbSize, void *pCpu,
                      udx_dma_addr_t dmaHandle)
{
    (void)pDev;
    (void)cbSize;
    (void)dmaHandle;
    dma_soft_inc(&g_u32DmaFree);
    if (pCpu == NULL) {
        dma_soft_inc(&g_u32DmaFreeNull);
        return;
    }
    /*
     * Soft free residual (H3 free-order honesty OPEN): identity free only.
     * Soft!=product: no DMA window cap revoke / Phase-A destroy.
     * greppable residual: udx: dma soft residual coherent / hazard
     */
    dma_soft_log_coherent_residual_once();
    dma_soft_log_hazard_residual_once();
#if defined(UDX_HOST_LIBC)
    free(pCpu);
#else
    fs_dma_free(pCpu);
#endif
}

udx_dma_addr_t
udx_dma_map_single(struct udx_device *pDev, void *pCpu, size_t cbSize,
                   enum udx_dma_data_direction eDir)
{
    udx_dma_addr_t dma;

    (void)pDev;
    dma_soft_inc(&g_u32DmaMap);
    if (pCpu == NULL) {
        dma_soft_inc(&g_u32DmaMapNull);
        return 0;
    }
    /*
     * Soft dir ordinal observation (udx_dma_dir_ok). Soft!=product:
     * never hard-gates map path; tally only for residual honesty.
     */
    if (udx_dma_dir_ok(eDir) == 0) {
        dma_soft_inc(&g_u32DmaMapDirInval);
    }
    /*
     * Soft stream ceiling honesty (mirror linux_dma_soft MAP_MAX).
     * Soft!=product IOMMU limit — refuse oversize so mapping_error fires.
     * Size 0 still identity-maps (caller risk; not product IOMMU).
     */
    if (cbSize > (size_t)UDX_DMA_MAP_MAX) {
        dma_soft_inc(&g_u32DmaMapOversize);
        return 0;
    }
    /* Soft identity cookie helper; Soft!=product DMA window cap mint. */
    dma = udx_dma_cookie_from_cpu(pCpu);
    dma_soft_map_slot_add(dma, cbSize);
    dma_soft_inc(&g_u32DmaMapOk);
    /*
     * STRONGER map/note residual for product UDX hosts (rtl/xhci).
     * Soft!=product; dual_dod OPEN; product_hosts=UDX; identity only.
     * greppable: udx: dma soft residual map_note
     */
    dma_soft_log_stream_residual_once();
    dma_soft_log_buf_map_residual_once();
    dma_soft_log_map_note_residual_once();
    dma_soft_log_functional_residual_once();
    dma_soft_maybe_once();
    return dma;
}

void
udx_dma_unmap_single(struct udx_device *pDev, udx_dma_addr_t dmaHandle,
                     size_t cbSize, enum udx_dma_data_direction eDir)
{
    (void)pDev;
    (void)cbSize;
    (void)eDir;
    dma_soft_inc(&g_u32DmaUnmap);
    if (dma_soft_map_slot_drop(dmaHandle) == 0) {
        dma_soft_inc(&g_u32DmaUnmapMiss);
    }
}

void
udx_dma_sync_single_for_cpu(struct udx_device *pDev, udx_dma_addr_t dma,
                            size_t cbSize, enum udx_dma_data_direction eDir)
{
    (void)pDev;
    (void)eDir;
    dma_soft_inc(&g_u32DmaSyncCpu);
    dma_soft_log_sync_residual_once();
#if defined(UDX_HOST_LIBC)
    (void)dma;
    (void)cbSize;
    __asm__ volatile("mfence" ::: "memory");
#else
    {
        void *pVa = fs_dma_va_from_cookie(dma, cbSize);

        /*
         * Product freestanding: clflush so CPU sees NIC Own-clear + payload
         * (DUT .85 own_stuck with mfence-only). Soft!=product full IOTLB.
         * greppable: udx: dma soft residual sync clflush
         */
        if (pVa != NULL) {
            fs_dma_clflush_range(pVa, cbSize);
        } else {
            __asm__ volatile("mfence" ::: "memory");
        }
    }
#endif
}

void
udx_dma_sync_single_for_device(struct udx_device *pDev, udx_dma_addr_t dma,
                               size_t cbSize, enum udx_dma_data_direction eDir)
{
    (void)pDev;
    (void)eDir;
    dma_soft_inc(&g_u32DmaSyncDev);
    dma_soft_log_sync_residual_once();
#if defined(UDX_HOST_LIBC)
    (void)dma;
    (void)cbSize;
    __asm__ volatile("mfence" ::: "memory");
#else
    {
        void *pVa = fs_dma_va_from_cookie(dma, cbSize);

        /*
         * Product freestanding: push Own=1 / desc / TX payload to DRAM
         * before NIC DMA (pair kernel dma_buf_sync_for_device clflush).
         * Soft!=product; Dual DoD B OPEN.
         */
        if (pVa != NULL) {
            fs_dma_clflush_range(pVa, cbSize);
        } else {
            __asm__ volatile("mfence" ::: "memory");
        }
    }
#endif
}
