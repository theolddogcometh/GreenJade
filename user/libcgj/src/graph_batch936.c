/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch936: low nibble of a byte.
 *
 * Surface (unique symbols):
 *   uint8_t gj_nibble_lo(uint8_t x);
 *     — Return x & 0x0f (low 4 bits).
 *   uint8_t __gj_nibble_lo  (alias)
 *   __libcgj_batch936_marker = "libcgj-batch936"
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch936_marker[] = "libcgj-batch936";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nibble_lo — extract the low 4-bit nibble of a byte.
 */
uint8_t
gj_nibble_lo(uint8_t u8X)
{
	return (uint8_t)(u8X & 0x0fu);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_nibble_lo(uint8_t u8X)
    __attribute__((alias("gj_nibble_lo")));
