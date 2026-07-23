/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch938: swap high and low nibbles of a byte.
 *
 * Surface (unique symbols):
 *   uint8_t gj_nibble_swap(uint8_t x);
 *     — Return (lo << 4) | hi; e.g. 0xab → 0xba.
 *   uint8_t __gj_nibble_swap  (alias)
 *   __libcgj_batch938_marker = "libcgj-batch938"
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch938_marker[] = "libcgj-batch938";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nibble_swap — exchange the high and low 4-bit halves of a byte.
 *
 * Example: 0xab → 0xba; 0x0f → 0xf0; 0x00 → 0x00.
 */
uint8_t
gj_nibble_swap(uint8_t u8X)
{
	return (uint8_t)(((u8X & 0x0fu) << 4) | ((u8X >> 4) & 0x0fu));
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_nibble_swap(uint8_t u8X)
    __attribute__((alias("gj_nibble_swap")));
