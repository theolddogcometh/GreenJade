/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Linux-shaped DMA API over GreenJade IOMMU window caps (hidden).
 *
 * Host (UDX_HOST_LIBC): calloc identity cookies.
 * Freestanding: static DMA slab (no libc heap); grant via PLATFORM_INFO op5.
 */
#pragma once

#include <udx/types.h>

struct udx_device;

enum udx_dma_data_direction {
    UDX_DMA_BIDIRECTIONAL = 0,
    UDX_DMA_TO_DEVICE     = 1,
    UDX_DMA_FROM_DEVICE   = 2,
    UDX_DMA_NONE          = 3,
};

/** Linux dma_alloc_coherent shape. Flags reserved (pass 0). */
void *udx_dma_alloc_coherent(struct udx_device *pDev, size_t cbSize,
                             udx_dma_addr_t *pDmaHandle, u32 u32Flags);

/**
 * Best-effort IOMMU window grant for a DMA cookie.
 * Freestanding: PLATFORM_INFO op5 (bdf, pa, size).
 * Host (UDX_HOST_LIBC): no-op success (identity map).
 */
int udx_dma_iommu_grant(u32 u32Bdf, udx_dma_addr_t dma, size_t cbSize);

void udx_dma_free_coherent(struct udx_device *pDev, size_t cbSize, void *pCpu,
                           udx_dma_addr_t dmaHandle);

udx_dma_addr_t udx_dma_map_single(struct udx_device *pDev, void *pCpu,
                                  size_t cbSize,
                                  enum udx_dma_data_direction eDir);

void udx_dma_unmap_single(struct udx_device *pDev, udx_dma_addr_t dmaHandle,
                          size_t cbSize, enum udx_dma_data_direction eDir);

/** Linux dma_mapping_error — non-zero means map_single failed. */
static inline int
udx_dma_mapping_error(struct udx_device *pDev, udx_dma_addr_t dmaHandle)
{
    (void)pDev;
    return dmaHandle == 0;
}

/**
 * Sync helpers (Linux dma_sync_single_*).
 * Host/identity: compiler barrier only. Full GJ may flush/invalidate.
 */
void udx_dma_sync_single_for_cpu(struct udx_device *pDev, udx_dma_addr_t dma,
                                 size_t cbSize,
                                 enum udx_dma_data_direction eDir);
void udx_dma_sync_single_for_device(struct udx_device *pDev, udx_dma_addr_t dma,
                                    size_t cbSize,
                                    enum udx_dma_data_direction eDir);

/** Pack BDF for udx_dma_iommu_grant (bus, slot 5-bit, func 3-bit). */
static inline u32
udx_dma_bdf(u8 u8Bus, u8 u8Slot, u8 u8Func)
{
    return ((u32)u8Bus << 16) | (((u32)u8Slot & 0x1fu) << 8) |
           ((u32)u8Func & 0x7u);
}
