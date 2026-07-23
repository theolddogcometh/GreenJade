/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5418: Vulkan queue families transfer/compute.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_queue_transfer_family_5418(void);
 *     - Returns soft transfer queue family index product hint (2).
 *   uint32_t gj_vk_queue_compute_family_5418(void);
 *     - Returns soft compute queue family index product hint (1).
 *   uint32_t gj_vk_queue_family_index_ok_5418(uint32_t index);
 *     - Returns 1 if family index is within Deck soft range [0, 2].
 *   uint32_t gj_vk_queue_count_at_family_5418(uint32_t index);
 *     - Returns soft queueCount for a family index (0 if out of range).
 *   uint32_t __gj_vk_queue_transfer_family_5418  (alias)
 *   uint32_t __gj_vk_queue_compute_family_5418  (alias)
 *   uint32_t __gj_vk_queue_family_index_ok_5418  (alias)
 *   uint32_t __gj_vk_queue_count_at_family_5418  (alias)
 *   __libcgj_batch5418_marker = "libcgj-batch5418"
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

const char __libcgj_batch5418_marker[] = "libcgj-batch5418";

#define B5418_XFER_FAMILY  2u
#define B5418_CMP_FAMILY   1u
#define B5418_MAX_INDEX    2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5418_index_ok(uint32_t uIndex)
{
	return (uIndex <= B5418_MAX_INDEX) ? 1u : 0u;
}

static uint32_t
b5418_qcount(uint32_t uIndex)
{
	if (uIndex == 0u)
		return 1u; /* graphics+present */
	if (uIndex == 1u)
		return 2u; /* compute */
	if (uIndex == 2u)
		return 2u; /* transfer */
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_queue_transfer_family_5418 - soft transfer family index.
 *
 * Always returns 2.
 */
uint32_t
gj_vk_queue_transfer_family_5418(void)
{
	(void)NULL;
	return B5418_XFER_FAMILY;
}

/*
 * gj_vk_queue_compute_family_5418 - soft compute family index.
 *
 * Always returns 1.
 */
uint32_t
gj_vk_queue_compute_family_5418(void)
{
	return B5418_CMP_FAMILY;
}

/*
 * gj_vk_queue_family_index_ok_5418 - 1 if index is in Deck soft range.
 *
 * index: queue family index candidate.
 */
uint32_t
gj_vk_queue_family_index_ok_5418(uint32_t uIndex)
{
	return b5418_index_ok(uIndex);
}

/*
 * gj_vk_queue_count_at_family_5418 - soft queueCount for family index.
 *
 * index: queue family index. Returns 0 when out of soft range.
 */
uint32_t
gj_vk_queue_count_at_family_5418(uint32_t uIndex)
{
	return b5418_qcount(uIndex);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_queue_transfer_family_5418(void)
    __attribute__((alias("gj_vk_queue_transfer_family_5418")));

uint32_t __gj_vk_queue_compute_family_5418(void)
    __attribute__((alias("gj_vk_queue_compute_family_5418")));

uint32_t __gj_vk_queue_family_index_ok_5418(uint32_t uIndex)
    __attribute__((alias("gj_vk_queue_family_index_ok_5418")));

uint32_t __gj_vk_queue_count_at_family_5418(uint32_t uIndex)
    __attribute__((alias("gj_vk_queue_count_at_family_5418")));
