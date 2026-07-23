/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3606: uint64_t mul overflow predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_u64_mul_ovf_p_u(uint64_t a, uint64_t b);
 *     - Return 1 if a * b is not representable in uint64_t (would
 *       wrap), else 0. Software range test only; no product formed
 *       on the overflow path.
 *   int __gj_u64_mul_ovf_p_u  (alias)
 *   __libcgj_batch3606_marker = "libcgj-batch3606"
 *
 * Exclusive continuum CREATE-ONLY wave (3601-3610). Distinct from
 * gj_u64_mul_ovf_p_soft (batch2319) and gj_u32_mul_ovf_p_u (batch3605)
 * — unique gj_u64_mul_ovf_p_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3606_marker[] = "libcgj-batch3606";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft overflow test: zero factor never overflows; otherwise
 * overflow iff a > floor(UINT64_MAX / b). No product formed on ovf.
 */
static int
b3606_ovf_p(uint64_t u64A, uint64_t u64B)
{
	if (u64A == 0ull || u64B == 0ull) {
		return 0;
	}
	if (u64A > (UINT64_MAX / u64B)) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mul_ovf_p_u - true when uint64_t multiply would overflow.
 *
 * a, b: factors
 *
 * Returns 1 if a * b is not representable in uint64_t, else 0.
 * Portable software path: no __int128, no __uint128_t, no
 * __builtin_mul_overflow. Does not call libc.
 */
int
gj_u64_mul_ovf_p_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3606_ovf_p(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_mul_ovf_p_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_mul_ovf_p_u")));
