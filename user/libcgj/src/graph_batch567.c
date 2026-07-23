/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch567: saturating uint64_t multiply.
 *
 * Surface (unique symbols):
 *   uint64_t gj_sat_mul_u64(uint64_t a, uint64_t b);
 *     — a * b with saturation at UINT64_MAX on overflow (no wrap).
 *   uint64_t __gj_sat_mul_u64  (alias)
 *   __libcgj_batch567_marker = "libcgj-batch567"
 *
 * Does NOT define gj_sat_add_u64 / gj_sat_sub_u64 — batch377 owns those
 * (avoid multi-def sat).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch567_marker[] = "libcgj-batch567";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sat_mul_u64 — unsigned multiply; clamp to UINT64_MAX if a * b overflows.
 *
 * Zero factors are exact (product 0). Otherwise overflow when
 * a > floor(UINT64_MAX / b); same check as batch229 umul helper but
 * saturates instead of failing.
 */
uint64_t
gj_sat_mul_u64(uint64_t a, uint64_t b)
{
	if (a == 0u || b == 0u) {
		return 0u;
	}
	if (a > (UINT64_MAX / b)) {
		return UINT64_MAX;
	}
	return a * b;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sat_mul_u64(uint64_t a, uint64_t b)
    __attribute__((alias("gj_sat_mul_u64")));
