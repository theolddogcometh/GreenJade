/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Vulkan-shaped helpers: instance / device / swapchain + present.
 * With GJ_VK_KERNEL_SMOKE: linked into the kernel (virtio-gpu + PMM).
 * Without it: stubs return failure so host builds only need vulkan_icd.c.
 *
 * Soft inventory (Wave 123 exclusive deepen; greppable; not bar3 GPU):
 *   gj_vk_icd: soft inventory wave=70 surfaces=16 pools=inst,dev,sc
 *   gj_vk_icd: soft deepen wave=70 areas=display,present,instance,device,
 *              swapchain,acquire,pixels,features,name,host,path,counts,note,area
 *   gj_vk_icd: soft path kernel_smoke|host_stub bar3=0
 * Soft counters wrap OK; never hard-gate product returns.
 * greppable: "gj_vk_icd: soft"
 */
#include <gj_vk_icd.h>

#define GJ_VK_INST_MAGIC 0x49564a47u /* 'GJVI' le */
#define GJ_VK_DEV_MAGIC  0x44564a47u /* 'GJVD' */
#define GJ_VK_SC_MAGIC   0x53564a47u /* 'GJVS' */

/* Wave 123 exclusive soft inventory stamp. */
/* Wave 123 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retlayerangle continuum_toward=26500 soft_ne_product=1 wave=123
 *   greppable: soft retcanvasangle exclusive=1 continuum_toward=26500 soft_ne_product=1 wave=123
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

#define GJ_VK_ICD_SOFT_WAVE 70u
#define GJ_VK_ICD_SOFT_SURFACES  16u /* public convenience entry count + note/area */
#define GJ_VK_ICD_SOFT_AREAS     14u

/*
 * Soft path counters (cold; wrap OK). Shared host-stub + kernel smoke.
 * Grep: gj_vk_icd: soft
 */
static volatile uint32_t g_u32SoftDisplayN;
static volatile uint32_t g_u32SoftPresentN;
static volatile uint32_t g_u32SoftPresentOk;
static volatile uint32_t g_u32SoftPresentMiss;
static volatile uint32_t g_u32SoftCreateInstN;
static volatile uint32_t g_u32SoftCreateInstOk;
static volatile uint32_t g_u32SoftDestroyInstN;
static volatile uint32_t g_u32SoftCreateDevN;
static volatile uint32_t g_u32SoftCreateDevOk;
static volatile uint32_t g_u32SoftDestroyDevN;
static volatile uint32_t g_u32SoftCreateScN;
static volatile uint32_t g_u32SoftCreateScOk;
static volatile uint32_t g_u32SoftDestroyScN;
static volatile uint32_t g_u32SoftAcquireN;
static volatile uint32_t g_u32SoftAcquireOk;
static volatile uint32_t g_u32SoftPresentImgN;
static volatile uint32_t g_u32SoftPresentImgOk;
static volatile uint32_t g_u32SoftPixelsN;
static volatile uint32_t g_u32SoftInvN;

static void
gj_vk_icd_soft_inc(volatile uint32_t *pCtr)
{
    if (pCtr != 0) {
        (*pCtr)++;
    }
}

#ifndef GJ_VK_KERNEL_SMOKE

int
gj_vk_get_display(struct gj_vk_display_info *pOut)
{
    gj_vk_icd_soft_inc(&g_u32SoftDisplayN);
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
    gj_vk_icd_soft_inc(&g_u32SoftPresentN);
    gj_vk_icd_soft_inc(&g_u32SoftPresentMiss);
    (void)pImg;
    return -1;
}

int
gj_vk_create_instance(gj_vk_instance_t *ppOut)
{
    gj_vk_icd_soft_inc(&g_u32SoftCreateInstN);
    (void)ppOut;
    return -1;
}

void
gj_vk_destroy_instance(gj_vk_instance_t pInst)
{
    gj_vk_icd_soft_inc(&g_u32SoftDestroyInstN);
    (void)pInst;
}

int
gj_vk_create_device(gj_vk_instance_t pInst, gj_vk_device_t *ppOut)
{
    gj_vk_icd_soft_inc(&g_u32SoftCreateDevN);
    (void)pInst;
    (void)ppOut;
    return -1;
}

void
gj_vk_destroy_device(gj_vk_device_t pDev)
{
    gj_vk_icd_soft_inc(&g_u32SoftDestroyDevN);
    (void)pDev;
}

int
gj_vk_create_swapchain(gj_vk_device_t pDev, uint32_t u32ImageCount,
                       gj_vk_swapchain_t *ppOut)
{
    gj_vk_icd_soft_inc(&g_u32SoftCreateScN);
    (void)pDev;
    (void)u32ImageCount;
    (void)ppOut;
    return -1;
}

void
gj_vk_destroy_swapchain(gj_vk_swapchain_t pSc)
{
    gj_vk_icd_soft_inc(&g_u32SoftDestroyScN);
    (void)pSc;
}

int
gj_vk_acquire_next_image(gj_vk_swapchain_t pSc, uint32_t *pIndex)
{
    gj_vk_icd_soft_inc(&g_u32SoftAcquireN);
    (void)pSc;
    (void)pIndex;
    return -1;
}

int
gj_vk_queue_present_image(gj_vk_swapchain_t pSc, uint32_t u32Index)
{
    gj_vk_icd_soft_inc(&g_u32SoftPresentImgN);
    (void)pSc;
    (void)u32Index;
    return -1;
}

void *
gj_vk_image_pixels(gj_vk_swapchain_t pSc, uint32_t u32Index)
{
    gj_vk_icd_soft_inc(&g_u32SoftPixelsN);
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

    gj_vk_icd_soft_inc(&g_u32SoftDisplayN);
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
    int nRet;

    gj_vk_icd_soft_inc(&g_u32SoftPresentN);
    if (pImg == 0 || pImg->pPixels == 0 || pImg->u32Width == 0 ||
        pImg->u32Height == 0) {
        gj_vk_icd_soft_inc(&g_u32SoftPresentMiss);
        return -1;
    }
    stride = pImg->u32Stride ? pImg->u32Stride : pImg->u32Width * 4u;
    nRet = virtio_gpu_present(pImg->u32Width, pImg->u32Height, pImg->pPixels,
                              stride);
    if (nRet == 0) {
        gj_vk_icd_soft_inc(&g_u32SoftPresentOk);
    } else {
        gj_vk_icd_soft_inc(&g_u32SoftPresentMiss);
    }
    return nRet;
}

int
gj_vk_create_instance(gj_vk_instance_t *ppOut)
{
    u32 i;

    gj_vk_icd_soft_inc(&g_u32SoftCreateInstN);
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
            gj_vk_icd_soft_inc(&g_u32SoftCreateInstOk);
            return 0;
        }
    }
    return -1;
}

void
gj_vk_destroy_instance(gj_vk_instance_t pInst)
{
    u32 i;

    gj_vk_icd_soft_inc(&g_u32SoftDestroyInstN);
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

    gj_vk_icd_soft_inc(&g_u32SoftCreateDevN);
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
            gj_vk_icd_soft_inc(&g_u32SoftCreateDevOk);
            return 0;
        }
    }
    return -1;
}

void
gj_vk_destroy_device(gj_vk_device_t pDev)
{
    u32 i;

    gj_vk_icd_soft_inc(&g_u32SoftDestroyDevN);
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

    gj_vk_icd_soft_inc(&g_u32SoftCreateScN);
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
            gj_vk_icd_soft_inc(&g_u32SoftCreateScOk);
            return 0;
        }
    }
    return -1;
}

void
gj_vk_destroy_swapchain(gj_vk_swapchain_t pSc)
{
    u32 i;

    gj_vk_icd_soft_inc(&g_u32SoftDestroyScN);
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

    gj_vk_icd_soft_inc(&g_u32SoftAcquireN);
    if (pSc == 0 || pSc->u32Magic != GJ_VK_SC_MAGIC || pIndex == 0) {
        return -1;
    }
    idx = pSc->u32Current % pSc->u32ImageCount;
    pSc->u32Current = (idx + 1u) % pSc->u32ImageCount;
    pSc->aImages[idx].u8Acquired = 1;
    *pIndex = idx;
    gj_vk_icd_soft_inc(&g_u32SoftAcquireOk);
    return 0;
}

int
gj_vk_queue_present_image(gj_vk_swapchain_t pSc, uint32_t u32Index)
{
    struct gj_vk_image *pImg;
    int nRet;

    gj_vk_icd_soft_inc(&g_u32SoftPresentImgN);
    if (pSc == 0 || pSc->u32Magic != GJ_VK_SC_MAGIC ||
        u32Index >= pSc->u32ImageCount) {
        return -1;
    }
    if (!pSc->aImages[u32Index].u8Acquired) {
        return -1;
    }
    pImg = &pSc->aImages[u32Index].img;
    pSc->aImages[u32Index].u8Acquired = 0;
    nRet = gj_vk_queue_present(pImg);
    if (nRet == 0) {
        gj_vk_icd_soft_inc(&g_u32SoftPresentImgOk);
    }
    return nRet;
}

void *
gj_vk_image_pixels(gj_vk_swapchain_t pSc, uint32_t u32Index)
{
    gj_vk_icd_soft_inc(&g_u32SoftPixelsN);
    if (pSc == 0 || pSc->u32Magic != GJ_VK_SC_MAGIC ||
        u32Index >= pSc->u32ImageCount) {
        return 0;
    }
    return pSc->aImages[u32Index].img.pPixels;
}

#endif /* GJ_VK_KERNEL_SMOKE */

/*
 * Soft inventory blob (Wave 123). Shared host-stub + kernel smoke.
 * Grep: gj_vk_icd: soft inventory
 */
static const char g_szGjVkIcdSoftInventory[] =
    "gj_vk_icd: soft inventory wave=70 surfaces=16 areas=14 "
    "display=1 present=1 instance=1 device=1 swapchain=1 acquire=1 "
    "pixels=1 features=1 name=1 host=1 path=1 counts=1 note=1 area=1 "
    "feat=present_2d,swapchain,instance,named_export "
    "bar3=0 (soft inventory; not bar3)";

/*
 * Grep: gj_vk_icd: soft deepen
 */
static const char g_szGjVkIcdSoftDeepen[] =
    "gj_vk_icd: soft deepen wave=70 areas=14 "
    "display,present,instance,device,swapchain,acquire,pixels,"
    "features,name,host,path,counts,note,area "
    "hot_path=clean multi_server=0 confine=0 bar3=0";

/*
 * Grep: gj_vk_icd: soft path
 */
static const char g_szGjVkIcdSoftPath[] =
#ifdef GJ_VK_KERNEL_SMOKE
    "gj_vk_icd: soft path mode=kernel_smoke virtio_gpu=1 pools=inst,dev,sc "
    "freestanding=1 multi_server=0 confine=0 bar3=0 "
    "(soft inventory; not bar3)";
#else
    "gj_vk_icd: soft path mode=host_stub virtio_gpu=0 pools=none "
    "freestanding=1 multi_server=0 confine=0 bar3=0 "
    "(soft inventory; not bar3)";
#endif

/*
 * Grep: gj_vk_icd: soft honesty
 */
static const char g_szGjVkIcdSoftHonesty[] =
    "gj_vk_icd: soft honesty multi_server=0 confine=0 bar3=0 exclusive=1 "
    "soft=1 product_kernel=OPEN wave=70";

/* Soft area name catalog (Wave 111; cold only). */
static const char *const g_apszGjVkIcdSoftAreas[] = {
    "display",
    "present",
    "instance",
    "device",
    "swapchain",
    "acquire",
    "pixels",
    "features",
    "name",
    "host",
    "path",
    "counts",
    "note",
    "area",
};
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

/*
 * Cold soft inventory accessor. Grep: gj_vk_icd: soft inventory
 */
const char *
gj_vk_icd_soft_inventory(void)
{
    gj_vk_icd_soft_inc(&g_u32SoftInvN);
    return g_szGjVkIcdSoftInventory;
}

/*
 * Cold soft deepen stamp. Grep: gj_vk_icd: soft deepen
 */
const char *
gj_vk_icd_soft_deepen(void)
{
    return g_szGjVkIcdSoftDeepen;
}

/*
 * Cold soft path honesty. Grep: gj_vk_icd: soft path
 */
const char *
gj_vk_icd_soft_path(void)
{
    return g_szGjVkIcdSoftPath;
}

/*
 * Cold soft honesty line (Wave 123 exclusive). Grep: gj_vk_icd: soft honesty
 */
const char *
gj_vk_icd_soft_honesty(void)
{
    return g_szGjVkIcdSoftHonesty;
}

/*
 * Soft wave stamp (37). Grep: gj_vk_icd: soft wave=
 */
uint32_t
gj_vk_icd_soft_wave(void)
{
    return (uint32_t)GJ_VK_ICD_SOFT_WAVE;
}

/*
 * Soft convenience surface count. Grep: gj_vk_icd: soft surfaces=
 */
uint32_t
gj_vk_icd_soft_surface_count(void)
{
    return (uint32_t)GJ_VK_ICD_SOFT_SURFACES;
}

/*
 * Soft area catalog size. Grep: gj_vk_icd: soft areas=
 */
uint32_t
gj_vk_icd_soft_area_count(void)
{
    return (uint32_t)GJ_VK_ICD_SOFT_AREAS;
}

/*
 * Soft area name by index (0..areas-1), or NULL.
 * Grep: gj_vk_icd: soft areas=
 */
const char *
gj_vk_icd_soft_area_name(uint32_t uArea)
{
    if (uArea >= (uint32_t)GJ_VK_ICD_SOFT_AREAS) {
        return 0;
    }
    return g_apszGjVkIcdSoftAreas[uArea];
}

/*
 * Soft note counters (optional out args; NULL soft-skipped).
 * Grep: gj_vk_icd: soft note
 */
void
gj_vk_icd_soft_note_counts(uint32_t *pPresent, uint32_t *pPresentOk,
                           uint32_t *pCreateInst, uint32_t *pCreateDev,
                           uint32_t *pCreateSc, uint32_t *pAcquire,
                           uint32_t *pInv)
{
    if (pPresent != 0) {
        *pPresent = g_u32SoftPresentN;
    }
    if (pPresentOk != 0) {
        *pPresentOk = g_u32SoftPresentOk;
    }
    if (pCreateInst != 0) {
        *pCreateInst = g_u32SoftCreateInstN;
    }
    if (pCreateDev != 0) {
        *pCreateDev = g_u32SoftCreateDevN;
    }
    if (pCreateSc != 0) {
        *pCreateSc = g_u32SoftCreateScN;
    }
    if (pAcquire != 0) {
        *pAcquire = g_u32SoftAcquireN;
    }
    if (pInv != 0) {
        *pInv = g_u32SoftInvN;
    }
    (void)g_u32SoftPresentMiss;
    (void)g_u32SoftCreateInstOk;
    (void)g_u32SoftDestroyInstN;
    (void)g_u32SoftCreateDevOk;
    (void)g_u32SoftDestroyDevN;
    (void)g_u32SoftCreateScOk;
    (void)g_u32SoftDestroyScN;
    (void)g_u32SoftAcquireOk;
    (void)g_u32SoftPresentImgN;
    (void)g_u32SoftPresentImgOk;
    (void)g_u32SoftPixelsN;
    (void)g_u32SoftDisplayN;
}
