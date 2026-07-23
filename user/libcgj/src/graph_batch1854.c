/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1854: full mipmap chain level-count hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mip_level_count_hint(uint32_t width, uint32_t height,
 *                                    uint32_t depth);
 *     — floor(log2(max(w,h,d))) + 1 for the complete mip chain.
 *       Returns 0 when max(w,h,d) is 0. Matches common GPU full-chain
 *       level counts (Vulkan mipLevels for complete pyramid).
 *   uint32_t __gj_mip_level_count_hint  (alias)
 *   __libcgj_batch1854_marker = "libcgj-batch1854"
 *
 * GPU/buffer exclusive integer-hint wave (1851–1859); milestone 1860.
 * Distinct from gj_mip_level_size (batch1853). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1854_marker[] = "libcgj-batch1854";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(log2(u)) for u >= 1. */
static uint32_t
b1854_floor_log2(uint32_t u32V)
{
	uint32_t u32N;

	u32N = 0u;
	while (u32V > 1u) {
		u32V >>= 1u;
		u32N++;
	}
	return u32N;
}

/* Complete mip chain length: floor(log2(max(w,h,d))) + 1. */
static uint32_t
b1854_mip_count(uint32_t u32W, uint32_t u32H, uint32_t u32D)
{
	uint32_t u32Max;

	u32Max = u32W;
	if (u32H > u32Max) {
		u32Max = u32H;
	}
	if (u32D > u32Max) {
		u32Max = u32D;
	}
	if (u32Max == 0u) {
		return 0u;
	}
	return b1854_floor_log2(u32Max) + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mip_level_count_hint — complete mip chain level count.
 *
 * width/height/depth: base-level extents (use 1 for unused axes)
 * Returns floor(log2(max(...))) + 1, or 0 when all extents are 0.
 */
uint32_t
gj_mip_level_count_hint(uint32_t u32Width, uint32_t u32Height, uint32_t u32Depth)
{
	(void)sizeof(NULL);
	return b1854_mip_count(u32Width, u32Height, u32Depth);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mip_level_count_hint(uint32_t u32Width, uint32_t u32Height,
    uint32_t u32Depth)
    __attribute__((alias("gj_mip_level_count_hint")));
