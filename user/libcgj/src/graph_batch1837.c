/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1837: IEEE-754 binary64 zero bit-pattern test.
 *
 * Surface (unique symbols):
 *   int gj_f64_bits_is_zero(uint64_t bits);
 *     — Return 1 if bits encodes ±0 (all non-sign bits clear), else 0.
 *       Integer-only; no float ops.
 *   int __gj_f64_bits_is_zero  (alias)
 *   __libcgj_batch1837_marker = "libcgj-batch1837"
 *
 * Float bit-pattern exclusive wave (1831–1839); integer-only IEEE bits.
 * Distinct from f64 is_nan / is_inf. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1837_marker[] = "libcgj-batch1837";

/* Clear sign: remaining bits zero iff ±0. */
#define B1837_F64_ABS   0x7fffffffffffffffull

/* ---- freestanding helpers ---------------------------------------------- */

/* True when abs(bits) is zero (+0 or -0). */
static int
b1837_is_zero(uint64_t uBits)
{
	return ((uBits & B1837_F64_ABS) == 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_f64_bits_is_zero — 1 if binary64 bit pattern is ±0.
 *
 * bits: IEEE-754 binary64 encoding as uint64_t
 * Returns 1 for +0 / -0, else 0.
 */
int
gj_f64_bits_is_zero(uint64_t uBits)
{
	return b1837_is_zero(uBits);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_f64_bits_is_zero(uint64_t uBits)
    __attribute__((alias("gj_f64_bits_is_zero")));
