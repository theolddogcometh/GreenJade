/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1069: power-of-two alignment predicate.
 *
 * Surface (unique symbols):
 *   int gj_u64_is_aligned(uint64_t addr, uint64_t align);
 *     — Return 1 if addr is a multiple of align when align is a power
 *       of two (addr & (align - 1) == 0). Return 0 if align == 0 or
 *       align is not a power of two, or if addr is not aligned.
 *   int __gj_u64_is_aligned  (alias)
 *   __libcgj_batch1069_marker = "libcgj-batch1069"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1069_marker[] = "libcgj-batch1069";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_is_aligned — true when u64Addr is aligned to u64Align.
 *
 * u64Align must be a non-zero power of two; otherwise returns 0.
 */
int
gj_u64_is_aligned(uint64_t u64Addr, uint64_t u64Align)
{
	if (u64Align == 0ull) {
		return 0;
	}
	if ((u64Align & (u64Align - 1ull)) != 0ull) {
		return 0;
	}
	return ((u64Addr & (u64Align - 1ull)) == 0ull) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_is_aligned(uint64_t u64Addr, uint64_t u64Align)
    __attribute__((alias("gj_u64_is_aligned")));
