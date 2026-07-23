/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2609: uint64_t absolute difference (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_absdiff_u(uint64_t a, uint64_t b);
 *     - |a - b| as uint64_t (always non-negative; no wrap).
 *   uint64_t __gj_u64_absdiff_u  (alias)
 *   __libcgj_batch2609_marker = "libcgj-batch2609"
 *
 * Distinct from gj_u64_absdiff (batch1379) — exclusive _u name surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2609_marker[] = "libcgj-batch2609";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2609_absdiff(uint64_t u64A, uint64_t u64B)
{
	if (u64A >= u64B) {
		return u64A - u64B;
	}
	return u64B - u64A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_absdiff_u — absolute difference of two uint64_t values.
 *
 * a, b: operands
 * Returns |a - b| without signed overflow or wraparound.
 */
uint64_t
gj_u64_absdiff_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2609_absdiff(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_absdiff_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_absdiff_u")));
