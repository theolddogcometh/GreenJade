/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7977: srand seed ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seed_ok_u_7977(uint32_t seed);
 *     - Return 1 for any seed (srand accepts all unsigned seeds).
 *   uint32_t __gj_seed_ok_u_7977  (alias)
 *   __libcgj_batch7977_marker = "libcgj-batch7977"
 *
 * Exclusive continuum CREATE-ONLY (7971-7980: rand/srand seed stubs —
 * rand_max_id, srand_default_id, lcg_mult_id, lcg_incr_id, seed_is_zero,
 * seed_is_default, seed_ok, seed_lcg_step, seed_errorish, batch_id_7980).
 * Unique gj_seed_ok_u_7977 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7977_marker[] = "libcgj-batch7977";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7977_seed_ok(uint32_t u32Seed)
{
	/* Catalog: every uint32 seed is valid for srand. */
	(void)u32Seed;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seed_ok_u_7977 - 1 if seed is acceptable for srand.
 *
 * seed: candidate srand seed value
 *
 * Always returns 1 (soft catalog: all seeds are ok). Does not call
 * srand(3). No parent wires.
 */
uint32_t
gj_seed_ok_u_7977(uint32_t u32Seed)
{
	(void)NULL;
	return b7977_seed_ok(u32Seed);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seed_ok_u_7977(uint32_t u32Seed)
    __attribute__((alias("gj_seed_ok_u_7977")));
