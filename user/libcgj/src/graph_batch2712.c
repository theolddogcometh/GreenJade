/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2712: soft saturating uint32_t multiply.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sat_mul_u_soft(uint32_t a, uint32_t b);
 *     - a * b with saturation at UINT32_MAX on overflow (no wrap).
 *   uint32_t __gj_u32_sat_mul_u_soft  (alias)
 *   __libcgj_batch2712_marker = "libcgj-batch2712"
 *
 * Post-2710 continuum exclusive wave (2711-2720). Renamed from the
 * planned gj_u32_sat_mul_u because that symbol is owned by batch2379;
 * _soft suffix keeps the surface unique (no multi-def). Also distinct
 * from gj_u32_saturating_mul (batch1373). No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2712_marker[] = "libcgj-batch2712";

/* ---- freestanding helpers ---------------------------------------------- */

/* Overflow iff a > floor(UINT32_MAX / b) for nonzero b. */
static int
b2712_umul_overflows(uint32_t u32A, uint32_t u32B)
{
	if (u32A == 0u || u32B == 0u) {
		return 0;
	}
	return (u32A > (UINT32_MAX / u32B)) ? 1 : 0;
}

/* Saturating product; zero factors are exact. */
static uint32_t
b2712_sat_mul(uint32_t u32A, uint32_t u32B)
{
	if (b2712_umul_overflows(u32A, u32B)) {
		return UINT32_MAX;
	}
	return u32A * u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sat_mul_u_soft - unsigned 32-bit multiply; clamp to UINT32_MAX.
 *
 * a, b: factors
 */
uint32_t
gj_u32_sat_mul_u_soft(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2712_sat_mul(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sat_mul_u_soft(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_sat_mul_u_soft")));
