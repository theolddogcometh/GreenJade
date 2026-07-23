/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4906: signed 2-element dot product (widened).
 *
 * Surface (unique symbols):
 *   int64_t gj_i32_dot2_s(int32_t a0, int32_t a1, int32_t b0, int32_t b1);
 *     - a0*b0 + a1*b1 with each product formed in int64_t.
 *   int64_t __gj_i32_dot2_s  (alias)
 *   __libcgj_batch4906_marker = "libcgj-batch4906"
 *
 * Exclusive continuum CREATE-ONLY (4901-4910). Distinct from
 * gj_u32_dot2_u (batch4903) — unique gj_i32_dot2_s surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4906_marker[] = "libcgj-batch4906";

/* ---- freestanding helpers ---------------------------------------------- */

static int64_t
b4906_dot2(int32_t i32A0, int32_t i32A1, int32_t i32B0, int32_t i32B1)
{
	int64_t i64P0;
	int64_t i64P1;

	i64P0 = (int64_t)i32A0 * (int64_t)i32B0;
	i64P1 = (int64_t)i32A1 * (int64_t)i32B1;
	return i64P0 + i64P1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_dot2_s - two-lane signed dot product a·b.
 *
 * a0, a1: left vector lanes
 * b0, b1: right vector lanes
 *
 * Returns a0*b0 + a1*b1 with widened multiplies. Self-contained;
 * no parent wires.
 */
int64_t
gj_i32_dot2_s(int32_t i32A0, int32_t i32A1, int32_t i32B0, int32_t i32B1)
{
	(void)NULL;
	return b4906_dot2(i32A0, i32A1, i32B0, i32B1);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i32_dot2_s(int32_t i32A0, int32_t i32A1,
    int32_t i32B0, int32_t i32B1)
    __attribute__((alias("gj_i32_dot2_s")));
