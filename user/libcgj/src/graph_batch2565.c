/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2565: delta encode for uint32_t arrays.
 *
 * Surface (unique symbols):
 *   void gj_delta_encode_u32(const uint32_t *in, uint32_t *out, size_t n);
 *     — First sample absolute: out[0] = in[0]; for i>0,
 *       out[i] = in[i] - in[i-1] (modular uint32 wrap).
 *       Supports in-place (in == out) and out-of-place. n==0 or NULL
 *       in/out with n>0 is a no-op.
 *   void __gj_delta_encode_u32  (alias)
 *   __libcgj_batch2565_marker = "libcgj-batch2565"
 *
 * Compression exclusive wave (2561–2570). Distinct from
 * gj_delta_encode_i32 (batch290) — unique u32 surface, no multi-def.
 * Inverse is gj_delta_decode_u32 (batch2566).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2565_marker[] = "libcgj-batch2565";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Forward delta of cN uint32 samples.
 * out[0] = in[0]; out[i] = in[i] - in[i-1] for i > 0.
 * in == out is safe: previous absolute is captured before overwrite.
 */
static void
b2565_delta(const uint32_t *pIn, uint32_t *pOut, size_t cN)
{
	size_t i;
	uint32_t uPrev;
	uint32_t uCurr;

	if (cN == 0u || pIn == NULL || pOut == NULL) {
		return;
	}

	uPrev = pIn[0];
	pOut[0] = uPrev;

	for (i = 1u; i < cN; i++) {
		uCurr = pIn[i];
		pOut[i] = uCurr - uPrev;
		uPrev = uCurr;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_delta_encode_u32 — forward delta of n uint32 samples.
 *
 * Encoding (first sample absolute):
 *   out[0] = in[0]
 *   out[i] = in[i] - in[i-1]   for i in 1..n-1
 *
 * Subtraction is modular uint32_t (no signed UB). in == out is safe.
 */
void
gj_delta_encode_u32(const uint32_t *in, uint32_t *out, size_t n)
{
	(void)NULL;
	b2565_delta(in, out, n);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_delta_encode_u32(const uint32_t *in, uint32_t *out, size_t n)
    __attribute__((alias("gj_delta_encode_u32")));
