/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7974: rand ANSI LCG increment id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rand_lcg_incr_id_7974(void);
 *     - Return soft ANSI C rand LCG increment (12345).
 *   uint32_t __gj_rand_lcg_incr_id_7974  (alias)
 *   __libcgj_batch7974_marker = "libcgj-batch7974"
 *
 * Exclusive continuum CREATE-ONLY (7971-7980: rand/srand seed stubs —
 * rand_max_id, srand_default_id, lcg_mult_id, lcg_incr_id, seed_is_zero,
 * seed_is_default, seed_ok, seed_lcg_step, seed_errorish, batch_id_7980).
 * Unique gj_rand_lcg_incr_id_7974 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7974_marker[] = "libcgj-batch7974";

/* Soft ANSI C example LCG increment (ISO C catalog constant). */
#define B7974_LCG_INCR ((uint32_t)12345u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7974_rand_lcg_incr_id(void)
{
	return B7974_LCG_INCR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rand_lcg_incr_id_7974 - soft ANSI LCG increment constant.
 *
 * Always returns 12345. Catalog id only; does not call rand(3).
 * No parent wires.
 */
uint32_t
gj_rand_lcg_incr_id_7974(void)
{
	(void)NULL;
	return b7974_rand_lcg_incr_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rand_lcg_incr_id_7974(void)
    __attribute__((alias("gj_rand_lcg_incr_id_7974")));
