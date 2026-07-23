/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1374: saturating uint64_t multiply (range form).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_saturating_mul(uint64_t a, uint64_t b);
 *     — a * b with saturation at UINT64_MAX on overflow (no wrap).
 *   uint64_t __gj_u64_saturating_mul  (alias)
 *   __libcgj_batch1374_marker = "libcgj-batch1374"
 *
 * Distinct from gj_sat_mul_u64 (batch567) — different public name for the
 * continuum surface. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1374_marker[] = "libcgj-batch1374";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1374_umul_overflows_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64A == 0ull || u64B == 0ull) {
		return 0;
	}
	return (u64A > (UINT64_MAX / u64B)) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_saturating_mul — unsigned 64-bit multiply; clamp to UINT64_MAX.
 *
 * Zero factors are exact (product 0). Overflow when a > floor(UINT64_MAX / b).
 */
uint64_t
gj_u64_saturating_mul(uint64_t u64A, uint64_t u64B)
{
	if (b1374_umul_overflows_u64(u64A, u64B)) {
		return UINT64_MAX;
	}
	return u64A * u64B;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_saturating_mul(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_saturating_mul")));
