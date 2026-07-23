/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3816: even predicate for uint32_t (_u).
 *
 * Surface (unique symbols):
 *   int gj_u32_is_even_u(uint32_t x);
 *     - Return 1 if x is even ((x & 1) == 0), else 0.
 *   int __gj_u32_is_even_u  (alias)
 *   __libcgj_batch3816_marker = "libcgj-batch3816"
 *
 * Exclusive continuum CREATE-ONLY (3811-3820). Unique gj_u32_is_even_u
 * surface only; no multi-def. Distinct from gj_u64_is_even_u (batch3817)
 * / gj_u32_is_odd_u (batch3818). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3816_marker[] = "libcgj-batch3816";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3816_is_even(uint32_t u32X)
{
	return ((u32X & 1u) == 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_is_even_u - 1 if x is even, else 0.
 *
 * LSB clear test. Does not call libc. No parent wires.
 */
int
gj_u32_is_even_u(uint32_t u32X)
{
	(void)NULL;
	return b3816_is_even(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_is_even_u(uint32_t u32X)
    __attribute__((alias("gj_u32_is_even_u")));
