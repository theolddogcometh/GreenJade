/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1045: left-rotate a uint32_t array in place.
 *
 * Surface (unique symbols):
 *   void gj_u32_rotate_left_arr(uint32_t *a, size_t n, size_t k);
 *     — Circular left rotate of a[0..n) by k positions (k reduced mod n).
 *       Element formerly at index i moves to index (i + n - (k%n)) % n.
 *       No-op if a == NULL, n < 2, or k % n == 0.
 *   void __gj_u32_rotate_left_arr  (alias)
 *   __libcgj_batch1045_marker = "libcgj-batch1045"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Uses reverse-triple algorithm.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1045_marker[] = "libcgj-batch1045";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b1045_rev(uint32_t *a, size_t iLo, size_t iHi)
{
	while (iLo < iHi) {
		uint32_t uT;

		uT = a[iLo];
		a[iLo] = a[iHi];
		a[iHi] = uT;
		iLo++;
		iHi--;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_rotate_left_arr — left-rotate a[0..n) by k slots.
 *
 * a: array of n uint32_t values (may be NULL → no-op)
 * n: element count
 * k: rotate distance (reduced modulo n when n > 0)
 */
void
gj_u32_rotate_left_arr(uint32_t *a, size_t n, size_t k)
{
	size_t kMod;

	if (a == NULL || n < 2u) {
		return;
	}

	kMod = k % n;
	if (kMod == 0u) {
		return;
	}

	/* reverse(0, k-1); reverse(k, n-1); reverse(0, n-1) */
	b1045_rev(a, 0u, kMod - 1u);
	b1045_rev(a, kMod, n - 1u);
	b1045_rev(a, 0u, n - 1u);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_rotate_left_arr(uint32_t *a, size_t n, size_t k)
    __attribute__((alias("gj_u32_rotate_left_arr")));
