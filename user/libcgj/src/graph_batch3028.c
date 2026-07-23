/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3028: modular uint64_t negation (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_neg_mod_u(uint64_t x);
 *     - Return modular additive inverse of x mod 2^64:
 *       (0 - x) with wrap. gj_u64_neg_mod_u(0) == 0.
 *       gj_u64_neg_mod_u(1) == UINT64_MAX.
 *   uint64_t __gj_u64_neg_mod_u  (alias)
 *   __libcgj_batch3028_marker = "libcgj-batch3028"
 *
 * Exclusive continuum CREATE-ONLY wave (3021-3030). Unique
 * gj_u64_neg_mod_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3028_marker[] = "libcgj-batch3028";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular negation: 0 - x mod 2^64 (two's-complement wrap). */
static uint64_t
b3028_neg_mod(uint64_t u64X)
{
	return (uint64_t)(0ull - u64X);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_neg_mod_u - modular additive inverse of x mod 2^64.
 *
 * Equivalent to wrapping 0 - x (or ~x + 1). Identity at 0.
 * No parent wires.
 */
uint64_t
gj_u64_neg_mod_u(uint64_t u64X)
{
	(void)NULL;
	return b3028_neg_mod(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_neg_mod_u(uint64_t u64X)
    __attribute__((alias("gj_u64_neg_mod_u")));
