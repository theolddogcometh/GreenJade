/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1839: IEEE-754 binary32 absolute-value bits.
 *
 * Surface (unique symbols):
 *   uint32_t gj_f32_bits_abs(uint32_t bits);
 *     — Clear the sign bit of a binary32 encoding; return the magnitude
 *       bit pattern (same as fabsf on the float, but integer-only).
 *   uint32_t __gj_f32_bits_abs  (alias)
 *   __libcgj_batch1839_marker = "libcgj-batch1839"
 *
 * Float bit-pattern exclusive wave (1831–1839); integer-only IEEE bits.
 * Distinct from predicate TUs and from float fabsf (batch92). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1839_marker[] = "libcgj-batch1839";

/* binary32 clear-sign mask (abs value bits). */
#define B1839_F32_ABS   0x7fffffffu

/* ---- freestanding helpers ---------------------------------------------- */

/* Clear sign bit 31; preserve exp+frac. */
static uint32_t
b1839_abs(uint32_t uBits)
{
	return uBits & B1839_F32_ABS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_f32_bits_abs — binary32 magnitude bit pattern (sign cleared).
 *
 * bits: IEEE-754 binary32 encoding as uint32_t
 * Returns bits with bit 31 forced to 0.
 */
uint32_t
gj_f32_bits_abs(uint32_t uBits)
{
	return b1839_abs(uBits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_f32_bits_abs(uint32_t uBits)
    __attribute__((alias("gj_f32_bits_abs")));
