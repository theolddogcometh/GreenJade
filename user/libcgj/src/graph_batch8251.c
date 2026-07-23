/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8251: ffs-style find-first-set integer stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_ffs_u_8251(uint32_t v);
 *     - Find first set bit (1-based index from LSB). Returns 0 if v == 0.
 *       Examples: 0 → 0; 1 → 1; 2 → 2; 0x80000000 → 32.
 *   uint32_t __gj_bit_ffs_u_8251  (alias)
 *   __libcgj_batch8251_marker = "libcgj-batch8251"
 *
 * Exclusive continuum CREATE-ONLY (8251-8260: bitops / ffs-style integer
 * stubs — ffs, clz, ctz, popcount, parity, isolate_lsb, clear_lsb, mask,
 * test, batch_id_8260). Unique gj_bit_ffs_u_8251 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8251_marker[] = "libcgj-batch8251";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8251_ffs — 1-based index of lowest set bit, or 0 when v is zero.
 * Binary-search peel of 16/8/4/2/1 (ctz + 1).
 */
static uint32_t
b8251_ffs(uint32_t u32V)
{
	uint32_t u32X;
	uint32_t u32Ctz;

	if (u32V == 0u) {
		return 0u;
	}

	u32X = u32V;
	u32Ctz = 0u;

	if ((u32X & 0xffffu) == 0u) {
		u32Ctz += 16u;
		u32X >>= 16;
	}
	if ((u32X & 0xffu) == 0u) {
		u32Ctz += 8u;
		u32X >>= 8;
	}
	if ((u32X & 0xfu) == 0u) {
		u32Ctz += 4u;
		u32X >>= 4;
	}
	if ((u32X & 0x3u) == 0u) {
		u32Ctz += 2u;
		u32X >>= 2;
	}
	if ((u32X & 0x1u) == 0u) {
		u32Ctz += 1u;
	}

	return u32Ctz + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_ffs_u_8251 - find first set bit (1-based), 0 if zero.
 *
 * v: 32-bit word to inspect
 *
 * Returns the 1-based index of the least-significant 1-bit (LSB = 1),
 * or 0 when v is 0. Soft ffs catalog surface; pure integer, no builtins.
 * No parent wires.
 */
uint32_t
gj_bit_ffs_u_8251(uint32_t u32V)
{
	(void)NULL;
	return b8251_ffs(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_ffs_u_8251(uint32_t u32V)
    __attribute__((alias("gj_bit_ffs_u_8251")));
