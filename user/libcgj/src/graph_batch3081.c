/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3081: Chebyshev (L-infinity) distance on
 * length-n uint64_t vectors (exclusive u-suffix surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_chebyshev_u64_u(const uint64_t *a, const uint64_t *b,
 *                               size_t n);
 *     - Return max_i |a[i] - b[i]| over i in [0, n). Unsigned absolute
 *       difference (a>b ? a-b : b-a). NULL a or NULL b or n == 0 yields 0.
 *   uint64_t __gj_chebyshev_u64_u  (alias)
 *   __libcgj_batch3081_marker = "libcgj-batch3081"
 *
 * Exclusive unsigned vector reduce/scale wave (3081-3090). Unique
 * gj_chebyshev_u64_u surface only; no multi-def. Distinct from
 * gj_l2sq_i32 (batch540 Euclidean) and Hamming surfaces. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3081_marker[] = "libcgj-batch3081";

/* ---- freestanding helpers ---------------------------------------------- */

/* Unsigned absolute difference of two uint64_t values. */
static uint64_t
b3081_absdiff_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64A >= u64B) {
		return u64A - u64B;
	}
	return u64B - u64A;
}

/*
 * L-infinity distance: max abs element difference. Caller guarantees
 * both pointers non-NULL when cN > 0.
 */
static uint64_t
b3081_chebyshev(const uint64_t *pA, const uint64_t *pB, size_t cN)
{
	size_t i;
	uint64_t u64Max;

	u64Max = 0ull;
	for (i = 0u; i < cN; i++) {
		uint64_t u64D;

		u64D = b3081_absdiff_u64(pA[i], pB[i]);
		if (u64D > u64Max) {
			u64Max = u64D;
		}
	}
	return u64Max;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chebyshev_u64_u - Chebyshev (L-infinity) distance of two u64 vectors.
 *
 * a, b: length-n vectors (NULL either -> 0)
 * n:    element count (0 -> 0)
 *
 * Returns max |a[i]-b[i]|. Does not call libc. No parent wires.
 */
uint64_t
gj_chebyshev_u64_u(const uint64_t *a, const uint64_t *b, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL || b == NULL) {
		return 0ull;
	}
	return b3081_chebyshev(a, b, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_chebyshev_u64_u(const uint64_t *a, const uint64_t *b, size_t n)
    __attribute__((alias("gj_chebyshev_u64_u")));
