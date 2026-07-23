/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1572: 8x8 bit-matrix set (packed in uint64_t).
 *
 * Surface (unique symbols):
 *   void gj_bitmat8_set(uint64_t *m, unsigned r, unsigned c);
 *     — Set bit (r,c) of an 8x8 bit matrix packed in *m. Layout is
 *       row-major LSB-first: bit index = r*8 + c. NULL or OOB → no-op.
 *   void __gj_bitmat8_set  (alias)
 *   __libcgj_batch1572_marker = "libcgj-batch1572"
 *
 * Distinct from gj_bitset_set (batch118) and gj_bitmat8_get (batch1571).
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>
#include <stddef.h>

const char __libcgj_batch1572_marker[] = "libcgj-batch1572";

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned
b1572_bit(unsigned uR, unsigned uC)
{
	return (uR << 3) + uC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmat8_set — set one bit in an 8x8 matrix packed at *pM.
 *
 * pM NULL or uR/uC outside [0,7] → no-op.
 */
void
gj_bitmat8_set(uint64_t *pM, unsigned uR, unsigned uC)
{
	unsigned uBit;

	if (pM == NULL || uR >= 8u || uC >= 8u) {
		return;
	}
	uBit = b1572_bit(uR, uC);
	*pM |= (uint64_t)1 << uBit;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitmat8_set(uint64_t *pM, unsigned uR, unsigned uC)
    __attribute__((alias("gj_bitmat8_set")));
