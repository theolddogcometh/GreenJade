/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1853: mipmap level extent (max(1, dim >> level)).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mip_level_size(uint32_t dim, uint32_t level);
 *     — Extent of one dimension at mip level: max(1, dim >> level).
 *       Matches Vulkan/D3D mip extent rules for width/height/depth.
 *       level >= 32 shifts treat dim as fully reduced (result 1 if
 *       dim != 0, else 0).
 *   uint32_t __gj_mip_level_size  (alias)
 *   __libcgj_batch1853_marker = "libcgj-batch1853"
 *
 * GPU/buffer exclusive integer-hint wave (1851–1859); milestone 1860.
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1853_marker[] = "libcgj-batch1853";

/* ---- freestanding helpers ---------------------------------------------- */

/* max(1, dim >> level); level >= 32 → 1 if dim != 0 else 0. */
static uint32_t
b1853_mip_size(uint32_t u32Dim, uint32_t u32Level)
{
	uint32_t u32Shifted;

	if (u32Dim == 0u) {
		return 0u;
	}
	if (u32Level >= 32u) {
		return 1u;
	}
	u32Shifted = u32Dim >> u32Level;
	return (u32Shifted == 0u) ? 1u : u32Shifted;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mip_level_size — extent of one dimension at a mip level.
 *
 * dim:   base-level width, height, or depth
 * level: mip level index (0 = base)
 * Returns max(1, dim >> level), or 0 when dim is 0.
 */
uint32_t
gj_mip_level_size(uint32_t u32Dim, uint32_t u32Level)
{
	(void)sizeof(NULL);
	return b1853_mip_size(u32Dim, u32Level);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mip_level_size(uint32_t u32Dim, uint32_t u32Level)
    __attribute__((alias("gj_mip_level_size")));
