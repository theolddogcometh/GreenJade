/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8914: Vulkan QUEUE_GRAPHICS soft id (no ICD).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_queue_graphics_id_8914(void);
 *     - Returns VK_QUEUE_GRAPHICS_BIT (0x00000001). Soft queue-flag id
 *       stub for freestanding graphics path tagging.
 *   uint32_t __gj_vk_queue_graphics_id_8914  (alias)
 *   __libcgj_batch8914_marker = "libcgj-batch8914"
 *
 * Exclusive continuum CREATE-ONLY (8911-8920: vulkan soft id stubs, no
 * ICD). Unique gj_vk_queue_graphics_id_8914 surface only; no multi-def.
 * Distinct from gj_vk_queue_graphics_bit_p (batch1964). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Does not load a Vulkan ICD.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8914_marker[] = "libcgj-batch8914";

/* Vulkan VK_QUEUE_GRAPHICS_BIT. */
#define B8914_QUEUE_GRAPHICS  ((uint32_t)0x00000001u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8914_queue_graphics(void)
{
	return B8914_QUEUE_GRAPHICS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_queue_graphics_id_8914 - report VK_QUEUE_GRAPHICS_BIT soft id.
 *
 * Always returns 0x00000001. Soft compile-time queue-flag id stub; does
 * not call the Vulkan loader, ICD, or libc. No parent wires.
 */
uint32_t
gj_vk_queue_graphics_id_8914(void)
{
	(void)NULL;
	return b8914_queue_graphics();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vk_queue_graphics_id_8914(void)
    __attribute__((alias("gj_vk_queue_graphics_id_8914")));
