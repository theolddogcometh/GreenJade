/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1838: IEEE-754 binary64 sign bit extract.
 *
 * Surface (unique symbols):
 *   int gj_f64_bits_sign(uint64_t bits);
 *     — Return the sign bit of a binary64 encoding: 1 if negative
 *       (including -0, -Inf, negative NaN), else 0. Integer-only.
 *   int __gj_f64_bits_sign  (alias)
 *   __libcgj_batch1838_marker = "libcgj-batch1838"
 *
 * Float bit-pattern exclusive wave (1831–1839); integer-only IEEE bits.
 * Distinct from f32_bits_sign / f64 is_* predicates. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1838_marker[] = "libcgj-batch1838";

/* binary64 sign is bit 63. */
#define B1838_F64_SIGN  0x8000000000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Extract sign bit as 0 or 1. */
static int
b1838_sign(uint64_t uBits)
{
	return ((uBits & B1838_F64_SIGN) != 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_f64_bits_sign — 1 if binary64 bit pattern has sign bit set.
 *
 * bits: IEEE-754 binary64 encoding as uint64_t
 * Returns 1 when bit 63 is set, else 0.
 */
int
gj_f64_bits_sign(uint64_t uBits)
{
	return b1838_sign(uBits);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_f64_bits_sign(uint64_t uBits)
    __attribute__((alias("gj_f64_bits_sign")));
