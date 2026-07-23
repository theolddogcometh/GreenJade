/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1207: first differences of a uint32 array.
 *
 * Surface (unique symbols):
 *   void gj_u32_diff1(const uint32_t *in, uint32_t *out, size_t n);
 *     — Inverse of inclusive cumsum: out[0] = in[0];
 *       out[i] = in[i] - in[i-1] (mod 2^32) for i > 0.
 *       NULL in or out with n > 0 → no-op. n == 0 → no-op.
 *       in and out may alias (right-to-left would clobber; use a
 *       forward pass with a running previous when overlapping).
 *   void __gj_u32_diff1  (alias)
 *   __libcgj_batch1207_marker = "libcgj-batch1207"
 *
 * Unique public name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1207_marker[] = "libcgj-batch1207";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_diff1 — first differences (inverse of inclusive cumsum).
 *
 * Forward scan keeps the previous input in a local so in==out is safe.
 */
void
gj_u32_diff1(const uint32_t *pIn, uint32_t *pOut, size_t cN)
{
	size_t i;
	uint32_t u32Prev;
	uint32_t u32Cur;

	if (pIn == NULL || pOut == NULL || cN == 0u) {
		return;
	}

	u32Prev = pIn[0];
	pOut[0] = u32Prev;
	for (i = 1u; i < cN; i++) {
		u32Cur = pIn[i];
		pOut[i] = u32Cur - u32Prev;
		u32Prev = u32Cur;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_diff1(const uint32_t *pIn, uint32_t *pOut, size_t cN)
    __attribute__((alias("gj_u32_diff1")));
