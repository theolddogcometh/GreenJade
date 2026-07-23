/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7978: rand seed LCG step stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seed_lcg_step_u_7978(uint32_t seed);
 *     - One soft ANSI LCG step: (a*seed+c) & RAND_MAX.
 *   uint32_t __gj_seed_lcg_step_u_7978  (alias)
 *   __libcgj_batch7978_marker = "libcgj-batch7978"
 *
 * Exclusive continuum CREATE-ONLY (7971-7980: rand/srand seed stubs —
 * rand_max_id, srand_default_id, lcg_mult_id, lcg_incr_id, seed_is_zero,
 * seed_is_default, seed_ok, seed_lcg_step, seed_errorish, batch_id_7980).
 * Unique gj_seed_lcg_step_u_7978 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7978_marker[] = "libcgj-batch7978";

/* Soft ANSI C example LCG constants and RAND_MAX mask. */
#define B7978_LCG_MULT ((uint32_t)1103515245u)
#define B7978_LCG_INCR ((uint32_t)12345u)
#define B7978_RAND_MAX ((uint32_t)0x7fffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7978_seed_lcg_step(uint32_t u32Seed)
{
	uint32_t u32Next;

	/* Unsigned wrap is well-defined; mask into soft RAND_MAX domain. */
	u32Next = (u32Seed * B7978_LCG_MULT) + B7978_LCG_INCR;
	return u32Next & B7978_RAND_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seed_lcg_step_u_7978 - soft one-step ANSI LCG advance.
 *
 * seed: current seed / state word
 *
 * Returns (1103515245 * seed + 12345) & 0x7fffffff. Pure integer catalog
 * step; does not call rand(3)/srand(3). No parent wires.
 */
uint32_t
gj_seed_lcg_step_u_7978(uint32_t u32Seed)
{
	(void)NULL;
	return b7978_seed_lcg_step(u32Seed);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seed_lcg_step_u_7978(uint32_t u32Seed)
    __attribute__((alias("gj_seed_lcg_step_u_7978")));
