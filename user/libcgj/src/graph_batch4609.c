/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4609: zero predicate for tick values.
 *
 * Surface (unique symbols):
 *   int gj_tick_is_zero_u(uint64_t x);
 *     - Return 1 if x == 0, else 0. Tick-domain zero check for idle
 *       budgets / cleared deadlines.
 *   int __gj_tick_is_zero_u  (alias)
 *   __libcgj_batch4609_marker = "libcgj-batch4609"
 *
 * Exclusive continuum CREATE-ONLY (4601-4610). Distinct from
 * gj_u64_is_zero_u (batch3811) — unique gj_tick_is_zero_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4609_marker[] = "libcgj-batch4609";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4609_is_zero(uint64_t u64X)
{
	return (u64X == 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tick_is_zero_u - 1 if tick value x is zero, else 0.
 *
 * Pure integer compare. Does not call libc. No parent wires.
 */
int
gj_tick_is_zero_u(uint64_t u64X)
{
	(void)NULL;
	return b4609_is_zero(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_tick_is_zero_u(uint64_t u64X)
    __attribute__((alias("gj_tick_is_zero_u")));
