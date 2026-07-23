/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1573: 8x8 bit-matrix clear (packed in uint64_t).
 *
 * Surface (unique symbols):
 *   void gj_bitmat8_clear(uint64_t *m, unsigned r, unsigned c);
 *     — Clear bit (r,c) of an 8x8 bit matrix packed in *m. Layout is
 *       row-major LSB-first: bit index = r*8 + c. NULL or OOB → no-op.
 *   void __gj_bitmat8_clear  (alias)
 *   __libcgj_batch1573_marker = "libcgj-batch1573"
 *
 * Distinct from gj_bitset_clear (batch118) and gj_bitmat8_set (batch1572).
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>
#include <stddef.h>

const char __libcgj_batch1573_marker[] = "libcgj-batch1573";

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned
b1573_bit(unsigned uR, unsigned uC)
{
	return (uR << 3) + uC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmat8_clear — clear one bit in an 8x8 matrix packed at *pM.
 *
 * pM NULL or uR/uC outside [0,7] → no-op.
 */
void
gj_bitmat8_clear(uint64_t *pM, unsigned uR, unsigned uC)
{
	unsigned uBit;

	if (pM == NULL || uR >= 8u || uC >= 8u) {
		return;
	}
	uBit = b1573_bit(uR, uC);
	*pM &= ~((uint64_t)1 << uBit);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitmat8_clear(uint64_t *pM, unsigned uR, unsigned uC)
    __attribute__((alias("gj_bitmat8_clear")));
