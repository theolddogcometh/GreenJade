/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3603: uint32_t sub underflow predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_u32_sub_ovf_p_u(uint32_t a, uint32_t b);
 *     - Return 1 if a - b is not representable in uint32_t (would
 *       wrap / underflow), else 0. Does not form the difference on
 *       the overflow path.
 *   int __gj_u32_sub_ovf_p_u  (alias)
 *   __libcgj_batch3603_marker = "libcgj-batch3603"
 *
 * Exclusive continuum CREATE-ONLY wave (3601-3610). Unique
 * gj_u32_sub_ovf_p_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3603_marker[] = "libcgj-batch3603";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if a - b underflows uint32_t.
 * Underflow iff a < b (equivalently difference would wrap).
 */
static int
b3603_ovf_p(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sub_ovf_p_u - true when uint32_t subtraction would underflow.
 *
 * a: minuend; b: subtrahend
 *
 * Returns 1 if a - b is not representable in uint32_t, else 0.
 * Range test only; the difference is never formed on the overflow path.
 */
int
gj_u32_sub_ovf_p_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3603_ovf_p(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_sub_ovf_p_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_sub_ovf_p_u")));
