/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch947: small-range uint32 histogram into k bins.
 *
 * Surface (unique symbols):
 *   void gj_u32_histogram_small(const uint32_t *a, size_t n, uint32_t *bins,
 *                               size_t k);
 *     — Zero bins[0..k), then for each a[i] with a[i] < k, bins[a[i]]++.
 *       Values >= k are ignored. NULL bins or k==0 → no-op.
 *   void __gj_u32_histogram_small  (alias)
 *   __libcgj_batch947_marker = "libcgj-batch947"

 *
 * Does NOT redefine gj_hist_u8 (batch306) — variable-k u32 surface.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch947_marker[] = "libcgj-batch947";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_histogram_small — count values in 0..k-1 into bins of size k.
 *
 * a:    input array (NULL with n>0 treated as empty)
 * n:    number of elements
 * bins: destination; must hold k uint32_t entries
 * k:    number of bins / exclusive upper bound for counted values
 */
void
gj_u32_histogram_small(const uint32_t *pA, size_t cN, uint32_t *pBins,
                       size_t cK)
{
	size_t i;

	if (pBins == NULL || cK == 0u) {
		return;
	}

	for (i = 0u; i < cK; i++) {
		pBins[i] = 0u;
	}

	if (pA == NULL || cN == 0u) {
		return;
	}

	for (i = 0u; i < cN; i++) {
		uint32_t u32V = pA[i];

		if ((size_t)u32V < cK) {
			pBins[u32V]++;
		}
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_histogram_small(const uint32_t *pA, size_t cN, uint32_t *pBins,
                              size_t cK)
    __attribute__((alias("gj_u32_histogram_small")));
