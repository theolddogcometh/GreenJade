/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4306: fill-safe uint64_t left shift (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_shl_fill_u(uint64_t x, unsigned n);
 *     - Return x << n for n in 0..63. If n >= 64, return 0 (fill/zero
 *       the result; no shift UB). Pure integer; no builtins.
 *   uint64_t __gj_u64_shl_fill_u  (alias)
 *   __libcgj_batch4306_marker = "libcgj-batch4306"
 *
 * Exclusive continuum CREATE-ONLY (4301-4310). Distinct from
 * gj_u32_shl_fill_u (batch4305), gj_u64_shl_checked_u (batch4303), and
 * gj_u64_shl_sat (batch2405) — unique gj_u64_shl_fill_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4306_marker[] = "libcgj-batch4306";

/* ---- freestanding helpers ---------------------------------------------- */

/* Left shift; n >= 64 fills result with 0. */
static uint64_t
b4306_shl_fill(uint64_t u64X, unsigned uN)
{
	if (uN >= 64u) {
		return 0ULL;
	}
	return u64X << uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_shl_fill_u - fill-safe left shift of a 64-bit word.
 *
 * x: value to shift
 * n: shift count; n >= 64 -> 0
 *
 * Returns x << n for n in 0..63; otherwise 0. Does not call libc.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_shl_fill_u(uint64_t u64X, unsigned uN)
{
	(void)NULL;
	return b4306_shl_fill(u64X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_shl_fill_u(uint64_t u64X, unsigned uN)
    __attribute__((alias("gj_u64_shl_fill_u")));
