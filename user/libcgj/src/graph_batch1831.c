/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1831: IEEE-754 binary32 NaN bit-pattern test.
 *
 * Surface (unique symbols):
 *   int gj_f32_bits_is_nan(uint32_t bits);
 *     — Return 1 if bits encodes an IEEE-754 binary32 NaN (exp all-ones
 *       and nonzero payload), else 0. Sign bit ignored. Integer-only;
 *       no float ops / no soft-float.
 *   int __gj_f32_bits_is_nan  (alias)
 *   __libcgj_batch1831_marker = "libcgj-batch1831"
 *
 * Float bit-pattern exclusive wave (1831–1839); integer-only IEEE bits.
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1831_marker[] = "libcgj-batch1831";

/* binary32: exp bits 30..23, frac bits 22..0; abs mask clears sign. */
#define B1831_F32_ABS   0x7fffffffu
#define B1831_F32_EXP   0x7f800000u

/* ---- freestanding helpers ---------------------------------------------- */

/* True when abs(bits) has max exponent and nonzero fraction (NaN). */
static int
b1831_is_nan(uint32_t uBits)
{
	uint32_t uAbs;

	uAbs = uBits & B1831_F32_ABS;
	return (uAbs > B1831_F32_EXP) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_f32_bits_is_nan — 1 if binary32 bit pattern is a NaN.
 *
 * bits: IEEE-754 binary32 encoding as uint32_t (no float load/store)
 * Returns 1 for any quiet/signaling NaN, else 0 (incl. Inf/finite/±0).
 */
int
gj_f32_bits_is_nan(uint32_t uBits)
{
	return b1831_is_nan(uBits);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_f32_bits_is_nan(uint32_t uBits)
    __attribute__((alias("gj_f32_bits_is_nan")));
