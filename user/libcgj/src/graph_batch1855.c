/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1855: texel / block footprint byte hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_texel_block_bytes_hint(uint32_t bits_per_texel,
 *                                      uint32_t block_w, uint32_t block_h);
 *     — Bytes for one compressed/uncompressed block:
 *       ((bits_per_texel * block_w * block_h) + 7) / 8.
 *       Returns 0 when any factor is 0 or the bit product overflows.
 *       Uncompressed 1x1 blocks reduce to ceil(bpp/8).
 *   uint32_t __gj_texel_block_bytes_hint  (alias)
 *   __libcgj_batch1855_marker = "libcgj-batch1855"
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

const char __libcgj_batch1855_marker[] = "libcgj-batch1855";

/* ---- freestanding helpers ---------------------------------------------- */

/* Checked product of three u32 into u64; 0 on zero or overflow path. */
static uint32_t
b1855_block_bytes(uint32_t u32Bpp, uint32_t u32Bw, uint32_t u32Bh)
{
	uint64_t u64Bits;
	uint64_t u64Tmp;

	if (u32Bpp == 0u || u32Bw == 0u || u32Bh == 0u) {
		return 0u;
	}

	u64Tmp = (uint64_t)u32Bpp * (uint64_t)u32Bw;
	if (u64Tmp / (uint64_t)u32Bpp != (uint64_t)u32Bw) {
		return 0u;
	}
	u64Bits = u64Tmp * (uint64_t)u32Bh;
	if (u64Bits / (uint64_t)u32Bh != u64Tmp) {
		return 0u;
	}
	/* ceil(bits / 8) with overflow guard on +7 */
	if (u64Bits > (UINT64_MAX - 7u)) {
		return 0u;
	}
	u64Bits = (u64Bits + 7u) / 8u;
	if (u64Bits > (uint64_t)UINT32_MAX) {
		return 0u;
	}
	return (uint32_t)u64Bits;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_texel_block_bytes_hint — bytes covering one texel block.
 *
 * bits_per_texel: bits per logical texel (e.g. 32 for RGBA8, 4 for BC1)
 * block_w/h:      block footprint in texels (1x1 uncompressed; 4x4 BC)
 * Returns ceil(bits_per_texel * block_w * block_h / 8), or 0 on error.
 */
uint32_t
gj_texel_block_bytes_hint(uint32_t u32BitsPerTexel, uint32_t u32BlockW,
    uint32_t u32BlockH)
{
	(void)sizeof(NULL);
	return b1855_block_bytes(u32BitsPerTexel, u32BlockW, u32BlockH);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_texel_block_bytes_hint(uint32_t u32BitsPerTexel,
    uint32_t u32BlockW, uint32_t u32BlockH)
    __attribute__((alias("gj_texel_block_bytes_hint")));
