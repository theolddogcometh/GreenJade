/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1341: unpack a u32 into eight 4-bit nibbles.
 *
 * Surface (unique symbols):
 *   void gj_u32_unpack_nibbles(uint32_t v, uint8_t out[8]);
 *     — Write out[i] = (v >> (i*4)) & 0xF for i in 0..7 (LSB nibble first).
 *       out == NULL is a no-op.
 *   void __gj_u32_unpack_nibbles  (alias)
 *   __libcgj_batch1341_marker = "libcgj-batch1341"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1341_marker[] = "libcgj-batch1341";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_unpack_nibbles — expand one uint32 into eight 4-bit values.
 *
 * u32V: packed word; nibble 0 is bits [3:0]
 * pOut: array of at least 8 bytes; NULL → no-op
 */
void
gj_u32_unpack_nibbles(uint32_t u32V, uint8_t *pOut)
{
	unsigned iNib;

	if (pOut == NULL) {
		return;
	}
	for (iNib = 0u; iNib < 8u; iNib++) {
		pOut[iNib] = (uint8_t)((u32V >> (iNib * 4u)) & 0xFu);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_unpack_nibbles(uint32_t u32V, uint8_t *pOut)
    __attribute__((alias("gj_u32_unpack_nibbles")));
