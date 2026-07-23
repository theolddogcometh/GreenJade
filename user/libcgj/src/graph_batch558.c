/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch558: 32-bit byte swap (byteswap surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_byteswap32(uint32_t x);
 *     — Reverse the four octets of a 32-bit word
 *       (abcd → dcba). Pure integer, no builtins.
 *   uint32_t __gj_byteswap32  (alias)
 *   __libcgj_batch558_marker = "libcgj-batch558"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch558_marker[] = "libcgj-batch558";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_byteswap32 — reverse octet order of a 32-bit word.
 *
 * x: value whose octets are to be reversed
 *
 * Example: 0x11223344 → 0x44332211.
 */
uint32_t
gj_byteswap32(uint32_t x)
{
	return ((x & 0x000000ffu) << 24) |
	       ((x & 0x0000ff00u) << 8) |
	       ((x & 0x00ff0000u) >> 8) |
	       ((x & 0xff000000u) >> 24);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_byteswap32(uint32_t x)
    __attribute__((alias("gj_byteswap32")));
