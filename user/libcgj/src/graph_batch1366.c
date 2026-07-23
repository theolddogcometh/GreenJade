/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1366: stream q-quantile rank positions.
 *
 * Surface (unique symbols):
 *   uint32_t gj_stream_quantile_ranks(uint64_t *out, uint32_t q,
 *                                    uint64_t n);
 *     — Fill out[0..q] with inclusive rank positions for q-quantiles
 *       over a stream of n ordered samples:
 *         out[i] = i * (n - 1) / q   for i = 0..q  (when n >= 1, q >= 1)
 *       Writes q+1 entries. Returns the number of ranks written, or 0
 *       on bad args (out NULL, q == 0, or n == 0).
 *   uint32_t __gj_stream_quantile_ranks  (alias)
 *   __libcgj_batch1366_marker = "libcgj-batch1366"
 *
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1366_marker[] = "libcgj-batch1366";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stream_quantile_ranks — rank indices for q-quantiles of n samples.
 *
 * pOut: caller array of at least (q + 1) uint64_t slots.
 * u32Q: number of quantile intervals (e.g. 4 → quartiles, 5 ranks).
 * u64N: sample count.
 *
 * Ranks are 0-based inclusive endpoints spanning [0, n-1].
 */
uint32_t
gj_stream_quantile_ranks(uint64_t *pOut, uint32_t u32Q, uint64_t u64N)
{
	uint32_t u32I;
	uint64_t u64Span;

	/* Reject NULL, empty stream, or q that would make q+1 wrap. */
	if (pOut == NULL || u32Q == 0u || u32Q == UINT32_MAX ||
	    u64N == 0ull) {
		return 0u;
	}

	u64Span = u64N - 1ull;
	for (u32I = 0u; u32I <= u32Q; u32I++) {
		/* out[i] = i * (n-1) / q  — exact for endpoints */
		if (u32I == 0u) {
			pOut[u32I] = 0ull;
		} else if (u32I == u32Q) {
			pOut[u32I] = u64Span;
		} else if (u64Span != 0ull &&
		    (uint64_t)u32I > UINT64_MAX / u64Span) {
			/* Degenerate huge product: clamp to span. */
			pOut[u32I] = u64Span;
		} else {
			pOut[u32I] = ((uint64_t)u32I * u64Span) /
			    (uint64_t)u32Q;
		}
	}
	return u32Q + 1u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_stream_quantile_ranks(uint64_t *pOut, uint32_t u32Q,
    uint64_t u64N)
    __attribute__((alias("gj_stream_quantile_ranks")));
