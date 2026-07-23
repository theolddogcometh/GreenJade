/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3605: uint32_t mul overflow predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_u32_mul_ovf_p_u(uint32_t a, uint32_t b);
 *     - Return 1 if a * b is not representable in uint32_t (would
 *       wrap), else 0. Software range test only; no product formed
 *       on the overflow path.
 *   int __gj_u32_mul_ovf_p_u  (alias)
 *   __libcgj_batch3605_marker = "libcgj-batch3605"
 *
 * Exclusive continuum CREATE-ONLY wave (3601-3610). Unique
 * gj_u32_mul_ovf_p_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3605_marker[] = "libcgj-batch3605";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft overflow test: zero factor never overflows; otherwise
 * overflow iff a > floor(UINT32_MAX / b). No product formed on ovf.
 */
static int
b3605_ovf_p(uint32_t u32A, uint32_t u32B)
{
	if (u32A == 0u || u32B == 0u) {
		return 0;
	}
	if (u32A > (UINT32_MAX / u32B)) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mul_ovf_p_u - true when uint32_t multiply would overflow.
 *
 * a, b: factors
 *
 * Returns 1 if a * b is not representable in uint32_t, else 0.
 * Portable software path: no __int128, no __builtin_mul_overflow.
 * Does not call libc.
 */
int
gj_u32_mul_ovf_p_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3605_ovf_p(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_mul_ovf_p_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_mul_ovf_p_u")));
