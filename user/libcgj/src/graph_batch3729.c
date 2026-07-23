/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3729: freestanding fair coin from a u32 draw
 * (_u surface).
 *
 * Surface (unique symbols):
 *   int gj_rng_bool_u(uint32_t r);
 *     - Return 1 if the most-significant bit of r is set, else 0.
 *       Interprets a full-range uniform uint32_t draw as a fair Bernoulli
 *       trial with p = 1/2 (MSB test). Pure predicate; does not advance
 *       a PRNG.
 *   int __gj_rng_bool_u  (alias)
 *   __libcgj_batch3729_marker = "libcgj-batch3729"
 *
 * CREATE-ONLY exclusive continuum wave (3721-3730). Distinct from
 * gj_bernoulli_u32 (batch718 rational p_num/p_den) — unique fair-coin
 * rng_bool_u surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3729_marker[] = "libcgj-batch3729";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if MSB of u32R is set (fair coin from full-range u32). */
static int
b3729_coin(uint32_t u32R)
{
	return ((u32R & 0x80000000u) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rng_bool_u — fair Bernoulli trial (p = 1/2) from a u32 sample.
 *
 * r: uniform integer on the full uint32 domain [0, 2^32)
 *
 * Returns 1 when bit 31 of r is set, else 0. Exactly half of the
 * uint32 domain maps to each outcome.
 */
int
gj_rng_bool_u(uint32_t r)
{
	(void)NULL;
	return b3729_coin(r);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rng_bool_u(uint32_t r)
    __attribute__((alias("gj_rng_bool_u")));
