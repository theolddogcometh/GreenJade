/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * DMA helpers. Host: calloc identity cookies.
 * Freestanding: static slab (UDX_FS_DMA_SLOTS × UDX_FS_DMA_SLOT_CB).
 */
#include "udx_internal.h"

#include <udx/dma.h>
#include <udx/device.h>

#if defined(UDX_HOST_LIBC)
#include <stdlib.h>
#include <string.h>
#else
static struct udx_fs_dma_slot g_aFsDma[UDX_FS_DMA_SLOTS];

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
            return g_aFsDma[iSlot].aBytes;
        }
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
    if (cbSize == 0 || pDmaHandle == NULL) {
        return NULL;
    }
#if defined(UDX_HOST_LIBC)
    p = calloc(1, cbSize);
#else
    p = fs_dma_alloc(cbSize);
#endif
    if (p == NULL) {
        return NULL;
    }
    /*
     * Host/GJ: DMA address == CPU cookie. Full path: IOMMU window grant
     * via PLATFORM_INFO op5 when BDF known (udx_dma_iommu_grant).
     */
    *pDmaHandle = (udx_dma_addr_t)(uintptr_t)p;
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
#if defined(UDX_HOST_LIBC)
    (void)u32Bdf;
    (void)dma;
    (void)cbSize;
    return 0;
#else
    long ret;

    if (dma == 0 || cbSize == 0) {
        return -1;
    }
    ret = udx_gj_iommu_grant(u32Bdf, dma, cbSize);
    return (ret < 0) ? -1 : 0;
#endif
}

void
udx_dma_free_coherent(struct udx_device *pDev, size_t cbSize, void *pCpu,
                      udx_dma_addr_t dmaHandle)
{
    (void)pDev;
    (void)cbSize;
    (void)dmaHandle;
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
    if (pCpu == NULL) {
        return 0;
    }
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
}

void
udx_dma_sync_single_for_cpu(struct udx_device *pDev, udx_dma_addr_t dma,
                            size_t cbSize, enum udx_dma_data_direction eDir)
{
    (void)pDev;
    (void)dma;
    (void)cbSize;
    (void)eDir;
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
    __asm__ volatile("mfence" ::: "memory");
}
