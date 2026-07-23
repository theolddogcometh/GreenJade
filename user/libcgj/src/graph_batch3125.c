/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3125: gj_ones_complement16_u - 16-bit invert.
 *
 * Surface (unique symbols):
 *   uint16_t gj_ones_complement16_u(uint16_t x);
 *     - Ones-complement (bitwise invert) of a 16-bit value:
 *       return (uint16_t)~x. Used as the final invert step of an
 *       Internet checksum after end-around fold.
 *   uint16_t __gj_ones_complement16_u  (alias)
 *   __libcgj_batch3125_marker = "libcgj-batch3125"
 *
 * Exclusive checksum/net CREATE-ONLY wave (3121-3130). Distinct from
 * gj_ip_checksum_fold (batch124) / gj_inet_csum_finish_u (batch2173)
 * which fold then invert — pure invert surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3125_marker[] = "libcgj-batch3125";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b3125_ones_comp(uint16_t u16X)
{
	return (uint16_t)~u16X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ones_complement16_u - bitwise ones-complement of a 16-bit word.
 *
 * x: value to invert
 *
 * Returns ~x as uint16_t. No parent wires.
 */
uint16_t
gj_ones_complement16_u(uint16_t u16X)
{
	(void)NULL;
	return b3125_ones_comp(u16X);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_ones_complement16_u(uint16_t u16X)
    __attribute__((alias("gj_ones_complement16_u")));
