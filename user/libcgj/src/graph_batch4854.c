/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4854: extract workgroup local y from flat id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wg_y_u(uint32_t flat, uint32_t lx, uint32_t ly);
 *     - Y component: (flat / lx) % ly.
 *       lx == 0 or ly == 0 yields 0 (degenerate extent).
 *   uint32_t __gj_wg_y_u  (alias)
 *   __libcgj_batch4854_marker = "libcgj-batch4854"
 *
 * Exclusive continuum CREATE-ONLY (4851-4860). Unique gj_wg_y_u
 * surface only; no multi-def. Inverse of gj_wg_flat_u (batch4852)
 * for the y axis. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4854_marker[] = "libcgj-batch4854";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4854_y(uint32_t u32Flat, uint32_t u32Lx, uint32_t u32Ly)
{
	if (u32Lx == 0u || u32Ly == 0u) {
		return 0u;
	}
	return (u32Flat / u32Lx) % u32Ly;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wg_y_u - recover local y from a flattened workgroup index.
 *
 * flat: linear local id (x + y * lx + z * lx * ly)
 * lx:   workgroup extent along x
 * ly:   workgroup extent along y
 *
 * Returns (flat / lx) % ly, or 0 when lx or ly is 0. Self-contained;
 * no parent wires.
 */
uint32_t
gj_wg_y_u(uint32_t u32Flat, uint32_t u32Lx, uint32_t u32Ly)
{
	(void)NULL;
	return b4854_y(u32Flat, u32Lx, u32Ly);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wg_y_u(uint32_t u32Flat, uint32_t u32Lx, uint32_t u32Ly)
    __attribute__((alias("gj_wg_y_u")));
