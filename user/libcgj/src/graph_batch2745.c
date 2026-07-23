/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2745: 16-bit octet swap (bitops wave).
 *
 * Surface (unique symbols):
 *   uint16_t gj_bswap16_bo_u(uint16_t x);
 *     - Reverse the two octets of a 16-bit word (ab -> ba). Pure
 *       integer, no compiler builtins.
 *   uint16_t __gj_bswap16_bo_u  (alias)
 *   __libcgj_batch2745_marker = "libcgj-batch2745"
 *
 * Bitops exclusive wave (2741-2750). Distinct from gj_bswap16_u
 * (batch2089), gj_bswap_u16 (batch773), and gj_byteswap16 (batch557) —
 * unique gj_bswap16_bo_u surface only; no multi-def. (Suggested name
 * gj_bswap16_u is already claimed by batch2089.)
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2745_marker[] = "libcgj-batch2745";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b2745_bswap16(uint16_t u16X)
{
	return (uint16_t)(((u16X & 0x00ffu) << 8) | ((u16X & 0xff00u) >> 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bswap16_bo_u - reverse octet order of a 16-bit word (bitops wave).
 *
 * x: value whose octets are to be reversed
 *
 * Example: 0x1122 -> 0x2211.
 */
uint16_t
gj_bswap16_bo_u(uint16_t x)
{
	(void)NULL;
	return b2745_bswap16(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_bswap16_bo_u(uint16_t x)
    __attribute__((alias("gj_bswap16_bo_u")));
