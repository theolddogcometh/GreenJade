/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3809: uint32_t zero predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_u32_is_zero_u(uint32_t x);
 *     - Return 1 if x is zero, else 0. Pure unsigned zero test.
 *   int __gj_u32_is_zero_u  (alias)
 *   __libcgj_batch3809_marker = "libcgj-batch3809"
 *
 * Exclusive continuum CREATE-ONLY wave (3801-3810). Unique
 * gj_u32_is_zero_u surface only; no multi-def. Distinct from
 * gj_mem_is_zero (batch607), gj_mac_is_zero (batch798), and
 * gj_f32_bits_is_zero (batch1833). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3809_marker[] = "libcgj-batch3809";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if x is zero, else 0. */
static int
b3809_is_zero(uint32_t u32X)
{
	return u32X == 0u ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_is_zero_u - true if x is the zero uint32_t value.
 *
 * Returns 1 when x == 0, otherwise 0. Pure integer test; no libc.
 * No parent wires.
 */
int
gj_u32_is_zero_u(uint32_t u32X)
{
	(void)NULL;
	return b3809_is_zero(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_is_zero_u(uint32_t u32X)
    __attribute__((alias("gj_u32_is_zero_u")));
