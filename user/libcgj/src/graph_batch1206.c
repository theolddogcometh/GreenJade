/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1206: inclusive uint32 prefix sums into out.
 *
 * Surface (unique symbols):
 *   void gj_u32_cumsum(const uint32_t *in, uint32_t *out, size_t n);
 *     — Inclusive prefix: out[i] = in[0] + ... + in[i] (mod 2^32).
 *       NULL in or out with n > 0 → no-op. n == 0 → no-op.
 *       in and out may alias (in-place left-to-right is safe).
 *   void __gj_u32_cumsum  (alias)
 *   __libcgj_batch1206_marker = "libcgj-batch1206"
 *
 * Unique public name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1206_marker[] = "libcgj-batch1206";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_cumsum — inclusive modular prefix sums into out.
 */
void
gj_u32_cumsum(const uint32_t *pIn, uint32_t *pOut, size_t cN)
{
	size_t i;
	uint32_t u32Sum;

	if (pIn == NULL || pOut == NULL || cN == 0u) {
		return;
	}

	u32Sum = 0u;
	for (i = 0u; i < cN; i++) {
		u32Sum += pIn[i];
		pOut[i] = u32Sum;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_cumsum(const uint32_t *pIn, uint32_t *pOut, size_t cN)
    __attribute__((alias("gj_u32_cumsum")));
