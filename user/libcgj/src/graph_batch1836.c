/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1836: IEEE-754 binary64 infinity bit-pattern test.
 *
 * Surface (unique symbols):
 *   int gj_f64_bits_is_inf(uint64_t bits);
 *     — Return 1 if bits encodes ±Inf (exp all-ones, zero fraction),
 *       else 0. Integer-only; no float ops.
 *   int __gj_f64_bits_is_inf  (alias)
 *   __libcgj_batch1836_marker = "libcgj-batch1836"
 *
 * Float bit-pattern exclusive wave (1831–1839); integer-only IEEE bits.
 * Distinct from gj_f64_bits_is_nan (batch1835). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1836_marker[] = "libcgj-batch1836";

/* binary64 abs / exp masks. */
#define B1836_F64_ABS   0x7fffffffffffffffull
#define B1836_F64_EXP   0x7ff0000000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* True when abs(bits) equals the +Inf encoding. */
static int
b1836_is_inf(uint64_t uBits)
{
	uint64_t uAbs;

	uAbs = uBits & B1836_F64_ABS;
	return (uAbs == B1836_F64_EXP) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_f64_bits_is_inf — 1 if binary64 bit pattern is ±infinity.
 *
 * bits: IEEE-754 binary64 encoding as uint64_t
 * Returns 1 for +Inf / -Inf, else 0.
 */
int
gj_f64_bits_is_inf(uint64_t uBits)
{
	return b1836_is_inf(uBits);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_f64_bits_is_inf(uint64_t uBits)
    __attribute__((alias("gj_f64_bits_is_inf")));
