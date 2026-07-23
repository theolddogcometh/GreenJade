/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7972: srand default seed id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_srand_default_seed_id_7972(void);
 *     - Return soft default srand seed constant (1).
 *   uint32_t __gj_srand_default_seed_id_7972  (alias)
 *   __libcgj_batch7972_marker = "libcgj-batch7972"
 *
 * Exclusive continuum CREATE-ONLY (7971-7980: rand/srand seed stubs —
 * rand_max_id, srand_default_id, lcg_mult_id, lcg_incr_id, seed_is_zero,
 * seed_is_default, seed_ok, seed_lcg_step, seed_errorish, batch_id_7980).
 * Unique gj_srand_default_seed_id_7972 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7972_marker[] = "libcgj-batch7972";

/* Soft default seed used when srand is never called (common catalog 1). */
#define B7972_SRAND_DEFAULT_SEED ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7972_srand_default_seed_id(void)
{
	return B7972_SRAND_DEFAULT_SEED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_srand_default_seed_id_7972 - soft default srand seed constant.
 *
 * Always returns 1. Catalog id only; does not call srand(3).
 * No parent wires.
 */
uint32_t
gj_srand_default_seed_id_7972(void)
{
	(void)NULL;
	return b7972_srand_default_seed_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_srand_default_seed_id_7972(void)
    __attribute__((alias("gj_srand_default_seed_id_7972")));
