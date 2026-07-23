/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3039: zero-extend 16-bit value to uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_zero_extend_u16_u(uint16_t x);
 *     - Widen a 16-bit value to uint32_t with high bits zero.
 *   uint32_t __gj_zero_extend_u16_u  (alias)
 *   __libcgj_batch3039_marker = "libcgj-batch3039"
 *
 * CREATE-ONLY exclusive continuum (3031-3040). Unique gj_zero_extend_u16_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3039_marker[] = "libcgj-batch3039";

/* ---- freestanding helpers ---------------------------------------------- */

/* Zero-extend 16-bit payload to uint32_t. */
static uint32_t
b3039_zext_u16(uint16_t u16X)
{
	return (uint32_t)u16X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zero_extend_u16_u - zero-extend a 16-bit value to uint32_t.
 *
 * x: source halfword
 *
 * Always in [0, 65535]. Explicit widen for call-site clarity.
 */
uint32_t
gj_zero_extend_u16_u(uint16_t u16X)
{
	(void)NULL;
	return b3039_zext_u16(u16X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zero_extend_u16_u(uint16_t u16X)
    __attribute__((alias("gj_zero_extend_u16_u")));
