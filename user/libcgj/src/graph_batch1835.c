/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1835: IEEE-754 binary64 NaN bit-pattern test.
 *
 * Surface (unique symbols):
 *   int gj_f64_bits_is_nan(uint64_t bits);
 *     — Return 1 if bits encodes an IEEE-754 binary64 NaN (exp all-ones
 *       and nonzero payload), else 0. Integer-only; no float ops.
 *   int __gj_f64_bits_is_nan  (alias)
 *   __libcgj_batch1835_marker = "libcgj-batch1835"
 *
 * Float bit-pattern exclusive wave (1831–1839); integer-only IEEE bits.
 * Distinct from gj_f32_bits_is_nan (batch1831). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1835_marker[] = "libcgj-batch1835";

/* binary64: exp bits 62..52; abs clears sign bit 63. */
#define B1835_F64_ABS   0x7fffffffffffffffull
#define B1835_F64_EXP   0x7ff0000000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* True when abs(bits) has max exponent and nonzero fraction (NaN). */
static int
b1835_is_nan(uint64_t uBits)
{
	uint64_t uAbs;

	uAbs = uBits & B1835_F64_ABS;
	return (uAbs > B1835_F64_EXP) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_f64_bits_is_nan — 1 if binary64 bit pattern is a NaN.
 *
 * bits: IEEE-754 binary64 encoding as uint64_t
 * Returns 1 for any quiet/signaling NaN, else 0.
 */
int
gj_f64_bits_is_nan(uint64_t uBits)
{
	return b1835_is_nan(uBits);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_f64_bits_is_nan(uint64_t uBits)
    __attribute__((alias("gj_f64_bits_is_nan")));
