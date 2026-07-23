/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3604: uint64_t sub underflow predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_u64_sub_ovf_p_u(uint64_t a, uint64_t b);
 *     - Return 1 if a - b is not representable in uint64_t (would
 *       wrap / underflow), else 0. Does not form the difference on
 *       the overflow path.
 *   int __gj_u64_sub_ovf_p_u  (alias)
 *   __libcgj_batch3604_marker = "libcgj-batch3604"
 *
 * Exclusive continuum CREATE-ONLY wave (3601-3610). Distinct from
 * gj_u32_sub_ovf_p_u (batch3603) — unique gj_u64_sub_ovf_p_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3604_marker[] = "libcgj-batch3604";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if a - b underflows uint64_t.
 * Underflow iff a < b (equivalently difference would wrap).
 */
static int
b3604_ovf_p(uint64_t u64A, uint64_t u64B)
{
	if (u64A < u64B) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sub_ovf_p_u - true when uint64_t subtraction would underflow.
 *
 * a: minuend; b: subtrahend
 *
 * Returns 1 if a - b is not representable in uint64_t, else 0.
 * Range test only; the difference is never formed on the overflow path.
 */
int
gj_u64_sub_ovf_p_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3604_ovf_p(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_sub_ovf_p_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_sub_ovf_p_u")));
