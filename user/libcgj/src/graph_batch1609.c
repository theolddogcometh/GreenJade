/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1609: squared Euclidean distance (int32 points).
 *
 * Surface (unique symbols):
 *   int64_t gj_dist2_i32(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
 *     — Returns (x1-x0)^2 + (y1-y0)^2 as int64_t. Differences and
 *       products widened to int64_t so full int32 domain is safe.
 *   int64_t __gj_dist2_i32  (alias)
 *   __libcgj_batch1609_marker = "libcgj-batch1609"
 *
 * Geometry integer exclusive (beyond mat2). Unique surface only; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float, no sqrt. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1609_marker[] = "libcgj-batch1609";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dist2_i32 — squared distance between two int32 points.
 *
 * Prefer this over Euclidean distance when only comparisons matter.
 */
int64_t
gj_dist2_i32(int32_t i32X0, int32_t i32Y0, int32_t i32X1, int32_t i32Y1)
{
	int64_t i64Dx;
	int64_t i64Dy;

	i64Dx = (int64_t)i32X1 - (int64_t)i32X0;
	i64Dy = (int64_t)i32Y1 - (int64_t)i32Y0;
	return i64Dx * i64Dx + i64Dy * i64Dy;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_dist2_i32(int32_t i32X0, int32_t i32Y0, int32_t i32X1,
    int32_t i32Y1)
    __attribute__((alias("gj_dist2_i32")));
