/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3037: sign-extend 16-bit value to int32_t (_u).
 *
 * Surface (unique symbols):
 *   int32_t gj_sign_extend_u16_u(uint16_t x);
 *     - Interpret low 16 bits as signed int16_t and widen to int32_t.
 *   int32_t __gj_sign_extend_u16_u  (alias)
 *   __libcgj_batch3037_marker = "libcgj-batch3037"
 *
 * CREATE-ONLY exclusive continuum (3031-3040). Unique gj_sign_extend_u16_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3037_marker[] = "libcgj-batch3037";

/* ---- freestanding helpers ---------------------------------------------- */

/* Sign-extend 16-bit payload to int32_t. */
static int32_t
b3037_sext_u16(uint16_t u16X)
{
	return (int32_t)(int16_t)u16X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sign_extend_u16_u - sign-extend a 16-bit value to int32_t.
 *
 * x: source halfword (bit 15 is the sign)
 *
 * 0x7fff → 32767; 0x8000 → -32768; 0xffff → -1.
 */
int32_t
gj_sign_extend_u16_u(uint16_t u16X)
{
	(void)NULL;
	return b3037_sext_u16(u16X);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_sign_extend_u16_u(uint16_t u16X)
    __attribute__((alias("gj_sign_extend_u16_u")));
