/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1373: saturating uint32_t multiply.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_saturating_mul(uint32_t a, uint32_t b);
 *     — a * b with saturation at UINT32_MAX on overflow (no wrap).
 *   uint32_t __gj_u32_saturating_mul  (alias)
 *   __libcgj_batch1373_marker = "libcgj-batch1373"
 *
 * Distinct from gj_sat_mul_u64 (batch567). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1373_marker[] = "libcgj-batch1373";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1373_umul_overflows_u32(uint32_t u32A, uint32_t u32B)
{
	if (u32A == 0u || u32B == 0u) {
		return 0;
	}
	return (u32A > (UINT32_MAX / u32B)) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_saturating_mul — unsigned 32-bit multiply; clamp to UINT32_MAX.
 *
 * Zero factors are exact (product 0). Overflow when a > floor(UINT32_MAX / b).
 */
uint32_t
gj_u32_saturating_mul(uint32_t u32A, uint32_t u32B)
{
	if (b1373_umul_overflows_u32(u32A, u32B)) {
		return UINT32_MAX;
	}
	return u32A * u32B;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_saturating_mul(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_saturating_mul")));
