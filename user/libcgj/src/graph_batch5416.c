/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5416: Vulkan device create / select stubs.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_device_create_stub_5416(void);
 *     - Returns 1 (logical device create soft success stub).
 *   uint32_t gj_vk_device_select_score_5416(uint32_t queue_flags);
 *     - Returns a soft selection score from queue capability bits.
 *   uint32_t gj_vk_device_ext_count_hint_5416(void);
 *     - Returns soft device extension count hint for Deck swapchain.
 *   uint32_t __gj_vk_device_create_stub_5416  (alias)
 *   uint32_t __gj_vk_device_select_score_5416  (alias)
 *   uint32_t __gj_vk_device_ext_count_hint_5416  (alias)
 *   __libcgj_batch5416_marker = "libcgj-batch5416"
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

const char __libcgj_batch5416_marker[] = "libcgj-batch5416";

/* VK_QUEUE_GRAPHICS_BIT | COMPUTE | TRANSFER soft bits. */
#define B5416_Q_GRAPHICS  0x00000001u
#define B5416_Q_COMPUTE   0x00000002u
#define B5416_Q_TRANSFER  0x00000004u
#define B5416_EXT_HINT    3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5416_score(uint32_t uFlags)
{
	uint32_t uScore;

	uScore = 0u;
	if ((uFlags & B5416_Q_GRAPHICS) != 0u)
		uScore += 100u;
	if ((uFlags & B5416_Q_COMPUTE) != 0u)
		uScore += 10u;
	if ((uFlags & B5416_Q_TRANSFER) != 0u)
		uScore += 1u;
	return uScore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_device_create_stub_5416 - soft vkCreateDevice success stub.
 *
 * Always returns 1. Product-path integer tag only.
 */
uint32_t
gj_vk_device_create_stub_5416(void)
{
	(void)NULL;
	return 1u;
}

/*
 * gj_vk_device_select_score_5416 - soft device selection score.
 *
 * queue_flags: VkQueueFlags style bitfield. Graphics heavily weighted.
 * Returns 0..111 integer score (no float).
 */
uint32_t
gj_vk_device_select_score_5416(uint32_t uQueueFlags)
{
	return b5416_score(uQueueFlags);
}

/*
 * gj_vk_device_ext_count_hint_5416 - soft device extension count.
 *
 * Always returns 3 (swapchain + common Deck WSI set product hint).
 */
uint32_t
gj_vk_device_ext_count_hint_5416(void)
{
	return B5416_EXT_HINT;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_device_create_stub_5416(void)
    __attribute__((alias("gj_vk_device_create_stub_5416")));

uint32_t __gj_vk_device_select_score_5416(uint32_t uQueueFlags)
    __attribute__((alias("gj_vk_device_select_score_5416")));

uint32_t __gj_vk_device_ext_count_hint_5416(void)
    __attribute__((alias("gj_vk_device_ext_count_hint_5416")));
