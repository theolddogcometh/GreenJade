/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4859: count workgroup tiles covering extent n.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wg_tiles_u(uint32_t n, uint32_t tile);
 *     - Number of tiles of size tile needed to cover n elements:
 *       ceil(n / tile). tile == 0 → 0. n == 0 → 0 for any tile.
 *   uint32_t __gj_wg_tiles_u  (alias)
 *   __libcgj_batch4859_marker = "libcgj-batch4859"
 *
 * Exclusive continuum CREATE-ONLY (4851-4860). Unique gj_wg_tiles_u
 * surface only; no multi-def. Distinct from gj_wg_div_ceil_u
 * (batch4858) — tile-count naming for dispatch grids. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4859_marker[] = "libcgj-batch4859";

/* ---- freestanding helpers ---------------------------------------------- */

/* Tile count = ceil(n / tile); tile == 0 → 0. */
static uint32_t
b4859_tiles(uint32_t u32N, uint32_t u32Tile)
{
	if (u32Tile == 0u) {
		return 0u;
	}
	return (u32N / u32Tile) + ((u32N % u32Tile) != 0u ? 1u : 0u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wg_tiles_u - how many tiles of size tile cover extent n.
 *
 * n:    total elements / global extent along one axis
 * tile: tile / workgroup size along that axis; 0 → 0
 *
 * Returns ceil(n / tile). Useful for gridDim = tiles(n, localSize).
 * Self-contained; no parent wires.
 */
uint32_t
gj_wg_tiles_u(uint32_t u32N, uint32_t u32Tile)
{
	(void)NULL;
	return b4859_tiles(u32N, u32Tile);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wg_tiles_u(uint32_t u32N, uint32_t u32Tile)
    __attribute__((alias("gj_wg_tiles_u")));
