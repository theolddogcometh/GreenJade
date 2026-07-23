/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch649: saturating uint64_t increment.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_inc_sat(uint64_t x);
 *     — Return x + 1, saturating at UINT64_MAX (no wrap).
 *       gj_u64_inc_sat(UINT64_MAX) == UINT64_MAX.
 *       gj_u64_inc_sat(0) == 1.
 *   uint64_t __gj_u64_inc_sat  (alias)
 *   __libcgj_batch649_marker = "libcgj-batch649"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch649_marker[] = "libcgj-batch649";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_inc_sat — increment x with saturation at UINT64_MAX.
 *
 * If x is already UINT64_MAX, return UINT64_MAX (no wrap-around).
 * Otherwise return x + 1.
 */
uint64_t
gj_u64_inc_sat(uint64_t x)
{
	if (x == UINT64_MAX) {
		return UINT64_MAX;
	}
	return x + 1u;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_inc_sat(uint64_t x)
    __attribute__((alias("gj_u64_inc_sat")));
