/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Vulkan-shaped helpers: instance / device / swapchain + present.
 * With GJ_VK_KERNEL_SMOKE: linked into the kernel (virtio-gpu + PMM).
 * Without it: stubs return failure so host builds only need vulkan_icd.c.
 */
#include <gj_vk_icd.h>

#define GJ_VK_INST_MAGIC 0x49564a47u /* 'GJVI' le */
#define GJ_VK_DEV_MAGIC  0x44564a47u /* 'GJVD' */
#define GJ_VK_SC_MAGIC   0x53564a47u /* 'GJVS' */

#ifndef GJ_VK_KERNEL_SMOKE

int
gj_vk_get_display(struct gj_vk_display_info *pOut)
{
    if (pOut == 0) {
        return -1;
    }
    pOut->u32Width = 0;
    pOut->u32Height = 0;
    return -1;
}

int
gj_vk_queue_present(const struct gj_vk_image *pImg)
{
    (void)pImg;
    return -1;
}

int
gj_vk_create_instance(gj_vk_instance_t *ppOut)
{
    (void)ppOut;
    return -1;
}

void
gj_vk_destroy_instance(gj_vk_instance_t pInst)
{
    (void)pInst;
}

int
gj_vk_create_device(gj_vk_instance_t pInst, gj_vk_device_t *ppOut)
{
    (void)pInst;
    (void)ppOut;
    return -1;
}

void
gj_vk_destroy_device(gj_vk_device_t pDev)
{
    (void)pDev;
}

int
gj_vk_create_swapchain(gj_vk_device_t pDev, uint32_t u32ImageCount,
                       gj_vk_swapchain_t *ppOut)
{
    (void)pDev;
    (void)u32ImageCount;
    (void)ppOut;
    return -1;
}

void
gj_vk_destroy_swapchain(gj_vk_swapchain_t pSc)
{
    (void)pSc;
}

int
gj_vk_acquire_next_image(gj_vk_swapchain_t pSc, uint32_t *pIndex)
{
    (void)pSc;
    (void)pIndex;
    return -1;
}

int
gj_vk_queue_present_image(gj_vk_swapchain_t pSc, uint32_t u32Index)
{
    (void)pSc;
    (void)u32Index;
    return -1;
}

void *
gj_vk_image_pixels(gj_vk_swapchain_t pSc, uint32_t u32Index)
{
    (void)pSc;
    (void)u32Index;
    return 0;
}

#else /* GJ_VK_KERNEL_SMOKE */

#include <gj/config.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/virtio_gpu.h>
#include <gj/vmm.h>

/* Defined at end of file (also used when allocating instance features). */
uint32_t gj_vk_icd_features(void);

/* Static object pools — freestanding; no malloc beyond PMM-backed fbs. */
#define GJ_VK_POOL_INST 2
#define GJ_VK_POOL_DEV  2
#define GJ_VK_POOL_SC   2

static struct gj_vk_instance  g_aInst[GJ_VK_POOL_INST];
static struct gj_vk_device    g_aDev[GJ_VK_POOL_DEV];
static struct gj_vk_swapchain g_aSc[GJ_VK_POOL_SC];
static u8 g_aInstUsed[GJ_VK_POOL_INST];
static u8 g_aDevUsed[GJ_VK_POOL_DEV];
static u8 g_aScUsed[GJ_VK_POOL_SC];

int
gj_vk_get_display(struct gj_vk_display_info *pOut)
{
    u32 w = 0;
    u32 h = 0;

    if (pOut == 0) {
        return -1;
    }
    if (!virtio_gpu_ready() || virtio_gpu_get_display(&w, &h) != 0) {
        return -1;
    }
    pOut->u32Width = w;
    pOut->u32Height = h;
    return 0;
}

int
gj_vk_queue_present(const struct gj_vk_image *pImg)
{
    u32 stride;

    if (pImg == 0 || pImg->pPixels == 0 || pImg->u32Width == 0 ||
        pImg->u32Height == 0) {
        return -1;
    }
    stride = pImg->u32Stride ? pImg->u32Stride : pImg->u32Width * 4u;
    return virtio_gpu_present(pImg->u32Width, pImg->u32Height, pImg->pPixels,
                              stride);
}

int
gj_vk_create_instance(gj_vk_instance_t *ppOut)
{
    u32 i;

    if (ppOut == 0) {
        return -1;
    }
    for (i = 0; i < GJ_VK_POOL_INST; i++) {
        if (!g_aInstUsed[i]) {
            g_aInstUsed[i] = 1;
            memset(&g_aInst[i], 0, sizeof(g_aInst[i]));
            g_aInst[i].u32Magic = GJ_VK_INST_MAGIC;
            g_aInst[i].u32Version = (1u << 16) | 0u; /* major.minor 1.0 */
            g_aInst[i].u32Features = gj_vk_icd_features();
            *ppOut = &g_aInst[i];
            return 0;
        }
    }
    return -1;
}

void
gj_vk_destroy_instance(gj_vk_instance_t pInst)
{
    u32 i;

    if (pInst == 0) {
        return;
    }
    for (i = 0; i < GJ_VK_POOL_INST; i++) {
        if (&g_aInst[i] == pInst) {
            g_aInstUsed[i] = 0;
            pInst->u32Magic = 0;
            return;
        }
    }
}

int
gj_vk_create_device(gj_vk_instance_t pInst, gj_vk_device_t *ppOut)
{
    u32 i;
    u32 w = 64;
    u32 h = 64;

    if (pInst == 0 || pInst->u32Magic != GJ_VK_INST_MAGIC || ppOut == 0) {
        return -1;
    }
    if (!virtio_gpu_ready()) {
        return -1;
    }
    (void)virtio_gpu_get_display(&w, &h);
    if (w == 0 || h == 0 || w > 256 || h > 256) {
        w = 64;
        h = 64;
    }
    for (i = 0; i < GJ_VK_POOL_DEV; i++) {
        if (!g_aDevUsed[i]) {
            g_aDevUsed[i] = 1;
            memset(&g_aDev[i], 0, sizeof(g_aDev[i]));
            g_aDev[i].u32Magic = GJ_VK_DEV_MAGIC;
            g_aDev[i].pInst = pInst;
            g_aDev[i].u32Width = w;
            g_aDev[i].u32Height = h;
            *ppOut = &g_aDev[i];
            return 0;
        }
    }
    return -1;
}

void
gj_vk_destroy_device(gj_vk_device_t pDev)
{
    u32 i;

    if (pDev == 0) {
        return;
    }
    for (i = 0; i < GJ_VK_POOL_DEV; i++) {
        if (&g_aDev[i] == pDev) {
            g_aDevUsed[i] = 0;
            pDev->u32Magic = 0;
            return;
        }
    }
}

static void *
alloc_fb(u32 u32W, u32 u32H, u32 *pStride)
{
    u32 bytes;
    u32 pages;
    gj_paddr_t pa;
    u8 *p;
    u32 i;

    *pStride = u32W * 4u;
    bytes = *pStride * u32H;
    pages = (bytes + GJ_PAGE_SIZE - 1) / GJ_PAGE_SIZE;
    pa = pmm_alloc_pages(pages);
    if (pa == 0) {
        pa = pmm_alloc();
        if (pa == 0) {
            return 0;
        }
        /* Fallback: single page → shrink fb so it still fits. */
        u32W = 32;
        u32H = 32;
        *pStride = u32W * 4u;
    }
    p = (u8 *)hhdm_to_virt(pa);
    for (i = 0; i < (*pStride / 4u) * u32H && i < 32u * 32u; i++) {
        p[i * 4u + 0] = 0x30;
        p[i * 4u + 1] = 0xa0;
        p[i * 4u + 2] = 0x20;
        p[i * 4u + 3] = 0xff;
    }
    return p;
}

int
gj_vk_create_swapchain(gj_vk_device_t pDev, uint32_t u32ImageCount,
                       gj_vk_swapchain_t *ppOut)
{
    u32 i;
    u32 j;
    u32 stride;

    if (pDev == 0 || pDev->u32Magic != GJ_VK_DEV_MAGIC || ppOut == 0) {
        return -1;
    }
    if (u32ImageCount == 0 || u32ImageCount > GJ_VK_MAX_SWAP_IMAGES) {
        u32ImageCount = 2;
    }
    for (i = 0; i < GJ_VK_POOL_SC; i++) {
        if (!g_aScUsed[i]) {
            g_aScUsed[i] = 1;
            memset(&g_aSc[i], 0, sizeof(g_aSc[i]));
            g_aSc[i].u32Magic = GJ_VK_SC_MAGIC;
            g_aSc[i].pDev = pDev;
            g_aSc[i].u32ImageCount = u32ImageCount;
            g_aSc[i].u32Current = 0;
            g_aSc[i].u32Width = pDev->u32Width;
            g_aSc[i].u32Height = pDev->u32Height;
            for (j = 0; j < u32ImageCount; j++) {
                g_aSc[i].aImages[j].img.u32Width = pDev->u32Width;
                g_aSc[i].aImages[j].img.u32Height = pDev->u32Height;
                g_aSc[i].aImages[j].img.u32Format = GJ_VK_FORMAT_B8G8R8X8;
                g_aSc[i].aImages[j].pBacking =
                    alloc_fb(pDev->u32Width, pDev->u32Height, &stride);
                g_aSc[i].aImages[j].img.u32Stride = stride;
                g_aSc[i].aImages[j].img.pPixels =
                    g_aSc[i].aImages[j].pBacking;
                g_aSc[i].aImages[j].u8Acquired = 0;
                if (g_aSc[i].aImages[j].pBacking == 0) {
                    g_aScUsed[i] = 0;
                    return -1;
                }
            }
            *ppOut = &g_aSc[i];
            return 0;
        }
    }
    return -1;
}

void
gj_vk_destroy_swapchain(gj_vk_swapchain_t pSc)
{
    u32 i;

    if (pSc == 0) {
        return;
    }
    for (i = 0; i < GJ_VK_POOL_SC; i++) {
        if (&g_aSc[i] == pSc) {
            g_aScUsed[i] = 0;
            pSc->u32Magic = 0;
            return;
        }
    }
}

int
gj_vk_acquire_next_image(gj_vk_swapchain_t pSc, uint32_t *pIndex)
{
    uint32_t idx;

    if (pSc == 0 || pSc->u32Magic != GJ_VK_SC_MAGIC || pIndex == 0) {
        return -1;
    }
    idx = pSc->u32Current % pSc->u32ImageCount;
    pSc->u32Current = (idx + 1u) % pSc->u32ImageCount;
    pSc->aImages[idx].u8Acquired = 1;
    *pIndex = idx;
    return 0;
}

int
gj_vk_queue_present_image(gj_vk_swapchain_t pSc, uint32_t u32Index)
{
    struct gj_vk_image *pImg;

    if (pSc == 0 || pSc->u32Magic != GJ_VK_SC_MAGIC ||
        u32Index >= pSc->u32ImageCount) {
        return -1;
    }
    if (!pSc->aImages[u32Index].u8Acquired) {
        return -1;
    }
    pImg = &pSc->aImages[u32Index].img;
    pSc->aImages[u32Index].u8Acquired = 0;
    return gj_vk_queue_present(pImg);
}

void *
gj_vk_image_pixels(gj_vk_swapchain_t pSc, uint32_t u32Index)
{
    if (pSc == 0 || pSc->u32Magic != GJ_VK_SC_MAGIC ||
        u32Index >= pSc->u32ImageCount) {
        return 0;
    }
    return pSc->aImages[u32Index].img.pPixels;
}

#endif /* GJ_VK_KERNEL_SMOKE */

const char *
gj_vk_icd_name(void)
{
    return "GreenJade-virtio-gpu-2d-icd";
}

uint32_t
gj_vk_icd_features(void)
{
    return GJ_VK_FEAT_PRESENT_2D | GJ_VK_FEAT_SWAPCHAIN | GJ_VK_FEAT_INSTANCE |
           GJ_VK_FEAT_NAMED_EXPORT;
}
