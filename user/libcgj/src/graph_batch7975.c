/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7975: srand seed is-zero presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seed_is_zero_u_7975(uint32_t seed);
 *     - Return 1 if seed is 0, else 0.
 *   uint32_t __gj_seed_is_zero_u_7975  (alias)
 *   __libcgj_batch7975_marker = "libcgj-batch7975"
 *
 * Exclusive continuum CREATE-ONLY (7971-7980: rand/srand seed stubs —
 * rand_max_id, srand_default_id, lcg_mult_id, lcg_incr_id, seed_is_zero,
 * seed_is_default, seed_ok, seed_lcg_step, seed_errorish, batch_id_7980).
 * Unique gj_seed_is_zero_u_7975 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7975_marker[] = "libcgj-batch7975";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7975_seed_is_zero(uint32_t u32Seed)
{
	return (u32Seed == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seed_is_zero_u_7975 - 1 if seed is zero.
 *
 * seed: candidate srand seed value
 *
 * Returns 1 when seed is 0, else 0. Soft catalog test; does not call
 * srand(3). No parent wires.
 */
uint32_t
gj_seed_is_zero_u_7975(uint32_t u32Seed)
{
	(void)NULL;
	return b7975_seed_is_zero(u32Seed);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seed_is_zero_u_7975(uint32_t u32Seed)
    __attribute__((alias("gj_seed_is_zero_u_7975")));
