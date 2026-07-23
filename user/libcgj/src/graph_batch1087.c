/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1087: sliding-window sum over a uint32 array
 *
 * Surface (unique symbols):
 *   void gj_u32_moving_sum(const uint32_t *a, size_t n, size_t window,
 *                          uint32_t *out);
 *     — For each i in [0, n - window + 1), write the sum of
 *       a[i .. i+window) into out[i]. Uses uint32 modular arithmetic.
 *       NULL a/out, n == 0, window == 0, or window > n → no-op.
 *   void __gj_u32_moving_sum  (alias)

 *   __libcgj_batch1087_marker = "libcgj-batch1087"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1087_marker[] = "libcgj-batch1087";

/*
 * gj_u32_moving_sum — sliding window sums into pOut (length n-window+1).
 */
void
gj_u32_moving_sum(const uint32_t *pA, size_t cN, size_t cWin, uint32_t *pOut)
{
	size_t i;
	size_t iOut;
	size_t cOut;
	uint32_t u32Sum;

	if (pA == NULL || pOut == NULL || cN == 0u || cWin == 0u ||
	    cWin > cN) {
		return;
	}

	cOut = cN - cWin + 1u;
	u32Sum = 0u;
	for (i = 0u; i < cWin; i++) {
		u32Sum += pA[i];
	}
	pOut[0] = u32Sum;

	for (iOut = 1u; iOut < cOut; iOut++) {
		u32Sum = u32Sum - pA[iOut - 1u] + pA[iOut + cWin - 1u];
		pOut[iOut] = u32Sum;
	}
}

void __gj_u32_moving_sum(const uint32_t *pA, size_t cN, size_t cWin,
			 uint32_t *pOut)
    __attribute__((alias("gj_u32_moving_sum")));
