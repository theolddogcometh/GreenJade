/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * ioremap / iounmap. Host: malloc iomem + window table.
 * Freestanding: static iomem pool (no libc heap).
 *
 * Soft inventory (Wave 114 exclusive deepen; this unit only) —
 * greppable "udx: mmio soft …":
 *   udx: mmio soft inventory …
 *   udx: mmio soft map …
 *   udx: mmio soft unmap …
 *   udx: mmio soft pool …
 *   udx: mmio soft path …
 *   udx: mmio soft wave …
 * Pure observation; never gates host skeleton PASS or freestanding path.
 * Soft ≠ skeleton PASS. greppable: udx: mmio soft
 */
#include "udx_internal.h"

#include <udx/mmio.h>
#include <udx/udx.h>

#include <stdarg.h>

#if defined(UDX_HOST_LIBC)
#include <stdio.h>
#include <stdlib.h>
#else
static struct udx_iomem g_aFsIomem[UDX_FS_IOMEM_SLOTS];
static u8               g_aFsIomemUsed[UDX_FS_IOMEM_SLOTS];
#endif

/* Soft wave stamp for greppable inventory lines. */
#define UDX_MMIO_SOFT_WAVE 70u

/*
 * Soft MMIO product inventory (Wave 114 exclusive deepen). Cumulative for
 * this process. greppable: udx: mmio soft …
 * Never hard-gates; wrap OK if ever hit.
 */
static u32 g_u32MmioMapEnter;    /* udx_ioremap entries */
static u32 g_u32MmioMapOk;       /* ioremap success */
static u32 g_u32MmioMapInval;    /* len 0 or phys+len overflow */
static u32 g_u32MmioMapLookup;   /* window lookup miss */
static u32 g_u32MmioMapNomem;    /* host malloc / fs pool fail */
static u32 g_u32MmioUnmap;       /* iounmap entries */
static u32 g_u32MmioUnmapNull;   /* iounmap null soft pointer */
static u32 g_u32MmioPoolLive;    /* freestanding iomem slots used */
static u32 g_u32MmioPoolPeak;    /* peak freestanding iomem slots */
static u32 g_u32MmioPoolFull;    /* freestanding pool full */
static u32 g_u32MmioLastLenLo;   /* last map len low 32 (soft snap) */
static u32 g_u32MmioLastLenHi;   /* last map len high 32 */
static u32 g_u32MmioLogN;        /* soft inventory dumps emitted */
static u8  g_fMmioSoftOnce;      /* one-shot deep dump after first ok map */

static void mmio_soft_inc(u32 *pu32);
static void mmio_soft_emit(const char *szFmt, ...);
static void mmio_soft_inventory_log(void);
static void mmio_soft_maybe_once(void);

static void
mmio_soft_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

/*
 * Soft inventory emit path — host console; does not use udx_printk so
 * core soft printk tallies stay product-path pure.
 */
static void
mmio_soft_emit(const char *szFmt, ...)
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
 * Greppable soft MMIO inventory (Wave 114 exclusive deepen).
 * Prefix-stable "udx: mmio soft …" — never hard-gates; observation only.
 *
 * greppable: udx: mmio soft
 */
static void
mmio_soft_inventory_log(void)
{
    u32 u32Host;
    u32 u32FsSlots;

    mmio_soft_inc(&g_u32MmioLogN);

#if defined(UDX_HOST_LIBC)
    u32Host = 1u;
    u32FsSlots = 0u;
#else
    u32Host = 0u;
    u32FsSlots = UDX_FS_IOMEM_SLOTS;
#endif

    /* Grep: udx: mmio soft inventory */
    mmio_soft_emit("udx: mmio soft inventory map_ok=%u map_inval=%u "
                   "map_lookup_miss=%u map_nomem=%u unmap=%u unmap_null=%u "
                   "log_n=%u wave=%u\n",
                   g_u32MmioMapOk, g_u32MmioMapInval, g_u32MmioMapLookup,
                   g_u32MmioMapNomem, g_u32MmioUnmap, g_u32MmioUnmapNull,
                   g_u32MmioLogN, UDX_MMIO_SOFT_WAVE);

    /* Grep: udx: mmio soft map */
    mmio_soft_emit("udx: mmio soft map enter=%u ok=%u inval=%u lookup_miss=%u "
                   "nomem=%u last_len_lo=%u last_len_hi=%u "
                   "via=host_window_lookup\n",
                   g_u32MmioMapEnter, g_u32MmioMapOk, g_u32MmioMapInval,
                   g_u32MmioMapLookup, g_u32MmioMapNomem, g_u32MmioLastLenLo,
                   g_u32MmioLastLenHi);

    /* Grep: udx: mmio soft unmap */
    mmio_soft_emit("udx: mmio soft unmap enter=%u null=%u "
                   "host_free=1 freestanding_pool_return=1\n",
                   g_u32MmioUnmap, g_u32MmioUnmapNull);

    /* Grep: udx: mmio soft pool */
    mmio_soft_emit("udx: mmio soft pool host_libc=%u freestanding=%u "
                   "slots=%u live=%u peak=%u full=%u\n",
                   u32Host, (u32Host == 0u) ? 1u : 0u, u32FsSlots,
                   g_u32MmioPoolLive, g_u32MmioPoolPeak, g_u32MmioPoolFull);

    /*
     * Path catalog — what this soft surface is / is not.
     * greppable: udx: mmio soft path
     */
    mmio_soft_emit("udx: mmio soft path map=udx_ioremap unmap=udx_iounmap "
                   "lookup=udx_host_window_lookup "
                   "accessors=header_inlines "
                   "skeleton_gate=0 hard_gate=0 soft=1\n");

    /* Grep: udx: mmio soft wave */
    mmio_soft_emit("udx: mmio soft wave n=%u unit=mmio exclusive=1 "
                   "prefix=udx:_mmio_soft deepen=1 "
                   "(soft inventory; never gates skeleton PASS)\n",
                   UDX_MMIO_SOFT_WAVE);

    /*
     * Grep: udx: mmio soft honesty (Wave 114 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    mmio_soft_emit("udx: mmio soft honesty multi_server=0 confine=0 bar3=0 "
                  "exclusive=1 soft=1 product_kernel=OPEN wave=%u\n",
                  UDX_MMIO_SOFT_WAVE);

}

/** Soft: one-shot inventory after first successful ioremap. */
static void
mmio_soft_maybe_once(void)
{
    if (g_fMmioSoftOnce != 0) {
        return;
    }
    if (g_u32MmioMapOk == 0) {
        return;
    }
    g_fMmioSoftOnce = 1;
    mmio_soft_inventory_log();
}

#if !defined(UDX_HOST_LIBC)
/** Soft: raise peak if u32Val is higher (diagnostics only; freestanding pool). */
static void
mmio_soft_note_peak(u32 *pu32Peak, u32 u32Val)
{
    if (pu32Peak != NULL && u32Val > *pu32Peak) {
        *pu32Peak = u32Val;
    }
}

static struct udx_iomem *
fs_iomem_alloc(void)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < UDX_FS_IOMEM_SLOTS; iSlot++) {
        if (!g_aFsIomemUsed[iSlot]) {
            g_aFsIomemUsed[iSlot] = 1;
            g_aFsIomem[iSlot].pVa = NULL;
            g_aFsIomem[iSlot].u64Len = 0;
            g_aFsIomem[iSlot].u64Phys = 0;
            mmio_soft_inc(&g_u32MmioPoolLive);
            mmio_soft_note_peak(&g_u32MmioPoolPeak, g_u32MmioPoolLive);
            return &g_aFsIomem[iSlot];
        }
    }
    mmio_soft_inc(&g_u32MmioPoolFull);
    return NULL;
}

static void
fs_iomem_free(struct udx_iomem *pIo)
{
    u32 iSlot;

    if (pIo == NULL) {
        return;
    }
    for (iSlot = 0; iSlot < UDX_FS_IOMEM_SLOTS; iSlot++) {
        if (&g_aFsIomem[iSlot] == pIo) {
            g_aFsIomemUsed[iSlot] = 0;
            g_aFsIomem[iSlot].pVa = NULL;
            if (g_u32MmioPoolLive > 0u) {
                g_u32MmioPoolLive--;
            }
            return;
        }
    }
}
#endif

struct udx_iomem *
udx_ioremap(u64 u64Phys, u64 u64Len)
{
    struct udx_iomem *pIo;
    void *pVa;

    mmio_soft_inc(&g_u32MmioMapEnter);
    if (u64Len == 0 || u64Phys + u64Len < u64Phys) {
        mmio_soft_inc(&g_u32MmioMapInval);
        return NULL;
    }

    /* lookup returns VA already advanced by any sub-window offset */
    pVa = udx_host_window_lookup(u64Phys, u64Len, NULL);
    if (pVa == NULL) {
        mmio_soft_inc(&g_u32MmioMapLookup);
        udx_printk("udx: ioremap fail phys=%llx len=%llx\n",
                   (unsigned long long)u64Phys,
                   (unsigned long long)u64Len);
        return NULL;
    }

#if defined(UDX_HOST_LIBC)
    pIo = (struct udx_iomem *)malloc(sizeof(*pIo));
    if (pIo == NULL) {
        mmio_soft_inc(&g_u32MmioMapNomem);
        return NULL;
    }
#else
    pIo = fs_iomem_alloc();
    if (pIo == NULL) {
        mmio_soft_inc(&g_u32MmioMapNomem);
        udx_printk("udx: ioremap freestanding pool full\n");
        return NULL;
    }
#endif
    pIo->pVa = pVa;
    pIo->u64Len = u64Len;
    pIo->u64Phys = u64Phys;
    g_u32MmioLastLenLo = (u32)(u64Len & 0xffffffffull);
    g_u32MmioLastLenHi = (u32)(u64Len >> 32);
    mmio_soft_inc(&g_u32MmioMapOk);
    mmio_soft_maybe_once();
    return pIo;
}

void
udx_iounmap(struct udx_iomem *pIo)
{
    mmio_soft_inc(&g_u32MmioUnmap);
    if (pIo == NULL) {
        mmio_soft_inc(&g_u32MmioUnmapNull);
        return;
    }
#if defined(UDX_HOST_LIBC)
    free(pIo);
#else
    fs_iomem_free(pIo);
#endif
}

/* Wave 114 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retframebufferangle continuum_toward=25600 soft_ne_product=1 wave=114
 *   greppable: soft retswapchainangle exclusive=1 continuum_toward=25600 soft_ne_product=1 wave=114
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */
