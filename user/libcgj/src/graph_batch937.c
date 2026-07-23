/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch937: high nibble of a byte.
 *
 * Surface (unique symbols):
 *   uint8_t gj_nibble_hi(uint8_t x);
 *     — Return (x >> 4) & 0x0f (high 4 bits as a low nibble).
 *   uint8_t __gj_nibble_hi  (alias)
 *   __libcgj_batch937_marker = "libcgj-batch937"
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch937_marker[] = "libcgj-batch937";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nibble_hi — extract the high 4-bit nibble of a byte (right-aligned).
 */
uint8_t
gj_nibble_hi(uint8_t u8X)
{
	return (uint8_t)((u8X >> 4) & 0x0fu);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_nibble_hi(uint8_t u8X)
    __attribute__((alias("gj_nibble_hi")));
