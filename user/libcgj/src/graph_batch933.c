/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch933: unpack one byte into eight 0/1 ints.
 *
 * Surface (unique symbols):
 *   void gj_unpack_bools8(uint8_t packed, int out[8]);
 *     — Write out[i] = (packed >> i) & 1 for i in 0..7. out == NULL is a no-op.
 *   void __gj_unpack_bools8  (alias)
 *   __libcgj_batch933_marker = "libcgj-batch933"

 *
 * Does NOT redefine gj_pack_bools8 (batch932) — unpack half only.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch933_marker[] = "libcgj-batch933";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpack_bools8 — expand a packed byte into eight 0/1 integers.
 *
 * u8Packed: source bits; bit 0 (LSB) → out[0]
 * pOut:     array of at least 8 ints; NULL → no-op
 */
void
gj_unpack_bools8(uint8_t u8Packed, int *pOut)
{
	unsigned iBit;

	if (pOut == NULL) {
		return;
	}
	for (iBit = 0u; iBit < 8u; iBit++) {
		pOut[iBit] = (int)((u8Packed >> iBit) & 1u);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_unpack_bools8(uint8_t u8Packed, int *pOut)
    __attribute__((alias("gj_unpack_bools8")));
