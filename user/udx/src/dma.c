/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * DMA helpers. Host: calloc identity cookies.
 * Freestanding: static slab (UDX_FS_DMA_SLOTS × UDX_FS_DMA_SLOT_CB).
 *
 * Soft inventory (Wave 70 exclusive deepen; this unit only) —
 * greppable "udx: dma soft …":
 *   udx: dma soft inventory …
 *   udx: dma soft alloc …
 *   udx: dma soft map …
 *   udx: dma soft sync …
 *   udx: dma soft iommu …
 *   udx: dma soft slab …
 *   udx: dma soft path …
 *   udx: dma soft wave …
 * Pure observation; never gates host skeleton PASS or freestanding path.
 * Soft ≠ skeleton PASS. greppable: udx: dma soft
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
static struct udx_fs_dma_slot g_aFsDma[UDX_FS_DMA_SLOTS];
#endif

/* Soft wave stamp for greppable inventory lines. */
#define UDX_DMA_SOFT_WAVE 70u

/*
 * Soft DMA product inventory (Wave 70 exclusive deepen). Cumulative for
 * this process. greppable: udx: dma soft …
 * Never hard-gates; wrap OK if ever hit.
 */
static u32 g_u32DmaAllocEnter;   /* udx_dma_alloc_coherent entries */
static u32 g_u32DmaAllocOk;      /* coherent alloc success */
static u32 g_u32DmaAllocFail;    /* allocator returned NULL */
static u32 g_u32DmaAllocInval;   /* size 0 or null handle */
static u32 g_u32DmaFree;         /* free_coherent calls */
static u32 g_u32DmaFreeNull;     /* free with null cpu cookie */
static u32 g_u32DmaMap;          /* map_single entries */
static u32 g_u32DmaMapOk;        /* map_single returned non-zero */
static u32 g_u32DmaMapNull;      /* map_single rejected null cpu */
static u32 g_u32DmaUnmap;        /* unmap_single calls */
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
static u32 g_u32DmaLogN;         /* soft inventory dumps emitted */
static u8  g_fDmaSoftOnce;       /* one-shot deep dump after first ok path */

static void dma_soft_inc(u32 *pu32);
static void dma_soft_note_peak(u32 *pu32Peak, u32 u32Val);
static void dma_soft_emit(const char *szFmt, ...);
static void dma_soft_inventory_log(void);
static void dma_soft_maybe_once(void);

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
 * Greppable soft DMA inventory (Wave 70 exclusive deepen).
 * Prefix-stable "udx: dma soft …" — never hard-gates; observation only.
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
                  "log_n=%u wave=%u\n",
                  g_u32DmaAllocOk, g_u32DmaAllocFail, g_u32DmaAllocInval,
                  g_u32DmaFree, g_u32DmaMapOk, g_u32DmaMapNull, g_u32DmaUnmap,
                  g_u32DmaSyncCpu, g_u32DmaSyncDev, g_u32DmaIommuOk,
                  g_u32DmaIommuFail, g_u32DmaLogN, UDX_DMA_SOFT_WAVE);

    /* Grep: udx: dma soft alloc */
    dma_soft_emit("udx: dma soft alloc enter=%u ok=%u fail=%u inval=%u "
                  "free=%u free_null=%u last_cb=%u peak_cb=%u "
                  "identity_cookie=1\n",
                  g_u32DmaAllocEnter, g_u32DmaAllocOk, g_u32DmaAllocFail,
                  g_u32DmaAllocInval, g_u32DmaFree, g_u32DmaFreeNull,
                  g_u32DmaLastCb, g_u32DmaPeakCb);

    /* Grep: udx: dma soft map */
    dma_soft_emit("udx: dma soft map enter=%u ok=%u null=%u unmap=%u "
                  "identity=1 mapping_error=zero_cookie\n",
                  g_u32DmaMap, g_u32DmaMapOk, g_u32DmaMapNull, g_u32DmaUnmap);

    /* Grep: udx: dma soft sync */
    dma_soft_emit("udx: dma soft sync for_cpu=%u for_device=%u "
                  "host_barrier=mfence full_iommu_flush=0\n",
                  g_u32DmaSyncCpu, g_u32DmaSyncDev);

    /* Grep: udx: dma soft iommu */
    dma_soft_emit("udx: dma soft iommu enter=%u ok=%u fail=%u host_nop=%u "
                  "plat_op=5 freestanding_syscall=%u\n",
                  g_u32DmaIommuEnter, g_u32DmaIommuOk, g_u32DmaIommuFail,
                  g_u32DmaIommuHostNop, (u32Host == 0u) ? 1u : 0u);

    /* Grep: udx: dma soft slab */
    dma_soft_emit("udx: dma soft slab host_libc=%u freestanding=%u "
                  "slots=%u slot_cb=%u live=%u peak=%u full=%u\n",
                  u32Host, (u32Host == 0u) ? 1u : 0u, u32FsSlots, u32FsSlotCb,
                  g_u32DmaSlabLive, g_u32DmaSlabPeak, g_u32DmaSlabFull);

    /*
     * Path catalog — what this soft surface is / is not.
     * greppable: udx: dma soft path
     */
    dma_soft_emit("udx: dma soft path alloc=udx_dma_alloc_coherent "
                  "free=udx_dma_free_coherent map=udx_dma_map_single "
                  "unmap=udx_dma_unmap_single sync_cpu=udx_dma_sync_single_for_cpu "
                  "sync_dev=udx_dma_sync_single_for_device "
                  "iommu=udx_dma_iommu_grant "
                  "skeleton_gate=0 hard_gate=0 soft=1\n");

    /* Grep: udx: dma soft wave */
    dma_soft_emit("udx: dma soft wave n=%u unit=dma exclusive=1 "
                  "prefix=udx:_dma_soft deepen=1 "
                  "(soft inventory; never gates skeleton PASS)\n",
                  UDX_DMA_SOFT_WAVE);

    /*
     * Grep: udx: dma soft honesty (Wave 70 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    dma_soft_emit("udx: dma soft honesty multi_server=0 confine=0 bar3=0 "
                  "exclusive=1 soft=1 product_kernel=OPEN wave=%u\n",
                  UDX_DMA_SOFT_WAVE);

}

/** Soft: one-shot inventory after first successful alloc/map/grant. */
static void
dma_soft_maybe_once(void)
{
    if (g_fDmaSoftOnce != 0) {
        return;
    }
    if (g_u32DmaAllocOk == 0 && g_u32DmaMapOk == 0 && g_u32DmaIommuOk == 0) {
        return;
    }
    g_fDmaSoftOnce = 1;
    dma_soft_inventory_log();
}

#if !defined(UDX_HOST_LIBC)
static void *
fs_dma_alloc(size_t cbSize)
{
    u32 iSlot;

    if (cbSize == 0 || cbSize > UDX_FS_DMA_SLOT_CB) {
        return NULL;
    }
    for (iSlot = 0; iSlot < UDX_FS_DMA_SLOTS; iSlot++) {
        if (!g_aFsDma[iSlot].u8Used) {
            u32 iByte;

            g_aFsDma[iSlot].u8Used = 1;
            g_aFsDma[iSlot].cbAlloc = cbSize;
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
            if (g_u32DmaSlabLive > 0u) {
                g_u32DmaSlabLive--;
            }
            return;
        }
    }
}
#endif

void *
udx_dma_alloc_coherent(struct udx_device *pDev, size_t cbSize,
                       udx_dma_addr_t *pDmaHandle, u32 u32Flags)
{
    void *p;

    (void)pDev;
    (void)u32Flags;
    dma_soft_inc(&g_u32DmaAllocEnter);
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
     * Host/GJ: DMA address == CPU cookie. Full path: IOMMU window grant
     * via PLATFORM_INFO op5 when BDF known (udx_dma_iommu_grant).
     */
    *pDmaHandle = (udx_dma_addr_t)(uintptr_t)p;
    g_u32DmaLastCb = (cbSize > 0xffffffffu) ? 0xffffffffu : (u32)cbSize;
    dma_soft_note_peak(&g_u32DmaPeakCb, g_u32DmaLastCb);
    dma_soft_inc(&g_u32DmaAllocOk);
    dma_soft_maybe_once();
    return p;
}

/**
 * Grant an IOMMU window for a DMA cookie (GJ freestanding).
 * bdf = bus<<16|slot<<8|func; dma/size describe the DMA region.
 * Host (UDX_HOST_LIBC): no-op success — sim identity map.
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
    dma_soft_maybe_once();
    return 0;
#else
    long ret;

    if (dma == 0 || cbSize == 0) {
        dma_soft_inc(&g_u32DmaIommuFail);
        return -1;
    }
    ret = udx_gj_iommu_grant(u32Bdf, dma, cbSize);
    if (ret < 0) {
        dma_soft_inc(&g_u32DmaIommuFail);
        return -1;
    }
    dma_soft_inc(&g_u32DmaIommuOk);
    dma_soft_maybe_once();
    return 0;
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
    (void)pDev;
    (void)cbSize;
    (void)eDir;
    dma_soft_inc(&g_u32DmaMap);
    if (pCpu == NULL) {
        dma_soft_inc(&g_u32DmaMapNull);
        return 0;
    }
    dma_soft_inc(&g_u32DmaMapOk);
    dma_soft_maybe_once();
    return (udx_dma_addr_t)(uintptr_t)pCpu;
}

void
udx_dma_unmap_single(struct udx_device *pDev, udx_dma_addr_t dmaHandle,
                     size_t cbSize, enum udx_dma_data_direction eDir)
{
    (void)pDev;
    (void)dmaHandle;
    (void)cbSize;
    (void)eDir;
    dma_soft_inc(&g_u32DmaUnmap);
}

void
udx_dma_sync_single_for_cpu(struct udx_device *pDev, udx_dma_addr_t dma,
                            size_t cbSize, enum udx_dma_data_direction eDir)
{
    (void)pDev;
    (void)dma;
    (void)cbSize;
    (void)eDir;
    dma_soft_inc(&g_u32DmaSyncCpu);
    __asm__ volatile("mfence" ::: "memory");
}

void
udx_dma_sync_single_for_device(struct udx_device *pDev, udx_dma_addr_t dma,
                               size_t cbSize, enum udx_dma_data_direction eDir)
{
    (void)pDev;
    (void)dma;
    (void)cbSize;
    (void)eDir;
    dma_soft_inc(&g_u32DmaSyncDev);
    __asm__ volatile("mfence" ::: "memory");
}

/* Wave 70 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retfosseangle continuum_toward=21200 soft_ne_product=1
 *   greppable: soft retcounterscarple exclusive=1 continuum_toward=21200
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */
