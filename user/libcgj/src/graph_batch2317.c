/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2317: uint32_t add overflow predicate
 * (post-2310 arith exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_u32_add_ovf_p(uint32_t a, uint32_t b);
 *     - Return 1 if a + b is not representable in uint32_t (would
 *       wrap), else 0. Does not form the sum when overflow is detected.
 *   int __gj_u32_add_ovf_p  (alias)
 *   __libcgj_batch2317_marker = "libcgj-batch2317"
 *
 * Post-2310 wrap/ovf exclusive wave (2311-2320). Distinct from
 * gj_u32_add_wrap (batch2311 which wraps) - unique gj_u32_add_ovf_p
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2317_marker[] = "libcgj-batch2317";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if a + b overflows uint32_t.
 * Overflow iff b > UINT32_MAX - a (equivalently sum would wrap).
 */
static int
b2317_ovf_p(uint32_t u32A, uint32_t u32B)
{
	if (u32B > (UINT32_MAX - u32A)) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_add_ovf_p - true when uint32_t addition would overflow.
 *
 * a, b: summands
 *
 * Returns 1 if a + b is not representable in uint32_t, else 0.
 * Range test only; the sum is never formed on the overflow path.
 */
int
gj_u32_add_ovf_p(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2317_ovf_p(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_add_ovf_p(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_add_ovf_p")));
