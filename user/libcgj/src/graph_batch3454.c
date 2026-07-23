/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3454: median of three uint64_t values
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_median3_u(uint64_t a, uint64_t b, uint64_t c);
 *     - Return the median (middle value) of a, b, and c under
 *       unsigned 64-bit order. Ties may return any equal middle.
 *   uint64_t __gj_u64_median3_u  (alias)
 *   __libcgj_batch3454_marker = "libcgj-batch3454"
 *
 * CREATE-ONLY exclusive continuum wave (3451-3460). Unique
 * gj_u64_median3_u surface only; no multi-def. Distinct from
 * gj_u64_median3 (batch2352) and gj_median3_u64 (batch590).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3454_marker[] = "libcgj-batch3454";

/* ---- freestanding helpers ---------------------------------------------- */

/* Median of three uint64 values via between-tests. */
static uint64_t
b3454_median3(uint64_t uA, uint64_t uB, uint64_t uC)
{
	if ((uB <= uA && uA <= uC) || (uC <= uA && uA <= uB)) {
		return uA;
	}
	if ((uA <= uB && uB <= uC) || (uC <= uB && uB <= uA)) {
		return uB;
	}
	return uC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_median3_u - median of three uint64_t values.
 *
 * a, b, c: candidates
 *
 * Returns the middle value when the three are ordered under unsigned
 * comparison. Does not call libc. No parent wires.
 */
uint64_t
gj_u64_median3_u(uint64_t uA, uint64_t uB, uint64_t uC)
{
	(void)NULL;
	return b3454_median3(uA, uB, uC);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_median3_u(uint64_t uA, uint64_t uB, uint64_t uC)
    __attribute__((alias("gj_u64_median3_u")));
