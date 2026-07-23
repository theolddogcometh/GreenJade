/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2089: 16-bit octet swap (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_bswap16_u(uint16_t x);
 *     - Reverse the two octets of a 16-bit word (ab -> ba). Pure
 *       integer, no compiler builtins.
 *   uint16_t __gj_bswap16_u  (alias)
 *   __libcgj_batch2089_marker = "libcgj-batch2089"
 *
 * Endian store exclusive wave (2081-2090). Distinct from gj_bswap32
 * (batch416), gj_bswap64 (batch417), and htons-family symbols - unique
 * gj_bswap16_u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2089_marker[] = "libcgj-batch2089";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b2089_bswap16(uint16_t u16X)
{
	return (uint16_t)(((u16X & 0x00ffu) << 8) | ((u16X & 0xff00u) >> 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bswap16_u - reverse octet order of a 16-bit word.
 *
 * x: value whose octets are to be reversed
 *
 * Example: 0x1122 -> 0x2211.
 */
uint16_t
gj_bswap16_u(uint16_t x)
{
	(void)NULL;
	return b2089_bswap16(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_bswap16_u(uint16_t x)
    __attribute__((alias("gj_bswap16_u")));
