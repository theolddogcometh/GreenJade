/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch921: saturating uint64_t add (type-first name).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sat_add(uint64_t a, uint64_t b);
 *     — a + b with saturation at UINT64_MAX on overflow (no wrap).
 *   uint64_t __gj_u64_sat_add  (alias)
 *   __libcgj_batch921_marker = "libcgj-batch921"

 *
 * Does NOT define gj_sat_add_u64 / __gj_sat_add_u64 (batch377) —
 * different symbol order (u64_sat_add vs sat_add_u64); avoid multi-def.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch921_marker[] = "libcgj-batch921";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sat_add — unsigned add; clamp to UINT64_MAX if a + b overflows.
 *
 * Detect overflow without wrapping: b > UINT64_MAX - a.
 */
uint64_t
gj_u64_sat_add(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sat_add(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_sat_add")));
