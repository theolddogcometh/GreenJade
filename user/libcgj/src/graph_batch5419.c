/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5419: Vulkan swapchain surface caps (Deck).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_surface_caps_stub_5419(void);
 *     - Returns 1 (surface capabilities query soft success stub).
 *   uint32_t gj_vk_surface_min_image_count_5419(void);
 *     - Returns soft minImageCount product hint (2 for double-buffer).
 *   uint32_t gj_vk_surface_max_image_count_5419(void);
 *     - Returns soft maxImageCount product hint (0 = unlimited style).
 *   uint32_t gj_vk_surface_deck_extent_w_5419(void);
 *     - Returns Steam Deck LCD native width (1280).
 *   uint32_t gj_vk_surface_deck_extent_h_5419(void);
 *     - Returns Steam Deck LCD native height (800).
 *   uint32_t __gj_vk_surface_caps_stub_5419  (alias)
 *   uint32_t __gj_vk_surface_min_image_count_5419  (alias)
 *   uint32_t __gj_vk_surface_max_image_count_5419  (alias)
 *   uint32_t __gj_vk_surface_deck_extent_w_5419  (alias)
 *   uint32_t __gj_vk_surface_deck_extent_h_5419  (alias)
 *   __libcgj_batch5419_marker = "libcgj-batch5419"
 *
 * Vulkan ICD / instance / device / queue / swapchain Deck exclusive
 * CREATE-ONLY wave (5411-5420). Unique batch-suffixed symbols only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5419_marker[] = "libcgj-batch5419";

/* Steam Deck LCD native panel; maxImageCount 0 = no upper soft bound. */
#define B5419_MIN_IMAGES  2u
#define B5419_MAX_IMAGES  0u
#define B5419_DECK_W      1280u
#define B5419_DECK_H      800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5419_min_images(void)
{
	return B5419_MIN_IMAGES;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_surface_caps_stub_5419 - soft GetPhysicalDeviceSurfaceCaps stub.
 *
 * Always returns 1. Product-path integer tag only.
 */
uint32_t
gj_vk_surface_caps_stub_5419(void)
{
	(void)NULL;
	return 1u;
}

/*
 * gj_vk_surface_min_image_count_5419 - soft minImageCount (double-buffer).
 *
 * Always returns 2.
 */
uint32_t
gj_vk_surface_min_image_count_5419(void)
{
	return b5419_min_images();
}

/*
 * gj_vk_surface_max_image_count_5419 - soft maxImageCount product hint.
 *
 * Always returns 0 (Vulkan unlimited-style soft tag).
 */
uint32_t
gj_vk_surface_max_image_count_5419(void)
{
	return B5419_MAX_IMAGES;
}

/*
 * gj_vk_surface_deck_extent_w_5419 - Deck LCD native currentExtent width.
 *
 * Always returns 1280.
 */
uint32_t
gj_vk_surface_deck_extent_w_5419(void)
{
	return B5419_DECK_W;
}

/*
 * gj_vk_surface_deck_extent_h_5419 - Deck LCD native currentExtent height.
 *
 * Always returns 800.
 */
uint32_t
gj_vk_surface_deck_extent_h_5419(void)
{
	return B5419_DECK_H;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_surface_caps_stub_5419(void)
    __attribute__((alias("gj_vk_surface_caps_stub_5419")));

uint32_t __gj_vk_surface_min_image_count_5419(void)
    __attribute__((alias("gj_vk_surface_min_image_count_5419")));

uint32_t __gj_vk_surface_max_image_count_5419(void)
    __attribute__((alias("gj_vk_surface_max_image_count_5419")));

uint32_t __gj_vk_surface_deck_extent_w_5419(void)
    __attribute__((alias("gj_vk_surface_deck_extent_w_5419")));

uint32_t __gj_vk_surface_deck_extent_h_5419(void)
    __attribute__((alias("gj_vk_surface_deck_extent_h_5419")));
