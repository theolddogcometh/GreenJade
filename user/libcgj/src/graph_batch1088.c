/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1088: sliding-window floor average over a uint32 array
 *
 * Surface (unique symbols):
 *   void gj_u32_moving_avg(const uint32_t *a, size_t n, size_t window,
 *                          uint32_t *out);
 *     — For each i in [0, n - window + 1), write floor(sum / window)
 *       of a[i .. i+window) into out[i]. Sum accumulates in uint64 to
 *       avoid intermediate wrap of the mean. NULL a/out, n == 0,
 *       window == 0, or window > n → no-op.
 *   void __gj_u32_moving_avg  (alias)

 *   __libcgj_batch1088_marker = "libcgj-batch1088"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1088_marker[] = "libcgj-batch1088";

/*
 * gj_u32_moving_avg — sliding window floor means into pOut.
 */
void
gj_u32_moving_avg(const uint32_t *pA, size_t cN, size_t cWin, uint32_t *pOut)
{
	size_t i;
	size_t iOut;
	size_t cOut;
	uint64_t u64Sum;

	if (pA == NULL || pOut == NULL || cN == 0u || cWin == 0u ||
	    cWin > cN) {
		return;
	}

	cOut = cN - cWin + 1u;
	u64Sum = 0ull;
	for (i = 0u; i < cWin; i++) {
		u64Sum += (uint64_t)pA[i];
	}
	pOut[0] = (uint32_t)(u64Sum / (uint64_t)cWin);

	for (iOut = 1u; iOut < cOut; iOut++) {
		u64Sum = u64Sum - (uint64_t)pA[iOut - 1u] +
			 (uint64_t)pA[iOut + cWin - 1u];
		pOut[iOut] = (uint32_t)(u64Sum / (uint64_t)cWin);
	}
}

void __gj_u32_moving_avg(const uint32_t *pA, size_t cN, size_t cWin,
			 uint32_t *pOut)
    __attribute__((alias("gj_u32_moving_avg")));
