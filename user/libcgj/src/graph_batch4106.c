/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4106: uint64_t absolute difference (wave rename).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_absdiff_u3(uint64_t a, uint64_t b);
 *     - |a - b| as uint64_t (always non-negative; no wrap).
 *       Renamed from gj_u64_absdiff_u (collision with batch2609;
 *       u2 taken by batch3252).
 *   uint64_t __gj_u64_absdiff_u3  (alias)
 *   __libcgj_batch4106_marker = "libcgj-batch4106"
 *
 * Exclusive continuum CREATE-ONLY (4101-4110). Unique
 * gj_u64_absdiff_u3 surface only; no multi-def. Distinct from
 * gj_u64_absdiff (batch1379), gj_u64_absdiff_u (batch2609),
 * gj_u64_absdiff_u2 (batch3252), and gj_u64_abs_diff_u (batch3452).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4106_marker[] = "libcgj-batch4106";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4106_absdiff(uint64_t u64A, uint64_t u64B)
{
	if (u64A >= u64B) {
		return u64A - u64B;
	}
	return u64B - u64A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_absdiff_u3 - absolute difference of two uint64_t values.
 *
 * a, b: operands
 *
 * Returns |a - b| without signed overflow or wraparound.
 * Wave-unique rename: gj_u64_absdiff_u owned by batch2609;
 * gj_u64_absdiff_u2 owned by batch3252.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_absdiff_u3(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4106_absdiff(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_absdiff_u3(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_absdiff_u3")));
