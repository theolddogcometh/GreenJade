/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8915: Vulkan QUEUE_COMPUTE soft id (no ICD).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_queue_compute_id_8915(void);
 *     - Returns VK_QUEUE_COMPUTE_BIT (0x00000002). Soft queue-flag id
 *       stub for freestanding compute path tagging.
 *   uint32_t __gj_vk_queue_compute_id_8915  (alias)
 *   __libcgj_batch8915_marker = "libcgj-batch8915"
 *
 * Exclusive continuum CREATE-ONLY (8911-8920: vulkan soft id stubs, no
 * ICD). Unique gj_vk_queue_compute_id_8915 surface only; no multi-def.
 * Distinct from gj_vk_queue_compute_bit_p (batch1965). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Does not load a Vulkan ICD.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8915_marker[] = "libcgj-batch8915";

/* Vulkan VK_QUEUE_COMPUTE_BIT. */
#define B8915_QUEUE_COMPUTE  ((uint32_t)0x00000002u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8915_queue_compute(void)
{
	return B8915_QUEUE_COMPUTE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_queue_compute_id_8915 - report VK_QUEUE_COMPUTE_BIT soft id.
 *
 * Always returns 0x00000002. Soft compile-time queue-flag id stub; does
 * not call the Vulkan loader, ICD, or libc. No parent wires.
 */
uint32_t
gj_vk_queue_compute_id_8915(void)
{
	(void)NULL;
	return b8915_queue_compute();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vk_queue_compute_id_8915(void)
    __attribute__((alias("gj_vk_queue_compute_id_8915")));
