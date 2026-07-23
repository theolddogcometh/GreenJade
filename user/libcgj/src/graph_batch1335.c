/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1335: in-place endian swap of a uint32_t array.
 *
 * Surface (unique symbols):
 *   void gj_u32_bswap_arr(uint32_t *a, size_t n);
 *     — For each of a[0..n), reverse four octets (abcd → dcba).
 *       NULL a or n == 0 → no-op.
 *   void __gj_u32_bswap_arr  (alias)
 *
 * Distinct from gj_u32_bswap_buf (batch640) / gj_bswap32 — unique name.
 *   __libcgj_batch1335_marker = "libcgj-batch1335"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1335_marker[] = "libcgj-batch1335";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1335_bswap32(uint32_t u32X)
{
	return ((u32X & 0x000000ffu) << 24) |
	       ((u32X & 0x0000ff00u) << 8) |
	       ((u32X & 0x00ff0000u) >> 8) |
	       ((u32X & 0xff000000u) >> 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bswap_arr — in-place 32-bit octet reverse over a[0..n).
 */
void
gj_u32_bswap_arr(uint32_t *pA, size_t cN)
{
	size_t i;

	if (pA == NULL || cN == 0u) {
		return;
	}

	for (i = 0u; i < cN; i++) {
		pA[i] = b1335_bswap32(pA[i]);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_bswap_arr(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_bswap_arr")));
