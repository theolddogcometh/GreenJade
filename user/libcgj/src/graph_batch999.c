/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch999: safe unsigned 64-bit division.
 *
 * Surface (unique symbols):
 *   uint64_t gj_safe_div_u64(uint64_t a, uint64_t b);
 *     — Return a / b when b != 0; else 0.
 *   uint64_t __gj_safe_div_u64  (alias)
 *   __libcgj_batch999_marker = "libcgj-batch999"
 *
 * Does NOT redefine gj_div_round_nearest_u64 / gj_udiv_round / gj_ratio_u64.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch999_marker[] = "libcgj-batch999";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_safe_div_u64 — unsigned division with zero-divisor guard.
 *
 * u64A: dividend
 * u64B: divisor (0 → return 0)
 */
uint64_t
gj_safe_div_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64B == 0u) {
		return 0u;
	}

	return u64A / u64B;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_safe_div_u64(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_safe_div_u64")));
