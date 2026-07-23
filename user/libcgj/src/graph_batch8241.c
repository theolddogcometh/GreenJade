/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8241: freestanding 16-bit octet swap (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_bswap16_u_8241(uint16_t x);
 *     - Reverse the two octets of a 16-bit word (ab -> ba). Pure
 *       integer, no compiler builtins.
 *   uint16_t __gj_bswap16_u_8241  (alias)
 *   __libcgj_batch8241_marker = "libcgj-batch8241"
 *
 * Exclusive continuum CREATE-ONLY (8241-8250: byteswap / endian helpers —
 * bswap16, bswap32, htons, ntohs, htonl, ntohl, le_id, be_id, swap_need,
 * batch_id). Distinct from gj_bswap16_u (batch2089), gj_bswap16_bo_u
 * (batch2745) — unique gj_bswap16_u_8241 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8241_marker[] = "libcgj-batch8241";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b8241_bswap16(uint16_t u16X)
{
	return (uint16_t)(((u16X & 0x00ffu) << 8) | ((u16X & 0xff00u) >> 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bswap16_u_8241 - reverse octet order of a 16-bit word.
 *
 * x: value whose octets are to be reversed
 *
 * Example: 0x1122 -> 0x2211.
 */
uint16_t
gj_bswap16_u_8241(uint16_t x)
{
	(void)NULL;
	return b8241_bswap16(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_bswap16_u_8241(uint16_t x)
    __attribute__((alias("gj_bswap16_u_8241")));
