/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1832: IEEE-754 binary32 infinity bit-pattern test.
 *
 * Surface (unique symbols):
 *   int gj_f32_bits_is_inf(uint32_t bits);
 *     — Return 1 if bits encodes ±Inf (exp all-ones, zero fraction),
 *       else 0. Integer-only; no float ops.
 *   int __gj_f32_bits_is_inf  (alias)
 *   __libcgj_batch1832_marker = "libcgj-batch1832"
 *
 * Float bit-pattern exclusive wave (1831–1839); integer-only IEEE bits.
 * Distinct from gj_f32_bits_is_nan (batch1831). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1832_marker[] = "libcgj-batch1832";

/* binary32 abs / exp masks. */
#define B1832_F32_ABS   0x7fffffffu
#define B1832_F32_EXP   0x7f800000u

/* ---- freestanding helpers ---------------------------------------------- */

/* True when abs(bits) equals the +Inf encoding (max exp, zero frac). */
static int
b1832_is_inf(uint32_t uBits)
{
	uint32_t uAbs;

	uAbs = uBits & B1832_F32_ABS;
	return (uAbs == B1832_F32_EXP) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_f32_bits_is_inf — 1 if binary32 bit pattern is ±infinity.
 *
 * bits: IEEE-754 binary32 encoding as uint32_t
 * Returns 1 for +Inf / -Inf, else 0 (NaN/finite/±0).
 */
int
gj_f32_bits_is_inf(uint32_t uBits)
{
	return b1832_is_inf(uBits);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_f32_bits_is_inf(uint32_t uBits)
    __attribute__((alias("gj_f32_bits_is_inf")));
