/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch557: 16-bit byte swap.
 *
 * Surface (unique symbols):
 *   uint16_t gj_byteswap16(uint16_t x);
 *     — Reverse the two octets of a 16-bit word
 *       (ab → ba). Pure integer, no builtins.
 *   uint16_t __gj_byteswap16  (alias)
 *   __libcgj_batch557_marker = "libcgj-batch557"
 *
 * Distinct from gj_bswap32/64 (batch416/417) and htons-family
 * (batch486–489). Clean-room freestanding pure C (integer only).
 * Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno, no libc, no compiler builtins.
 * No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch557_marker[] = "libcgj-batch557";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_byteswap16 — reverse octet order of a 16-bit word.
 *
 * x: value whose octets are to be reversed
 *
 * Example: 0x1122 → 0x2211.
 */
uint16_t
gj_byteswap16(uint16_t x)
{
	return (uint16_t)(((x & 0x00ffu) << 8) | ((x & 0xff00u) >> 8));
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_byteswap16(uint16_t x)
    __attribute__((alias("gj_byteswap16")));
