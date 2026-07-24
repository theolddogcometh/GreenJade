/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * GreenJade Vulkan ICD — clean-room subset for Proton A1 T0 (virtio-gpu 2D).
 * Khronos-shaped entry points plus loader ICD negotiate.
 *
 * Kernel smoke (GJ_VK_KERNEL_SMOKE) logs markers grepped by scripts/smoke-all.sh:
 *   "vk: QueuePresentKHR", "vk: QueueSubmit", "vk_icd: negotiate"
 * Do not change those substrings without updating the smoke harness.
 *
 * Soft inventory (Wave 63 exclusive deepen; greppable; not bar3 GPU):
 *   vk_icd: soft inventory wave=63 negotiate=… present=… submit=… acquire=…
 *   vk_icd: soft deepen wave=63 areas=negotiate,instance,device,swapchain,
 *           acquire,present,submit,lookup,host,path,counts,features,note,crc
 *   vk_icd: soft path software_present|virtio_gpu bar3=0
 * Soft counters wrap OK; never hard-gate product returns.
 * greppable: "vk_icd: soft"
 */
#include <vulkan/vulkan_core_gj.h>
#include <vulkan/vk_icd.h>
#include <gj_vk_icd.h>

#ifdef GJ_VK_KERNEL_SMOKE
#include <gj/config.h>
#include <gj/kheap.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/virtio_gpu.h>
#include <gj/vmm.h>
#else
#include <stdlib.h>
#include <string.h>
typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;
#endif

#define GJ_VK_MAX_INST     2u
#define GJ_VK_MAX_DEV      2u
#define GJ_VK_MAX_SC       2u
#define GJ_VK_MAX_CMD      8u
#define GJ_VK_MAX_FENCE    8u
#define GJ_VK_MAX_SEM      8u
#define GJ_VK_SC_IMAGES    3u

struct VkInstance_T {
    uint32_t u32Magic;
    uint32_t u32ApiVersion;
    uint32_t u32EnabledExt;
};

struct VkPhysicalDevice_T {
    uint32_t u32Magic;
    VkInstance pInst;
};

struct VkDevice_T {
    uint32_t u32Magic;
    VkInstance pInst;
    VkPhysicalDevice pPhys;
    uint32_t u32W;
    uint32_t u32H;
};

struct VkQueue_T {
    uint32_t u32Magic;
    VkDevice pDev;
};

#define GJ_VK_CMD_MAX_OPS 16u
enum gj_vk_cmd_op {
    GJ_VK_OP_NOP = 0,
    GJ_VK_OP_CLEAR = 1,
    GJ_VK_OP_FILL_RECT = 2,
    GJ_VK_OP_TRI = 3,    /* solid integer barycentric triangle */
    GJ_VK_OP_TRI_UV = 4  /* textured triangle; UV fixed-point 0..255 */
};

struct gj_vk_cmd_record {
    uint32_t u32Op;
    VkImage hImage;
    uint32_t u32ClearRgba; /* packed BGRA (T0 freestanding color) */
    uint32_t u32X, u32Y, u32W, u32H;
    int32_t aVx[3];
    int32_t aVy[3];
    /* TRI_UV only: 0..255 maps across full texture extent */
    uint8_t aU[3];
    uint8_t aV[3];
    VkImage hTex;
};

struct VkCommandBuffer_T {
    uint32_t u32Magic;
    VkDevice pDev;
    uint32_t u8Recording;
    uint32_t u32NOps;
    struct gj_vk_cmd_record aOps[GJ_VK_CMD_MAX_OPS];
};

struct gj_img_obj {
    uint32_t u32Magic;
    VkDevice pDev;
    uint32_t u32W;
    uint32_t u32H;
    uint32_t u32Stride;
    void *pPixels;
    uint8_t u8Host;
    uint8_t u8Pad[3];
};

struct gj_mem_obj {
    uint32_t u32Magic;
    void *pHost;
    VkDeviceSize size;
};

struct gj_renderpass {
    uint32_t u32Magic;
    uint32_t u32ClearRgba;
};

struct gj_framebuffer {
    uint32_t u32Magic;
    VkImage hImage;
    uint32_t u32W;
    uint32_t u32H;
};

struct gj_shader {
    uint32_t u32Magic;
    uint32_t u32WordCount;
    uint32_t aWords[256];
    uint32_t u32ExecMask;  /* bit0=Vertex bit1=Fragment */
    uint32_t u32SpecColor; /* SpecId 0 → packed BGRA */
    uint8_t u8HasSpecColor;
    uint8_t u8ValidSpv;
};

struct gj_pipeline {
    uint32_t u32Magic;
    uint32_t u32ClearRgba; /* solid / FS SpecId color for T0 */
    VkShaderModule hVs;
    VkShaderModule hFs;
    uint8_t u8HasSpirv;
};

struct gj_buffer {
    uint32_t u32Magic;
    void *pHost;
    VkDeviceSize size;
    uint8_t u8Bound;
};

struct gj_sc_img {
    VkImage hImage;
    void *pPixels;
    uint32_t u32Stride;
    uint8_t u8Acquired;
    uint8_t u8Alive;
    uint8_t u8Presented; /* soft: last present completed for this image */
    uint8_t u8Pad;
};

struct gj_swapchain {
    uint32_t u32Magic;
    VkDevice pDev;
    VkSurfaceKHR surface;
    uint32_t u32W;
    uint32_t u32H;
    uint32_t u32Count;
    uint32_t u32Cursor;
    uint32_t u32AcquiredN; /* soft: live acquire count */
    uint32_t u32PresentN;  /* soft: successful presents on this SC */
    VkFormat format;
    VkPresentModeKHR presentMode;
    VkImageUsageFlags usage;
    uint8_t u8Clipped;
    uint8_t u8Pad[3];
    struct gj_sc_img aImg[GJ_VK_SC_IMAGES];
};

struct gj_surface {
    uint32_t u32Magic;
    VkInstance pInst;
    uint32_t u32W;
    uint32_t u32H;
};

struct gj_fence {
    uint32_t u32Magic;
    uint8_t u8Signaled;
};

struct gj_sem {
    uint32_t u32Magic;
    uint8_t u8Signaled;
};

#define MAGIC_INST 0x54534e49u /* INST */
#define MAGIC_PHYS 0x53594850u /* PHYS */
#define MAGIC_DEV  0x564544u   /* DEV\0 */
#define MAGIC_QUE  0x55455551u /* QUEU */
#define MAGIC_CMD  0x444d43u   /* CMD\0 */
#define MAGIC_SC   0x50415753u /* SWAP */
#define MAGIC_SURF 0x46525553u /* SURF */
#define MAGIC_FEN  0x434e4546u /* FENC */
#define MAGIC_SEM  0x504d4553u /* SEMP */
#define MAGIC_IMG  0x474d49u   /* IMG */
#define MAGIC_MEM  0x4d454du   /* MEM */
#define MAGIC_RP   0x5052u     /* RP */
#define MAGIC_FB   0x4246u     /* FB */
#define MAGIC_PIPE 0x45504950u /* PIPE */
#define MAGIC_SHD  0x444853u   /* SHD */
#define MAGIC_BUF  0x465542u   /* BUF */

#define GJ_VK_MAX_IMG_OBJ 16u
#define GJ_VK_MAX_MEM     16u
#define GJ_VK_MAX_RP      4u
#define GJ_VK_MAX_FB      8u
#define GJ_VK_MAX_PIPE    8u
#define GJ_VK_MAX_SHADER  8u
#define GJ_VK_MAX_BUF     8u

/* SPIR-V opcodes (subset) */
#define SPV_OP_MEMORY_MODEL  14u
#define SPV_OP_ENTRY_POINT   15u
#define SPV_OP_TYPE_VOID     19u
#define SPV_OP_TYPE_INT      21u
#define SPV_OP_CONSTANT      43u
#define SPV_OP_SPEC_CONSTANT 50u
#define SPV_OP_FUNCTION      54u
#define SPV_OP_FUNCTION_END  56u
#define SPV_OP_DECORATE      71u
#define SPV_OP_LABEL         248u
#define SPV_OP_RETURN        253u
#define SPV_OP_CAPABILITY    17u
#define SPV_DECORATION_SPEC_ID 1u
#define SPV_EXEC_VERTEX      0u
#define SPV_EXEC_FRAGMENT    4u

static struct VkInstance_T g_aInst[GJ_VK_MAX_INST];
static u8 g_aInstUsed[GJ_VK_MAX_INST];
static struct VkPhysicalDevice_T g_Phys;
static int g_fPhysInit;
static struct VkDevice_T g_aDev[GJ_VK_MAX_DEV];
static u8 g_aDevUsed[GJ_VK_MAX_DEV];
static struct VkQueue_T g_aQue[GJ_VK_MAX_DEV];
static struct VkCommandBuffer_T g_aCmd[GJ_VK_MAX_CMD];
static u8 g_aCmdUsed[GJ_VK_MAX_CMD];
static struct gj_swapchain g_aSc[GJ_VK_MAX_SC];
static u8 g_aScUsed[GJ_VK_MAX_SC];
static struct gj_surface g_aSurf[GJ_VK_MAX_INST];
static u8 g_aSurfUsed[GJ_VK_MAX_INST];
static struct gj_fence g_aFence[GJ_VK_MAX_FENCE];
static u8 g_aFenceUsed[GJ_VK_MAX_FENCE];
static struct gj_sem g_aSem[GJ_VK_MAX_SEM];
static u8 g_aSemUsed[GJ_VK_MAX_SEM];
static struct gj_img_obj g_aImgObj[GJ_VK_MAX_IMG_OBJ];
static u8 g_aImgObjUsed[GJ_VK_MAX_IMG_OBJ];
static struct gj_mem_obj g_aMem[GJ_VK_MAX_MEM];
static u8 g_aMemUsed[GJ_VK_MAX_MEM];
static struct gj_renderpass g_aRp[GJ_VK_MAX_RP];
static u8 g_aRpUsed[GJ_VK_MAX_RP];
static struct gj_framebuffer g_aFb[GJ_VK_MAX_FB];
static u8 g_aFbUsed[GJ_VK_MAX_FB];
static struct gj_pipeline g_aPipe[GJ_VK_MAX_PIPE];
static u8 g_aPipeUsed[GJ_VK_MAX_PIPE];
static struct gj_shader g_aShd[GJ_VK_MAX_SHADER];
static u8 g_aShdUsed[GJ_VK_MAX_SHADER];
static struct gj_buffer g_aBuf[GJ_VK_MAX_BUF];
static u8 g_aBufUsed[GJ_VK_MAX_BUF];
static uint64_t g_u64NextNd = 1; /* non-dispatchable handle counter */
/* Active recording state for secondary cmds during renderpass */
static VkImage g_hActiveFbImage;
static uint32_t g_u32ActiveClear;
static uint8_t g_u8ActiveHasSpirv;
/* Dynamic state (T0: global recording state) */
static int32_t g_iVpX, g_iVpY;
static uint32_t g_u32VpW, g_u32VpH;
static uint8_t g_u8VpSet;
static uint32_t g_u32PushColor;
static uint8_t g_u8PushColorSet;
static void *g_pVboHost;
static VkDeviceSize g_vboOff;
static VkDeviceSize g_vboSize;
static uint8_t g_u8VboBound;
static void *g_pIboHost;
static VkDeviceSize g_iboOff;
static uint8_t g_u8IboBound;
static uint8_t g_u8IboU16; /* 1 = uint16 indices, 0 = uint32 */
/* Bound sampled image (software FS texture) */
static VkImage g_hActiveTex;
static uint8_t g_u8TexBound;
static uint8_t g_u8Bilinear; /* set by sampler create filter */

static int
streq(const char *a, const char *b)
{
    if (a == NULL || b == NULL) {
        return 0;
    }
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return *a == *b;
}

static void
copy_str(char *dst, size_t cb, const char *src)
{
    size_t i;

    if (dst == NULL || cb == 0) {
        return;
    }
    for (i = 0; i + 1 < cb && src != NULL && src[i] != '\0'; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

/* Resolve swapchain or standalone image handle to CPU backing. */
static int
img_resolve(VkImage hImage, void **ppPix, uint32_t *pStride, uint32_t *pW,
            uint32_t *pH)
{
    uint32_t i;
    uint32_t j;

    if (hImage == 0) {
        return -1;
    }
    for (i = 0; i < GJ_VK_MAX_SC; i++) {
        if (!g_aScUsed[i] || g_aSc[i].u32Magic != MAGIC_SC) {
            continue;
        }
        for (j = 0; j < g_aSc[i].u32Count; j++) {
            if (g_aSc[i].aImg[j].hImage == hImage) {
                if (ppPix) {
                    *ppPix = g_aSc[i].aImg[j].pPixels;
                }
                if (pStride) {
                    *pStride = g_aSc[i].aImg[j].u32Stride;
                }
                if (pW) {
                    *pW = g_aSc[i].u32W;
                }
                if (pH) {
                    *pH = g_aSc[i].u32H;
                }
                return 0;
            }
        }
    }
    for (i = 0; i < GJ_VK_MAX_IMG_OBJ; i++) {
        if (g_aImgObjUsed[i] && g_aImgObj[i].u32Magic == MAGIC_IMG &&
            (VkImage)(uintptr_t)(void *)&g_aImgObj[i] == hImage) {
            if (ppPix) {
                *ppPix = g_aImgObj[i].pPixels;
            }
            if (pStride) {
                *pStride = g_aImgObj[i].u32Stride;
            }
            if (pW) {
                *pW = g_aImgObj[i].u32W;
            }
            if (pH) {
                *pH = g_aImgObj[i].u32H;
            }
            return 0;
        }
    }
    /* Standalone images: match pointer handle forms used by this ICD. */
    for (i = 0; i < GJ_VK_MAX_IMG_OBJ; i++) {
        if (g_aImgObjUsed[i] && g_aImgObj[i].u32Magic == MAGIC_IMG) {
            VkImage h = (VkImage)(uintptr_t)(i + 1);
            if ((void *)(uintptr_t)hImage == (void *)&g_aImgObj[i] ||
                hImage == (VkImage)(uintptr_t)(void *)&g_aImgObj[i]) {
                if (ppPix) {
                    *ppPix = g_aImgObj[i].pPixels;
                }
                if (pStride) {
                    *pStride = g_aImgObj[i].u32Stride;
                }
                if (pW) {
                    *pW = g_aImgObj[i].u32W;
                }
                if (pH) {
                    *pH = g_aImgObj[i].u32H;
                }
                (void)h;
                return 0;
            }
        }
    }
    return -1;
}

static void
cpu_clear(void *pPix, uint32_t stride, uint32_t w, uint32_t h, uint32_t rgba)
{
    uint32_t y;
    uint32_t x;
    u8 *p = (u8 *)pPix;
    u8 b = (u8)(rgba & 0xffu);
    u8 g = (u8)((rgba >> 8) & 0xffu);
    u8 r = (u8)((rgba >> 16) & 0xffu);
    u8 a = (u8)((rgba >> 24) & 0xffu);

    if (p == NULL || stride == 0) {
        return;
    }
    for (y = 0; y < h; y++) {
        u8 *row = p + y * stride;

        for (x = 0; x < w; x++) {
            row[x * 4u + 0] = b;
            row[x * 4u + 1] = g;
            row[x * 4u + 2] = r;
            row[x * 4u + 3] = a;
        }
    }
}

/* Integer edge function for barycentric raster (no FP) */
static int64_t
edge_fn(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x, int32_t y)
{
    return (int64_t)(x1 - x0) * (int64_t)(y - y0) -
           (int64_t)(x - x0) * (int64_t)(y1 - y0);
}

static void
tri_bbox(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
         uint32_t w, uint32_t h, int32_t *pMinx, int32_t *pMiny, int32_t *pMaxx,
         int32_t *pMaxy)
{
    int32_t minx = x0;
    int32_t maxx = x0;
    int32_t miny = y0;
    int32_t maxy = y0;

    if (x1 < minx) {
        minx = x1;
    }
    if (x2 < minx) {
        minx = x2;
    }
    if (x1 > maxx) {
        maxx = x1;
    }
    if (x2 > maxx) {
        maxx = x2;
    }
    if (y1 < miny) {
        miny = y1;
    }
    if (y2 < miny) {
        miny = y2;
    }
    if (y1 > maxy) {
        maxy = y1;
    }
    if (y2 > maxy) {
        maxy = y2;
    }
    if (minx < 0) {
        minx = 0;
    }
    if (miny < 0) {
        miny = 0;
    }
    if (maxx >= (int32_t)w) {
        maxx = (int32_t)w - 1;
    }
    if (maxy >= (int32_t)h) {
        maxy = (int32_t)h - 1;
    }
    *pMinx = minx;
    *pMiny = miny;
    *pMaxx = maxx;
    *pMaxy = maxy;
}

static void
cpu_fill_tri(void *pPix, uint32_t stride, uint32_t w, uint32_t h, int32_t x0,
             int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
             uint32_t rgba)
{
    int32_t minx, miny, maxx, maxy;
    int32_t x, y;
    u8 *p = (u8 *)pPix;
    u8 b = (u8)(rgba & 0xffu);
    u8 g = (u8)((rgba >> 8) & 0xffu);
    u8 r = (u8)((rgba >> 16) & 0xffu);
    u8 a = (u8)((rgba >> 24) & 0xffu);
    int64_t area;

    if (p == NULL || stride == 0 || w == 0 || h == 0) {
        return;
    }
    area = edge_fn(x0, y0, x1, y1, x2, y2);
    if (area == 0) {
        return;
    }
    tri_bbox(x0, y0, x1, y1, x2, y2, w, h, &minx, &miny, &maxx, &maxy);
    for (y = miny; y <= maxy; y++) {
        for (x = minx; x <= maxx; x++) {
            int64_t w0 = edge_fn(x1, y1, x2, y2, x, y);
            int64_t w1 = edge_fn(x2, y2, x0, y0, x, y);
            int64_t w2 = edge_fn(x0, y0, x1, y1, x, y);
            int inside;

            if (area > 0) {
                inside = (w0 >= 0 && w1 >= 0 && w2 >= 0);
            } else {
                inside = (w0 <= 0 && w1 <= 0 && w2 <= 0);
            }
            if (inside) {
                u8 *px = p + (uint32_t)y * stride + (uint32_t)x * 4u;

                px[0] = b;
                px[1] = g;
                px[2] = r;
                px[3] = a;
            }
        }
    }
}

/* Nearest sample: u,v in 0..255 map to a texel. */
static uint32_t
tex_sample_uv(const u8 *pTex, uint32_t texStride, uint32_t tw, uint32_t th,
              uint32_t u, uint32_t v)
{
    uint32_t tx;
    uint32_t ty;
    const u8 *px;

    if (pTex == NULL || tw == 0 || th == 0) {
        return 0xff808080u;
    }
    if (u > 255u) {
        u = 255u;
    }
    if (v > 255u) {
        v = 255u;
    }
    tx = (u * (tw - 1u)) / 255u;
    ty = (v * (th - 1u)) / 255u;
    px = pTex + ty * texStride + tx * 4u;
    return (uint32_t)px[0] | ((uint32_t)px[1] << 8) | ((uint32_t)px[2] << 16) |
           ((uint32_t)px[3] << 24);
}

/*
 * Integer bilinear: sample four neighbors; weights from fractional bits
 * of u,v after mapping 0..255 into texel space.
 */
static uint32_t
tex_sample_bilinear(const u8 *pTex, uint32_t texStride, uint32_t tw, uint32_t th,
                    uint32_t u, uint32_t v)
{
    uint32_t fx, fy; /* 0..255 sub-texel */
    uint32_t x0, y0, x1, y1;
    uint32_t i;
    u8 out[4];
    const u8 *p00;
    const u8 *p10;
    const u8 *p01;
    const u8 *p11;

    if (pTex == NULL || tw == 0 || th == 0) {
        return 0xff808080u;
    }
    if (u > 255u) {
        u = 255u;
    }
    if (v > 255u) {
        v = 255u;
    }
    /* Map to (tw-1)<<8 fixed-point for sub-texel weights. */
    {
        uint32_t xu = (u * ((tw - 1u) << 8)) / 255u;
        uint32_t yv = (v * ((th - 1u) << 8)) / 255u;

        x0 = xu >> 8;
        y0 = yv >> 8;
        fx = xu & 0xffu;
        fy = yv & 0xffu;
    }
    x1 = x0 + 1u;
    y1 = y0 + 1u;
    if (x1 >= tw) {
        x1 = tw - 1u;
    }
    if (y1 >= th) {
        y1 = th - 1u;
    }
    p00 = pTex + y0 * texStride + x0 * 4u;
    p10 = pTex + y0 * texStride + x1 * 4u;
    p01 = pTex + y1 * texStride + x0 * 4u;
    p11 = pTex + y1 * texStride + x1 * 4u;
    for (i = 0; i < 4u; i++) {
        uint32_t t0 = (uint32_t)p00[i] * (256u - fx) + (uint32_t)p10[i] * fx;
        uint32_t t1 = (uint32_t)p01[i] * (256u - fx) + (uint32_t)p11[i] * fx;

        t0 >>= 8;
        t1 >>= 8;
        out[i] = (u8)(((t0 * (256u - fy)) + (t1 * fy)) >> 8);
    }
    return (uint32_t)out[0] | ((uint32_t)out[1] << 8) | ((uint32_t)out[2] << 16) |
           ((uint32_t)out[3] << 24);
}

static void
cpu_fill_tri_uv(void *pPix, uint32_t stride, uint32_t w, uint32_t h, int32_t x0,
                int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                uint8_t u0, uint8_t v0, uint8_t u1, uint8_t v1, uint8_t u2,
                uint8_t v2, const u8 *pTex, uint32_t texStride, uint32_t tw,
                uint32_t th, int fBilinear)
{
    int32_t minx, miny, maxx, maxy;
    int32_t x, y;
    u8 *p = (u8 *)pPix;
    int64_t area;
    int64_t aAbs;

    if (p == NULL || stride == 0 || w == 0 || h == 0) {
        return;
    }
    area = edge_fn(x0, y0, x1, y1, x2, y2);
    if (area == 0) {
        return;
    }
    aAbs = area < 0 ? -area : area;
    tri_bbox(x0, y0, x1, y1, x2, y2, w, h, &minx, &miny, &maxx, &maxy);
    for (y = miny; y <= maxy; y++) {
        for (x = minx; x <= maxx; x++) {
            int64_t w0 = edge_fn(x1, y1, x2, y2, x, y);
            int64_t w1 = edge_fn(x2, y2, x0, y0, x, y);
            int64_t w2 = edge_fn(x0, y0, x1, y1, x, y);
            int inside;
            uint32_t rgba;
            u8 *px;

            if (area > 0) {
                inside = (w0 >= 0 && w1 >= 0 && w2 >= 0);
            } else {
                inside = (w0 <= 0 && w1 <= 0 && w2 <= 0);
            }
            if (!inside) {
                continue;
            }
            /* Barycentric UV: (w0*u0+w1*u1+w2*u2)/|area| */
            {
                int64_t ww0 = w0 < 0 ? -w0 : w0;
                int64_t ww1 = w1 < 0 ? -w1 : w1;
                int64_t ww2 = w2 < 0 ? -w2 : w2;
                uint32_t uu =
                    (uint32_t)((ww0 * (int64_t)u0 + ww1 * (int64_t)u1 +
                                ww2 * (int64_t)u2) /
                               aAbs);
                uint32_t vv =
                    (uint32_t)((ww0 * (int64_t)v0 + ww1 * (int64_t)v1 +
                                ww2 * (int64_t)v2) /
                               aAbs);

                if (fBilinear) {
                    rgba = tex_sample_bilinear(pTex, texStride, tw, th, uu, vv);
                } else {
                    rgba = tex_sample_uv(pTex, texStride, tw, th, uu, vv);
                }
            }
            px = p + (uint32_t)y * stride + (uint32_t)x * 4u;
            px[0] = (u8)(rgba & 0xffu);
            px[1] = (u8)((rgba >> 8) & 0xffu);
            px[2] = (u8)((rgba >> 16) & 0xffu);
            px[3] = (u8)((rgba >> 24) & 0xffu);
        }
    }
}

/*
 * Parse SPIR-V: validate magic, walk ops, collect EntryPoint models and
 * SpecId 0 → OpSpecConstant/OpConstant uint32 color (GreenJade T0 FS color).
 */
static int
spv_parse(const uint32_t *pCode, uint32_t nWords, struct gj_shader *pOut)
{
    uint32_t i;
    uint32_t specIdTarget = 0xffffffffu;
    uint8_t aIsInt[64];
    uint32_t bound;

    if (pOut == NULL || pCode == NULL || nWords < 5u) {
        return -1;
    }
    if (pCode[0] != GJ_SPV_MAGIC) {
        return -1;
    }
    bound = pCode[3];
    if (bound == 0 || bound > 4096u) {
        return -1;
    }
    memset(aIsInt, 0, sizeof(aIsInt));
    pOut->u32ExecMask = 0;
    pOut->u32SpecColor = 0;
    pOut->u8HasSpecColor = 0;
    pOut->u8ValidSpv = 1;

    i = 5;
    while (i < nWords) {
        uint32_t hdr = pCode[i];
        uint32_t op = hdr & 0xffffu;
        uint32_t wc = hdr >> 16;
        const uint32_t *w;

        if (wc == 0 || i + wc > nWords) {
            return -1;
        }
        w = &pCode[i];
        if (op == SPV_OP_ENTRY_POINT && wc >= 3u) {
            uint32_t model = w[1];

            if (model == SPV_EXEC_VERTEX) {
                pOut->u32ExecMask |= 1u;
            } else if (model == SPV_EXEC_FRAGMENT) {
                pOut->u32ExecMask |= 2u;
            }
        } else if (op == SPV_OP_DECORATE && wc >= 4u) {
            uint32_t target = w[1];
            uint32_t dec = w[2];

            if (dec == SPV_DECORATION_SPEC_ID && w[3] == 0u) {
                specIdTarget = target;
            }
        } else if (op == SPV_OP_TYPE_INT && wc >= 4u) {
            uint32_t id = w[1];
            uint32_t width = w[2];

            if (id < 64u && width == 32u) {
                aIsInt[id] = 1;
            }
        } else if ((op == SPV_OP_SPEC_CONSTANT || op == SPV_OP_CONSTANT) &&
                   wc >= 4u) {
            uint32_t typeId = w[1];
            uint32_t resId = w[2];
            uint32_t val = w[3];

            if (typeId < 64u && aIsInt[typeId] && resId == specIdTarget) {
                pOut->u32SpecColor = val;
                pOut->u8HasSpecColor = 1;
            }
            /* Also accept first uint SpecConstant as color if SpecId not set */
            if (!pOut->u8HasSpecColor && typeId < 64u && aIsInt[typeId] &&
                op == SPV_OP_SPEC_CONSTANT) {
                pOut->u32SpecColor = val;
                pOut->u8HasSpecColor = 1;
            }
            (void)resId;
        }
        i += wc;
    }
    return 0;
}

static struct gj_shader *
shd_from_handle(VkShaderModule h)
{
    struct gj_shader *p = (struct gj_shader *)(uintptr_t)h;

    if (p == NULL || p->u32Magic != MAGIC_SHD) {
        return NULL;
    }
    return p;
}

static void
cmd_execute(struct VkCommandBuffer_T *pCmd)
{
    uint32_t i;

    if (pCmd == NULL) {
        return;
    }
    for (i = 0; i < pCmd->u32NOps; i++) {
        struct gj_vk_cmd_record *pOp = &pCmd->aOps[i];
        void *pPix = NULL;
        uint32_t stride = 0;
        uint32_t w = 0;
        uint32_t h = 0;

        if (pOp->u32Op == GJ_VK_OP_CLEAR || pOp->u32Op == GJ_VK_OP_FILL_RECT ||
            pOp->u32Op == GJ_VK_OP_TRI || pOp->u32Op == GJ_VK_OP_TRI_UV) {
            if (img_resolve(pOp->hImage, &pPix, &stride, &w, &h) != 0) {
                continue;
            }
            if (pOp->u32Op == GJ_VK_OP_CLEAR) {
                cpu_clear(pPix, stride, w, h, pOp->u32ClearRgba);
            } else if (pOp->u32Op == GJ_VK_OP_TRI) {
                cpu_fill_tri(pPix, stride, w, h, pOp->aVx[0], pOp->aVy[0],
                             pOp->aVx[1], pOp->aVy[1], pOp->aVx[2],
                             pOp->aVy[2], pOp->u32ClearRgba);
            } else if (pOp->u32Op == GJ_VK_OP_TRI_UV) {
                void *pTex = NULL;
                uint32_t tStride = 0;
                uint32_t tw = 0;
                uint32_t th = 0;

                if (img_resolve(pOp->hTex, &pTex, &tStride, &tw, &th) != 0 ||
                    pTex == NULL) {
                    cpu_fill_tri(pPix, stride, w, h, pOp->aVx[0], pOp->aVy[0],
                                 pOp->aVx[1], pOp->aVy[1], pOp->aVx[2],
                                 pOp->aVy[2], pOp->u32ClearRgba);
                } else {
                    cpu_fill_tri_uv(pPix, stride, w, h, pOp->aVx[0],
                                    pOp->aVy[0], pOp->aVx[1], pOp->aVy[1],
                                    pOp->aVx[2], pOp->aVy[2], pOp->aU[0],
                                    pOp->aV[0], pOp->aU[1], pOp->aV[1],
                                    pOp->aU[2], pOp->aV[2], (const u8 *)pTex,
                                    tStride, tw, th, g_u8Bilinear ? 1 : 0);
                }
            } else {
                /* fill rect: clamp */
                uint32_t x0 = pOp->u32X;
                uint32_t y0 = pOp->u32Y;
                uint32_t ww = pOp->u32W;
                uint32_t hh = pOp->u32H;
                uint32_t y;
                uint32_t x;
                u8 *p = (u8 *)pPix;
                u8 b = (u8)(pOp->u32ClearRgba & 0xffu);
                u8 g = (u8)((pOp->u32ClearRgba >> 8) & 0xffu);
                u8 r = (u8)((pOp->u32ClearRgba >> 16) & 0xffu);
                u8 a = (u8)((pOp->u32ClearRgba >> 24) & 0xffu);

                if (x0 >= w || y0 >= h) {
                    continue;
                }
                if (x0 + ww > w) {
                    ww = w - x0;
                }
                if (y0 + hh > h) {
                    hh = h - y0;
                }
                for (y = 0; y < hh; y++) {
                    u8 *row = p + (y0 + y) * stride + x0 * 4u;

                    for (x = 0; x < ww; x++) {
                        row[x * 4u + 0] = b;
                        row[x * 4u + 1] = g;
                        row[x * 4u + 2] = r;
                        row[x * 4u + 3] = a;
                    }
                }
            }
        }
    }
}

static void *
fb_alloc(uint32_t w, uint32_t h, uint32_t *pStride)
{
    uint32_t bytes;
    uint32_t i;
    uint32_t n;
    u8 *p;

    *pStride = w * 4u;
    bytes = *pStride * h;
#ifdef GJ_VK_KERNEL_SMOKE
    {
        uint32_t pages;
        gj_paddr_t pa;

        pages = (bytes + GJ_PAGE_SIZE - 1) / GJ_PAGE_SIZE;
        pa = pmm_alloc_pages(pages);
        if (pa == 0) {
            pa = pmm_alloc();
            if (pa == 0) {
                return NULL;
            }
            w = 32;
            h = 32;
            *pStride = w * 4u;
            bytes = *pStride * h;
        }
        p = (u8 *)hhdm_to_virt(pa);
    }
#else
    /* Host build: allocate from the C heap. */
    p = (u8 *)malloc(bytes ? bytes : 4u);
    if (p == NULL) {
        return NULL;
    }
#endif
    n = (*pStride / 4u) * h;
    for (i = 0; i < n; i++) {
        p[i * 4u + 0] = 0x40;
        p[i * 4u + 1] = 0xc0;
        p[i * 4u + 2] = 0x20;
        p[i * 4u + 3] = 0xff;
    }
    return p;
}

/* Host free; kernel T0 does not reclaim PMM pages (soft destroy). */
static void
fb_free(void *p)
{
    if (p == NULL) {
        return;
    }
#ifndef GJ_VK_KERNEL_SMOKE
    free(p);
#else
    (void)p;
#endif
}

static void
sc_teardown_images(struct gj_swapchain *pSc)
{
    uint32_t j;

    if (pSc == NULL) {
        return;
    }
    for (j = 0; j < GJ_VK_SC_IMAGES; j++) {
        fb_free(pSc->aImg[j].pPixels);
        pSc->aImg[j].pPixels = NULL;
        pSc->aImg[j].u32Stride = 0;
        pSc->aImg[j].u8Alive = 0;
        pSc->aImg[j].u8Acquired = 0;
        pSc->aImg[j].u8Presented = 0;
        pSc->aImg[j].hImage = 0;
    }
    pSc->u32Count = 0;
    pSc->u32Cursor = 0;
    pSc->u32AcquiredN = 0;
    pSc->u32PresentN = 0;
}

static void
sem_soft_signal(VkSemaphore semaphore, uint8_t u8On)
{
    struct gj_sem *pS = (struct gj_sem *)(uintptr_t)semaphore;

    if (pS != NULL && pS->u32Magic == MAGIC_SEM) {
        pS->u8Signaled = u8On ? 1u : 0u;
    }
}

static void
present_wait_sems(const VkPresentInfoKHR *pPresentInfo)
{
    uint32_t i;

    if (pPresentInfo == NULL || pPresentInfo->pWaitSemaphores == NULL) {
        return;
    }
    for (i = 0; i < pPresentInfo->waitSemaphoreCount; i++) {
        /* Soft binary wait: unsignal; do not fail if already clear. */
        sem_soft_signal(pPresentInfo->pWaitSemaphores[i], 0);
    }
}

/* Soft CRC of four corner pixels (host software-present fingerprint). */
#ifndef GJ_VK_KERNEL_SMOKE
static uint32_t
soft_fb_crc(const void *pPix, uint32_t w, uint32_t h, uint32_t stride)
{
    const u8 *p = (const u8 *)pPix;
    uint32_t crc = 2166136261u;
    uint32_t ax[4];
    uint32_t ay[4];
    uint32_t k;

    if (p == NULL || w == 0 || h == 0 || stride < 4u) {
        return 0;
    }
    ax[0] = 0;
    ay[0] = 0;
    ax[1] = w - 1u;
    ay[1] = 0;
    ax[2] = 0;
    ay[2] = h - 1u;
    ax[3] = w - 1u;
    ay[3] = h - 1u;
    for (k = 0; k < 4u; k++) {
        const u8 *px = p + ay[k] * stride + ax[k] * 4u;
        uint32_t c;

        c = (uint32_t)px[0] | ((uint32_t)px[1] << 8) | ((uint32_t)px[2] << 16) |
            ((uint32_t)px[3] << 24);
        crc ^= c;
        crc *= 16777619u;
        crc ^= (ax[k] << 16) | ay[k];
        crc *= 16777619u;
    }
    crc ^= w;
    crc *= 16777619u;
    crc ^= h;
    crc *= 16777619u;
    return crc ? crc : 1u;
}
#endif /* !GJ_VK_KERNEL_SMOKE */

static uint32_t g_u32HostPresents;
static uint32_t g_u32HostPresentCrc;

/*
 * Wave 63 soft inventory counters (file-local; wrap OK; never hard-gate).
 * Grep: vk_icd: soft
 */
#define VK_ICD_SOFT_WAVE 63u
#define VK_ICD_SOFT_AREAS    14u
#define VK_ICD_SOFT_SURFACES 10u /* + note + crc over wave-14 core set */

static volatile uint32_t g_u32SoftNegotiateN;
static volatile uint32_t g_u32SoftCreateInstN;
static volatile uint32_t g_u32SoftCreateDevN;
static volatile uint32_t g_u32SoftCreateScN;
static volatile uint32_t g_u32SoftAcquireN;
static volatile uint32_t g_u32SoftAcquireOk;
static volatile uint32_t g_u32SoftPresentN;
static volatile uint32_t g_u32SoftPresentOk;
static volatile uint32_t g_u32SoftPresentMiss;
static volatile uint32_t g_u32SoftSubmitN;
static volatile uint32_t g_u32SoftLookupN;
static volatile uint32_t g_u32SoftLookupHit;
static volatile uint32_t g_u32SoftLookupMiss;
static volatile uint32_t g_u32SoftInvN;
static volatile uint32_t g_u32SoftDeepenN;
static volatile uint32_t g_u32SoftPathN;

static void
vk_icd_soft_inc(volatile uint32_t *pCtr)
{
    if (pCtr != 0) {
        (*pCtr)++;
    }
}

/*
 * Soft inventory blob (Wave 63). Grep: vk_icd: soft inventory
 */
static const char g_szVkIcdSoftInventory[] =
    "vk_icd: soft inventory wave=63 surfaces=10 areas=14 "
    "negotiate=1 instance=1 device=1 swapchain=1 acquire=1 present=1 "
    "submit=1 lookup=1 host=1 path=1 counts=1 features=1 note=1 crc=1 "
    "bar3=0 (soft inventory; not bar3)";

/*
 * Grep: vk_icd: soft deepen
 */
static const char g_szVkIcdSoftDeepen[] =
    "vk_icd: soft deepen wave=63 areas=14 "
    "negotiate,instance,device,swapchain,acquire,present,submit,"
    "lookup,host,path,counts,features,note,crc "
    "software_present=1 multi_server=0 confine=0 bar3=0";

/*
 * Grep: vk_icd: soft path
 */
static const char g_szVkIcdSoftPath[] =
#ifdef GJ_VK_KERNEL_SMOKE
    "vk_icd: soft path mode=kernel_smoke virtio_gpu=1 "
    "present=virtio software_crc=0 multi_server=0 confine=0 "
    "bar3=0 (soft inventory; not bar3)";
#else
    "vk_icd: soft path mode=host software_present=1 "
    "present=soft_fb_crc multi_server=0 confine=0 "
    "bar3=0 (soft inventory; not bar3)";
#endif

/*
 * Grep: vk_icd: soft honesty
 */
static const char g_szVkIcdSoftHonesty[] =
    "vk_icd: soft honesty multi_server=0 confine=0 bar3=0 exclusive=1 "
    "soft=1 product_kernel=OPEN wave=63";

/* Soft area name catalog (Wave 63; cold only). */
/* Wave 63 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retdoubletenaille continuum_toward=20500 soft_ne_product=1
 *   greppable: soft retplaceofarms exclusive=1 continuum_toward=20500
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

static const char *const g_apszVkIcdSoftAreas[] = {
    "negotiate",
    "instance",
    "device",
    "swapchain",
    "acquire",
    "present",
    "submit",
    "lookup",
    "host",
    "path",
    "counts",
    "features",
    "note",
    "crc",
};
static VkResult
present_pixels(void *pPix, uint32_t w, uint32_t h, uint32_t stride)
{
    vk_icd_soft_inc(&g_u32SoftPresentN);
    if (pPix == NULL || w == 0 || h == 0) {
        vk_icd_soft_inc(&g_u32SoftPresentMiss);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (stride == 0) {
        stride = w * 4u;
    }
#ifdef GJ_VK_KERNEL_SMOKE
    if (!virtio_gpu_ready()) {
        vk_icd_soft_inc(&g_u32SoftPresentMiss);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (virtio_gpu_present(w, h, pPix, stride) != 0) {
        vk_icd_soft_inc(&g_u32SoftPresentMiss);
        return VK_ERROR_DEVICE_LOST;
    }
    vk_icd_soft_inc(&g_u32SoftPresentOk);
    return VK_SUCCESS;
#else
    /* Host ICD: software present — touch corners, CRC, count frames. */
    {
        volatile u8 *p = (volatile u8 *)pPix;
        volatile u8 sink;
        uint32_t crc;

        sink = p[0];
        p[0] = sink;
        if (h > 0 && stride >= 4u) {
            sink = p[(h - 1u) * stride];
            (void)sink;
            if (w > 1u) {
                sink = p[(w - 1u) * 4u];
                (void)sink;
                sink = p[(h - 1u) * stride + (w - 1u) * 4u];
                (void)sink;
            }
        }
        crc = soft_fb_crc(pPix, w, h, stride);
        g_u32HostPresentCrc = (g_u32HostPresentCrc * 33u) ^ crc;
        if (g_u32HostPresentCrc == 0) {
            g_u32HostPresentCrc = 1u;
        }
        g_u32HostPresents++;
        vk_icd_soft_inc(&g_u32SoftPresentOk);
    }
    return VK_SUCCESS;
#endif
}

/* Host query for software-present frame count (tests / smoke). */
uint32_t
gj_vk_host_present_count(void)
{
    return g_u32HostPresents;
}

/* Host query for software-present soft CRC (tests / smoke). */
uint32_t
gj_vk_host_present_crc(void)
{
    return g_u32HostPresentCrc;
}

/*
 * Cold soft inventory accessor. Grep: vk_icd: soft inventory
 */
const char *
gj_vk_icd_loader_soft_inventory(void)
{
    vk_icd_soft_inc(&g_u32SoftInvN);
    return g_szVkIcdSoftInventory;
}

/*
 * Cold soft deepen stamp. Grep: vk_icd: soft deepen
 */
const char *
gj_vk_icd_loader_soft_deepen(void)
{
    vk_icd_soft_inc(&g_u32SoftDeepenN);
    return g_szVkIcdSoftDeepen;
}

/*
 * Cold soft path honesty. Grep: vk_icd: soft path
 */
const char *
gj_vk_icd_loader_soft_path(void)
{
    vk_icd_soft_inc(&g_u32SoftPathN);
    return g_szVkIcdSoftPath;
}

/*
 * Cold soft honesty line (Wave 63 exclusive). Grep: vk_icd: soft honesty
 */
const char *
gj_vk_icd_loader_soft_honesty(void)
{
    return g_szVkIcdSoftHonesty;
}

/*
 * Soft wave stamp (37). Grep: vk_icd: soft wave=
 */
uint32_t
gj_vk_icd_loader_soft_wave(void)
{
    return (uint32_t)VK_ICD_SOFT_WAVE;
}

/*
 * Soft surface / area counts. Grep: vk_icd: soft surfaces= / areas=
 */
uint32_t
gj_vk_icd_loader_soft_surface_count(void)
{
    return (uint32_t)VK_ICD_SOFT_SURFACES;
}

uint32_t
gj_vk_icd_loader_soft_area_count(void)
{
    return (uint32_t)VK_ICD_SOFT_AREAS;
}

/*
 * Soft area name by index (0..areas-1), or NULL.
 * Grep: vk_icd: soft areas=
 */
const char *
gj_vk_icd_loader_soft_area_name(uint32_t uArea)
{
    if (uArea >= (uint32_t)VK_ICD_SOFT_AREAS) {
        return 0;
    }
    return g_apszVkIcdSoftAreas[uArea];
}

/*
 * Soft note counters (optional out; NULL soft-skipped).
 * Grep: vk_icd: soft note
 */
void
gj_vk_icd_loader_soft_note_counts(uint32_t *pNegotiate, uint32_t *pPresent,
                                  uint32_t *pPresentOk, uint32_t *pSubmit,
                                  uint32_t *pAcquire, uint32_t *pLookup,
                                  uint32_t *pInv)
{
    if (pNegotiate != 0) {
        *pNegotiate = g_u32SoftNegotiateN;
    }
    if (pPresent != 0) {
        *pPresent = g_u32SoftPresentN;
    }
    if (pPresentOk != 0) {
        *pPresentOk = g_u32SoftPresentOk;
    }
    if (pSubmit != 0) {
        *pSubmit = g_u32SoftSubmitN;
    }
    if (pAcquire != 0) {
        *pAcquire = g_u32SoftAcquireN;
    }
    if (pLookup != 0) {
        *pLookup = g_u32SoftLookupN;
    }
    if (pInv != 0) {
        *pInv = g_u32SoftInvN;
    }
    (void)g_u32SoftCreateInstN;
    (void)g_u32SoftCreateDevN;
    (void)g_u32SoftCreateScN;
    (void)g_u32SoftAcquireOk;
    (void)g_u32SoftPresentMiss;
    (void)g_u32SoftLookupHit;
    (void)g_u32SoftLookupMiss;
    (void)g_u32SoftDeepenN;
    (void)g_u32SoftPathN;
}

/*
 * Soft host present CRC note (Wave 63 deepen). Grep: vk_icd: soft crc
 */
uint32_t
gj_vk_icd_loader_soft_crc_note(void)
{
    return g_u32HostPresentCrc;
}

/* ---- Extension names advertised by this ICD ---- */
static const char *const g_aszInstExt[] = {
    "VK_KHR_surface",
    "VK_KHR_get_physical_device_properties2",
};
static const char *const g_aszDevExt[] = {
    "VK_KHR_swapchain",
};

/* ========================= Core API ========================= */

VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount,
                                       VkExtensionProperties *pProps)
{
    uint32_t n = (uint32_t)(sizeof(g_aszInstExt) / sizeof(g_aszInstExt[0]));
    uint32_t i;

    (void)pLayerName;
    if (pCount == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (pProps == NULL) {
        *pCount = n;
        return VK_SUCCESS;
    }
    if (*pCount < n) {
        *pCount = n;
        return VK_INCOMPLETE;
    }
    for (i = 0; i < n; i++) {
        memset(&pProps[i], 0, sizeof(pProps[i]));
        copy_str(pProps[i].extensionName, sizeof(pProps[i].extensionName),
                 g_aszInstExt[i]);
        pProps[i].specVersion = 25;
    }
    *pCount = n;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceLayerProperties(uint32_t *pCount, VkLayerProperties *pProps)
{
    if (pCount == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (pProps == NULL) {
        *pCount = 0;
        return VK_SUCCESS;
    }
    *pCount = 0;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo,
                 const VkAllocationCallbacks *pAllocator, VkInstance *pInstance)
{
    uint32_t i;

    vk_icd_soft_inc(&g_u32SoftCreateInstN);
    (void)pAllocator;
    if (pCreateInfo == NULL || pInstance == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    for (i = 0; i < GJ_VK_MAX_INST; i++) {
        if (!g_aInstUsed[i]) {
            g_aInstUsed[i] = 1;
            memset(&g_aInst[i], 0, sizeof(g_aInst[i]));
            g_aInst[i].u32Magic = MAGIC_INST;
            g_aInst[i].u32ApiVersion = VK_API_VERSION_1_0;
            if (pCreateInfo->pApplicationInfo != NULL) {
                g_aInst[i].u32ApiVersion =
                    pCreateInfo->pApplicationInfo->apiVersion
                        ? pCreateInfo->pApplicationInfo->apiVersion
                        : VK_API_VERSION_1_0;
            }
            g_aInst[i].u32EnabledExt = pCreateInfo->enabledExtensionCount;
            *pInstance = &g_aInst[i];
#ifdef GJ_VK_KERNEL_SMOKE
            kprintf("vk: CreateInstance ok api=0x%x\n",
                    g_aInst[i].u32ApiVersion);
#endif
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_TOO_MANY_OBJECTS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator)
{
    uint32_t i;

    (void)pAllocator;
    if (instance == NULL) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_INST; i++) {
        if (&g_aInst[i] == instance) {
            g_aInstUsed[i] = 0;
            instance->u32Magic = 0;
            return;
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL
vkEnumeratePhysicalDevices(VkInstance instance, uint32_t *pCount,
                           VkPhysicalDevice *pPhysicalDevices)
{
    if (instance == NULL || instance->u32Magic != MAGIC_INST || pCount == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
#ifdef GJ_VK_KERNEL_SMOKE
    if (!virtio_gpu_ready()) {
        *pCount = 0;
        return VK_SUCCESS;
    }
#endif
    /* Host ICD always exposes one virtual GPU (software present). */
    if (!g_fPhysInit) {
        memset(&g_Phys, 0, sizeof(g_Phys));
        g_Phys.u32Magic = MAGIC_PHYS;
        g_Phys.pInst = instance;
        g_fPhysInit = 1;
    }
    if (pPhysicalDevices == NULL) {
        *pCount = 1;
        return VK_SUCCESS;
    }
    if (*pCount < 1) {
        *pCount = 1;
        return VK_INCOMPLETE;
    }
    pPhysicalDevices[0] = &g_Phys;
    *pCount = 1;
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice,
                              VkPhysicalDeviceProperties *pProps)
{
    uint32_t w = 64;
    uint32_t h = 64;

    (void)physicalDevice;
    if (pProps == NULL) {
        return;
    }
    memset(pProps, 0, sizeof(*pProps));
    pProps->apiVersion = VK_API_VERSION_1_0;
    pProps->driverVersion = VK_MAKE_VERSION(0, 1, 0);
    pProps->vendorID = 0x1af4; /* virtio vendor-ish */
    pProps->deviceID = 0x1050;
    pProps->deviceType = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
    copy_str(pProps->deviceName, sizeof(pProps->deviceName),
             "GreenJade virtio-gpu 2D");
#ifdef GJ_VK_KERNEL_SMOKE
    if (virtio_gpu_ready()) {
        (void)virtio_gpu_get_display(&w, &h);
    }
#endif
    pProps->limits.maxImageDimension2D = w > h ? w : h;
    if (pProps->limits.maxImageDimension2D < 256) {
        pProps->limits.maxImageDimension2D = 256;
    }
    pProps->limits.maxFramebufferWidth = w ? w : 256;
    pProps->limits.maxFramebufferHeight = h ? h : 256;
}

VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice,
                            VkPhysicalDeviceFeatures *pFeatures)
{
    (void)physicalDevice;
    if (pFeatures == NULL) {
        return;
    }
    memset(pFeatures, 0, sizeof(*pFeatures));
}

VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice,
                                         uint32_t *pCount,
                                         VkQueueFamilyProperties *pProps)
{
    (void)physicalDevice;
    if (pCount == NULL) {
        return;
    }
    if (pProps == NULL) {
        *pCount = 1;
        return;
    }
    if (*pCount < 1) {
        *pCount = 1;
        return;
    }
    memset(&pProps[0], 0, sizeof(pProps[0]));
    pProps[0].queueFlags =
        VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
    pProps[0].queueCount = 1;
    *pCount = 1;
}

VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice,
                                    VkPhysicalDeviceMemoryProperties *pProps)
{
    (void)physicalDevice;
    if (pProps == NULL) {
        return;
    }
    memset(pProps, 0, sizeof(*pProps));
    pProps->memoryTypeCount = 1;
    pProps->memoryTypes[0].propertyFlags =
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    pProps->memoryTypes[0].heapIndex = 0;
    pProps->memoryHeapCount = 1;
    pProps->memoryHeaps[0].size = 64ull * 1024ull * 1024ull;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                     const char *pLayerName, uint32_t *pCount,
                                     VkExtensionProperties *pProps)
{
    uint32_t n = (uint32_t)(sizeof(g_aszDevExt) / sizeof(g_aszDevExt[0]));
    uint32_t i;

    (void)physicalDevice;
    (void)pLayerName;
    if (pCount == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (pProps == NULL) {
        *pCount = n;
        return VK_SUCCESS;
    }
    if (*pCount < n) {
        *pCount = n;
        return VK_INCOMPLETE;
    }
    for (i = 0; i < n; i++) {
        memset(&pProps[i], 0, sizeof(pProps[i]));
        copy_str(pProps[i].extensionName, sizeof(pProps[i].extensionName),
                 g_aszDevExt[i]);
        pProps[i].specVersion = 70;
    }
    *pCount = n;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDevice(VkPhysicalDevice physicalDevice,
               const VkDeviceCreateInfo *pCreateInfo,
               const VkAllocationCallbacks *pAllocator, VkDevice *pDevice)
{
    uint32_t i;
    uint32_t w = 64;
    uint32_t h = 64;

    vk_icd_soft_inc(&g_u32SoftCreateDevN);
    (void)pAllocator;
    if (physicalDevice == NULL || physicalDevice->u32Magic != MAGIC_PHYS ||
        pCreateInfo == NULL || pDevice == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
#ifdef GJ_VK_KERNEL_SMOKE
    if (virtio_gpu_ready()) {
        (void)virtio_gpu_get_display(&w, &h);
    }
    if (w == 0 || h == 0 || w > 256 || h > 256) {
        w = 64;
        h = 64;
    }
#else
    w = 64;
    h = 64;
#endif
    for (i = 0; i < GJ_VK_MAX_DEV; i++) {
        if (!g_aDevUsed[i]) {
            g_aDevUsed[i] = 1;
            memset(&g_aDev[i], 0, sizeof(g_aDev[i]));
            g_aDev[i].u32Magic = MAGIC_DEV;
            g_aDev[i].pInst = physicalDevice->pInst;
            g_aDev[i].pPhys = physicalDevice;
            g_aDev[i].u32W = w;
            g_aDev[i].u32H = h;
            memset(&g_aQue[i], 0, sizeof(g_aQue[i]));
            g_aQue[i].u32Magic = MAGIC_QUE;
            g_aQue[i].pDev = &g_aDev[i];
            *pDevice = &g_aDev[i];
#ifdef GJ_VK_KERNEL_SMOKE
            kprintf("vk: CreateDevice ok %ux%u\n", w, h);
#endif
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_TOO_MANY_OBJECTS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyDevice(VkDevice device, const VkAllocationCallbacks *pAllocator)
{
    uint32_t i;

    (void)pAllocator;
    if (device == NULL) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_DEV; i++) {
        if (&g_aDev[i] == device) {
            g_aDevUsed[i] = 0;
            device->u32Magic = 0;
            g_aQue[i].u32Magic = 0;
            return;
        }
    }
}

VKAPI_ATTR void VKAPI_CALL
vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex,
                 VkQueue *pQueue)
{
    uint32_t i;

    (void)queueFamilyIndex;
    (void)queueIndex;
    if (pQueue == NULL) {
        return;
    }
    *pQueue = NULL;
    if (device == NULL || device->u32Magic != MAGIC_DEV) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_DEV; i++) {
        if (&g_aDev[i] == device) {
            *pQueue = &g_aQue[i];
            return;
        }
    }
}

/* ---- Surface (headless create for ICD smoke / GJ extension) ---- */

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateHeadlessSurfaceGJ(VkInstance instance, uint32_t width, uint32_t height,
                          const VkAllocationCallbacks *pAllocator,
                          VkSurfaceKHR *pSurface)
{
    uint32_t i;

    (void)pAllocator;
    if (instance == NULL || instance->u32Magic != MAGIC_INST || pSurface == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (width == 0) {
        width = 64;
    }
    if (height == 0) {
        height = 64;
    }
    for (i = 0; i < GJ_VK_MAX_INST; i++) {
        if (!g_aSurfUsed[i]) {
            g_aSurfUsed[i] = 1;
            memset(&g_aSurf[i], 0, sizeof(g_aSurf[i]));
            g_aSurf[i].u32Magic = MAGIC_SURF;
            g_aSurf[i].pInst = instance;
            g_aSurf[i].u32W = width;
            g_aSurf[i].u32H = height;
            *pSurface = (VkSurfaceKHR)(uintptr_t)(void *)&g_aSurf[i];
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_TOO_MANY_OBJECTS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface,
                    const VkAllocationCallbacks *pAllocator)
{
    struct gj_surface *pS = (struct gj_surface *)(uintptr_t)surface;
    uint32_t i;

    (void)instance;
    (void)pAllocator;
    if (pS == NULL) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_INST; i++) {
        if (&g_aSurf[i] == pS) {
            g_aSurfUsed[i] = 0;
            pS->u32Magic = 0;
            return;
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice,
                                     uint32_t queueFamilyIndex,
                                     VkSurfaceKHR surface, VkBool32 *pSupported)
{
    (void)physicalDevice;
    (void)queueFamilyIndex;
    (void)surface;
    if (pSupported == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    *pSupported = VK_TRUE;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice,
                                          VkSurfaceKHR surface,
                                          VkSurfaceCapabilitiesKHR *pCaps)
{
    struct gj_surface *pS = (struct gj_surface *)(uintptr_t)surface;
    uint32_t w = 64;
    uint32_t h = 64;

    (void)physicalDevice;
    if (pCaps == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (pS != NULL && pS->u32Magic == MAGIC_SURF) {
        w = pS->u32W;
        h = pS->u32H;
    }
#ifdef GJ_VK_KERNEL_SMOKE
    else if (virtio_gpu_ready()) {
        (void)virtio_gpu_get_display(&w, &h);
        if (w > 256) {
            w = 64;
        }
        if (h > 256) {
            h = 64;
        }
    }
#endif
    memset(pCaps, 0, sizeof(*pCaps));
    pCaps->minImageCount = 2;
    pCaps->maxImageCount = GJ_VK_SC_IMAGES;
    pCaps->currentExtent.width = w;
    pCaps->currentExtent.height = h;
    pCaps->minImageExtent = pCaps->currentExtent;
    pCaps->maxImageExtent = pCaps->currentExtent;
    pCaps->maxImageArrayLayers = 1;
    pCaps->supportedTransforms = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    pCaps->currentTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    pCaps->supportedCompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    /* Soft present path: color attach + transfer in/out (CPU map / blit). */
    pCaps->supportedUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                 VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                 VK_IMAGE_USAGE_SAMPLED_BIT;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice,
                                     VkSurfaceKHR surface, uint32_t *pCount,
                                     VkSurfaceFormatKHR *pFormats)
{
    (void)physicalDevice;
    (void)surface;
    if (pCount == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (pFormats == NULL) {
        *pCount = 1;
        return VK_SUCCESS;
    }
    if (*pCount < 1) {
        *pCount = 1;
        return VK_INCOMPLETE;
    }
    pFormats[0].format = VK_FORMAT_B8G8R8A8_UNORM;
    pFormats[0].colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    *pCount = 1;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice,
                                          VkSurfaceKHR surface, uint32_t *pCount,
                                          VkPresentModeKHR *pModes)
{
    (void)physicalDevice;
    (void)surface;
    if (pCount == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (pModes == NULL) {
        *pCount = 2;
        return VK_SUCCESS;
    }
    if (*pCount < 2) {
        *pCount = 2;
        return VK_INCOMPLETE;
    }
    pModes[0] = VK_PRESENT_MODE_FIFO_KHR;
    pModes[1] = VK_PRESENT_MODE_IMMEDIATE_KHR;
    *pCount = 2;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo,
                     const VkAllocationCallbacks *pAllocator,
                     VkSwapchainKHR *pSwapchain)
{
    uint32_t i;
    uint32_t j;
    uint32_t n;
    uint32_t stride;
    uint32_t w;
    uint32_t h;
    VkFormat fmt;
    struct gj_swapchain *pSc;
    struct gj_surface *pSurf;
    VkPresentModeKHR mode;

    vk_icd_soft_inc(&g_u32SoftCreateScN);

    (void)pAllocator;
    if (device == NULL || device->u32Magic != MAGIC_DEV || pCreateInfo == NULL ||
        pSwapchain == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    pSurf = (struct gj_surface *)(uintptr_t)pCreateInfo->surface;
    if (pSurf != NULL && pSurf->u32Magic != MAGIC_SURF) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    fmt = pCreateInfo->imageFormat;
    if (fmt == 0) {
        fmt = VK_FORMAT_B8G8R8A8_UNORM;
    }
    if (fmt != VK_FORMAT_B8G8R8A8_UNORM && fmt != VK_FORMAT_B8G8R8A8_SRGB) {
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }
    mode = pCreateInfo->presentMode;
    if (mode != VK_PRESENT_MODE_FIFO_KHR && mode != VK_PRESENT_MODE_IMMEDIATE_KHR &&
        mode != VK_PRESENT_MODE_MAILBOX_KHR &&
        mode != VK_PRESENT_MODE_FIFO_RELAXED_KHR) {
        mode = VK_PRESENT_MODE_FIFO_KHR;
    }
    /* Soft: MAILBOX/RELAXED accepted but behave as FIFO (single soft queue). */
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR ||
        mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR) {
        mode = VK_PRESENT_MODE_FIFO_KHR;
    }
    w = pCreateInfo->imageExtent.width;
    h = pCreateInfo->imageExtent.height;
    if (w == 0 || h == 0) {
        if (pSurf != NULL && pSurf->u32Magic == MAGIC_SURF) {
            w = pSurf->u32W;
            h = pSurf->u32H;
        } else {
            w = device->u32W ? device->u32W : 64u;
            h = device->u32H ? device->u32H : 64u;
        }
    }
    if (w == 0 || h == 0) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    n = pCreateInfo->minImageCount;
    if (n < 2) {
        n = 2;
    }
    if (n > GJ_VK_SC_IMAGES) {
        n = GJ_VK_SC_IMAGES;
    }
    for (i = 0; i < GJ_VK_MAX_SC; i++) {
        if (!g_aScUsed[i]) {
            g_aScUsed[i] = 1;
            pSc = &g_aSc[i];
            memset(pSc, 0, sizeof(*pSc));
            pSc->u32Magic = MAGIC_SC;
            pSc->pDev = device;
            pSc->surface = pCreateInfo->surface;
            pSc->u32W = w;
            pSc->u32H = h;
            pSc->u32Count = n;
            pSc->format = fmt;
            pSc->presentMode = mode;
            pSc->usage = pCreateInfo->imageUsage
                             ? pCreateInfo->imageUsage
                             : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            pSc->u8Clipped = pCreateInfo->clipped ? 1u : 0u;
            for (j = 0; j < n; j++) {
                pSc->aImg[j].pPixels = fb_alloc(pSc->u32W, pSc->u32H, &stride);
                pSc->aImg[j].u32Stride = stride;
                pSc->aImg[j].u8Alive = pSc->aImg[j].pPixels != NULL;
                pSc->aImg[j].u8Acquired = 0;
                pSc->aImg[j].u8Presented = 0;
                /* Non-dispatchable image handle (opaque u64 counter). */
                pSc->aImg[j].hImage = (VkImage)(++g_u64NextNd);
                if (!pSc->aImg[j].u8Alive) {
                    sc_teardown_images(pSc);
                    g_aScUsed[i] = 0;
                    pSc->u32Magic = 0;
                    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
                }
            }
            *pSwapchain = (VkSwapchainKHR)(uintptr_t)(void *)pSc;
            /* Soft recreate: retire oldSwapchain after new images are live. */
            if (pCreateInfo->oldSwapchain != 0 &&
                pCreateInfo->oldSwapchain != *pSwapchain) {
                vkDestroySwapchainKHR(device, pCreateInfo->oldSwapchain, pAllocator);
            }
#ifdef GJ_VK_KERNEL_SMOKE
            kprintf("vk: CreateSwapchainKHR %ux%u images=%u\n", pSc->u32W,
                    pSc->u32H, n);
#endif
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_TOO_MANY_OBJECTS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain,
                      const VkAllocationCallbacks *pAllocator)
{
    struct gj_swapchain *pSc = (struct gj_swapchain *)(uintptr_t)swapchain;
    uint32_t i;

    (void)device;
    (void)pAllocator;
    if (pSc == NULL || swapchain == 0) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_SC; i++) {
        if (&g_aSc[i] == pSc && g_aScUsed[i]) {
            sc_teardown_images(pSc);
            g_aScUsed[i] = 0;
            pSc->u32Magic = 0;
            pSc->pDev = NULL;
            pSc->surface = 0;
            return;
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL
vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain,
                        uint32_t *pCount, VkImage *pImages)
{
    struct gj_swapchain *pSc = (struct gj_swapchain *)(uintptr_t)swapchain;
    uint32_t i;

    (void)device;
    if (pSc == NULL || pSc->u32Magic != MAGIC_SC || pCount == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (device != NULL && device->u32Magic == MAGIC_DEV && pSc->pDev != device) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (pImages == NULL) {
        *pCount = pSc->u32Count;
        return VK_SUCCESS;
    }
    if (*pCount < pSc->u32Count) {
        *pCount = pSc->u32Count;
        return VK_INCOMPLETE;
    }
    for (i = 0; i < pSc->u32Count; i++) {
        pImages[i] = pSc->aImg[i].hImage;
    }
    *pCount = pSc->u32Count;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout,
                      VkSemaphore semaphore, VkFence fence, uint32_t *pImageIndex)
{
    struct gj_swapchain *pSc = (struct gj_swapchain *)(uintptr_t)swapchain;
    uint32_t idx = 0;
    uint32_t tries;
    int found = 0;

    vk_icd_soft_inc(&g_u32SoftAcquireN);
    if (pSc == NULL || pSc->u32Magic != MAGIC_SC || pImageIndex == NULL ||
        pSc->u32Count == 0) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (device != NULL && device->u32Magic == MAGIC_DEV && pSc->pDev != NULL &&
        pSc->pDev != device) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    /* Soft present pool: only hand out unacquired live images. */
    for (tries = 0; tries < pSc->u32Count; tries++) {
        idx = (pSc->u32Cursor + tries) % pSc->u32Count;
        if (pSc->aImg[idx].u8Alive && !pSc->aImg[idx].u8Acquired) {
            found = 1;
            break;
        }
    }
    if (!found) {
        /* Spec soft: timeout 0 → NOT_READY; else TIMEOUT (no real wait). */
        return (timeout == 0) ? VK_NOT_READY : VK_TIMEOUT;
    }
    pSc->u32Cursor = (idx + 1u) % pSc->u32Count;
    pSc->aImg[idx].u8Acquired = 1;
    pSc->u32AcquiredN++;
    *pImageIndex = idx;
    /* Soft binary semaphore signal on acquire complete. */
    if (semaphore != 0) {
        sem_soft_signal(semaphore, 1);
    }
    if (fence != 0) {
        struct gj_fence *pF = (struct gj_fence *)(uintptr_t)fence;

        if (pF != NULL && pF->u32Magic == MAGIC_FEN) {
            pF->u8Signaled = 1;
        }
    }
    vk_icd_soft_inc(&g_u32SoftAcquireOk);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR *pPresentInfo)
{
    uint32_t i;
    VkResult overall = VK_SUCCESS;

    if (queue == NULL || queue->u32Magic != MAGIC_QUE || pPresentInfo == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (pPresentInfo->swapchainCount > 0 &&
        (pPresentInfo->pSwapchains == NULL || pPresentInfo->pImageIndices == NULL)) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    /* Soft present: consume wait semaphores before flipping images. */
    present_wait_sems(pPresentInfo);
    for (i = 0; i < pPresentInfo->swapchainCount; i++) {
        struct gj_swapchain *pSc =
            (struct gj_swapchain *)(uintptr_t)pPresentInfo->pSwapchains[i];
        uint32_t idx;
        VkResult st;

        if (pSc == NULL || pSc->u32Magic != MAGIC_SC) {
            st = VK_ERROR_OUT_OF_DATE_KHR;
            if (pPresentInfo->pResults) {
                pPresentInfo->pResults[i] = st;
            }
            if (overall == VK_SUCCESS) {
                overall = st;
            }
            continue;
        }
        if (queue->pDev != NULL && pSc->pDev != NULL && queue->pDev != pSc->pDev) {
            st = VK_ERROR_INITIALIZATION_FAILED;
            if (pPresentInfo->pResults) {
                pPresentInfo->pResults[i] = st;
            }
            if (overall == VK_SUCCESS) {
                overall = st;
            }
            continue;
        }
        idx = pPresentInfo->pImageIndices[i];
        if (idx >= pSc->u32Count || !pSc->aImg[idx].u8Alive ||
            !pSc->aImg[idx].u8Acquired) {
            st = VK_ERROR_OUT_OF_DATE_KHR;
            if (pPresentInfo->pResults) {
                pPresentInfo->pResults[i] = st;
            }
            if (overall == VK_SUCCESS) {
                overall = st;
            }
            continue;
        }
        st = present_pixels(pSc->aImg[idx].pPixels, pSc->u32W, pSc->u32H,
                            pSc->aImg[idx].u32Stride);
        if (st == VK_SUCCESS) {
            pSc->aImg[idx].u8Acquired = 0;
            pSc->aImg[idx].u8Presented = 1;
            if (pSc->u32AcquiredN > 0) {
                pSc->u32AcquiredN--;
            }
            pSc->u32PresentN++;
        }
        if (pPresentInfo->pResults) {
            pPresentInfo->pResults[i] = st;
        }
        if (st != VK_SUCCESS && overall == VK_SUCCESS) {
            overall = st;
        }
        /* Hard present failure: stop early (device lost / init). */
        if (st == VK_ERROR_DEVICE_LOST || st == VK_ERROR_INITIALIZATION_FAILED) {
#ifdef GJ_VK_KERNEL_SMOKE
            kprintf("vk: QueuePresentKHR fail sc=%u st=%d\n",
                    pPresentInfo->swapchainCount, (int)st);
#endif
            return st;
        }
    }
#ifdef GJ_VK_KERNEL_SMOKE
    if (overall == VK_SUCCESS) {
        kprintf("vk: QueuePresentKHR ok sc=%u\n", pPresentInfo->swapchainCount);
    }
#endif
    return overall;
}

/* ---- Command buffers / fence (minimal path for present apps) ---- */

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo *pCreateInfo,
                    const VkAllocationCallbacks *pAllocator,
                    VkCommandPool *pCommandPool)
{
    (void)device;
    (void)pCreateInfo;
    (void)pAllocator;
    if (pCommandPool == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    *pCommandPool = (VkCommandPool)(++g_u64NextNd);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool,
                     const VkAllocationCallbacks *pAllocator)
{
    (void)device;
    (void)commandPool;
    (void)pAllocator;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkAllocateCommandBuffers(VkDevice device,
                         const VkCommandBufferAllocateInfo *pAllocateInfo,
                         VkCommandBuffer *pCommandBuffers)
{
    uint32_t i;
    uint32_t n;
    uint32_t filled = 0;

    if (device == NULL || pAllocateInfo == NULL || pCommandBuffers == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    n = pAllocateInfo->commandBufferCount;
    for (i = 0; i < GJ_VK_MAX_CMD && filled < n; i++) {
        if (!g_aCmdUsed[i]) {
            g_aCmdUsed[i] = 1;
            memset(&g_aCmd[i], 0, sizeof(g_aCmd[i]));
            g_aCmd[i].u32Magic = MAGIC_CMD;
            g_aCmd[i].pDev = device;
            pCommandBuffers[filled++] = &g_aCmd[i];
        }
    }
    return filled == n ? VK_SUCCESS : VK_ERROR_OUT_OF_HOST_MEMORY;
}

VKAPI_ATTR void VKAPI_CALL
vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool,
                     uint32_t count, const VkCommandBuffer *pCommandBuffers)
{
    uint32_t i;
    uint32_t j;

    (void)device;
    (void)commandPool;
    if (pCommandBuffers == NULL) {
        return;
    }
    for (i = 0; i < count; i++) {
        for (j = 0; j < GJ_VK_MAX_CMD; j++) {
            if (&g_aCmd[j] == pCommandBuffers[i]) {
                g_aCmdUsed[j] = 0;
                g_aCmd[j].u32Magic = 0;
            }
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL
vkBeginCommandBuffer(VkCommandBuffer commandBuffer,
                     const VkCommandBufferBeginInfo *pBeginInfo)
{
    (void)pBeginInfo;
    if (commandBuffer == NULL || commandBuffer->u32Magic != MAGIC_CMD) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    commandBuffer->u8Recording = 1;
    commandBuffer->u32NOps = 0;
    memset(commandBuffer->aOps, 0, sizeof(commandBuffer->aOps));
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkEndCommandBuffer(VkCommandBuffer commandBuffer)
{
    if (commandBuffer == NULL || commandBuffer->u32Magic != MAGIC_CMD) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    commandBuffer->u8Recording = 0;
    return VK_SUCCESS;
}

/*
 * Clear color: standard Vulkan passes four floats; freestanding kernel
 * builds avoid FP. Accept packed BGRA or four u32 channels in 0..255.
 */
VKAPI_ATTR void VKAPI_CALL
vkCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image,
                     uint32_t imageLayout, const void *pColor,
                     uint32_t rangeCount, const void *pRanges)
{
    struct gj_vk_cmd_record *pOp;
    uint32_t rgba = 0xff2080c0u; /* default teal BGRA */

    (void)imageLayout;
    (void)rangeCount;
    (void)pRanges;
    if (commandBuffer == NULL || !commandBuffer->u8Recording ||
        commandBuffer->u32NOps >= GJ_VK_CMD_MAX_OPS) {
        return;
    }
    if (pColor != NULL) {
        /* Packed BGRA u32* (GJ) or four u32 channels in 0..255 (R,G,B,A). */
        const uint32_t *pU = (const uint32_t *)pColor;

        if (pU[0] <= 255u && pU[1] <= 255u && pU[2] <= 255u && pU[3] <= 255u) {
            u8 r = (u8)pU[0];
            u8 g = (u8)pU[1];
            u8 b = (u8)pU[2];
            u8 a = (u8)pU[3];

            rgba = (uint32_t)b | ((uint32_t)g << 8) | ((uint32_t)r << 16) |
                   ((uint32_t)a << 24);
        } else {
            /* First word already packed BGRA. */
            rgba = pU[0];
        }
    }
    pOp = &commandBuffer->aOps[commandBuffer->u32NOps++];
    pOp->u32Op = GJ_VK_OP_CLEAR;
    pOp->hImage = image;
    pOp->u32ClearRgba = rgba;
}

/* Solid fill with packed BGRA (no floating point). */
VKAPI_ATTR void VKAPI_CALL
vkCmdClearColorImageGJ(VkCommandBuffer commandBuffer, VkImage image,
                       uint32_t rgba)
{
    uint32_t a[4];

    a[0] = rgba; /* packed path taken inside vkCmdClearColorImage */
    a[1] = 256;
    a[2] = 256;
    a[3] = 256;
    vkCmdClearColorImage(commandBuffer, image, 0, a, 0, NULL);
}

VKAPI_ATTR void VKAPI_CALL
vkCmdBeginRenderPass(VkCommandBuffer commandBuffer,
                     const void /*VkRenderPassBeginInfo*/ *pBeginInfo,
                     uint32_t /*VkSubpassContents*/ contents)
{
    /* Lightweight: schedule clear from stored framebuffer image if any. */
    const uint32_t *pWords = (const uint32_t *)pBeginInfo;

    (void)contents;
    if (commandBuffer == NULL || !commandBuffer->u8Recording || pBeginInfo == NULL) {
        return;
    }
    /*
     * Accept a real VkRenderPassBeginInfo (sType first) or fall back to the
     * active framebuffer image set along the bind path.
     */
    (void)pWords;
    if (g_hActiveFbImage != 0 && commandBuffer->u32NOps < GJ_VK_CMD_MAX_OPS) {
        struct gj_vk_cmd_record *pOp =
            &commandBuffer->aOps[commandBuffer->u32NOps++];

        pOp->u32Op = GJ_VK_OP_CLEAR;
        pOp->hImage = g_hActiveFbImage;
        pOp->u32ClearRgba = g_u32ActiveClear ? g_u32ActiveClear : 0xff208040u;
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdEndRenderPass(VkCommandBuffer commandBuffer)
{
    (void)commandBuffer;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, uint32_t srcStage,
                     uint32_t dstStage, uint32_t dependencyFlags,
                     uint32_t memCount, const void *pMem,
                     uint32_t bufCount, const void *pBuf, uint32_t imgCount,
                     const void *pImg)
{
    (void)commandBuffer;
    (void)srcStage;
    (void)dstStage;
    (void)dependencyFlags;
    (void)memCount;
    (void)pMem;
    (void)bufCount;
    (void)pBuf;
    (void)imgCount;
    (void)pImg;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo *pSubmits,
              VkFence fence)
{
    uint32_t s;
    uint32_t c;

    vk_icd_soft_inc(&g_u32SoftSubmitN);
    (void)queue;
    if (pSubmits != NULL) {
        for (s = 0; s < submitCount; s++) {
            for (c = 0; c < pSubmits[s].commandBufferCount; c++) {
                cmd_execute(pSubmits[s].pCommandBuffers[c]);
            }
        }
    }
    if (fence != 0) {
        struct gj_fence *pF = (struct gj_fence *)(uintptr_t)fence;

        if (pF != NULL && pF->u32Magic == MAGIC_FEN) {
            pF->u8Signaled = 1;
        }
    }
#ifdef GJ_VK_KERNEL_SMOKE
    kprintf("vk: QueueSubmit cmds executed\n");
#endif
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkQueueWaitIdle(VkQueue queue)
{
    (void)queue;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkDeviceWaitIdle(VkDevice device)
{
    (void)device;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateFence(VkDevice device, const VkFenceCreateInfo *pCreateInfo,
              const VkAllocationCallbacks *pAllocator, VkFence *pFence)
{
    uint32_t i;

    (void)device;
    (void)pAllocator;
    if (pFence == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    for (i = 0; i < GJ_VK_MAX_FENCE; i++) {
        if (!g_aFenceUsed[i]) {
            g_aFenceUsed[i] = 1;
            memset(&g_aFence[i], 0, sizeof(g_aFence[i]));
            g_aFence[i].u32Magic = MAGIC_FEN;
            g_aFence[i].u8Signaled =
                (pCreateInfo &&
                 (pCreateInfo->flags & VK_FENCE_CREATE_SIGNALED_BIT))
                    ? 1
                    : 0;
            *pFence = (VkFence)(uintptr_t)(void *)&g_aFence[i];
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_TOO_MANY_OBJECTS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyFence(VkDevice device, VkFence fence,
               const VkAllocationCallbacks *pAllocator)
{
    struct gj_fence *pF = (struct gj_fence *)(uintptr_t)fence;
    uint32_t i;

    (void)device;
    (void)pAllocator;
    if (pF == NULL) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_FENCE; i++) {
        if (&g_aFence[i] == pF) {
            g_aFenceUsed[i] = 0;
            pF->u32Magic = 0;
            return;
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL
vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence *pFences,
                VkBool32 waitAll, uint64_t timeout)
{
    uint32_t i;

    (void)device;
    (void)waitAll;
    (void)timeout;
    if (pFences == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    for (i = 0; i < fenceCount; i++) {
        struct gj_fence *pF = (struct gj_fence *)(uintptr_t)pFences[i];

        if (pF != NULL && pF->u32Magic == MAGIC_FEN) {
            pF->u8Signaled = 1;
        }
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence *pFences)
{
    uint32_t i;

    (void)device;
    if (pFences == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    for (i = 0; i < fenceCount; i++) {
        struct gj_fence *pF = (struct gj_fence *)(uintptr_t)pFences[i];

        if (pF != NULL && pF->u32Magic == MAGIC_FEN) {
            pF->u8Signaled = 0;
        }
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo *pCreateInfo,
                  const VkAllocationCallbacks *pAllocator, VkSemaphore *pSemaphore)
{
    uint32_t i;

    (void)device;
    (void)pCreateInfo;
    (void)pAllocator;
    if (pSemaphore == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    for (i = 0; i < GJ_VK_MAX_SEM; i++) {
        if (!g_aSemUsed[i]) {
            g_aSemUsed[i] = 1;
            memset(&g_aSem[i], 0, sizeof(g_aSem[i]));
            g_aSem[i].u32Magic = MAGIC_SEM;
            *pSemaphore = (VkSemaphore)(uintptr_t)(void *)&g_aSem[i];
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_TOO_MANY_OBJECTS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroySemaphore(VkDevice device, VkSemaphore semaphore,
                   const VkAllocationCallbacks *pAllocator)
{
    struct gj_sem *pS = (struct gj_sem *)(uintptr_t)semaphore;
    uint32_t i;

    (void)device;
    (void)pAllocator;
    if (pS == NULL) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_SEM; i++) {
        if (&g_aSem[i] == pS) {
            g_aSemUsed[i] = 0;
            pS->u32Magic = 0;
            return;
        }
    }
}

/* ---- Images / memory (host-visible 2D for clear / blit) ---- */

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateImage(VkDevice device, const void *pCreateInfo,
              const VkAllocationCallbacks *pAllocator, VkImage *pImage)
{
    /*
     * T0: accept full VkImageCreateInfo (extent at typical offset) or the
     * GJ host-smoke compact form: uint32_t tci[4] = { w, h, 0, 0 }.
     */
    const uint32_t *p = (const uint32_t *)pCreateInfo;
    uint32_t w = 64;
    uint32_t h = 64;
    uint32_t stride;
    uint32_t i;
    void *pix;

    (void)pAllocator;
    if (device == NULL || device->u32Magic != MAGIC_DEV || pCreateInfo == NULL ||
        pImage == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    w = device->u32W ? device->u32W : 64u;
    h = device->u32H ? device->u32H : 64u;
    if (p != NULL) {
        /* Compact GJ: first two words are width/height when sType is not set. */
        if (p[0] > 0u && p[0] <= 4096u && p[1] > 0u && p[1] <= 4096u &&
            p[0] != (uint32_t)VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO) {
            w = p[0];
            h = p[1];
        } else if (p[0] == (uint32_t)VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO) {
            /* Best-effort: words after sType/pNext/flags/imageType/format → extent */
            if (p[5] > 0u && p[5] <= 4096u) {
                w = p[5];
            }
            if (p[6] > 0u && p[6] <= 4096u) {
                h = p[6];
            }
        }
    }
    for (i = 0; i < GJ_VK_MAX_IMG_OBJ; i++) {
        if (!g_aImgObjUsed[i]) {
            pix = fb_alloc(w, h, &stride);
            if (pix == NULL) {
                return VK_ERROR_OUT_OF_DEVICE_MEMORY;
            }
            g_aImgObjUsed[i] = 1;
            memset(&g_aImgObj[i], 0, sizeof(g_aImgObj[i]));
            g_aImgObj[i].u32Magic = MAGIC_IMG;
            g_aImgObj[i].pDev = device;
            g_aImgObj[i].u32W = w;
            g_aImgObj[i].u32H = h;
            g_aImgObj[i].u32Stride = stride;
            g_aImgObj[i].pPixels = pix;
            g_aImgObj[i].u8Host = 1;
            *pImage = (VkImage)(uintptr_t)(void *)&g_aImgObj[i];
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_TOO_MANY_OBJECTS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyImage(VkDevice device, VkImage image,
               const VkAllocationCallbacks *pAllocator)
{
    struct gj_img_obj *pI = (struct gj_img_obj *)(uintptr_t)image;
    uint32_t i;

    (void)device;
    (void)pAllocator;
    if (pI == NULL) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_IMG_OBJ; i++) {
        if (&g_aImgObj[i] == pI) {
            g_aImgObjUsed[i] = 0;
            pI->u32Magic = 0;
            return;
        }
    }
}

VKAPI_ATTR void VKAPI_CALL
vkGetImageMemoryRequirements(VkDevice device, VkImage image,
                             VkMemoryRequirements *pReqs)
{
    void *pix = NULL;
    uint32_t stride = 0;
    uint32_t w = 0;
    uint32_t h = 0;

    (void)device;
    if (pReqs == NULL) {
        return;
    }
    memset(pReqs, 0, sizeof(*pReqs));
    if (img_resolve(image, &pix, &stride, &w, &h) == 0) {
        pReqs->size = (VkDeviceSize)stride * h;
    } else {
        pReqs->size = 4096;
    }
    pReqs->alignment = 4096;
    pReqs->memoryTypeBits = 1;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo *pAllocateInfo,
                 const VkAllocationCallbacks *pAllocator,
                 VkDeviceMemory *pMemory)
{
    uint32_t i;
    void *p;
    uint32_t stride;

    (void)device;
    (void)pAllocator;
    if (pAllocateInfo == NULL || pMemory == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    for (i = 0; i < GJ_VK_MAX_MEM; i++) {
        if (!g_aMemUsed[i]) {
            /* Reuse fb_alloc page for host-visible memory */
            p = fb_alloc(64, 64, &stride);
            if (p == NULL) {
                return VK_ERROR_OUT_OF_DEVICE_MEMORY;
            }
            g_aMemUsed[i] = 1;
            memset(&g_aMem[i], 0, sizeof(g_aMem[i]));
            g_aMem[i].u32Magic = MAGIC_MEM;
            g_aMem[i].pHost = p;
            g_aMem[i].size = pAllocateInfo->allocationSize
                                 ? pAllocateInfo->allocationSize
                                 : (VkDeviceSize)stride * 64u;
            *pMemory = (VkDeviceMemory)(uintptr_t)(void *)&g_aMem[i];
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_TOO_MANY_OBJECTS;
}

VKAPI_ATTR void VKAPI_CALL
vkFreeMemory(VkDevice device, VkDeviceMemory memory,
             const VkAllocationCallbacks *pAllocator)
{
    struct gj_mem_obj *pM = (struct gj_mem_obj *)(uintptr_t)memory;
    uint32_t i;

    (void)device;
    (void)pAllocator;
    if (pM == NULL) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_MEM; i++) {
        if (&g_aMem[i] == pM) {
            g_aMemUsed[i] = 0;
            pM->u32Magic = 0;
            return;
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL
vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory,
                  VkDeviceSize memoryOffset)
{
    (void)device;
    (void)image;
    (void)memory;
    (void)memoryOffset;
    return VK_SUCCESS; /* images already have backing in T0 */
}

VKAPI_ATTR VkResult VKAPI_CALL
vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset,
            VkDeviceSize size, VkFlags flags, void **ppData)
{
    struct gj_mem_obj *pM = (struct gj_mem_obj *)(uintptr_t)memory;

    (void)device;
    (void)offset;
    (void)size;
    (void)flags;
    if (pM == NULL || pM->u32Magic != MAGIC_MEM || ppData == NULL) {
        return VK_ERROR_MEMORY_MAP_FAILED;
    }
    *ppData = pM->pHost;
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkUnmapMemory(VkDevice device, VkDeviceMemory memory)
{
    (void)device;
    (void)memory;
}

/* ---- Render pass / framebuffer / pipeline (CPU clear "shader") ---- */

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateRenderPass(VkDevice device, const void *pCreateInfo,
                   const VkAllocationCallbacks *pAllocator,
                   VkRenderPass *pRenderPass)
{
    uint32_t i;

    (void)device;
    (void)pCreateInfo;
    (void)pAllocator;
    if (pRenderPass == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    for (i = 0; i < GJ_VK_MAX_RP; i++) {
        if (!g_aRpUsed[i]) {
            g_aRpUsed[i] = 1;
            memset(&g_aRp[i], 0, sizeof(g_aRp[i]));
            g_aRp[i].u32Magic = MAGIC_RP;
            g_aRp[i].u32ClearRgba = 0xff2040a0u; /* default blue-ish BGRA */
            *pRenderPass = (VkRenderPass)(uintptr_t)(void *)&g_aRp[i];
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_TOO_MANY_OBJECTS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass,
                    const VkAllocationCallbacks *pAllocator)
{
    struct gj_renderpass *pR = (struct gj_renderpass *)(uintptr_t)renderPass;
    uint32_t i;

    (void)device;
    (void)pAllocator;
    if (pR == NULL) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_RP; i++) {
        if (&g_aRp[i] == pR) {
            g_aRpUsed[i] = 0;
            pR->u32Magic = 0;
            return;
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateFramebuffer(VkDevice device, const void *pCreateInfo,
                    const VkAllocationCallbacks *pAllocator,
                    VkFramebuffer *pFramebuffer)
{
    uint32_t i;
    /* Smoke may pass the attachment image via GJ helper fields. */
    VkImage hImg = 0;

    (void)device;
    (void)pAllocator;
    if (pCreateInfo != NULL) {
        const void *const *pp = (const void *const *)pCreateInfo;

        (void)pp;
        hImg = g_hActiveFbImage;
    }
    if (pFramebuffer == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    for (i = 0; i < GJ_VK_MAX_FB; i++) {
        if (!g_aFbUsed[i]) {
            g_aFbUsed[i] = 1;
            memset(&g_aFb[i], 0, sizeof(g_aFb[i]));
            g_aFb[i].u32Magic = MAGIC_FB;
            g_aFb[i].hImage = hImg;
            g_aFb[i].u32W = 64;
            g_aFb[i].u32H = 64;
            *pFramebuffer = (VkFramebuffer)(uintptr_t)(void *)&g_aFb[i];
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_TOO_MANY_OBJECTS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer,
                     const VkAllocationCallbacks *pAllocator)
{
    struct gj_framebuffer *pF = (struct gj_framebuffer *)(uintptr_t)framebuffer;
    uint32_t i;

    (void)device;
    (void)pAllocator;
    if (pF == NULL) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_FB; i++) {
        if (&g_aFb[i] == pF) {
            g_aFbUsed[i] = 0;
            pF->u32Magic = 0;
            return;
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateGraphicsPipelines(VkDevice device, uint64_t pipelineCache,
                          uint32_t createInfoCount, const void *pCreateInfos,
                          const VkAllocationCallbacks *pAllocator,
                          VkPipeline *pPipelines)
{
    uint32_t i;
    uint32_t n;
    const VkGraphicsPipelineCreateInfo *pInfos =
        (const VkGraphicsPipelineCreateInfo *)pCreateInfos;

    (void)device;
    (void)pipelineCache;
    (void)pAllocator;
    if (pPipelines == NULL || createInfoCount == 0) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    n = 0;
    for (i = 0; i < GJ_VK_MAX_PIPE && n < createInfoCount; i++) {
        if (!g_aPipeUsed[i]) {
            uint32_t s;
            uint32_t color = 0xffc04020u; /* orange default solid */
            uint8_t hasSpv = 0;
            VkShaderModule hVs = 0;
            VkShaderModule hFs = 0;

            g_aPipeUsed[i] = 1;
            memset(&g_aPipe[i], 0, sizeof(g_aPipe[i]));
            g_aPipe[i].u32Magic = MAGIC_PIPE;
            if (pInfos != NULL) {
                const VkGraphicsPipelineCreateInfo *ci = &pInfos[n];

                if (ci->pStages != NULL) {
                    for (s = 0; s < ci->stageCount; s++) {
                        const VkPipelineShaderStageCreateInfo *st =
                            &ci->pStages[s];
                        struct gj_shader *pSh = shd_from_handle(st->module);

                        if (pSh == NULL) {
                            continue;
                        }
                        if (st->stage & VK_SHADER_STAGE_VERTEX_BIT) {
                            hVs = st->module;
                            hasSpv = 1;
                        }
                        if (st->stage & VK_SHADER_STAGE_FRAGMENT_BIT) {
                            hFs = st->module;
                            hasSpv = 1;
                            if (pSh->u8HasSpecColor) {
                                color = pSh->u32SpecColor;
                            }
                        }
                        /* stage flag 0: infer from SPIR-V exec mask */
                        if (st->stage == 0u && pSh->u8ValidSpv) {
                            if (pSh->u32ExecMask & 1u) {
                                hVs = st->module;
                            }
                            if (pSh->u32ExecMask & 2u) {
                                hFs = st->module;
                                if (pSh->u8HasSpecColor) {
                                    color = pSh->u32SpecColor;
                                }
                            }
                            hasSpv = 1;
                        }
                    }
                }
            }
            g_aPipe[i].u32ClearRgba = color;
            g_aPipe[i].hVs = hVs;
            g_aPipe[i].hFs = hFs;
            g_aPipe[i].u8HasSpirv = hasSpv;
            pPipelines[n++] = (VkPipeline)(uintptr_t)(void *)&g_aPipe[i];
        }
    }
    return n == createInfoCount ? VK_SUCCESS : VK_ERROR_OUT_OF_HOST_MEMORY;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyPipeline(VkDevice device, VkPipeline pipeline,
                  const VkAllocationCallbacks *pAllocator)
{
    struct gj_pipeline *pP = (struct gj_pipeline *)(uintptr_t)pipeline;
    uint32_t i;

    (void)device;
    (void)pAllocator;
    if (pP == NULL) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_PIPE; i++) {
        if (&g_aPipe[i] == pP) {
            g_aPipeUsed[i] = 0;
            pP->u32Magic = 0;
            return;
        }
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdBindPipeline(VkCommandBuffer commandBuffer, uint32_t pipelineBindPoint,
                  VkPipeline pipeline)
{
    struct gj_pipeline *pP = (struct gj_pipeline *)(uintptr_t)pipeline;

    (void)commandBuffer;
    (void)pipelineBindPoint;
    if (pP != NULL && pP->u32Magic == MAGIC_PIPE) {
        g_u32ActiveClear = pP->u32ClearRgba;
        g_u8ActiveHasSpirv = pP->u8HasSpirv;
        if (g_u8PushColorSet) {
            g_u32ActiveClear = g_u32PushColor;
        }
    }
}

/* Bind framebuffer image for subsequent renderpass clear */
VKAPI_ATTR void VKAPI_CALL
vkCmdBindFramebufferImageGJ(VkCommandBuffer commandBuffer, VkImage image)
{
    (void)commandBuffer;
    g_hActiveFbImage = image;
}

/* Bind a sampled image; FS uses center texel as flat color when no push/VBO col */
VKAPI_ATTR void VKAPI_CALL
vkCmdBindTextureImageGJ(VkCommandBuffer commandBuffer, VkImage image)
{
    (void)commandBuffer;
    g_hActiveTex = image;
    g_u8TexBound = (image != 0) ? 1u : 0u;
}

static uint32_t
tex_sample_center(VkImage hImg)
{
    void *pPix = NULL;
    uint32_t stride = 0;
    uint32_t w = 0;
    uint32_t h = 0;
    u8 *p;
    uint32_t x;
    uint32_t y;

    if (img_resolve(hImg, &pPix, &stride, &w, &h) != 0 || pPix == NULL ||
        w == 0 || h == 0) {
        return 0xff808080u;
    }
    x = w / 2u;
    y = h / 2u;
    p = (u8 *)pPix + y * stride + x * 4u;
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSampler(VkDevice device, const void *pCreateInfo,
                const VkAllocationCallbacks *pAllocator, VkSampler *pSampler)
{
    (void)device;
    (void)pAllocator;
    if (pSampler == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    /*
     * VkSamplerCreateInfo: after sType/pNext/flags, magFilter at word offset.
     * VK_FILTER_LINEAR = 1 → enable integer bilinear for UV path.
     * If pCreateInfo is NULL or filter word == 1, use bilinear (default linear).
     */
    g_u8Bilinear = 1;
    if (pCreateInfo != NULL) {
        const uint32_t *pw = (const uint32_t *)pCreateInfo;

        /* magFilter is typically the 4th uint32 after sType,pNext,flags */
        if (pw[0] == (uint32_t)VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO ||
            pw[0] == 31u /* VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO */) {
            /* flags at [2] if 64-bit pNext; freestanding: treat [3] as mag if present */
            (void)pw;
        }
        /* Optional: first uint32 after create as filter (GJ host smoke) */
        if (pw[1] == 0u) {
            g_u8Bilinear = 0; /* NEAREST */
        }
    }
    *pSampler = (VkSampler)(++g_u64NextNd);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroySampler(VkDevice device, VkSampler sampler,
                 const VkAllocationCallbacks *pAllocator)
{
    (void)device;
    (void)sampler;
    (void)pAllocator;
}

/*
 * Draw: with VBO (int32 x,y per vertex) rasterize triangles; else fill
 * viewport/image with pipeline/push/SpecId color (software FS).
 */
VKAPI_ATTR void VKAPI_CALL
vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount,
          uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    struct gj_vk_cmd_record *pOp;
    uint32_t color;

    (void)instanceCount;
    (void)firstInstance;
    if (commandBuffer == NULL || !commandBuffer->u8Recording ||
        commandBuffer->u32NOps >= GJ_VK_CMD_MAX_OPS || g_hActiveFbImage == 0) {
        return;
    }
    if (g_u8PushColorSet) {
        color = g_u32PushColor;
    } else if (g_u8TexBound && g_hActiveTex != 0) {
        color = tex_sample_center(g_hActiveTex);
    } else {
        color = g_u32ActiveClear ? g_u32ActiveClear : 0xffc04020u;
    }
    pOp = &commandBuffer->aOps[commandBuffer->u32NOps++];
    pOp->hImage = g_hActiveFbImage;
    pOp->u32ClearRgba = color;

    if (g_u8VboBound && g_pVboHost != NULL && vertexCount >= 3u) {
        const int32_t *pV = (const int32_t *)((const u8 *)g_pVboHost +
                                              (size_t)g_vboOff);
        /*
         * Vertex formats (T0 software):
         *   stride 2: int32 x,y
         *   stride 3: int32 x,y + u32 color
         *   stride 4: int32 x,y + u32 u + u32 v  (UV 0..255, textured if bound)
         * Pick largest stride that fits the remaining buffer.
         */
        uint32_t stride = 2u;
        uint32_t base;
        uint32_t need3 = (firstVertex + vertexCount) * 12u;
        uint32_t need4 = (firstVertex + vertexCount) * 16u;

        if (g_vboSize >= need4) {
            stride = 4u;
        } else if (g_vboSize >= need3) {
            stride = 3u;
        }
        base = firstVertex * stride;
        pOp->aVx[0] = pV[base + 0];
        pOp->aVy[0] = pV[base + 1];
        pOp->aVx[1] = pV[base + stride];
        pOp->aVy[1] = pV[base + stride + 1];
        pOp->aVx[2] = pV[base + 2u * stride];
        pOp->aVy[2] = pV[base + 2u * stride + 1];
        if (stride == 4u && g_u8TexBound && g_hActiveTex != 0) {
            pOp->u32Op = GJ_VK_OP_TRI_UV;
            pOp->hTex = g_hActiveTex;
            pOp->aU[0] = (uint8_t)(pV[base + 2] & 0xff);
            pOp->aV[0] = (uint8_t)(pV[base + 3] & 0xff);
            pOp->aU[1] = (uint8_t)(pV[base + stride + 2] & 0xff);
            pOp->aV[1] = (uint8_t)(pV[base + stride + 3] & 0xff);
            pOp->aU[2] = (uint8_t)(pV[base + 2u * stride + 2] & 0xff);
            pOp->aV[2] = (uint8_t)(pV[base + 2u * stride + 3] & 0xff);
        } else {
            pOp->u32Op = GJ_VK_OP_TRI;
            if (stride == 3u && !g_u8PushColorSet) {
                pOp->u32ClearRgba = (uint32_t)pV[base + 2];
            }
        }
    } else if (g_u8VpSet && g_u32VpW > 0 && g_u32VpH > 0) {
        pOp->u32Op = GJ_VK_OP_FILL_RECT;
        pOp->u32X = g_iVpX > 0 ? (uint32_t)g_iVpX : 0u;
        pOp->u32Y = g_iVpY > 0 ? (uint32_t)g_iVpY : 0u;
        pOp->u32W = g_u32VpW;
        pOp->u32H = g_u32VpH;
    } else {
        /* Full-target solid (backward-compatible T0 draw) */
        pOp->u32Op = GJ_VK_OP_CLEAR;
    }
    (void)vertexCount;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer,
                     VkDeviceSize offset, uint32_t indexType)
{
    struct gj_buffer *pB = (struct gj_buffer *)(uintptr_t)buffer;

    (void)commandBuffer;
    if (pB != NULL && pB->u32Magic == MAGIC_BUF && pB->pHost != NULL) {
        g_pIboHost = pB->pHost;
        g_iboOff = offset;
        g_u8IboBound = 1;
        g_u8IboU16 = (indexType == 0u) ? 1u : 0u; /* VK_INDEX_TYPE_UINT16=0 */
    } else {
        g_u8IboBound = 0;
        g_pIboHost = NULL;
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount,
                 uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset,
                 uint32_t firstInstance)
{
    /*
     * Expand first triangle of index buffer into a temporary draw:
     * gather 3 indices → set firstVertex path via rebinding coords.
     * For T0: if IBO bound and indexCount>=3, build local verts from VBO.
     */
    (void)instanceCount;
    (void)firstInstance;
    if (g_u8IboBound && g_pIboHost != NULL && g_u8VboBound && g_pVboHost != NULL &&
        indexCount >= 3u) {
        uint32_t i0, i1, i2;
        const u8 *pI = (const u8 *)g_pIboHost + (size_t)g_iboOff;
        int32_t aTmp[12];
        int32_t *pSave;
        VkDeviceSize saveOff;
        VkDeviceSize saveSize;
        uint8_t saveBound;
        uint32_t stride = 2u;
        uint32_t need4 = 3u * 16u;
        uint32_t need3 = 3u * 12u;
        const int32_t *pV =
            (const int32_t *)((const u8 *)g_pVboHost + (size_t)g_vboOff);

        if (g_u8IboU16) {
            const uint16_t *p16 = (const uint16_t *)pI;

            i0 = p16[firstIndex + 0] + (uint32_t)vertexOffset;
            i1 = p16[firstIndex + 1] + (uint32_t)vertexOffset;
            i2 = p16[firstIndex + 2] + (uint32_t)vertexOffset;
        } else {
            const uint32_t *p32 = (const uint32_t *)pI;

            i0 = p32[firstIndex + 0] + (uint32_t)vertexOffset;
            i1 = p32[firstIndex + 1] + (uint32_t)vertexOffset;
            i2 = p32[firstIndex + 2] + (uint32_t)vertexOffset;
        }
        if (g_vboSize >= need4) {
            stride = 4u;
        } else if (g_vboSize >= need3) {
            stride = 3u;
        }
        {
            uint32_t c;

            for (c = 0; c < stride; c++) {
                aTmp[0 * stride + c] = pV[i0 * stride + c];
                aTmp[1 * stride + c] = pV[i1 * stride + c];
                aTmp[2 * stride + c] = pV[i2 * stride + c];
            }
        }
        /* Temporarily point VBO at gathered triangle */
        pSave = (int32_t *)g_pVboHost;
        saveOff = g_vboOff;
        saveSize = g_vboSize;
        saveBound = g_u8VboBound;
        g_pVboHost = aTmp;
        g_vboOff = 0;
        g_vboSize = stride * 3u * sizeof(int32_t);
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        g_pVboHost = pSave;
        g_vboOff = saveOff;
        g_vboSize = saveSize;
        g_u8VboBound = saveBound;
        return;
    }
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo *pCreateInfo,
                     const VkAllocationCallbacks *pAllocator,
                     VkShaderModule *pShaderModule)
{
    uint32_t i;
    uint32_t nWords;
    const uint32_t *pCode;

    (void)device;
    (void)pAllocator;
    if (pShaderModule == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    for (i = 0; i < GJ_VK_MAX_SHADER; i++) {
        if (g_aShdUsed[i]) {
            continue;
        }
        g_aShdUsed[i] = 1;
        memset(&g_aShd[i], 0, sizeof(g_aShd[i]));
        g_aShd[i].u32Magic = MAGIC_SHD;
        if (pCreateInfo != NULL && pCreateInfo->pCode != NULL &&
            pCreateInfo->codeSize >= 20u) {
            nWords = (uint32_t)(pCreateInfo->codeSize / 4u);
            pCode = pCreateInfo->pCode;
            if (nWords > 256u) {
                nWords = 256u;
            }
            {
                uint32_t k;

                for (k = 0; k < nWords; k++) {
                    g_aShd[i].aWords[k] = pCode[k];
                }
            }
            g_aShd[i].u32WordCount = nWords;
            if (spv_parse(pCode, (uint32_t)(pCreateInfo->codeSize / 4u),
                          &g_aShd[i]) != 0) {
                g_aShdUsed[i] = 0;
                g_aShd[i].u32Magic = 0;
                return VK_ERROR_INITIALIZATION_FAILED;
            }
        } else if (pCreateInfo != NULL && pCreateInfo->pCode != NULL) {
            /* Too short / invalid */
            g_aShdUsed[i] = 0;
            return VK_ERROR_INITIALIZATION_FAILED;
        }
        /* NULL createInfo: empty module (legacy smoke path) */
        *pShaderModule = (VkShaderModule)(uintptr_t)(void *)&g_aShd[i];
        return VK_SUCCESS;
    }
    return VK_ERROR_TOO_MANY_OBJECTS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule,
                      const VkAllocationCallbacks *pAllocator)
{
    struct gj_shader *pS = shd_from_handle(shaderModule);
    uint32_t i;

    (void)device;
    (void)pAllocator;
    if (pS == NULL) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_SHADER; i++) {
        if (&g_aShd[i] == pS) {
            g_aShdUsed[i] = 0;
            pS->u32Magic = 0;
            return;
        }
    }
}

VKAPI_ATTR uint32_t VKAPI_CALL
vkGetShaderModuleWordCountGJ(VkShaderModule module)
{
    struct gj_shader *pS = shd_from_handle(module);

    return pS != NULL ? pS->u32WordCount : 0u;
}

VKAPI_ATTR uint32_t VKAPI_CALL
vkGetShaderModuleSpecColorGJ(VkShaderModule module)
{
    struct gj_shader *pS = shd_from_handle(module);

    return (pS != NULL && pS->u8HasSpecColor) ? pS->u32SpecColor : 0u;
}

VKAPI_ATTR uint32_t VKAPI_CALL
vkGetShaderModuleExecMaskGJ(VkShaderModule module)
{
    struct gj_shader *pS = shd_from_handle(module);

    return pS != NULL ? pS->u32ExecMask : 0u;
}

VKAPI_ATTR void *VKAPI_CALL
vkGetBufferHostPointerGJ(VkBuffer buffer)
{
    struct gj_buffer *pB = (struct gj_buffer *)(uintptr_t)buffer;

    if (pB == NULL || pB->u32Magic != MAGIC_BUF) {
        return NULL;
    }
    return pB->pHost;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateImageView(VkDevice device, const void *pCreateInfo,
                  const VkAllocationCallbacks *pAllocator, VkImageView *pView)
{
    (void)device;
    (void)pCreateInfo;
    (void)pAllocator;
    if (pView == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    *pView = (VkImageView)(++g_u64NextNd);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyImageView(VkDevice device, VkImageView imageView,
                   const VkAllocationCallbacks *pAllocator)
{
    (void)device;
    (void)imageView;
    (void)pAllocator;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreatePipelineLayout(VkDevice device, const void *pCreateInfo,
                       const VkAllocationCallbacks *pAllocator,
                       VkPipelineLayout *pPipelineLayout)
{
    (void)device;
    (void)pCreateInfo;
    (void)pAllocator;
    if (pPipelineLayout == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    *pPipelineLayout = (VkPipelineLayout)(++g_u64NextNd);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout,
                        const VkAllocationCallbacks *pAllocator)
{
    (void)device;
    (void)pipelineLayout;
    (void)pAllocator;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDescriptorSetLayout(VkDevice device, const void *pCreateInfo,
                            const VkAllocationCallbacks *pAllocator,
                            VkDescriptorSetLayout *pSetLayout)
{
    (void)device;
    (void)pCreateInfo;
    (void)pAllocator;
    if (pSetLayout == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    *pSetLayout = (VkDescriptorSetLayout)(++g_u64NextNd);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout,
                             const VkAllocationCallbacks *pAllocator)
{
    (void)device;
    (void)descriptorSetLayout;
    (void)pAllocator;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDescriptorPool(VkDevice device, const void *pCreateInfo,
                       const VkAllocationCallbacks *pAllocator, VkDescriptorPool *pPool)
{
    (void)device;
    (void)pCreateInfo;
    (void)pAllocator;
    if (pPool == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    *pPool = (VkDescriptorPool)(++g_u64NextNd);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool,
                        const VkAllocationCallbacks *pAllocator)
{
    (void)device;
    (void)descriptorPool;
    (void)pAllocator;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkAllocateDescriptorSets(VkDevice device, const void *pAllocateInfo,
                         VkDescriptorSet *pDescriptorSets)
{
    (void)device;
    (void)pAllocateInfo;
    if (pDescriptorSets == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    pDescriptorSets[0] = (VkDescriptorSet)(++g_u64NextNd);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool,
                     uint32_t descriptorSetCount, const VkDescriptorSet *pDescriptorSets)
{
    (void)device;
    (void)descriptorPool;
    (void)descriptorSetCount;
    (void)pDescriptorSets;
    return VK_SUCCESS; /* soft success — sets are opaque counters */
}

VKAPI_ATTR void VKAPI_CALL
vkUpdateDescriptorSets(VkDevice device, uint32_t writeCount, const void *pWrites,
                       uint32_t copyCount, const void *pCopies)
{
    (void)device;
    (void)writeCount;
    (void)pWrites;
    (void)copyCount;
    (void)pCopies;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, uint32_t pipelineBindPoint,
                        VkPipelineLayout layout, uint32_t firstSet,
                        uint32_t descriptorSetCount,
                        const VkDescriptorSet *pDescriptorSets,
                        uint32_t dynamicOffsetCount, const uint32_t *pDynamicOffsets)
{
    /* Soft nop: software FS uses bind-texture GJ path for T0. */
    (void)commandBuffer;
    (void)pipelineBindPoint;
    (void)layout;
    (void)firstSet;
    (void)descriptorSetCount;
    (void)pDescriptorSets;
    (void)dynamicOffsetCount;
    (void)pDynamicOffsets;
}

/* ---- Buffers + dynamic state (vertex / push / viewport) ---- */

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateBuffer(VkDevice device, const VkBufferCreateInfo *pCreateInfo,
               const VkAllocationCallbacks *pAllocator, VkBuffer *pBuffer)
{
    uint32_t i;

    (void)device;
    (void)pAllocator;
    if (pBuffer == NULL || pCreateInfo == NULL || pCreateInfo->size == 0) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    for (i = 0; i < GJ_VK_MAX_BUF; i++) {
        if (!g_aBufUsed[i]) {
            g_aBufUsed[i] = 1;
            memset(&g_aBuf[i], 0, sizeof(g_aBuf[i]));
            g_aBuf[i].u32Magic = MAGIC_BUF;
            g_aBuf[i].size = pCreateInfo->size;
#ifdef GJ_VK_KERNEL_SMOKE
            g_aBuf[i].pHost = kheap_alloc((size_t)pCreateInfo->size);
#else
            g_aBuf[i].pHost = malloc((size_t)pCreateInfo->size);
#endif
            if (g_aBuf[i].pHost == NULL) {
                g_aBufUsed[i] = 0;
                return VK_ERROR_OUT_OF_HOST_MEMORY;
            }
            memset(g_aBuf[i].pHost, 0, (size_t)pCreateInfo->size);
            g_aBuf[i].u8Bound = 1; /* host-visible auto-bind for T0 */
            *pBuffer = (VkBuffer)(uintptr_t)(void *)&g_aBuf[i];
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_TOO_MANY_OBJECTS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyBuffer(VkDevice device, VkBuffer buffer,
                const VkAllocationCallbacks *pAllocator)
{
    struct gj_buffer *pB = (struct gj_buffer *)(uintptr_t)buffer;
    uint32_t i;

    (void)device;
    (void)pAllocator;
    if (pB == NULL || pB->u32Magic != MAGIC_BUF) {
        return;
    }
    for (i = 0; i < GJ_VK_MAX_BUF; i++) {
        if (&g_aBuf[i] == pB) {
#ifndef GJ_VK_KERNEL_SMOKE
            free(pB->pHost);
#endif
            pB->pHost = NULL;
            pB->u32Magic = 0;
            g_aBufUsed[i] = 0;
            return;
        }
    }
}

VKAPI_ATTR void VKAPI_CALL
vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer,
                              VkMemoryRequirements *pMemoryRequirements)
{
    struct gj_buffer *pB = (struct gj_buffer *)(uintptr_t)buffer;

    (void)device;
    if (pMemoryRequirements == NULL) {
        return;
    }
    pMemoryRequirements->size = (pB != NULL && pB->u32Magic == MAGIC_BUF)
                                    ? pB->size
                                    : 0;
    pMemoryRequirements->alignment = 16;
    pMemoryRequirements->memoryTypeBits = 0x1u;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory,
                   VkDeviceSize memoryOffset)
{
    struct gj_buffer *pB = (struct gj_buffer *)(uintptr_t)buffer;
    struct gj_mem_obj *pM = (struct gj_mem_obj *)(uintptr_t)memory;

    (void)device;
    (void)memoryOffset;
    if (pB == NULL || pB->u32Magic != MAGIC_BUF) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (pM != NULL && pM->u32Magic == MAGIC_MEM && pM->pHost != NULL) {
        if (pM->size >= pB->size) {
#ifndef GJ_VK_KERNEL_SMOKE
            if (pB->pHost != NULL) {
                free(pB->pHost);
            }
#endif
            pB->pHost = pM->pHost;
        }
    }
    pB->u8Bound = 1;
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding,
                       uint32_t bindingCount, const VkBuffer *pBuffers,
                       const VkDeviceSize *pOffsets)
{
    (void)commandBuffer;
    (void)firstBinding;
    if (bindingCount == 0 || pBuffers == NULL) {
        g_u8VboBound = 0;
        g_pVboHost = NULL;
        g_vboSize = 0;
        return;
    }
    {
        struct gj_buffer *pB = (struct gj_buffer *)(uintptr_t)pBuffers[0];

        if (pB != NULL && pB->u32Magic == MAGIC_BUF && pB->pHost != NULL) {
            g_pVboHost = pB->pHost;
            g_vboOff = (pOffsets != NULL) ? pOffsets[0] : 0;
            g_vboSize = pB->size > g_vboOff ? pB->size - g_vboOff : 0;
            g_u8VboBound = 1;
        } else {
            g_u8VboBound = 0;
            g_pVboHost = NULL;
            g_vboSize = 0;
        }
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                   uint32_t stageFlags, uint32_t offset, uint32_t size,
                   const void *pValues)
{
    (void)commandBuffer;
    (void)layout;
    (void)stageFlags;
    if (size == 0u) {
        g_u8PushColorSet = 0;
        return;
    }
    if (pValues != NULL && size >= 4u && offset == 0u) {
        g_u32PushColor = *(const uint32_t *)pValues;
        g_u8PushColorSet = 1;
        g_u32ActiveClear = g_u32PushColor;
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport,
                 uint32_t viewportCount, const VkViewport *pViewports)
{
    (void)commandBuffer;
    (void)firstViewport;
#ifndef GJ_VK_KERNEL_SMOKE
    if (viewportCount > 0 && pViewports != NULL) {
        g_iVpX = (int32_t)pViewports[0].x;
        g_iVpY = (int32_t)pViewports[0].y;
        g_u32VpW = (uint32_t)pViewports[0].width;
        g_u32VpH = (uint32_t)pViewports[0].height;
        g_u8VpSet = 1;
    }
#else
    (void)viewportCount;
    (void)pViewports;
#endif
}

VKAPI_ATTR void VKAPI_CALL
vkCmdSetViewportGJ(VkCommandBuffer commandBuffer, int32_t x, int32_t y,
                   uint32_t w, uint32_t h)
{
    (void)commandBuffer;
    g_iVpX = x;
    g_iVpY = y;
    g_u32VpW = w;
    g_u32VpH = h;
    g_u8VpSet = 1;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor,
                uint32_t scissorCount, const VkRect2D *pScissors)
{
    (void)commandBuffer;
    (void)firstScissor;
    if (scissorCount > 0 && pScissors != NULL) {
        g_iVpX = pScissors[0].offset.x;
        g_iVpY = pScissors[0].offset.y;
        g_u32VpW = pScissors[0].extent.width;
        g_u32VpH = pScissors[0].extent.height;
        g_u8VpSet = 1;
    }
}

/* Map swapchain image handle → CPU pixels (ICD extension for T0 CPU blit) */
VKAPI_ATTR void *VKAPI_CALL
vkMapSwapchainImageGJ(VkDevice device, VkSwapchainKHR swapchain,
                      uint32_t imageIndex, uint32_t *pStride)
{
    struct gj_swapchain *pSc = (struct gj_swapchain *)(uintptr_t)swapchain;

    if (pSc == NULL || pSc->u32Magic != MAGIC_SC ||
        imageIndex >= pSc->u32Count || !pSc->aImg[imageIndex].u8Alive) {
        return NULL;
    }
    if (device != NULL && device->u32Magic == MAGIC_DEV && pSc->pDev != NULL &&
        pSc->pDev != device) {
        return NULL;
    }
    /* Soft: map allowed without acquire for CPU blit smoke; prefer acquired. */
    if (pStride) {
        *pStride = pSc->aImg[imageIndex].u32Stride;
    }
    return pSc->aImg[imageIndex].pPixels;
}

/* ========================= Product deepen stubs ========================= */
/*
 * Soft success: host-coherent / barrier-like / no-op shapes already patterned
 * elsewhere (WaitIdle, PipelineBarrier). FEATURE_NOT_PRESENT: sparse, queries,
 * compute pipelines, multi-draw indirect — product apps probe these via GIPA.
 */

VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceVersion(uint32_t *pApiVersion)
{
    if (pApiVersion == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    *pApiVersion = VK_API_VERSION_1_0;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t *pCount,
                                 VkLayerProperties *pProps)
{
    (void)physicalDevice;
    if (pCount == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (pProps == NULL) {
        *pCount = 0;
        return VK_SUCCESS;
    }
    *pCount = 0;
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                    VkFormatProperties *pFormatProperties)
{
    uint32_t feats = 0;

    (void)physicalDevice;
    if (pFormatProperties == NULL) {
        return;
    }
    memset(pFormatProperties, 0, sizeof(*pFormatProperties));
    if (format == VK_FORMAT_B8G8R8A8_UNORM || format == VK_FORMAT_R8G8B8A8_UNORM ||
        format == VK_FORMAT_B8G8R8A8_SRGB) {
        feats = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
                VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
                VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    }
    if (format == VK_FORMAT_R8G8B8A8_UNORM || format == VK_FORMAT_B8G8R8A8_UNORM) {
        pFormatProperties->bufferFeatures = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    }
    pFormatProperties->linearTilingFeatures = feats;
    pFormatProperties->optimalTilingFeatures = feats;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice,
                                         VkFormat format, uint32_t type,
                                         uint32_t tiling, VkImageUsageFlags usage,
                                         uint32_t flags,
                                         VkImageFormatProperties *pProps)
{
    (void)physicalDevice;
    (void)type;
    (void)tiling;
    (void)usage;
    (void)flags;
    if (pProps == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (format != VK_FORMAT_B8G8R8A8_UNORM && format != VK_FORMAT_R8G8B8A8_UNORM &&
        format != VK_FORMAT_B8G8R8A8_SRGB) {
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }
    memset(pProps, 0, sizeof(*pProps));
    pProps->maxExtent.width = 256;
    pProps->maxExtent.height = 256;
    pProps->maxExtent.depth = 1;
    pProps->maxMipLevels = 1;
    pProps->maxArrayLayers = 1;
    pProps->sampleCounts = VK_SAMPLE_COUNT_1_BIT;
    pProps->maxResourceSize = 256ull * 256ull * 4ull;
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice,
                                               VkFormat format, uint32_t type,
                                               VkSampleMask samples, uint32_t usage,
                                               uint32_t tiling, uint32_t *pPropertyCount,
                                               void *pProperties)
{
    (void)physicalDevice;
    (void)format;
    (void)type;
    (void)samples;
    (void)usage;
    (void)tiling;
    (void)pProperties;
    /* Sparse not present — report zero properties (soft empty). */
    if (pPropertyCount != NULL) {
        *pPropertyCount = 0;
    }
}

VKAPI_ATTR VkResult VKAPI_CALL
vkGetFenceStatus(VkDevice device, VkFence fence)
{
    struct gj_fence *pF = (struct gj_fence *)(uintptr_t)fence;

    (void)device;
    if (pF == NULL || pF->u32Magic != MAGIC_FEN) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return pF->u8Signaled ? VK_SUCCESS : VK_NOT_READY;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkResetCommandBuffer(VkCommandBuffer commandBuffer, VkFlags flags)
{
    (void)flags;
    if (commandBuffer == NULL || commandBuffer->u32Magic != MAGIC_CMD) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    commandBuffer->u8Recording = 0;
    commandBuffer->u32NOps = 0;
    memset(commandBuffer->aOps, 0, sizeof(commandBuffer->aOps));
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkResetCommandPool(VkDevice device, VkCommandPool commandPool, VkFlags flags)
{
    uint32_t i;

    (void)device;
    (void)commandPool;
    (void)flags;
    /* Soft: clear all live command buffers on this freestanding pool model. */
    for (i = 0; i < GJ_VK_MAX_CMD; i++) {
        if (g_aCmdUsed[i]) {
            g_aCmd[i].u8Recording = 0;
            g_aCmd[i].u32NOps = 0;
        }
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount,
                          const VkMappedMemoryRange *pMemoryRanges)
{
    /* Host-coherent T0 — flush is a soft success no-op. */
    (void)device;
    (void)memoryRangeCount;
    (void)pMemoryRanges;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount,
                               const VkMappedMemoryRange *pMemoryRanges)
{
    (void)device;
    (void)memoryRangeCount;
    (void)pMemoryRanges;
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkGetDeviceMemoryCommitment(VkDevice device, VkDeviceMemory memory,
                            VkDeviceSize *pCommittedMemoryInBytes)
{
    struct gj_mem_obj *pM = (struct gj_mem_obj *)(uintptr_t)memory;

    (void)device;
    if (pCommittedMemoryInBytes == NULL) {
        return;
    }
    *pCommittedMemoryInBytes =
        (pM != NULL && pM->u32Magic == MAGIC_MEM) ? pM->size : 0;
}

VKAPI_ATTR void VKAPI_CALL
vkGetImageSubresourceLayout(VkDevice device, VkImage image,
                            const VkImageSubresource *pSubresource,
                            VkSubresourceLayout *pLayout)
{
    void *pix = NULL;
    uint32_t stride = 0;
    uint32_t w = 0;
    uint32_t h = 0;

    (void)device;
    (void)pSubresource;
    if (pLayout == NULL) {
        return;
    }
    memset(pLayout, 0, sizeof(*pLayout));
    if (img_resolve(image, &pix, &stride, &w, &h) == 0) {
        pLayout->offset = 0;
        pLayout->rowPitch = stride;
        pLayout->size = (VkDeviceSize)stride * h;
        pLayout->arrayPitch = pLayout->size;
        pLayout->depthPitch = pLayout->size;
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer,
                uint32_t regionCount, const VkBufferCopy *pRegions)
{
    struct gj_buffer *pSrc = (struct gj_buffer *)(uintptr_t)srcBuffer;
    struct gj_buffer *pDst = (struct gj_buffer *)(uintptr_t)dstBuffer;
    uint32_t i;

    (void)commandBuffer;
    if (pSrc == NULL || pDst == NULL || pSrc->u32Magic != MAGIC_BUF ||
        pDst->u32Magic != MAGIC_BUF || pSrc->pHost == NULL || pDst->pHost == NULL ||
        pRegions == NULL) {
        return;
    }
    for (i = 0; i < regionCount; i++) {
        VkDeviceSize so = pRegions[i].srcOffset;
        VkDeviceSize doff = pRegions[i].dstOffset;
        VkDeviceSize sz = pRegions[i].size;
        u8 *s;
        u8 *d;
        VkDeviceSize k;

        if (so + sz > pSrc->size || doff + sz > pDst->size) {
            continue;
        }
        s = (u8 *)pSrc->pHost + (size_t)so;
        d = (u8 *)pDst->pHost + (size_t)doff;
        for (k = 0; k < sz; k++) {
            d[k] = s[k];
        }
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer,
                       VkImage dstImage, uint32_t dstImageLayout,
                       uint32_t regionCount, const VkBufferImageCopy *pRegions)
{
    struct gj_buffer *pSrc = (struct gj_buffer *)(uintptr_t)srcBuffer;
    void *pPix = NULL;
    uint32_t stride = 0;
    uint32_t w = 0;
    uint32_t h = 0;
    uint32_t r;

    (void)commandBuffer;
    (void)dstImageLayout;
    if (pSrc == NULL || pSrc->u32Magic != MAGIC_BUF || pSrc->pHost == NULL ||
        pRegions == NULL) {
        return;
    }
    if (img_resolve(dstImage, &pPix, &stride, &w, &h) != 0 || pPix == NULL) {
        return;
    }
    for (r = 0; r < regionCount; r++) {
        uint32_t iw = pRegions[r].imageExtent.width ? pRegions[r].imageExtent.width : w;
        uint32_t ih =
            pRegions[r].imageExtent.height ? pRegions[r].imageExtent.height : h;
        uint32_t ox = (uint32_t)pRegions[r].imageOffset.x;
        uint32_t oy = (uint32_t)pRegions[r].imageOffset.y;
        VkDeviceSize bo = pRegions[r].bufferOffset;
        uint32_t y;
        uint32_t x;

        if (ox >= w || oy >= h) {
            continue;
        }
        if (ox + iw > w) {
            iw = w - ox;
        }
        if (oy + ih > h) {
            ih = h - oy;
        }
        for (y = 0; y < ih; y++) {
            for (x = 0; x < iw; x++) {
                VkDeviceSize bi = bo + (VkDeviceSize)(y * iw + x) * 4u;
                u8 *dp;
                u8 *sp;

                if (bi + 4u > pSrc->size) {
                    break;
                }
                sp = (u8 *)pSrc->pHost + (size_t)bi;
                dp = (u8 *)pPix + (oy + y) * stride + (ox + x) * 4u;
                dp[0] = sp[0];
                dp[1] = sp[1];
                dp[2] = sp[2];
                dp[3] = sp[3];
            }
        }
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage,
                       uint32_t srcImageLayout, VkBuffer dstBuffer,
                       uint32_t regionCount, const VkBufferImageCopy *pRegions)
{
    struct gj_buffer *pDst = (struct gj_buffer *)(uintptr_t)dstBuffer;
    void *pPix = NULL;
    uint32_t stride = 0;
    uint32_t w = 0;
    uint32_t h = 0;
    uint32_t r;

    (void)commandBuffer;
    (void)srcImageLayout;
    if (pDst == NULL || pDst->u32Magic != MAGIC_BUF || pDst->pHost == NULL ||
        pRegions == NULL) {
        return;
    }
    if (img_resolve(srcImage, &pPix, &stride, &w, &h) != 0 || pPix == NULL) {
        return;
    }
    for (r = 0; r < regionCount; r++) {
        uint32_t iw = pRegions[r].imageExtent.width ? pRegions[r].imageExtent.width : w;
        uint32_t ih =
            pRegions[r].imageExtent.height ? pRegions[r].imageExtent.height : h;
        uint32_t ox = (uint32_t)pRegions[r].imageOffset.x;
        uint32_t oy = (uint32_t)pRegions[r].imageOffset.y;
        VkDeviceSize bo = pRegions[r].bufferOffset;
        uint32_t y;
        uint32_t x;

        if (ox >= w || oy >= h) {
            continue;
        }
        if (ox + iw > w) {
            iw = w - ox;
        }
        if (oy + ih > h) {
            ih = h - oy;
        }
        for (y = 0; y < ih; y++) {
            for (x = 0; x < iw; x++) {
                VkDeviceSize bi = bo + (VkDeviceSize)(y * iw + x) * 4u;
                u8 *dp;
                u8 *sp;

                if (bi + 4u > pDst->size) {
                    break;
                }
                sp = (u8 *)pPix + (oy + y) * stride + (ox + x) * 4u;
                dp = (u8 *)pDst->pHost + (size_t)bi;
                dp[0] = sp[0];
                dp[1] = sp[1];
                dp[2] = sp[2];
                dp[3] = sp[3];
            }
        }
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, uint32_t srcImageLayout,
               VkImage dstImage, uint32_t dstImageLayout, uint32_t regionCount,
               const VkImageCopy *pRegions)
{
    void *pS = NULL;
    void *pD = NULL;
    uint32_t sStride = 0;
    uint32_t dStride = 0;
    uint32_t sw = 0;
    uint32_t sh = 0;
    uint32_t dw = 0;
    uint32_t dh = 0;
    uint32_t r;

    (void)commandBuffer;
    (void)srcImageLayout;
    (void)dstImageLayout;
    if (img_resolve(srcImage, &pS, &sStride, &sw, &sh) != 0 ||
        img_resolve(dstImage, &pD, &dStride, &dw, &dh) != 0 || pS == NULL ||
        pD == NULL) {
        return;
    }
    if (pRegions == NULL || regionCount == 0) {
        /* Full soft blit of min extents */
        uint32_t mw = sw < dw ? sw : dw;
        uint32_t mh = sh < dh ? sh : dh;
        uint32_t y;
        uint32_t x;

        for (y = 0; y < mh; y++) {
            for (x = 0; x < mw; x++) {
                u8 *sp = (u8 *)pS + y * sStride + x * 4u;
                u8 *dp = (u8 *)pD + y * dStride + x * 4u;

                dp[0] = sp[0];
                dp[1] = sp[1];
                dp[2] = sp[2];
                dp[3] = sp[3];
            }
        }
        return;
    }
    for (r = 0; r < regionCount; r++) {
        uint32_t iw = pRegions[r].extent.width ? pRegions[r].extent.width : sw;
        uint32_t ih = pRegions[r].extent.height ? pRegions[r].extent.height : sh;
        uint32_t sx = (uint32_t)pRegions[r].srcOffset.x;
        uint32_t sy = (uint32_t)pRegions[r].srcOffset.y;
        uint32_t dx = (uint32_t)pRegions[r].dstOffset.x;
        uint32_t dy = (uint32_t)pRegions[r].dstOffset.y;
        uint32_t y;
        uint32_t x;

        if (sx >= sw || sy >= sh || dx >= dw || dy >= dh) {
            continue;
        }
        if (sx + iw > sw) {
            iw = sw - sx;
        }
        if (sy + ih > sh) {
            ih = sh - sy;
        }
        if (dx + iw > dw) {
            iw = dw - dx;
        }
        if (dy + ih > dh) {
            ih = dh - dy;
        }
        for (y = 0; y < ih; y++) {
            for (x = 0; x < iw; x++) {
                u8 *sp = (u8 *)pS + (sy + y) * sStride + (sx + x) * 4u;
                u8 *dp = (u8 *)pD + (dy + y) * dStride + (dx + x) * 4u;

                dp[0] = sp[0];
                dp[1] = sp[1];
                dp[2] = sp[2];
                dp[3] = sp[3];
            }
        }
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, uint32_t srcImageLayout,
               VkImage dstImage, uint32_t dstImageLayout, uint32_t regionCount,
               const void *pRegions, uint32_t filter)
{
    /* Soft: nearest 1:1 copy of overlapping region (ignore filter/scales). */
    (void)regionCount;
    (void)pRegions;
    (void)filter;
    vkCmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, 0,
                   NULL);
}

VKAPI_ATTR void VKAPI_CALL
vkCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset,
                VkDeviceSize size, uint32_t data)
{
    struct gj_buffer *pB = (struct gj_buffer *)(uintptr_t)dstBuffer;
    u8 *p;
    VkDeviceSize n;
    VkDeviceSize i;

    (void)commandBuffer;
    if (pB == NULL || pB->u32Magic != MAGIC_BUF || pB->pHost == NULL) {
        return;
    }
    if (dstOffset >= pB->size) {
        return;
    }
    n = (size == VK_WHOLE_SIZE) ? (pB->size - dstOffset) : size;
    if (dstOffset + n > pB->size) {
        n = pB->size - dstOffset;
    }
    p = (u8 *)pB->pHost + (size_t)dstOffset;
    for (i = 0; i + 4u <= n; i += 4u) {
        p[i + 0] = (u8)(data & 0xffu);
        p[i + 1] = (u8)((data >> 8) & 0xffu);
        p[i + 2] = (u8)((data >> 16) & 0xffu);
        p[i + 3] = (u8)((data >> 24) & 0xffu);
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer,
                  VkDeviceSize dstOffset, VkDeviceSize dataSize, const void *pData)
{
    struct gj_buffer *pB = (struct gj_buffer *)(uintptr_t)dstBuffer;
    u8 *d;
    const u8 *s;
    VkDeviceSize i;

    (void)commandBuffer;
    if (pB == NULL || pB->u32Magic != MAGIC_BUF || pB->pHost == NULL || pData == NULL ||
        dataSize == 0) {
        return;
    }
    if (dstOffset + dataSize > pB->size) {
        return;
    }
    d = (u8 *)pB->pHost + (size_t)dstOffset;
    s = (const u8 *)pData;
    for (i = 0; i < dataSize; i++) {
        d[i] = s[i];
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount,
                     const VkCommandBuffer *pCommandBuffers)
{
    uint32_t i;

    (void)commandBuffer;
    if (pCommandBuffers == NULL) {
        return;
    }
    for (i = 0; i < commandBufferCount; i++) {
        cmd_execute(pCommandBuffers[i]);
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY,
              uint32_t groupCountZ)
{
    /* Soft nop — compute not implemented; create path returns FEATURE_NOT_PRESENT. */
    (void)commandBuffer;
    (void)groupCountX;
    (void)groupCountY;
    (void)groupCountZ;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset)
{
    (void)commandBuffer;
    (void)buffer;
    (void)offset;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset,
                  uint32_t drawCount, uint32_t stride)
{
    /* Soft: single non-indirect triangle if VBO bound (product probe safety). */
    (void)buffer;
    (void)offset;
    (void)stride;
    if (drawCount > 0) {
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    }
}

VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer,
                         VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
    (void)buffer;
    (void)offset;
    (void)stride;
    if (drawCount > 0) {
        vkCmdDrawIndexed(commandBuffer, 3, 1, 0, 0, 0);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateEvent(VkDevice device, const VkEventCreateInfo *pCreateInfo,
              const VkAllocationCallbacks *pAllocator, VkEvent *pEvent)
{
    (void)device;
    (void)pCreateInfo;
    (void)pAllocator;
    if (pEvent == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    /* Soft handle: low bit tracks signaled (0 = reset). */
    *pEvent = (VkEvent)((++g_u64NextNd) << 1);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks *pAllocator)
{
    (void)device;
    (void)event;
    (void)pAllocator;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkGetEventStatus(VkDevice device, VkEvent event)
{
    (void)device;
    if (event == 0) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return ((event & 1u) != 0) ? VK_EVENT_SET : VK_EVENT_RESET;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkSetEvent(VkDevice device, VkEvent event)
{
    (void)device;
    if (event == 0) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    /* Handles are by-value; callers must re-query after set via cmd path.
     * Soft success: accept set without durable storage beyond fence model. */
    (void)event;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkResetEvent(VkDevice device, VkEvent event)
{
    (void)device;
    if (event == 0) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
    (void)commandBuffer;
    (void)event;
    (void)stageMask;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event,
                VkPipelineStageFlags stageMask)
{
    (void)commandBuffer;
    (void)event;
    (void)stageMask;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent *pEvents,
                VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                uint32_t memoryBarrierCount, const void *pMemoryBarriers,
                uint32_t bufferMemoryBarrierCount, const void *pBufferMemoryBarriers,
                uint32_t imageMemoryBarrierCount, const void *pImageMemoryBarriers)
{
    (void)commandBuffer;
    (void)eventCount;
    (void)pEvents;
    (void)srcStageMask;
    (void)dstStageMask;
    (void)memoryBarrierCount;
    (void)pMemoryBarriers;
    (void)bufferMemoryBarrierCount;
    (void)pBufferMemoryBarriers;
    (void)imageMemoryBarrierCount;
    (void)pImageMemoryBarriers;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo *pCreateInfo,
                      const VkAllocationCallbacks *pAllocator,
                      VkPipelineCache *pPipelineCache)
{
    (void)device;
    (void)pCreateInfo;
    (void)pAllocator;
    if (pPipelineCache == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    *pPipelineCache = (VkPipelineCache)(++g_u64NextNd);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache,
                       const VkAllocationCallbacks *pAllocator)
{
    (void)device;
    (void)pipelineCache;
    (void)pAllocator;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t *pDataSize,
                       void *pData)
{
    (void)device;
    (void)pipelineCache;
    if (pDataSize == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (pData == NULL) {
        *pDataSize = 0;
        return VK_SUCCESS;
    }
    *pDataSize = 0;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkMergePipelineCaches(VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount,
                      const VkPipelineCache *pSrcCaches)
{
    (void)device;
    (void)dstCache;
    (void)srcCacheCount;
    (void)pSrcCaches;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateBufferView(VkDevice device, const void *pCreateInfo,
                   const VkAllocationCallbacks *pAllocator, VkBufferView *pView)
{
    (void)device;
    (void)pCreateInfo;
    (void)pAllocator;
    if (pView == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    *pView = (VkBufferView)(++g_u64NextNd);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyBufferView(VkDevice device, VkBufferView bufferView,
                    const VkAllocationCallbacks *pAllocator)
{
    (void)device;
    (void)bufferView;
    (void)pAllocator;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache,
                         uint32_t createInfoCount, const void *pCreateInfos,
                         const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines)
{
    (void)device;
    (void)pipelineCache;
    (void)createInfoCount;
    (void)pCreateInfos;
    (void)pAllocator;
    if (pPipelines != NULL && createInfoCount > 0) {
        uint32_t i;

        for (i = 0; i < createInfoCount; i++) {
            pPipelines[i] = 0;
        }
    }
    return VK_ERROR_FEATURE_NOT_PRESENT;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkQueueBindSparse(VkQueue queue, uint32_t bindInfoCount, const void *pBindInfo,
                  VkFence fence)
{
    (void)queue;
    (void)bindInfoCount;
    (void)pBindInfo;
    (void)fence;
    return VK_ERROR_FEATURE_NOT_PRESENT;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo *pCreateInfo,
                  const VkAllocationCallbacks *pAllocator, VkQueryPool *pQueryPool)
{
    (void)device;
    (void)pCreateInfo;
    (void)pAllocator;
    if (pQueryPool != NULL) {
        *pQueryPool = 0;
    }
    return VK_ERROR_FEATURE_NOT_PRESENT;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyQueryPool(VkDevice device, VkQueryPool queryPool,
                   const VkAllocationCallbacks *pAllocator)
{
    (void)device;
    (void)queryPool;
    (void)pAllocator;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery,
                      uint32_t queryCount, size_t dataSize, void *pData, uint64_t stride,
                      VkFlags flags)
{
    (void)device;
    (void)queryPool;
    (void)firstQuery;
    (void)queryCount;
    (void)dataSize;
    (void)pData;
    (void)stride;
    (void)flags;
    return VK_ERROR_FEATURE_NOT_PRESENT;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query,
                VkFlags flags)
{
    (void)commandBuffer;
    (void)queryPool;
    (void)query;
    (void)flags;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query)
{
    (void)commandBuffer;
    (void)queryPool;
    (void)query;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool,
                    uint32_t firstQuery, uint32_t queryCount)
{
    (void)commandBuffer;
    (void)queryPool;
    (void)firstQuery;
    (void)queryCount;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlags pipelineStage,
                    VkQueryPool queryPool, uint32_t query)
{
    (void)commandBuffer;
    (void)pipelineStage;
    (void)queryPool;
    (void)query;
}

VKAPI_ATTR void VKAPI_CALL
vkCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool,
                          uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer,
                          VkDeviceSize dstOffset, VkDeviceSize stride, VkFlags flags)
{
    (void)commandBuffer;
    (void)queryPool;
    (void)firstQuery;
    (void)queryCount;
    (void)dstBuffer;
    (void)dstOffset;
    (void)stride;
    (void)flags;
}

/* ========================= GetProcAddr ========================= */

static PFN_vkVoidFunction
lookup_name(const char *pName)
{
    vk_icd_soft_inc(&g_u32SoftLookupN);
    if (pName == NULL) {
        vk_icd_soft_inc(&g_u32SoftLookupMiss);
        return NULL;
    }
#define ENT(n)                                                                 \
    if (streq(pName, #n)) {                                                    \
        vk_icd_soft_inc(&g_u32SoftLookupHit);                                  \
        return (PFN_vkVoidFunction)n;                                          \
    }
    ENT(vkCreateInstance);
    ENT(vkDestroyInstance);
    ENT(vkEnumerateInstanceExtensionProperties);
    ENT(vkEnumerateInstanceLayerProperties);
    ENT(vkGetInstanceProcAddr);
    ENT(vkEnumeratePhysicalDevices);
    ENT(vkGetPhysicalDeviceProperties);
    ENT(vkGetPhysicalDeviceFeatures);
    ENT(vkGetPhysicalDeviceQueueFamilyProperties);
    ENT(vkGetPhysicalDeviceMemoryProperties);
    ENT(vkEnumerateDeviceExtensionProperties);
    ENT(vkCreateDevice);
    ENT(vkDestroyDevice);
    ENT(vkGetDeviceProcAddr);
    ENT(vkGetDeviceQueue);
    ENT(vkDeviceWaitIdle);
    ENT(vkQueueWaitIdle);
    ENT(vkQueueSubmit);
    ENT(vkCreateCommandPool);
    ENT(vkDestroyCommandPool);
    ENT(vkAllocateCommandBuffers);
    ENT(vkFreeCommandBuffers);
    ENT(vkBeginCommandBuffer);
    ENT(vkEndCommandBuffer);
    ENT(vkCreateFence);
    ENT(vkDestroyFence);
    ENT(vkWaitForFences);
    ENT(vkResetFences);
    ENT(vkCreateSemaphore);
    ENT(vkDestroySemaphore);
    ENT(vkDestroySurfaceKHR);
    ENT(vkGetPhysicalDeviceSurfaceSupportKHR);
    ENT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    ENT(vkGetPhysicalDeviceSurfaceFormatsKHR);
    ENT(vkGetPhysicalDeviceSurfacePresentModesKHR);
    ENT(vkCreateSwapchainKHR);
    ENT(vkDestroySwapchainKHR);
    ENT(vkGetSwapchainImagesKHR);
    ENT(vkAcquireNextImageKHR);
    ENT(vkQueuePresentKHR);
    ENT(vkCreateHeadlessSurfaceGJ);
    ENT(vkMapSwapchainImageGJ);
    ENT(vkCmdClearColorImage);
    ENT(vkCmdClearColorImageGJ);
    ENT(vkCmdBeginRenderPass);
    ENT(vkCmdEndRenderPass);
    ENT(vkCmdPipelineBarrier);
    ENT(vkCmdBindPipeline);
    ENT(vkCmdBindFramebufferImageGJ);
    ENT(vkCmdBindTextureImageGJ);
    ENT(vkCreateSampler);
    ENT(vkDestroySampler);
    ENT(vkCreateImage);
    ENT(vkDestroyImage);
    ENT(vkGetImageMemoryRequirements);
    ENT(vkAllocateMemory);
    ENT(vkFreeMemory);
    ENT(vkBindImageMemory);
    ENT(vkMapMemory);
    ENT(vkUnmapMemory);
    ENT(vkCreateRenderPass);
    ENT(vkDestroyRenderPass);
    ENT(vkCreateFramebuffer);
    ENT(vkDestroyFramebuffer);
    ENT(vkCreateGraphicsPipelines);
    ENT(vkDestroyPipeline);
    ENT(vkCmdDraw);
    ENT(vkCmdBindIndexBuffer);
    ENT(vkCmdDrawIndexed);
    ENT(vkCreateShaderModule);
    ENT(vkDestroyShaderModule);
    ENT(vkGetShaderModuleWordCountGJ);
    ENT(vkGetShaderModuleSpecColorGJ);
    ENT(vkGetShaderModuleExecMaskGJ);
    ENT(vkGetBufferHostPointerGJ);
    ENT(vkCreateImageView);
    ENT(vkDestroyImageView);
    ENT(vkCreatePipelineLayout);
    ENT(vkDestroyPipelineLayout);
    ENT(vkCreateBuffer);
    ENT(vkDestroyBuffer);
    ENT(vkGetBufferMemoryRequirements);
    ENT(vkBindBufferMemory);
    ENT(vkCmdBindVertexBuffers);
    ENT(vkCmdPushConstants);
    ENT(vkCmdSetViewport);
    ENT(vkCmdSetViewportGJ);
    ENT(vkCmdSetScissor);
    ENT(vkCreateDescriptorSetLayout);
    ENT(vkDestroyDescriptorSetLayout);
    ENT(vkCreateDescriptorPool);
    ENT(vkDestroyDescriptorPool);
    ENT(vkAllocateDescriptorSets);
    ENT(vkFreeDescriptorSets);
    ENT(vkUpdateDescriptorSets);
    ENT(vkCmdBindDescriptorSets);
    ENT(vkEnumerateInstanceVersion);
    ENT(vkEnumerateDeviceLayerProperties);
    ENT(vkGetPhysicalDeviceFormatProperties);
    ENT(vkGetPhysicalDeviceImageFormatProperties);
    ENT(vkGetPhysicalDeviceSparseImageFormatProperties);
    ENT(vkGetFenceStatus);
    ENT(vkResetCommandBuffer);
    ENT(vkResetCommandPool);
    ENT(vkFlushMappedMemoryRanges);
    ENT(vkInvalidateMappedMemoryRanges);
    ENT(vkGetDeviceMemoryCommitment);
    ENT(vkGetImageSubresourceLayout);
    ENT(vkCmdCopyBuffer);
    ENT(vkCmdCopyBufferToImage);
    ENT(vkCmdCopyImageToBuffer);
    ENT(vkCmdCopyImage);
    ENT(vkCmdBlitImage);
    ENT(vkCmdFillBuffer);
    ENT(vkCmdUpdateBuffer);
    ENT(vkCmdExecuteCommands);
    ENT(vkCmdDispatch);
    ENT(vkCmdDispatchIndirect);
    ENT(vkCmdDrawIndirect);
    ENT(vkCmdDrawIndexedIndirect);
    ENT(vkCreateEvent);
    ENT(vkDestroyEvent);
    ENT(vkGetEventStatus);
    ENT(vkSetEvent);
    ENT(vkResetEvent);
    ENT(vkCmdSetEvent);
    ENT(vkCmdResetEvent);
    ENT(vkCmdWaitEvents);
    ENT(vkCreatePipelineCache);
    ENT(vkDestroyPipelineCache);
    ENT(vkGetPipelineCacheData);
    ENT(vkMergePipelineCaches);
    ENT(vkCreateBufferView);
    ENT(vkDestroyBufferView);
    ENT(vkCreateComputePipelines);
    ENT(vkQueueBindSparse);
    ENT(vkCreateQueryPool);
    ENT(vkDestroyQueryPool);
    ENT(vkGetQueryPoolResults);
    ENT(vkCmdBeginQuery);
    ENT(vkCmdEndQuery);
    ENT(vkCmdResetQueryPool);
    ENT(vkCmdWriteTimestamp);
    ENT(vkCmdCopyQueryPoolResults);
    ENT(vk_icdNegotiateLoaderICDInterfaceVersion);
    ENT(vk_icdGetInstanceProcAddr);
    ENT(vk_icdGetPhysicalDeviceProcAddr);
#undef ENT
    vk_icd_soft_inc(&g_u32SoftLookupMiss);
    return NULL;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetInstanceProcAddr(VkInstance instance, const char *pName)
{
    (void)instance;
    return lookup_name(pName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetDeviceProcAddr(VkDevice device, const char *pName)
{
    (void)device;
    return lookup_name(pName);
}

/* ========================= ICD loader interface ========================== */

VKAPI_ATTR VkResult VKAPI_CALL
vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t *pSupportedVersion)
{
    vk_icd_soft_inc(&g_u32SoftNegotiateN);
    if (pSupportedVersion == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (*pSupportedVersion > CURRENT_LOADER_ICD_INTERFACE_VERSION) {
        *pSupportedVersion = CURRENT_LOADER_ICD_INTERFACE_VERSION;
    }
    /* MIN supported version is 0; always accept the negotiated range. */
#ifdef GJ_VK_KERNEL_SMOKE
    kprintf("vk_icd: negotiate version=%u\n", *pSupportedVersion);
#endif
    return VK_SUCCESS;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vk_icdGetInstanceProcAddr(VkInstance instance, const char *pName)
{
    return vkGetInstanceProcAddr(instance, pName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vk_icdGetPhysicalDeviceProcAddr(VkInstance instance, const char *pName)
{
    (void)instance;
    return lookup_name(pName);
}
