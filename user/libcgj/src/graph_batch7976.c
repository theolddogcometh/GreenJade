/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7976: srand seed is-default presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seed_is_default_u_7976(uint32_t seed);
 *     - Return 1 if seed equals soft default (1), else 0.
 *   uint32_t __gj_seed_is_default_u_7976  (alias)
 *   __libcgj_batch7976_marker = "libcgj-batch7976"
 *
 * Exclusive continuum CREATE-ONLY (7971-7980: rand/srand seed stubs —
 * rand_max_id, srand_default_id, lcg_mult_id, lcg_incr_id, seed_is_zero,
 * seed_is_default, seed_ok, seed_lcg_step, seed_errorish, batch_id_7980).
 * Unique gj_seed_is_default_u_7976 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7976_marker[] = "libcgj-batch7976";

/* Soft default srand seed (1). */
#define B7976_SRAND_DEFAULT_SEED ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7976_seed_is_default(uint32_t u32Seed)
{
	return (u32Seed == B7976_SRAND_DEFAULT_SEED) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seed_is_default_u_7976 - 1 if seed is the soft default (1).
 *
 * seed: candidate srand seed value
 *
 * Returns 1 when seed equals 1, else 0. Soft catalog test; does not
 * call srand(3). No parent wires.
 */
uint32_t
gj_seed_is_default_u_7976(uint32_t u32Seed)
{
	(void)NULL;
	return b7976_seed_is_default(u32Seed);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seed_is_default_u_7976(uint32_t u32Seed)
    __attribute__((alias("gj_seed_is_default_u_7976")));
