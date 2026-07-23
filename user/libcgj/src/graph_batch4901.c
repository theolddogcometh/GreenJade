/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4901: signed 2x2 matrix determinant (widened).
 *
 * Surface (unique symbols):
 *   int64_t gj_mat2_det_s(int32_t a, int32_t b, int32_t c, int32_t d);
 *     - Determinant of [[a, b], [c, d]] as ad - bc, computed in int64_t
 *       so the product terms do not wrap int32_t.
 *   int64_t __gj_mat2_det_s  (alias)
 *   __libcgj_batch4901_marker = "libcgj-batch4901"
 *
 * Exclusive continuum CREATE-ONLY (4901-4910: mat2_det_s, mat2_trace_s,
 * u32_dot2_u, u32_dot3_u, u32_dot4_u, i32_dot2_s, mat2_identity_a_u,
 * mat2_identity_b_u, mat2_scale_u, batch_id_4910). Unique
 * gj_mat2_det_s surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4901_marker[] = "libcgj-batch4901";

/* ---- freestanding helpers ---------------------------------------------- */

static int64_t
b4901_det(int32_t i32A, int32_t i32B, int32_t i32C, int32_t i32D)
{
	int64_t i64Ad;
	int64_t i64Bc;

	i64Ad = (int64_t)i32A * (int64_t)i32D;
	i64Bc = (int64_t)i32B * (int64_t)i32C;
	return i64Ad - i64Bc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat2_det_s - signed 2x2 determinant ad - bc as int64_t.
 *
 * a, b, c, d: matrix entries of [[a, b], [c, d]]
 *
 * Returns ad - bc with each product widened to int64_t before the
 * subtract. Self-contained; no parent wires.
 */
int64_t
gj_mat2_det_s(int32_t i32A, int32_t i32B, int32_t i32C, int32_t i32D)
{
	(void)NULL;
	return b4901_det(i32A, i32B, i32C, i32D);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_mat2_det_s(int32_t i32A, int32_t i32B, int32_t i32C, int32_t i32D)
    __attribute__((alias("gj_mat2_det_s")));
