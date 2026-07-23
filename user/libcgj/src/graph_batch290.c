/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch290: delta encode/decode for int32 arrays.
 *
 * Surface (unique symbols):
 *   void gj_delta_encode_i32(const int32_t *in, int32_t *out, size_t n);
 *     — First sample absolute: out[0] = in[0]; for i>0,
 *       out[i] = in[i] - in[i-1] (two's complement wrap).
 *       Supports in-place (in == out) and out-of-place. n==0 or NULL
 *       in/out with n>0 is a no-op.
 *   void gj_delta_decode_i32(const int32_t *in, int32_t *out, size_t n);
 *     — Inverse: out[0] = in[0]; for i>0, out[i] = out[i-1] + in[i].
 *       Supports in-place (in == out) and out-of-place. Same NULL/n
 *       rules as encode.
 *   __gj_delta_encode_i32 / __gj_delta_decode_i32  (aliases)
 *   __libcgj_batch290_marker = "libcgj-batch290"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch290_marker[] = "libcgj-batch290";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_delta_encode_i32 — forward delta of n int32 samples.
 *
 * Encoding (first sample absolute):
 *   out[0] = in[0]
 *   out[i] = in[i] - in[i-1]   for i in 1..n-1
 *
 * Subtraction is modular two's-complement via uint32_t (no signed UB).
 * in == out is safe: each previous absolute is captured before overwrite.
 */
void
gj_delta_encode_i32(const int32_t *in, int32_t *out, size_t n)
{
	size_t i;
	int32_t i32Prev;
	int32_t i32Curr;

	if (n == 0u || in == NULL || out == NULL) {
		return;
	}

	i32Prev = in[0];
	out[0] = i32Prev;

	for (i = 1u; i < n; i++) {
		i32Curr = in[i];
		/* Modular wrap via uint32_t (two's complement; no signed UB). */
		out[i] = (int32_t)((uint32_t)i32Curr - (uint32_t)i32Prev);
		i32Prev = i32Curr;
	}
}

/*
 * gj_delta_decode_i32 — inverse of gj_delta_encode_i32.
 *
 * Decoding (prefix sum from absolute first sample):
 *   out[0] = in[0]
 *   out[i] = out[i-1] + in[i]   for i in 1..n-1
 *
 * in == out is safe: out[i-1] is already reconstructed before out[i]
 * is written, and in[i] (the delta) is read first when buffers alias.
 */
void
gj_delta_decode_i32(const int32_t *in, int32_t *out, size_t n)
{
	size_t i;
	int32_t i32Acc;

	if (n == 0u || in == NULL || out == NULL) {
		return;
	}

	i32Acc = in[0];
	out[0] = i32Acc;

	for (i = 1u; i < n; i++) {
		/* Modular wrap via uint32_t (two's complement; no signed UB). */
		i32Acc = (int32_t)((uint32_t)i32Acc + (uint32_t)in[i]);
		out[i] = i32Acc;
	}
}

void __gj_delta_encode_i32(const int32_t *in, int32_t *out, size_t n)
    __attribute__((alias("gj_delta_encode_i32")));
void __gj_delta_decode_i32(const int32_t *in, int32_t *out, size_t n)
    __attribute__((alias("gj_delta_decode_i32")));
