/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4852: flatten 3-D workgroup local ids.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wg_flat_u(uint32_t x, uint32_t y, uint32_t z,
 *                         uint32_t lx, uint32_t ly);
 *     - Linear local id: x + y * lx + z * lx * ly (row-major x-fastest).
 *   uint32_t __gj_wg_flat_u  (alias)
 *   __libcgj_batch4852_marker = "libcgj-batch4852"
 *
 * Exclusive continuum CREATE-ONLY (4851-4860). Unique gj_wg_flat_u
 * surface only; no multi-def. Inverse of gj_wg_x_u / gj_wg_y_u /
 * gj_wg_z_u (batches 4853-4855). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4852_marker[] = "libcgj-batch4852";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * flat = x + y * lx + z * (lx * ly)
 * Pure modular uint32 arithmetic (matches common GPU packing).
 */
static uint32_t
b4852_flat(uint32_t u32X, uint32_t u32Y, uint32_t u32Z,
    uint32_t u32Lx, uint32_t u32Ly)
{
	uint32_t u32Plane;

	u32Plane = u32Lx * u32Ly;
	return u32X + (u32Y * u32Lx) + (u32Z * u32Plane);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wg_flat_u - pack local (x,y,z) into a linear workgroup index.
 *
 * x, y, z: local ids (typically in [0, lx), [0, ly), [0, lz))
 * lx, ly:  workgroup extents along x and y (z extent not required)
 *
 * Returns x + y * lx + z * lx * ly. Self-contained; no parent wires.
 */
uint32_t
gj_wg_flat_u(uint32_t u32X, uint32_t u32Y, uint32_t u32Z,
    uint32_t u32Lx, uint32_t u32Ly)
{
	(void)NULL;
	return b4852_flat(u32X, u32Y, u32Z, u32Lx, u32Ly);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wg_flat_u(uint32_t u32X, uint32_t u32Y, uint32_t u32Z,
    uint32_t u32Lx, uint32_t u32Ly)
    __attribute__((alias("gj_wg_flat_u")));
