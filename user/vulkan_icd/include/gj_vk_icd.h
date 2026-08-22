/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Vulkan-shaped convenience surface for Proton A1 (T0 virtio-gpu).
 * Not a full Khronos ICD — expands from Deck Top 50 present needs.
 *
 * ---------------------------------------------------------------------------
 * Soft residual deepen (C2 vulkan_icd; Soft!=product; G-AC-1; Dual DoD OPEN)
 * ---------------------------------------------------------------------------
 * Soft path (this header; observation / host smoke only):
 *   host software-present count + soft CRC of corner pixels
 *   soft inventory / deepen / path / honesty cold strings
 *   soft wave / surfaces / areas catalog + note counters
 *   freestanding convenience instance / device / swapchain helpers
 * Product (OPEN; soft lamps never close product AC):
 *   virtio-gpu T0 present + Proton A1 product WSI path
 *   full Khronos loader ICD product AC (OPEN residual)
 * Soft!=product: host present CRC/count never hard-gates product returns.
 * multi_server=0 confine=0; Soft residual lamps != Dual DoD A/B close.
 * Dual DoD A/B remain OPEN (UDX USB/NIC product track; not this unit).
 * product=UDX+ABI for class drivers; this unit is GPU T0 soft residual.
 * G-AC-1: no Linux .ko product AC; dual MIT OR Apache-2.0; no GPL.
 * Stamp-free bar honesty v2026.08.04.75. NEVER bump GJ_IMAGE_VERSION here.
 * No invent intermediate image stamps. Not GJ_IMAGE_VERSION.
 *
 * Soft residual surfaces (lean; Soft!=product):
 *   present residual  — queue_present / present_image + host soft CRC
 *   display residual  — get_display scanout size query (soft fail host stub)
 *   instance residual — create/destroy instance pool (kernel smoke) / stub
 *   device residual   — create/destroy device (virtio-gpu ready gate smoke)
 *   swapchain residual — create/destroy/acquire/pixels soft pool helpers
 *   host residual     — host_present_count / host_present_crc (smoke only)
 *   inventory residual — soft inventory/deepen/path/honesty cold lines
 *   catalog residual  — soft wave / surface / area name catalog
 *   honesty residual  — multi_server=0 confine=0 soft_ne_product=1
 *
 * Dual DoD residual honesty (reaffirm; Soft!=product; agent!=close):
 *   Dual DoD A OPEN — xhci_udx (8086:a12f) Linux-shaped USB userspace
 *   Dual DoD B OPEN — rtl8168_udx (10ec:8168) Linux-shaped NIC userspace
 * Soft host-present / inventory / convenience API != Dual DoD close.
 * freestanding class SKIP; product=UDX+ABI on class-driver track.
 *
 * greppable: gj_vk_icd: soft residual Soft!=product
 * greppable: gj_vk_icd: soft residual lean
 * greppable: gj_vk_icd: soft residual host_present
 * greppable: gj_vk_icd: soft residual inventory
 * greppable: gj_vk_icd: soft residual path
 * greppable: gj_vk_icd: soft residual honesty
 * greppable: gj_vk_icd: soft residual dual_dod
 * greppable: gj_vk_icd: soft residual catalog
 * greppable: Soft!=product
 * Law: dual MIT OR Apache-2.0; Soft!=product; G-AC-1; Dual DoD A/B OPEN;
 *      no GPL; no Linux .ko product; no version stamp; no stamp storms.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GJ_VK_FEAT_PRESENT_2D    (1u << 0)
#define GJ_VK_FEAT_NAMED_EXPORT  (1u << 1)
#define GJ_VK_FEAT_SWAPCHAIN     (1u << 2)
#define GJ_VK_FEAT_INSTANCE      (1u << 3)

#define GJ_VK_MAX_SWAP_IMAGES    3u
#define GJ_VK_FORMAT_B8G8R8X8    0u

/*
 * Soft Cap / Dual DoD residual honesty (eng only; Soft!=product).
 * Always-0 mint/server/confine; Dual DoD A/B remain OPEN until USB path / interactive SSH login.
 * soft_ne_product=1 is the claim-class law (Soft!=product ASCII).
 * greppable: gj_vk_icd: soft residual honesty / dual_dod / Soft!=product
 */
#define GJ_VK_ICD_SOFT_CAP_MINT        0u /* no MMIO/IRQ/DMA cap mint here */
#define GJ_VK_ICD_SOFT_MULTI_SERVER    0u /* soft residual; not multi-server */
#define GJ_VK_ICD_SOFT_CONFINE         0u /* soft residual; not product confine */
#define GJ_VK_ICD_SOFT_DOD_A_OPEN      1u /* xhci_udx Dual DoD A still OPEN */
#define GJ_VK_ICD_SOFT_DOD_B_OPEN      1u /* rtl8168_udx Dual DoD B still OPEN */
#define GJ_VK_ICD_SOFT_NE_PRODUCT      1u /* Soft!=product law flag always */

struct gj_vk_display_info {
    uint32_t u32Width;
    uint32_t u32Height;
};

struct gj_vk_image {
    uint32_t u32Width;
    uint32_t u32Height;
    uint32_t u32Stride;
    uint32_t u32Format;
    void    *pPixels;
    uint64_t u64NamedHint;
};

/* Opaque handles (plain pointers for freestanding C) */
typedef struct gj_vk_instance  *gj_vk_instance_t;
typedef struct gj_vk_device    *gj_vk_device_t;
typedef struct gj_vk_swapchain *gj_vk_swapchain_t;

struct gj_vk_instance {
    uint32_t u32Magic; /* 'GJVI' */
    uint32_t u32Version;
    uint32_t u32Features;
};

struct gj_vk_device {
    uint32_t u32Magic; /* 'GJVD' */
    gj_vk_instance_t pInst;
    uint32_t u32Width;
    uint32_t u32Height;
};

struct gj_vk_swap_image {
    struct gj_vk_image img;
    uint8_t u8Acquired;
    uint8_t u8Pad[3];
    void *pBacking; /* kernel smoke: owned buffer (or compositor fb) */
};

struct gj_vk_swapchain {
    uint32_t u32Magic; /* 'GJVS' */
    gj_vk_device_t pDev;
    uint32_t u32ImageCount;
    uint32_t u32Current;
    uint32_t u32Width;
    uint32_t u32Height;
    struct gj_vk_swap_image aImages[GJ_VK_MAX_SWAP_IMAGES];
};

/** Query scanout size. Returns 0 on success, -1 on failure. Soft!=product. */
int gj_vk_get_display(struct gj_vk_display_info *pOut);

/** Present a single image (legacy one-shot path). Returns 0 on success. Soft!=product. */
int gj_vk_queue_present(const struct gj_vk_image *pImg);

/** Instance / device / swapchain lifecycle (Vulkan-shaped names). Soft!=product. */
int gj_vk_create_instance(gj_vk_instance_t *ppOut);
void gj_vk_destroy_instance(gj_vk_instance_t pInst);
int gj_vk_create_device(gj_vk_instance_t pInst, gj_vk_device_t *ppOut);
void gj_vk_destroy_device(gj_vk_device_t pDev);
int gj_vk_create_swapchain(gj_vk_device_t pDev, uint32_t u32ImageCount,
                           gj_vk_swapchain_t *ppOut);
void gj_vk_destroy_swapchain(gj_vk_swapchain_t pSc);
/** Acquire next image index; returns 0 and writes *pIndex. Soft!=product. */
int gj_vk_acquire_next_image(gj_vk_swapchain_t pSc, uint32_t *pIndex);
/** Present image at index (must be acquired). Returns 0 on success. Soft!=product. */
int gj_vk_queue_present_image(gj_vk_swapchain_t pSc, uint32_t u32Index);
/** CPU write pointer for an acquired swapchain image (NULL if invalid). Soft!=product. */
void *gj_vk_image_pixels(gj_vk_swapchain_t pSc, uint32_t u32Index);

const char *gj_vk_icd_name(void);
uint32_t gj_vk_icd_features(void);

/*
 * Host software-present residual (ICD without virtio-gpu). Soft!=product.
 * Smoke / test observation only — never product WSI AC or Dual DoD close.
 * greppable: gj_vk_icd: soft residual host_present
 */
/** Host software-present frame counter (soft residual; Soft!=product). */
uint32_t gj_vk_host_present_count(void);
/** Host software-present soft CRC of corner pixels (0 if none presented). Soft!=product. */
uint32_t gj_vk_host_present_crc(void);

/*
 * Soft residual cold accessors (implemented in gj_vk_icd.c). Soft!=product.
 * Inventory / deepen / path / honesty strings + catalog counters.
 * Never hard-gate product returns; Dual DoD A/B remain OPEN.
 * greppable: gj_vk_icd: soft residual inventory / path / honesty / catalog
 */
const char *gj_vk_icd_soft_inventory(void);
const char *gj_vk_icd_soft_deepen(void);
const char *gj_vk_icd_soft_path(void);
const char *gj_vk_icd_soft_honesty(void);
uint32_t gj_vk_icd_soft_wave(void);
uint32_t gj_vk_icd_soft_surface_count(void);
uint32_t gj_vk_icd_soft_area_count(void);
const char *gj_vk_icd_soft_area_name(uint32_t uArea);
void gj_vk_icd_soft_note_counts(uint32_t *pPresent, uint32_t *pPresentOk,
                                uint32_t *pCreateInst, uint32_t *pCreateDev,
                                uint32_t *pCreateSc, uint32_t *pAcquire,
                                uint32_t *pInv);

#ifdef __cplusplus
}
#endif
