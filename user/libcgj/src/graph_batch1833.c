/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1833: IEEE-754 binary32 zero bit-pattern test.
 *
 * Surface (unique symbols):
 *   int gj_f32_bits_is_zero(uint32_t bits);
 *     — Return 1 if bits encodes ±0 (all non-sign bits clear), else 0.
 *       Integer-only; no float ops.
 *   int __gj_f32_bits_is_zero  (alias)
 *   __libcgj_batch1833_marker = "libcgj-batch1833"
 *
 * Float bit-pattern exclusive wave (1831–1839); integer-only IEEE bits.
 * Distinct from is_nan / is_inf. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1833_marker[] = "libcgj-batch1833";

/* Clear sign: remaining bits zero iff ±0. */
#define B1833_F32_ABS   0x7fffffffu

/* ---- freestanding helpers ---------------------------------------------- */

/* True when abs(bits) is zero (+0 or -0). */
static int
b1833_is_zero(uint32_t uBits)
{
	return ((uBits & B1833_F32_ABS) == 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_f32_bits_is_zero — 1 if binary32 bit pattern is ±0.
 *
 * bits: IEEE-754 binary32 encoding as uint32_t
 * Returns 1 for +0 / -0, else 0.
 */
int
gj_f32_bits_is_zero(uint32_t uBits)
{
	return b1833_is_zero(uBits);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_f32_bits_is_zero(uint32_t uBits)
    __attribute__((alias("gj_f32_bits_is_zero")));
