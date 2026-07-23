/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch927: clamped uint64_t increment.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_clamped_inc(uint64_t x);
 *     — x + 1 with saturation at UINT64_MAX (no wrap).
 *   uint64_t __gj_u64_clamped_inc  (alias)
 *   __libcgj_batch927_marker = "libcgj-batch927"

 *
 * Does NOT define gj_u64_inc_sat / __gj_u64_inc_sat (batch649) —
 * unique clamped_inc name; avoid multi-def.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch927_marker[] = "libcgj-batch927";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_clamped_inc — increment x; clamp at UINT64_MAX.
 *
 * gj_u64_clamped_inc(UINT64_MAX) == UINT64_MAX.
 * gj_u64_clamped_inc(0) == 1.
 */
uint64_t
gj_u64_clamped_inc(uint64_t u64X)
{
	if (u64X == UINT64_MAX) {
		return UINT64_MAX;
	}
	return u64X + 1u;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_clamped_inc(uint64_t u64X)
    __attribute__((alias("gj_u64_clamped_inc")));
