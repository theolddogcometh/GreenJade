/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch305: weighted random index from uint32 weights.
 *
 * Surface (unique symbols):
 *   int gj_weighted_choice(const uint32_t *w, size_t n, uint32_t r,
 *                          size_t *out_idx);
 *     — Map a uniform draw r into a discrete weighted choice.
 *       w[0..n) are non-negative item weights; sum = Σ w[i].
 *       r is a uniform integer in [0, sum). Finds the least index i such
 *       that Σ w[0..i] > r (equivalently: walk remaining r, pick when
 *       r < w[i], else r -= w[i]). Zero-weight items are never selected.
 *       On success *out_idx = i and return is 0.
 *       Returns -1 on error (NULL w/out_idx, n == 0, all-zero weights,
 *       or r >= sum so no bucket claims r). *out_idx is undefined on error.
 *   __gj_weighted_choice  (alias)
 *   __libcgj_batch305_marker = "libcgj-batch305"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch305_marker[] = "libcgj-batch305";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_weighted_choice — select index by cumulative uint32 weights.
 *
 * w:       weight array (required when n > 0; NULL is always rejected)
 * n:       number of weights
 * r:       random draw in [0, sum); values outside that range fail
 * out_idx: on success, set to the chosen index in [0, n)
 *
 * Walks left-to-right without materializing sum (avoids uint32 wrap of the
 * total while still matching the classic cumulative-bucket semantics for
 * every r that falls in [0, sum) when sum fits in uint32_t, and a natural
 * modular prefix when individual weights force intermediate wrap).
 *
 * Returns 0 on success, -1 on bad input / no claimable bucket.
 */
int
gj_weighted_choice(const uint32_t *w, size_t n, uint32_t r, size_t *out_idx)
{
	size_t i;
	uint32_t uRem;

	if (w == NULL || out_idx == NULL || n == 0u) {
		return -1;
	}

	uRem = r;
	for (i = 0u; i < n; i++) {
		uint32_t uW = w[i];

		if (uRem < uW) {
			*out_idx = i;
			return 0;
		}
		uRem = (uint32_t)(uRem - uW);
	}

	/* r >= sum, or every weight is zero. */
	return -1;
}

int __gj_weighted_choice(const uint32_t *w, size_t n, uint32_t r,
    size_t *out_idx)
    __attribute__((alias("gj_weighted_choice")));
