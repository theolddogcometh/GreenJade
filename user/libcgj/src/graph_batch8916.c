/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8916: Vulkan QUEUE_TRANSFER soft id (no ICD).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_queue_transfer_id_8916(void);
 *     - Returns VK_QUEUE_TRANSFER_BIT (0x00000004). Soft queue-flag id
 *       stub for freestanding transfer path tagging.
 *   uint32_t __gj_vk_queue_transfer_id_8916  (alias)
 *   __libcgj_batch8916_marker = "libcgj-batch8916"
 *
 * Exclusive continuum CREATE-ONLY (8911-8920: vulkan soft id stubs, no
 * ICD). Unique gj_vk_queue_transfer_id_8916 surface only; no multi-def.
 * Distinct from gj_vk_queue_transfer_bit_p (batch1966). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Does not load a Vulkan ICD.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8916_marker[] = "libcgj-batch8916";

/* Vulkan VK_QUEUE_TRANSFER_BIT. */
#define B8916_QUEUE_TRANSFER  ((uint32_t)0x00000004u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8916_queue_transfer(void)
{
	return B8916_QUEUE_TRANSFER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_queue_transfer_id_8916 - report VK_QUEUE_TRANSFER_BIT soft id.
 *
 * Always returns 0x00000004. Soft compile-time queue-flag id stub; does
 * not call the Vulkan loader, ICD, or libc. No parent wires.
 */
uint32_t
gj_vk_queue_transfer_id_8916(void)
{
	(void)NULL;
	return b8916_queue_transfer();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vk_queue_transfer_id_8916(void)
    __attribute__((alias("gj_vk_queue_transfer_id_8916")));
