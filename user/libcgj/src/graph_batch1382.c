/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1382: unpack a u64 into two u32 halves.
 *
 * Surface (unique symbols):
 *   void gj_u64_unpack_u32x2(uint64_t v, uint32_t *lo, uint32_t *hi);
 *     — Inverse of gj_u64_pack_u32x2: *lo = bits [31:0], *hi = bits
 *       [63:32]. NULL lo or NULL hi is ignored for that half only.
 *   void __gj_u64_unpack_u32x2  (alias)
 *   __libcgj_batch1382_marker = "libcgj-batch1382"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1382_marker[] = "libcgj-batch1382";

/* ---- freestanding helpers ---------------------------------------------- */

/* Low 32 bits of a u64. */
static uint32_t
b1382_lo(uint64_t u64V)
{
	return (uint32_t)u64V;
}

/* High 32 bits of a u64. */
static uint32_t
b1382_hi(uint64_t u64V)
{
	return (uint32_t)(u64V >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_unpack_u32x2 — split one uint64 into two uint32 halves.
 *
 * u64V: packed word
 * pLo:  receives bits [31:0];  NULL → skip
 * pHi:  receives bits [63:32]; NULL → skip
 */
void
gj_u64_unpack_u32x2(uint64_t u64V, uint32_t *pLo, uint32_t *pHi)
{
	if (pLo != NULL) {
		*pLo = b1382_lo(u64V);
	}
	if (pHi != NULL) {
		*pHi = b1382_hi(u64V);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u64_unpack_u32x2(uint64_t u64V, uint32_t *pLo, uint32_t *pHi)
    __attribute__((alias("gj_u64_unpack_u32x2")));
