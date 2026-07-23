/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1474: Q16.16 → integer, floor toward −∞.
 *
 * Surface (unique symbols):
 *   int gj_q16_to_int_floor(int32_t q);
 *     — Q16.16 → signed integer, floor (toward −∞). Distinct from
 *       truncate-toward-zero (gj_q16_to_int).
 *   int __gj_q16_to_int_floor  (alias)
 *   __libcgj_batch1474_marker = "libcgj-batch1474"
 *
 * Q16.16 layout (signed 32-bit): 1.0 == 0x00010000.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1474_marker[] = "libcgj-batch1474";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16_to_int_floor — convert Q16.16 to signed integer, floor toward −∞.
 *
 * Arithmetic right shift of a two's-complement Q16.16 value discards the
 * fraction toward −∞ (e.g. −0.5 → −1, +1.5 → 1).
 */
int
gj_q16_to_int_floor(int32_t i32Q)
{
	return (int)(i32Q >> 16);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_q16_to_int_floor(int32_t i32Q)
    __attribute__((alias("gj_q16_to_int_floor")));
