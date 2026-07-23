/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch377: saturating uint64_t add and subtract.
 *
 * Surface (unique symbols):
 *   uint64_t gj_sat_add_u64(uint64_t a, uint64_t b);
 *     — a + b with saturation at UINT64_MAX on overflow (no wrap).
 *   uint64_t gj_sat_sub_u64(uint64_t a, uint64_t b);
 *     — a - b with saturation at 0 on underflow (no wrap).
 *   uint64_t __gj_sat_add_u64  (alias)
 *   uint64_t __gj_sat_sub_u64  (alias)
 *   __libcgj_batch377_marker = "libcgj-batch377"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch377_marker[] = "libcgj-batch377";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sat_add_u64 — unsigned add; clamp to UINT64_MAX if a + b overflows.
 *
 * Detect overflow without wrapping: b > UINT64_MAX - a.
 */
uint64_t
gj_sat_add_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/*
 * gj_sat_sub_u64 — unsigned subtract; clamp to 0 if a < b (underflow).
 */
uint64_t
gj_sat_sub_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64A < u64B) {
		return 0u;
	}
	return u64A - u64B;
}

/* ---- underscored aliases ----------------------------------------------- */

uint64_t __gj_sat_add_u64(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_sat_add_u64")));

uint64_t __gj_sat_sub_u64(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_sat_sub_u64")));
