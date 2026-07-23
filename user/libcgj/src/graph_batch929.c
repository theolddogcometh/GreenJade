/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch929: boolean predicate to uint8_t 0/1.
 *
 * Surface (unique symbols):
 *   uint8_t gj_bool_to_u8(int f);
 *     — Return 1 if f is non-zero, else 0 (as uint8_t).
 *   uint8_t __gj_bool_to_u8  (alias)
 *   __libcgj_batch929_marker = "libcgj-batch929"
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch929_marker[] = "libcgj-batch929";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bool_to_u8 — map a C truthiness value to a single byte 0 or 1.
 *
 * f: any int treated as boolean (0 → false, non-zero → true)
 *
 * Always returns 0u or 1u; never other non-zero bytes.
 */
uint8_t
gj_bool_to_u8(int fVal)
{
	return (uint8_t)(fVal != 0);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_bool_to_u8(int fVal)
    __attribute__((alias("gj_bool_to_u8")));
