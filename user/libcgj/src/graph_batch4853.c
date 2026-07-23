/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4853: extract workgroup local x from flat id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wg_x_u(uint32_t flat, uint32_t lx);
 *     - X component of a linear local id: flat % lx.
 *       lx == 0 yields 0 (degenerate extent).
 *   uint32_t __gj_wg_x_u  (alias)
 *   __libcgj_batch4853_marker = "libcgj-batch4853"
 *
 * Exclusive continuum CREATE-ONLY (4851-4860). Unique gj_wg_x_u
 * surface only; no multi-def. Inverse of gj_wg_flat_u (batch4852)
 * for the x axis. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4853_marker[] = "libcgj-batch4853";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4853_x(uint32_t u32Flat, uint32_t u32Lx)
{
	if (u32Lx == 0u) {
		return 0u;
	}
	return u32Flat % u32Lx;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wg_x_u - recover local x from a flattened workgroup index.
 *
 * flat: linear local id (x + y * lx + z * lx * ly)
 * lx:   workgroup extent along x
 *
 * Returns flat % lx, or 0 when lx is 0. Self-contained; no parent wires.
 */
uint32_t
gj_wg_x_u(uint32_t u32Flat, uint32_t u32Lx)
{
	(void)NULL;
	return b4853_x(u32Flat, u32Lx);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wg_x_u(uint32_t u32Flat, uint32_t u32Lx)
    __attribute__((alias("gj_wg_x_u")));
