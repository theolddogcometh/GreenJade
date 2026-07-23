/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1208: scale a uint32 array by k into out.
 *
 * Surface (unique symbols):
 *   void gj_u32_scale(const uint32_t *in, uint32_t *out, size_t n,
 *                     uint32_t k);
 *     — out[i] = in[i] * k (mod 2^32). NULL in or out with n > 0 →
 *       no-op. n == 0 → no-op. in and out may alias.
 *   void __gj_u32_scale  (alias)
 *   __libcgj_batch1208_marker = "libcgj-batch1208"
 *
 * Unique public name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1208_marker[] = "libcgj-batch1208";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_scale — multiply each element by k into out.
 */
void
gj_u32_scale(const uint32_t *pIn, uint32_t *pOut, size_t cN, uint32_t u32K)
{
	size_t i;

	if (pIn == NULL || pOut == NULL || cN == 0u) {
		return;
	}

	for (i = 0u; i < cN; i++) {
		pOut[i] = pIn[i] * u32K;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_scale(const uint32_t *pIn, uint32_t *pOut, size_t cN,
    uint32_t u32K)
    __attribute__((alias("gj_u32_scale")));
