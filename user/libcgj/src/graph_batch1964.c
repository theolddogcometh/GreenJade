/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1964: Vulkan queue graphics bit predicate.
 *
 * Surface (unique symbols):
 *   int gj_vk_queue_graphics_bit_p(uint32_t flags);
 *     — Return 1 if VK_QUEUE_GRAPHICS_BIT (0x1) is set in flags, else 0.
 *       Pure queue-flag bit predicate; does not inspect family indices.
 *   int __gj_vk_queue_graphics_bit_p  (alias)
 *   __libcgj_batch1964_marker = "libcgj-batch1964"
 *
 * Vulkan/GL enum exclusive pure-data wave (1961–1970). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1964_marker[] = "libcgj-batch1964";

/* VK_QUEUE_GRAPHICS_BIT */
#define B1964_QUEUE_GRAPHICS  0x00000001u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if graphics queue bit is set. */
static int
b1964_graphics_p(uint32_t uFlags)
{
	return ((uFlags & B1964_QUEUE_GRAPHICS) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_queue_graphics_bit_p — 1 if QUEUE_GRAPHICS bit is set.
 *
 * flags: VkQueueFlags as uint32_t
 * Returns 1 when bit 0 is set, else 0.
 */
int
gj_vk_queue_graphics_bit_p(uint32_t uFlags)
{
	(void)NULL;
	return b1964_graphics_p(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_vk_queue_graphics_bit_p(uint32_t uFlags)
    __attribute__((alias("gj_vk_queue_graphics_bit_p")));
