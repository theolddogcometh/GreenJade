/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch774: freestanding 32-bit byteswap.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bswap_u32(uint32_t x);
 *     — Reverse the four octets of x.
 *   uint32_t __gj_bswap_u32  (alias)
 *   __libcgj_batch774_marker = "libcgj-batch774"
 *
 * Distinct from htonl/ntohl (byteorder.c) and from gj_bswap_u16 /
 * gj_bswap_u64. Unique gj_bswap_u32 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch774_marker[] = "libcgj-batch774";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bswap_u32 — reverse octet order of a 32-bit value.
 *
 * x: value to swap
 */
uint32_t
gj_bswap_u32(uint32_t u32X)
{
	return ((u32X & 0x000000ffu) << 24)
	    | ((u32X & 0x0000ff00u) << 8)
	    | ((u32X & 0x00ff0000u) >> 8)
	    | ((u32X & 0xff000000u) >> 24);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bswap_u32(uint32_t u32X)
    __attribute__((alias("gj_bswap_u32")));
