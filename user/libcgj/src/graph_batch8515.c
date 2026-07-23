/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8515: PRNG seed ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prng_seed_ok_u_8515(uint32_t seed);
 *     - Return 1 for any seed (soft LCG accepts all unsigned seeds).
 *   uint32_t __gj_prng_seed_ok_u_8515  (alias)
 *   __libcgj_batch8515_marker = "libcgj-batch8515"
 *
 * Exclusive continuum CREATE-ONLY (8511-8520: prng LCG stubs integer —
 * lcg_mul_id, lcg_add_id, lcg_mod_id, step_u, seed_ok_u, range_u,
 * bool_u, byte_u, ready_u, batch_id_8520). Unique
 * gj_prng_seed_ok_u_8515 surface only; no multi-def.
 * Distinct from gj_seed_ok_u_7977 (batch7977). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8515_marker[] = "libcgj-batch8515";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8515_prng_seed_ok(uint32_t u32Seed)
{
	/* Catalog: every uint32 seed is valid for soft LCG PRNG. */
	(void)u32Seed;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prng_seed_ok_u_8515 - 1 if seed is acceptable for soft PRNG.
 *
 * seed: candidate PRNG seed value
 *
 * Always returns 1 (soft catalog: all seeds are ok). Does not call
 * srand(3). No parent wires.
 */
uint32_t
gj_prng_seed_ok_u_8515(uint32_t u32Seed)
{
	(void)NULL;
	return b8515_prng_seed_ok(u32Seed);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prng_seed_ok_u_8515(uint32_t u32Seed)
    __attribute__((alias("gj_prng_seed_ok_u_8515")));
