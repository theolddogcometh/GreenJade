/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch416: 32-bit byte swap.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bswap32(uint32_t x);
 *     — Reverse the four octets of a 32-bit word
 *       (abcd → dcba). Pure integer, no builtins.
 *   uint32_t __gj_bswap32  (alias)
 *   __libcgj_batch416_marker = "libcgj-batch416"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch416_marker[] = "libcgj-batch416";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bswap32 — reverse octet order of a 32-bit word.
 *
 * x: value whose octets are to be reversed
 *
 * Example: 0x11223344 → 0x44332211.
 */
uint32_t
gj_bswap32(uint32_t u32X)
{
	return ((u32X & 0x000000ffu) << 24) |
	       ((u32X & 0x0000ff00u) << 8) |
	       ((u32X & 0x00ff0000u) >> 8) |
	       ((u32X & 0xff000000u) >> 24);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bswap32(uint32_t u32X)
    __attribute__((alias("gj_bswap32")));
