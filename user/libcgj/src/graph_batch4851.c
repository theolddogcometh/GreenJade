/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4851: GPU-ish workgroup size product (saturate).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wg_size_u(uint32_t x, uint32_t y, uint32_t z);
 *     - Return x * y * z as a workgroup volume, saturating at
 *       UINT32_MAX on overflow (no wrap). Any zero factor yields 0.
 *   uint32_t __gj_wg_size_u  (alias)
 *   __libcgj_batch4851_marker = "libcgj-batch4851"
 *
 * Exclusive continuum CREATE-ONLY (4851-4860: wg_size_u, wg_flat_u,
 * wg_x_u, wg_y_u, wg_z_u, wg_is_edge_u, wg_clamp_u, wg_div_ceil_u,
 * wg_tiles_u, batch_id_4860). Unique gj_wg_size_u surface only; no
 * multi-def. Distinct from gj_u32_sat_mul_u (batch2379) — three-factor
 * workgroup product, not a generic dyadic mul. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4851_marker[] = "libcgj-batch4851";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating uint32_t multiply; no __int128. */
static uint32_t
b4851_sat_mul(uint32_t u32A, uint32_t u32B)
{
	if (u32A == 0u || u32B == 0u) {
		return 0u;
	}
	if (u32A > (UINT32_MAX / u32B)) {
		return UINT32_MAX;
	}
	return u32A * u32B;
}

/* Workgroup volume x*y*z with saturation. */
static uint32_t
b4851_wg_size(uint32_t u32X, uint32_t u32Y, uint32_t u32Z)
{
	return b4851_sat_mul(b4851_sat_mul(u32X, u32Y), u32Z);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wg_size_u - saturating workgroup volume product x * y * z.
 *
 * x, y, z: local workgroup extents along each axis
 *
 * Returns the product, or UINT32_MAX if intermediate multiplication
 * would overflow. Zero if any factor is zero. Self-contained; no
 * parent wires.
 */
uint32_t
gj_wg_size_u(uint32_t u32X, uint32_t u32Y, uint32_t u32Z)
{
	(void)NULL;
	return b4851_wg_size(u32X, u32Y, u32Z);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wg_size_u(uint32_t u32X, uint32_t u32Y, uint32_t u32Z)
    __attribute__((alias("gj_wg_size_u")));
