/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch998: scaled ratio of two u64 values.
 *
 * Surface (unique symbols):
 *   uint64_t gj_ratio_u64(uint64_t a, uint64_t b, uint64_t scale);
 *     — Return (a * scale) / b when b != 0; else 0.
 *   uint64_t __gj_ratio_u64  (alias)
 *   __libcgj_batch998_marker = "libcgj-batch998"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch998_marker[] = "libcgj-batch998";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ratio_u64 — compute (a * scale) / b with zero-divisor guard.
 *
 * u64A:     numerator
 * u64B:     denominator (0 → return 0)
 * u64Scale: scale factor applied before division
 */
uint64_t
gj_ratio_u64(uint64_t u64A, uint64_t u64B, uint64_t u64Scale)
{
	if (u64B == 0u) {
		return 0u;
	}

	return (u64A * u64Scale) / u64B;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ratio_u64(uint64_t u64A, uint64_t u64B, uint64_t u64Scale)
    __attribute__((alias("gj_ratio_u64")));
