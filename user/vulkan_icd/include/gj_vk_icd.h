/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Vulkan-shaped convenience surface for Proton A1 (T0 virtio-gpu).
 * Not a full Khronos ICD — expands from Deck Top 50 present needs.
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

/** Query scanout size. Returns 0 on success, -1 on failure. */
int gj_vk_get_display(struct gj_vk_display_info *pOut);

/** Present a single image (legacy one-shot path). Returns 0 on success. */
int gj_vk_queue_present(const struct gj_vk_image *pImg);

/** Instance / device / swapchain lifecycle (Vulkan-shaped names). */
int gj_vk_create_instance(gj_vk_instance_t *ppOut);
void gj_vk_destroy_instance(gj_vk_instance_t pInst);
int gj_vk_create_device(gj_vk_instance_t pInst, gj_vk_device_t *ppOut);
void gj_vk_destroy_device(gj_vk_device_t pDev);
int gj_vk_create_swapchain(gj_vk_device_t pDev, uint32_t u32ImageCount,
                           gj_vk_swapchain_t *ppOut);
void gj_vk_destroy_swapchain(gj_vk_swapchain_t pSc);
/** Acquire next image index; returns 0 and writes *pIndex. */
int gj_vk_acquire_next_image(gj_vk_swapchain_t pSc, uint32_t *pIndex);
/** Present image at index (must be acquired). Returns 0 on success. */
int gj_vk_queue_present_image(gj_vk_swapchain_t pSc, uint32_t u32Index);
/** CPU write pointer for an acquired swapchain image (NULL if invalid). */
void *gj_vk_image_pixels(gj_vk_swapchain_t pSc, uint32_t u32Index);

const char *gj_vk_icd_name(void);
uint32_t gj_vk_icd_features(void);

/** Host software-present frame counter (ICD without virtio-gpu). */
uint32_t gj_vk_host_present_count(void);

#ifdef __cplusplus
}
#endif
