/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3154: power-of-two alignment predicate (u64, u).
 *
 * Surface (unique symbols):
 *   int gj_u64_is_aligned_u(uint64_t addr, uint64_t align);
 *     - Return 1 if addr is a multiple of align when align is a power
 *       of two (addr & (align - 1) == 0). Return 0 if align == 0 or
 *       align is not a power of two, or if addr is not aligned.
 *   int __gj_u64_is_aligned_u  (alias)
 *   __libcgj_batch3154_marker = "libcgj-batch3154"
 *
 * Milestone 3160 exclusive continuum CREATE-ONLY (3151-3160). Distinct
 * from gj_u64_is_aligned (batch1069) — unique gj_u64_is_aligned_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3154_marker[] = "libcgj-batch3154";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * True when addr is a multiple of align and align is a non-zero
 * power of two; otherwise 0.
 */
static int
b3154_is_aligned(uint64_t u64Addr, uint64_t u64Align)
{
	if (u64Align == 0ull) {
		return 0;
	}
	if ((u64Align & (u64Align - 1ull)) != 0ull) {
		return 0;
	}
	return ((u64Addr & (u64Align - 1ull)) == 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_is_aligned_u - true when u64Addr is aligned to u64Align.
 *
 * u64Align must be a non-zero power of two; otherwise returns 0.
 * No parent wires.
 */
int
gj_u64_is_aligned_u(uint64_t u64Addr, uint64_t u64Align)
{
	(void)NULL;
	return b3154_is_aligned(u64Addr, u64Align);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_is_aligned_u(uint64_t u64Addr, uint64_t u64Align)
    __attribute__((alias("gj_u64_is_aligned_u")));
