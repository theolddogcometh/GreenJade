/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3153: power-of-two alignment predicate (u32, u).
 *
 * Surface (unique symbols):
 *   int gj_u32_is_aligned_u(uint32_t addr, uint32_t align);
 *     - Return 1 if addr is a multiple of align when align is a power
 *       of two (addr & (align - 1) == 0). Return 0 if align == 0 or
 *       align is not a power of two, or if addr is not aligned.
 *   int __gj_u32_is_aligned_u  (alias)
 *   __libcgj_batch3153_marker = "libcgj-batch3153"
 *
 * Milestone 3160 exclusive continuum CREATE-ONLY (3151-3160). Distinct
 * from gj_u64_is_aligned (batch1069) — unique gj_u32_is_aligned_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3153_marker[] = "libcgj-batch3153";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * True when addr is a multiple of align and align is a non-zero
 * power of two; otherwise 0.
 */
static int
b3153_is_aligned(uint32_t u32Addr, uint32_t u32Align)
{
	if (u32Align == 0u) {
		return 0;
	}
	if ((u32Align & (u32Align - 1u)) != 0u) {
		return 0;
	}
	return ((u32Addr & (u32Align - 1u)) == 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_is_aligned_u - true when u32Addr is aligned to u32Align.
 *
 * u32Align must be a non-zero power of two; otherwise returns 0.
 * No parent wires.
 */
int
gj_u32_is_aligned_u(uint32_t u32Addr, uint32_t u32Align)
{
	(void)NULL;
	return b3153_is_aligned(u32Addr, u32Align);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_is_aligned_u(uint32_t u32Addr, uint32_t u32Align)
    __attribute__((alias("gj_u32_is_aligned_u")));
