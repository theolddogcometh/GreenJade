/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1856: image row pitch with power-of-two align.
 *
 * Surface (unique symbols):
 *   uint64_t gj_row_pitch_align(uint32_t width, uint32_t bytes_per_texel,
 *                               uint32_t alignment);
 *     — Row pitch = align_up(width * bytes_per_texel, alignment).
 *       alignment must be a non-zero power of two; otherwise the raw
 *       unaligned row bytes are returned (identity-on-bad-align).
 *       Returns 0 when width or bytes_per_texel is 0, or on overflow.
 *   uint64_t __gj_row_pitch_align  (alias)
 *   __libcgj_batch1856_marker = "libcgj-batch1856"
 *
 * GPU/buffer exclusive integer-hint wave (1851–1859); milestone 1860.
 * Distinct from gj_vk_align_up (batch1851). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1856_marker[] = "libcgj-batch1856";

/* ---- freestanding helpers ---------------------------------------------- */

/* Power-of-two align-up of u64; identity on bad align; 0 on overflow. */
static uint64_t
b1856_align_up(uint64_t u64Value, uint64_t u64Align)
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

/* Aligned row pitch from width * bpp. */
static uint64_t
b1856_row_pitch(uint32_t u32Width, uint32_t u32Bpt, uint32_t u32Align)
{
	uint64_t u64Raw;

	if (u32Width == 0u || u32Bpt == 0u) {
		return 0u;
	}
	u64Raw = (uint64_t)u32Width * (uint64_t)u32Bpt;
	return b1856_align_up(u64Raw, (uint64_t)u32Align);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_row_pitch_align — aligned bytes per image row.
 *
 * width:          texels (or blocks) per row
 * bytes_per_texel: bytes per texel/block element
 * alignment:      power-of-two row pitch requirement (e.g. 256 for linear)
 * Returns aligned pitch, or 0 on zero inputs / overflow.
 */
uint64_t
gj_row_pitch_align(uint32_t u32Width, uint32_t u32BytesPerTexel,
    uint32_t u32Align)
{
	(void)sizeof(NULL);
	return b1856_row_pitch(u32Width, u32BytesPerTexel, u32Align);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_row_pitch_align(uint32_t u32Width, uint32_t u32BytesPerTexel,
    uint32_t u32Align)
    __attribute__((alias("gj_row_pitch_align")));
