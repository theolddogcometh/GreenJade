/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3812: one predicate for uint32_t (_u).
 *
 * Surface (unique symbols):
 *   int gj_u32_is_one_u(uint32_t x);
 *     - Return 1 if x == 1, else 0.
 *   int __gj_u32_is_one_u  (alias)
 *   __libcgj_batch3812_marker = "libcgj-batch3812"
 *
 * Exclusive continuum CREATE-ONLY (3811-3820). Unique gj_u32_is_one_u
 * surface only; no multi-def. Distinct from gj_u64_is_one_u (batch3813).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3812_marker[] = "libcgj-batch3812";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3812_is_one(uint32_t u32X)
{
	return (u32X == 1u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_is_one_u - 1 if x is one, else 0.
 *
 * Pure integer compare. Does not call libc. No parent wires.
 */
int
gj_u32_is_one_u(uint32_t u32X)
{
	(void)NULL;
	return b3812_is_one(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_is_one_u(uint32_t u32X)
    __attribute__((alias("gj_u32_is_one_u")));
