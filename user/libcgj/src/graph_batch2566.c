/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2566: delta decode for uint32_t arrays.
 * Counterpart to batch2565 gj_delta_encode_u32.
 *
 * Surface (unique symbols):
 *   void gj_delta_decode_u32(const uint32_t *in, uint32_t *out, size_t n);
 *     — Inverse: out[0] = in[0]; for i>0, out[i] = out[i-1] + in[i]
 *       (modular uint32 wrap). Supports in-place (in == out) and
 *       out-of-place. n==0 or NULL in/out with n>0 is a no-op.
 *   void __gj_delta_decode_u32  (alias)
 *   __libcgj_batch2566_marker = "libcgj-batch2566"
 *
 * Compression exclusive wave (2561–2570). Distinct from
 * gj_delta_decode_i32 (batch290) — unique u32 surface, no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2566_marker[] = "libcgj-batch2566";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Inverse of b2565_delta / gj_delta_encode_u32.
 * out[0] = in[0]; out[i] = out[i-1] + in[i] for i > 0.
 * in == out is safe: out[i-1] is reconstructed before out[i] is written.
 */
static void
b2566_undelta(const uint32_t *pIn, uint32_t *pOut, size_t cN)
{
	size_t i;
	uint32_t uAcc;

	if (cN == 0u || pIn == NULL || pOut == NULL) {
		return;
	}

	uAcc = pIn[0];
	pOut[0] = uAcc;

	for (i = 1u; i < cN; i++) {
		uAcc = uAcc + pIn[i];
		pOut[i] = uAcc;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_delta_decode_u32 — inverse of gj_delta_encode_u32.
 *
 * Decoding (prefix sum from absolute first sample):
 *   out[0] = in[0]
 *   out[i] = out[i-1] + in[i]   for i in 1..n-1
 *
 * in == out is safe. Same NULL/n rules as encode.
 */
void
gj_delta_decode_u32(const uint32_t *in, uint32_t *out, size_t n)
{
	(void)NULL;
	b2566_undelta(in, out, n);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_delta_decode_u32(const uint32_t *in, uint32_t *out, size_t n)
    __attribute__((alias("gj_delta_decode_u32")));
