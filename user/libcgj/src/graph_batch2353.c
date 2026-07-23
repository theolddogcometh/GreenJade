/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2353: median of three int32_t values
 * (post-2350 exclusive surface).
 *
 * Surface (unique symbols):
 *   int32_t gj_i32_median3(int32_t a, int32_t b, int32_t c);
 *     - Return the median (middle value) of a, b, and c under
 *       signed 32-bit order. Ties may return any equal middle.
 *   int32_t __gj_i32_median3  (alias)
 *   __libcgj_batch2353_marker = "libcgj-batch2353"
 *
 * Distinct from gj_median3_u64 and gj_u32_median3 - unique
 * gj_i32_median3 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2353_marker[] = "libcgj-batch2353";

/* ---- freestanding helpers ---------------------------------------------- */

/* Median of three int32 values via between-tests. */
static int32_t
b2353_median3(int32_t iA, int32_t iB, int32_t iC)
{
	if ((iB <= iA && iA <= iC) || (iC <= iA && iA <= iB)) {
		return iA;
	}
	if ((iA <= iB && iB <= iC) || (iC <= iB && iB <= iA)) {
		return iB;
	}
	return iC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_median3 - median of three int32_t values.
 *
 * a, b, c: candidates
 *
 * Returns the middle value when the three are ordered under signed
 * comparison. Does not call libc.
 */
int32_t
gj_i32_median3(int32_t iA, int32_t iB, int32_t iC)
{
	(void)NULL;
	return b2353_median3(iA, iB, iC);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_i32_median3(int32_t iA, int32_t iB, int32_t iC)
    __attribute__((alias("gj_i32_median3")));
