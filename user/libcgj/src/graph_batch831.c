/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch831: int32_t clamp into closed interval.
 *
 * Surface (unique symbols):
 *   int32_t gj_clamp_i32(int32_t x, int32_t lo, int32_t hi);
 *     — Clamp x into [lo, hi]: if x < lo → lo; if x > hi → hi; else x.
 *       Caller should pass lo <= hi; if lo > hi the composition still
 *       follows the branch form (not remapped).
 *   int32_t __gj_clamp_i32  (alias)
 *   __libcgj_batch831_marker = "libcgj-batch831"
 *
 * Distinct from gj_clamp_i64 (batch376) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch831_marker[] = "libcgj-batch831";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clamp_i32 — confine x to the closed interval [lo, hi].
 */
int32_t
gj_clamp_i32(int32_t i32X, int32_t i32Lo, int32_t i32Hi)
{
	if (i32X < i32Lo) {
		return i32Lo;
	}
	if (i32X > i32Hi) {
		return i32Hi;
	}
	return i32X;
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_clamp_i32(int32_t i32X, int32_t i32Lo, int32_t i32Hi)
    __attribute__((alias("gj_clamp_i32")));
