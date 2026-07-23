/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4902: signed 2x2 matrix trace (widened).
 *
 * Surface (unique symbols):
 *   int64_t gj_mat2_trace_s(int32_t a, int32_t d);
 *     - Trace of [[a, b], [c, d]] is a + d (off-diagonal unused). Sum
 *       is formed in int64_t so the diagonal add does not wrap int32_t.
 *   int64_t __gj_mat2_trace_s  (alias)
 *   __libcgj_batch4902_marker = "libcgj-batch4902"
 *
 * Exclusive continuum CREATE-ONLY (4901-4910). Distinct from
 * gj_mat2_det_s (batch4901) — unique gj_mat2_trace_s surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4902_marker[] = "libcgj-batch4902";

/* ---- freestanding helpers ---------------------------------------------- */

static int64_t
b4902_trace(int32_t i32A, int32_t i32D)
{
	return (int64_t)i32A + (int64_t)i32D;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat2_trace_s - signed 2x2 diagonal sum a + d as int64_t.
 *
 * a, d: main-diagonal entries of a 2x2 matrix
 *
 * Returns a + d widened to int64_t. Self-contained; no parent wires.
 */
int64_t
gj_mat2_trace_s(int32_t i32A, int32_t i32D)
{
	(void)NULL;
	return b4902_trace(i32A, i32D);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_mat2_trace_s(int32_t i32A, int32_t i32D)
    __attribute__((alias("gj_mat2_trace_s")));
