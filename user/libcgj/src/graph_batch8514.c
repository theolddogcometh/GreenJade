/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8514: PRNG LCG step stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prng_step_u_8514(uint32_t state);
 *     - One soft ANSI LCG step: (a*state+c) & RAND_MAX.
 *   uint32_t __gj_prng_step_u_8514  (alias)
 *   __libcgj_batch8514_marker = "libcgj-batch8514"
 *
 * Exclusive continuum CREATE-ONLY (8511-8520: prng LCG stubs integer —
 * lcg_mul_id, lcg_add_id, lcg_mod_id, step_u, seed_ok_u, range_u,
 * bool_u, byte_u, ready_u, batch_id_8520). Unique
 * gj_prng_step_u_8514 surface only; no multi-def.
 * Distinct from gj_seed_lcg_step_u_7978 (batch7978). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8514_marker[] = "libcgj-batch8514";

/* Soft ANSI C example LCG constants and RAND_MAX mask. */
#define B8514_LCG_MUL  ((uint32_t)1103515245u)
#define B8514_LCG_ADD  ((uint32_t)12345u)
#define B8514_RAND_MAX ((uint32_t)0x7fffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8514_prng_step(uint32_t u32State)
{
	uint32_t u32Next;

	/* Unsigned wrap is well-defined; mask into soft RAND_MAX domain. */
	u32Next = (u32State * B8514_LCG_MUL) + B8514_LCG_ADD;
	return u32Next & B8514_RAND_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prng_step_u_8514 - soft one-step ANSI PRNG LCG advance.
 *
 * state: current PRNG state word
 *
 * Returns (1103515245 * state + 12345) & 0x7fffffff. Pure integer
 * catalog step; does not call rand(3)/srand(3). No parent wires.
 */
uint32_t
gj_prng_step_u_8514(uint32_t u32State)
{
	(void)NULL;
	return b8514_prng_step(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prng_step_u_8514(uint32_t u32State)
    __attribute__((alias("gj_prng_step_u_8514")));
