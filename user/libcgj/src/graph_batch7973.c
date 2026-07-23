/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7973: rand ANSI LCG multiplier id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rand_lcg_mult_id_7973(void);
 *     - Return soft ANSI C rand LCG multiplier (1103515245).
 *   uint32_t __gj_rand_lcg_mult_id_7973  (alias)
 *   __libcgj_batch7973_marker = "libcgj-batch7973"
 *
 * Exclusive continuum CREATE-ONLY (7971-7980: rand/srand seed stubs —
 * rand_max_id, srand_default_id, lcg_mult_id, lcg_incr_id, seed_is_zero,
 * seed_is_default, seed_ok, seed_lcg_step, seed_errorish, batch_id_7980).
 * Unique gj_rand_lcg_mult_id_7973 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7973_marker[] = "libcgj-batch7973";

/* Soft ANSI C example LCG multiplier (ISO C catalog constant). */
#define B7973_LCG_MULT ((uint32_t)1103515245u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7973_rand_lcg_mult_id(void)
{
	return B7973_LCG_MULT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rand_lcg_mult_id_7973 - soft ANSI LCG multiplier constant.
 *
 * Always returns 1103515245. Catalog id only; does not call rand(3).
 * No parent wires.
 */
uint32_t
gj_rand_lcg_mult_id_7973(void)
{
	(void)NULL;
	return b7973_rand_lcg_mult_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rand_lcg_mult_id_7973(void)
    __attribute__((alias("gj_rand_lcg_mult_id_7973")));
