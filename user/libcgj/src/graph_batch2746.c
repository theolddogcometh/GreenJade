/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2746: 32-bit octet swap (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bswap32_u(uint32_t x);
 *     - Reverse the four octets of a 32-bit word (abcd -> dcba). Pure
 *       integer, no compiler builtins.
 *   uint32_t __gj_bswap32_u  (alias)
 *   __libcgj_batch2746_marker = "libcgj-batch2746"
 *
 * Bitops exclusive wave (2741-2750). Distinct from gj_bswap32
 * (batch416), gj_bswap_u32 (batch774), and gj_byteswap32 (batch558) —
 * unique gj_bswap32_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2746_marker[] = "libcgj-batch2746";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2746_bswap32(uint32_t u32X)
{
	return ((u32X & 0x000000ffu) << 24) |
	       ((u32X & 0x0000ff00u) << 8) |
	       ((u32X & 0x00ff0000u) >> 8) |
	       ((u32X & 0xff000000u) >> 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bswap32_u - reverse octet order of a 32-bit word.
 *
 * x: value whose octets are to be reversed
 *
 * Example: 0x11223344 -> 0x44332211.
 */
uint32_t
gj_bswap32_u(uint32_t x)
{
	(void)NULL;
	return b2746_bswap32(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bswap32_u(uint32_t x)
    __attribute__((alias("gj_bswap32_u")));
