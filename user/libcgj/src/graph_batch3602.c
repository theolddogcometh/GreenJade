/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3602: uint64_t add overflow predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_u64_add_ovf_p_u(uint64_t a, uint64_t b);
 *     - Return 1 if a + b is not representable in uint64_t (would
 *       wrap), else 0. Does not form the sum when overflow is detected.
 *   int __gj_u64_add_ovf_p_u  (alias)
 *   __libcgj_batch3602_marker = "libcgj-batch3602"
 *
 * Exclusive continuum CREATE-ONLY wave (3601-3610). Distinct from
 * gj_u64_add_ovf_p (batch2318) and gj_u32_add_ovf_p_u (batch3601) —
 * unique gj_u64_add_ovf_p_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3602_marker[] = "libcgj-batch3602";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if a + b overflows uint64_t.
 * Overflow iff b > UINT64_MAX - a (equivalently sum would wrap).
 */
static int
b3602_ovf_p(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_add_ovf_p_u - true when uint64_t addition would overflow.
 *
 * a, b: summands
 *
 * Returns 1 if a + b is not representable in uint64_t, else 0.
 * Range test only; the sum is never formed on the overflow path.
 */
int
gj_u64_add_ovf_p_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3602_ovf_p(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_add_ovf_p_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_add_ovf_p_u")));
