/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch374: next power of two for uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_next_pow2_u64(uint64_t n);
 *     — Smallest power of two greater than or equal to n.
 *       gj_next_pow2_u64(0) == 1.
 *       If n is already a power of two, return n unchanged.
 *       If the result would exceed 2^63 (i.e. n > 2^63), return 0
 *       (overflow: 2^64 is not representable in uint64_t).
 *   uint64_t __gj_next_pow2_u64  (alias)
 *   __libcgj_batch374_marker = "libcgj-batch374"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch374_marker[] = "libcgj-batch374";

/* Highest representable power of two in uint64_t. */
#define B374_POW2_MAX 0x8000000000000000ULL

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_next_pow2_u64 — ceil to power of two; 0 on overflow past 2^63.
 *
 * Uses the classic smear-bits roundup after n-1 so that values that
 * are already powers of two round back to themselves.
 */
uint64_t
gj_next_pow2_u64(uint64_t u64N)
{
	uint64_t u64V;

	if (u64N == 0u) {
		return 1u;
	}

	/* Next power of two would be 2^64, not representable. */
	if (u64N > B374_POW2_MAX) {
		return 0u;
	}

	u64V = u64N - 1u;
	u64V |= u64V >> 1;
	u64V |= u64V >> 2;
	u64V |= u64V >> 4;
	u64V |= u64V >> 8;
	u64V |= u64V >> 16;
	u64V |= u64V >> 32;
	u64V += 1u;
	return u64V;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_next_pow2_u64(uint64_t u64N)
    __attribute__((alias("gj_next_pow2_u64")));
