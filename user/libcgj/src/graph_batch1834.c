/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1834: IEEE-754 binary32 sign bit extract.
 *
 * Surface (unique symbols):
 *   int gj_f32_bits_sign(uint32_t bits);
 *     — Return the sign bit of a binary32 encoding: 1 if negative
 *       (including -0, -Inf, negative NaN), else 0. Integer-only.
 *   int __gj_f32_bits_sign  (alias)
 *   __libcgj_batch1834_marker = "libcgj-batch1834"
 *
 * Float bit-pattern exclusive wave (1831–1839); integer-only IEEE bits.
 * Distinct from is_nan / is_inf / is_zero. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1834_marker[] = "libcgj-batch1834";

/* binary32 sign is bit 31. */
#define B1834_F32_SIGN  0x80000000u

/* ---- freestanding helpers ---------------------------------------------- */

/* Extract sign bit as 0 or 1. */
static int
b1834_sign(uint32_t uBits)
{
	return ((uBits & B1834_F32_SIGN) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_f32_bits_sign — 1 if binary32 bit pattern has sign bit set.
 *
 * bits: IEEE-754 binary32 encoding as uint32_t
 * Returns 1 when bit 31 is set, else 0.
 */
int
gj_f32_bits_sign(uint32_t uBits)
{
	return b1834_sign(uBits);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_f32_bits_sign(uint32_t uBits)
    __attribute__((alias("gj_f32_bits_sign")));
