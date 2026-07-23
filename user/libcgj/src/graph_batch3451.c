/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3451: uint32_t absolute difference (_u,
 * abs_diff spelling).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_abs_diff_u(uint32_t a, uint32_t b);
 *     - |a - b| as uint32_t (always non-negative; no wrap).
 *   uint32_t __gj_u32_abs_diff_u  (alias)
 *   __libcgj_batch3451_marker = "libcgj-batch3451"
 *
 * CREATE-ONLY exclusive continuum wave (3451-3460). Unique
 * gj_u32_abs_diff_u surface only; no multi-def. Distinct from
 * gj_u32_absdiff (batch1378) and gj_u32_absdiff_u (batch3251).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3451_marker[] = "libcgj-batch3451";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3451_abs_diff(uint32_t u32A, uint32_t u32B)
{
	if (u32A >= u32B) {
		return u32A - u32B;
	}
	return u32B - u32A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_abs_diff_u — absolute difference of two uint32_t values.
 *
 * a, b: operands
 * Returns |a - b| without signed overflow or wraparound.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_abs_diff_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3451_abs_diff(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_abs_diff_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_abs_diff_u")));
