/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1334: in-place endian swap of a uint16_t array.
 *
 * Surface (unique symbols):
 *   void gj_u16_bswap_arr(uint16_t *a, size_t n);
 *     — For each of a[0..n), reverse the two octets in place.
 *       NULL a or n == 0 → no-op.
 *   void __gj_u16_bswap_arr  (alias)
 *
 * Distinct from gj_u16_bswap_buf (batch639) — unique name.
 *   __libcgj_batch1334_marker = "libcgj-batch1334"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1334_marker[] = "libcgj-batch1334";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_bswap_arr — in-place 16-bit octet reverse over a[0..n).
 */
void
gj_u16_bswap_arr(uint16_t *pA, size_t cN)
{
	size_t i;

	if (pA == NULL || cN == 0u) {
		return;
	}

	for (i = 0u; i < cN; i++) {
		uint16_t u16X = pA[i];

		pA[i] = (uint16_t)(((u16X & 0x00ffu) << 8) |
		    ((u16X & 0xff00u) >> 8));
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u16_bswap_arr(uint16_t *pA, size_t cN)
    __attribute__((alias("gj_u16_bswap_arr")));
