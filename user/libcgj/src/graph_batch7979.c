/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7979: srand seed errorish stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seed_errorish_u_7979(uint32_t seed);
 *     - Return 0 for any seed (no seed is errorish for srand).
 *   uint32_t __gj_seed_errorish_u_7979  (alias)
 *   __libcgj_batch7979_marker = "libcgj-batch7979"
 *
 * Exclusive continuum CREATE-ONLY (7971-7980: rand/srand seed stubs —
 * rand_max_id, srand_default_id, lcg_mult_id, lcg_incr_id, seed_is_zero,
 * seed_is_default, seed_ok, seed_lcg_step, seed_errorish, batch_id_7980).
 * Unique gj_seed_errorish_u_7979 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7979_marker[] = "libcgj-batch7979";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7979_seed_errorish(uint32_t u32Seed)
{
	/* Soft inverse of seed_ok: no seed is catalog-errorish. */
	(void)u32Seed;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seed_errorish_u_7979 - 1 if seed is catalog-errorish.
 *
 * seed: candidate srand seed value
 *
 * Always returns 0 (soft inverse of seed_ok). Does not call srand(3).
 * No parent wires.
 */
uint32_t
gj_seed_errorish_u_7979(uint32_t u32Seed)
{
	(void)NULL;
	return b7979_seed_errorish(u32Seed);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seed_errorish_u_7979(uint32_t u32Seed)
    __attribute__((alias("gj_seed_errorish_u_7979")));
