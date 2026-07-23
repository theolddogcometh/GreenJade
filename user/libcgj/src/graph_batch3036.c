/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3036: sign-extend 8-bit value to int32_t (_u).
 *
 * Surface (unique symbols):
 *   int32_t gj_sign_extend_u8_u(uint8_t x);
 *     - Interpret low 8 bits as signed int8_t and widen to int32_t.
 *   int32_t __gj_sign_extend_u8_u  (alias)
 *   __libcgj_batch3036_marker = "libcgj-batch3036"
 *
 * CREATE-ONLY exclusive continuum (3031-3040). Unique gj_sign_extend_u8_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3036_marker[] = "libcgj-batch3036";

/* ---- freestanding helpers ---------------------------------------------- */

/* Sign-extend 8-bit payload to int32_t. */
static int32_t
b3036_sext_u8(uint8_t u8X)
{
	return (int32_t)(int8_t)u8X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sign_extend_u8_u - sign-extend an 8-bit value to int32_t.
 *
 * x: source octet (bit 7 is the sign)
 *
 * 0x7f → 127; 0x80 → -128; 0xff → -1.
 */
int32_t
gj_sign_extend_u8_u(uint8_t u8X)
{
	(void)NULL;
	return b3036_sext_u8(u8X);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_sign_extend_u8_u(uint8_t u8X)
    __attribute__((alias("gj_sign_extend_u8_u")));
