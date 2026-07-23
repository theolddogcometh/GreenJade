/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3815: max predicate for uint64_t (_u).
 *
 * Surface (unique symbols):
 *   int gj_u64_is_max_u(uint64_t x);
 *     - Return 1 if x == UINT64_MAX (0xffffffffffffffff), else 0.
 *   int __gj_u64_is_max_u  (alias)
 *   __libcgj_batch3815_marker = "libcgj-batch3815"
 *
 * Exclusive continuum CREATE-ONLY (3811-3820). Unique gj_u64_is_max_u
 * surface only; no multi-def. Distinct from gj_u32_is_max_u (batch3814).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3815_marker[] = "libcgj-batch3815";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3815_is_max(uint64_t u64X)
{
	return (u64X == UINT64_MAX) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_is_max_u - 1 if x is UINT64_MAX, else 0.
 *
 * Pure integer compare. Does not call libc. No parent wires.
 */
int
gj_u64_is_max_u(uint64_t u64X)
{
	(void)NULL;
	return b3815_is_max(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_is_max_u(uint64_t u64X)
    __attribute__((alias("gj_u64_is_max_u")));
