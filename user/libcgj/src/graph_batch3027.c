/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3027: modular uint32_t negation (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_neg_mod_u(uint32_t x);
 *     - Return modular additive inverse of x mod 2^32:
 *       (0 - x) with wrap. gj_u32_neg_mod_u(0) == 0.
 *       gj_u32_neg_mod_u(1) == UINT32_MAX.
 *   uint32_t __gj_u32_neg_mod_u  (alias)
 *   __libcgj_batch3027_marker = "libcgj-batch3027"
 *
 * Exclusive continuum CREATE-ONLY wave (3021-3030). Unique
 * gj_u32_neg_mod_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3027_marker[] = "libcgj-batch3027";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular negation: 0 - x mod 2^32 (two's-complement wrap). */
static uint32_t
b3027_neg_mod(uint32_t u32X)
{
	return (uint32_t)(0u - u32X);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_neg_mod_u - modular additive inverse of x mod 2^32.
 *
 * Equivalent to wrapping 0 - x (or ~x + 1). Identity at 0.
 * No parent wires.
 */
uint32_t
gj_u32_neg_mod_u(uint32_t u32X)
{
	(void)NULL;
	return b3027_neg_mod(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_neg_mod_u(uint32_t u32X)
    __attribute__((alias("gj_u32_neg_mod_u")));
