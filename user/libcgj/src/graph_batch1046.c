/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1046: right-rotate a uint32_t array in place.
 *
 * Surface (unique symbols):
 *   void gj_u32_rotate_right_arr(uint32_t *a, size_t n, size_t k);
 *     — Circular right rotate of a[0..n) by k positions (k reduced mod n).
 *       No-op if a == NULL, n < 2, or k % n == 0.
 *   void __gj_u32_rotate_right_arr  (alias)
 *   __libcgj_batch1046_marker = "libcgj-batch1046"
 *
 * Distinct from gj_u32_rotate_left_arr (batch1045). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied. Uses reverse-triple algorithm.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1046_marker[] = "libcgj-batch1046";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b1046_rev(uint32_t *a, size_t iLo, size_t iHi)
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
 * gj_u32_rotate_right_arr — right-rotate a[0..n) by k slots.
 *
 * Equivalent to left-rotate by (n - (k % n)) when k % n != 0.
 */
void
gj_u32_rotate_right_arr(uint32_t *a, size_t n, size_t k)
{
	size_t kMod;
	size_t kLeft;

	if (a == NULL || n < 2u) {
		return;
	}

	kMod = k % n;
	if (kMod == 0u) {
		return;
	}

	/* right by k == left by n-k */
	kLeft = n - kMod;
	b1046_rev(a, 0u, kLeft - 1u);
	b1046_rev(a, kLeft, n - 1u);
	b1046_rev(a, 0u, n - 1u);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_rotate_right_arr(uint32_t *a, size_t n, size_t k)
    __attribute__((alias("gj_u32_rotate_right_arr")));
