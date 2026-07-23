/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3558: uint64_t UB-safe logical right shift.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_shr_safe_u(uint64_t x, unsigned n);
 *     - Return x >> n (logical) for n in 0..63. If n >= 64, return 0
 *       (no shift UB). Pure integer; no builtins.
 *   uint64_t __gj_u64_shr_safe_u  (alias)
 *   __libcgj_batch3558_marker = "libcgj-batch3558"
 *
 * Exclusive continuum CREATE-ONLY (3551-3560). Distinct from
 * gj_u64_shr_sat (batch2406) and gj_u32_shr_safe_u (batch3557) —
 * unique u64 shr safe surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3558_marker[] = "libcgj-batch3558";

/* ---- freestanding helpers ---------------------------------------------- */

/* Logical right shift; n >= 64 is a no-UB zero. */
static uint64_t
b3558_shr_safe(uint64_t u64X, unsigned uN)
{
	if (uN >= 64u) {
		return 0u;
	}
	return u64X >> uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_shr_safe_u - UB-safe logical right shift of a 64-bit word.
 *
 * x: value to shift
 * n: shift count; n >= 64 -> 0
 *
 * Returns x >> n for n in 0..63; otherwise 0. Does not call libc.
 * No parent wires.
 */
uint64_t
gj_u64_shr_safe_u(uint64_t u64X, unsigned uN)
{
	(void)NULL;
	return b3558_shr_safe(u64X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_shr_safe_u(uint64_t u64X, unsigned uN)
    __attribute__((alias("gj_u64_shr_safe_u")));
