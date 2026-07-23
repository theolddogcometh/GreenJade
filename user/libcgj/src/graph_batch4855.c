/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4855: extract workgroup local z from flat id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wg_z_u(uint32_t flat, uint32_t lx, uint32_t ly);
 *     - Z component: flat / (lx * ly).
 *       lx == 0 or ly == 0 yields 0 (degenerate plane).
 *   uint32_t __gj_wg_z_u  (alias)
 *   __libcgj_batch4855_marker = "libcgj-batch4855"
 *
 * Exclusive continuum CREATE-ONLY (4851-4860). Unique gj_wg_z_u
 * surface only; no multi-def. Inverse of gj_wg_flat_u (batch4852)
 * for the z axis. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4855_marker[] = "libcgj-batch4855";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4855_z(uint32_t u32Flat, uint32_t u32Lx, uint32_t u32Ly)
{
	uint32_t u32Plane;

	if (u32Lx == 0u || u32Ly == 0u) {
		return 0u;
	}
	u32Plane = u32Lx * u32Ly;
	if (u32Plane == 0u) {
		/* Overflowed product of nonzero extents — treat as degenerate. */
		return 0u;
	}
	return u32Flat / u32Plane;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wg_z_u - recover local z from a flattened workgroup index.
 *
 * flat: linear local id (x + y * lx + z * lx * ly)
 * lx:   workgroup extent along x
 * ly:   workgroup extent along y
 *
 * Returns flat / (lx * ly), or 0 when the xy plane product is zero.
 * Self-contained; no parent wires.
 */
uint32_t
gj_wg_z_u(uint32_t u32Flat, uint32_t u32Lx, uint32_t u32Ly)
{
	(void)NULL;
	return b4855_z(u32Flat, u32Lx, u32Ly);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wg_z_u(uint32_t u32Flat, uint32_t u32Lx, uint32_t u32Ly)
    __attribute__((alias("gj_wg_z_u")));
