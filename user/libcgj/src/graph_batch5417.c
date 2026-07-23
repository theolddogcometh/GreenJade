/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5417: Vulkan queue families graphics/present.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_queue_family_count_5417(void);
 *     - Returns soft queue-family count product hint (always 3).
 *   uint32_t gj_vk_queue_graphics_family_5417(void);
 *     - Returns soft graphics queue family index (always 0).
 *   uint32_t gj_vk_queue_present_family_5417(void);
 *     - Returns soft present queue family index (always 0, unified).
 *   uint32_t gj_vk_queue_family_flags_ok_5417(uint32_t flags);
 *     - Returns 1 if GRAPHICS bit is set in flags.
 *   uint32_t __gj_vk_queue_family_count_5417  (alias)
 *   uint32_t __gj_vk_queue_graphics_family_5417  (alias)
 *   uint32_t __gj_vk_queue_present_family_5417  (alias)
 *   uint32_t __gj_vk_queue_family_flags_ok_5417  (alias)
 *   __libcgj_batch5417_marker = "libcgj-batch5417"
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

const char __libcgj_batch5417_marker[] = "libcgj-batch5417";

#define B5417_FAMILY_COUNT  3u
#define B5417_GFX_FAMILY    0u
#define B5417_PRESENT_FAM   0u
#define B5417_Q_GRAPHICS    0x00000001u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5417_flags_ok(uint32_t uFlags)
{
	return ((uFlags & B5417_Q_GRAPHICS) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_queue_family_count_5417 - soft queue-family count for Deck GPU.
 *
 * Always returns 3 (graphics/compute/transfer class product hint).
 */
uint32_t
gj_vk_queue_family_count_5417(void)
{
	(void)NULL;
	return B5417_FAMILY_COUNT;
}

/*
 * gj_vk_queue_graphics_family_5417 - soft graphics family index.
 *
 * Always returns 0.
 */
uint32_t
gj_vk_queue_graphics_family_5417(void)
{
	return B5417_GFX_FAMILY;
}

/*
 * gj_vk_queue_present_family_5417 - soft present family index.
 *
 * Always returns 0 (unified graphics+present Deck path).
 */
uint32_t
gj_vk_queue_present_family_5417(void)
{
	return B5417_PRESENT_FAM;
}

/*
 * gj_vk_queue_family_flags_ok_5417 - 1 if GRAPHICS queue bit is set.
 *
 * flags: VkQueueFamilyProperties::queueFlags style bitfield.
 */
uint32_t
gj_vk_queue_family_flags_ok_5417(uint32_t uFlags)
{
	return b5417_flags_ok(uFlags);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_queue_family_count_5417(void)
    __attribute__((alias("gj_vk_queue_family_count_5417")));

uint32_t __gj_vk_queue_graphics_family_5417(void)
    __attribute__((alias("gj_vk_queue_graphics_family_5417")));

uint32_t __gj_vk_queue_present_family_5417(void)
    __attribute__((alias("gj_vk_queue_present_family_5417")));

uint32_t __gj_vk_queue_family_flags_ok_5417(uint32_t uFlags)
    __attribute__((alias("gj_vk_queue_family_flags_ok_5417")));
