/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch559: 64-bit byte swap (byteswap surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_byteswap64(uint64_t x);
 *     — Reverse the eight octets of a 64-bit word
 *       (abcdefgh → hgfedcba). Pure integer, no builtins.
 *   uint64_t __gj_byteswap64  (alias)
 *   __libcgj_batch559_marker = "libcgj-batch559"
 *
 * Distinct from gj_bswap64 (batch417) and gj_byteswap16/32
 * (batch557/558). Clean-room freestanding pure C (integer only).
 * Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno, no libc, no compiler builtins.
 * No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch559_marker[] = "libcgj-batch559";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_byteswap64 — reverse octet order of a 64-bit word.
 *
 * x: value whose octets are to be reversed
 *
 * Example: 0x1122334455667788 → 0x8877665544332211.
 */
uint64_t
gj_byteswap64(uint64_t x)
{
	return ((x & 0x00000000000000ffull) << 56) |
	       ((x & 0x000000000000ff00ull) << 40) |
	       ((x & 0x0000000000ff0000ull) << 24) |
	       ((x & 0x00000000ff000000ull) << 8) |
	       ((x & 0x000000ff00000000ull) >> 8) |
	       ((x & 0x0000ff0000000000ull) >> 24) |
	       ((x & 0x00ff000000000000ull) >> 40) |
	       ((x & 0xff00000000000000ull) >> 56);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_byteswap64(uint64_t x)
    __attribute__((alias("gj_byteswap64")));
