/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch928: clamped uint64_t decrement.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_clamped_dec(uint64_t x);
 *     — x - 1 with saturation at 0 (no wrap).
 *   uint64_t __gj_u64_clamped_dec  (alias)
 *   __libcgj_batch928_marker = "libcgj-batch928"

 *
 * Does NOT define gj_u64_dec_sat / __gj_u64_dec_sat (batch656) —
 * unique clamped_dec name; avoid multi-def.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch928_marker[] = "libcgj-batch928";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_clamped_dec — decrement x; clamp at 0.
 *
 * gj_u64_clamped_dec(0) == 0; otherwise returns x - 1.
 */
uint64_t
gj_u64_clamped_dec(uint64_t u64X)
{
	if (u64X == 0u) {
		return 0u;
	}
	return u64X - 1u;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_clamped_dec(uint64_t u64X)
    __attribute__((alias("gj_u64_clamped_dec")));
