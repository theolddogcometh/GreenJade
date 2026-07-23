/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3452: uint64_t absolute difference (_u,
 * abs_diff spelling).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_abs_diff_u(uint64_t a, uint64_t b);
 *     - |a - b| as uint64_t (always non-negative; no wrap).
 *   uint64_t __gj_u64_abs_diff_u  (alias)
 *   __libcgj_batch3452_marker = "libcgj-batch3452"
 *
 * CREATE-ONLY exclusive continuum wave (3451-3460). Unique
 * gj_u64_abs_diff_u surface only; no multi-def. Distinct from
 * gj_u64_absdiff, gj_u64_absdiff_u, and gj_u64_absdiff_u2
 * (batch3252). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3452_marker[] = "libcgj-batch3452";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3452_abs_diff(uint64_t u64A, uint64_t u64B)
{
	if (u64A >= u64B) {
		return u64A - u64B;
	}
	return u64B - u64A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_abs_diff_u — absolute difference of two uint64_t values.
 *
 * a, b: operands
 * Returns |a - b| without signed overflow or wraparound.
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_abs_diff_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3452_abs_diff(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_abs_diff_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_abs_diff_u")));
