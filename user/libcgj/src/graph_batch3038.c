/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3038: zero-extend 8-bit value to uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_zero_extend_u8_u(uint8_t x);
 *     - Widen an 8-bit value to uint32_t with high bits zero.
 *   uint32_t __gj_zero_extend_u8_u  (alias)
 *   __libcgj_batch3038_marker = "libcgj-batch3038"
 *
 * CREATE-ONLY exclusive continuum (3031-3040). Unique gj_zero_extend_u8_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3038_marker[] = "libcgj-batch3038";

/* ---- freestanding helpers ---------------------------------------------- */

/* Zero-extend 8-bit payload to uint32_t. */
static uint32_t
b3038_zext_u8(uint8_t u8X)
{
	return (uint32_t)u8X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zero_extend_u8_u - zero-extend an 8-bit value to uint32_t.
 *
 * x: source octet
 *
 * Always in [0, 255]. Explicit widen for call-site clarity.
 */
uint32_t
gj_zero_extend_u8_u(uint8_t u8X)
{
	(void)NULL;
	return b3038_zext_u8(u8X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zero_extend_u8_u(uint8_t u8X)
    __attribute__((alias("gj_zero_extend_u8_u")));
