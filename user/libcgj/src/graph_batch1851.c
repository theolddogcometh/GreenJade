/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1851: Vulkan-style power-of-two alignment roundup.
 *
 * Surface (unique symbols):
 *   uint64_t gj_vk_align_up(uint64_t value, uint64_t alignment);
 *     — Round value up to the nearest multiple of alignment. alignment
 *       must be a non-zero power of two. When alignment is 0 or not a
 *       power of two, returns value unchanged. On overflow of the
 *       intermediate (value + alignment - 1), returns 0.
 *   uint64_t __gj_vk_align_up  (alias)
 *   __libcgj_batch1851_marker = "libcgj-batch1851"
 *
 * GPU/buffer exclusive integer-hint wave (1851–1859); milestone 1860.
 * Distinct from gj_align_up / gj_align_up_u64. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1851_marker[] = "libcgj-batch1851";

/* ---- freestanding helpers ---------------------------------------------- */

/* Power-of-two align-up; identity on bad alignment; 0 on overflow. */
static uint64_t
b1851_align_up(uint64_t u64Value, uint64_t u64Align)
{
	uint64_t u64Mask;

	if (u64Align == 0u || (u64Align & (u64Align - 1u)) != 0u) {
		return u64Value;
	}

	u64Mask = u64Align - 1u;
	if (u64Value > (UINT64_MAX - u64Mask)) {
		return 0u;
	}

	return (u64Value + u64Mask) & ~u64Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_align_up — Vulkan buffer/offset power-of-two alignment roundup.
 *
 * value:     byte offset or size to align
 * alignment: required power-of-two alignment (e.g. minStorageBufferOffsetAlignment)
 * Returns aligned value, identity on bad alignment, 0 on overflow.
 */
uint64_t
gj_vk_align_up(uint64_t u64Value, uint64_t u64Align)
{
	(void)sizeof(NULL);
	return b1851_align_up(u64Value, u64Align);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_vk_align_up(uint64_t u64Value, uint64_t u64Align)
    __attribute__((alias("gj_vk_align_up")));
