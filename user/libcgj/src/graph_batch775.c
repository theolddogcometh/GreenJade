/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch775: freestanding 64-bit byteswap.
 *
 * Surface (unique symbols):
 *   uint64_t gj_bswap_u64(uint64_t x);
 *     — Reverse the eight octets of x.
 *   uint64_t __gj_bswap_u64  (alias)
 *   __libcgj_batch775_marker = "libcgj-batch775"
 *
 * Distinct from file-static b61_bswap64 and from gj_bswap_u16 /
 * gj_bswap_u32. Unique gj_bswap_u64 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch775_marker[] = "libcgj-batch775";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bswap_u64 — reverse octet order of a 64-bit value.
 *
 * x: value to swap
 */
uint64_t
gj_bswap_u64(uint64_t u64X)
{
	return ((u64X & 0x00000000000000ffULL) << 56)
	    | ((u64X & 0x000000000000ff00ULL) << 40)
	    | ((u64X & 0x0000000000ff0000ULL) << 24)
	    | ((u64X & 0x00000000ff000000ULL) << 8)
	    | ((u64X & 0x000000ff00000000ULL) >> 8)
	    | ((u64X & 0x0000ff0000000000ULL) >> 24)
	    | ((u64X & 0x00ff000000000000ULL) >> 40)
	    | ((u64X & 0xff00000000000000ULL) >> 56);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bswap_u64(uint64_t u64X)
    __attribute__((alias("gj_bswap_u64")));
