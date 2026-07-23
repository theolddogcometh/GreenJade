/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7971: rand RAND_MAX id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rand_max_id_7971(void);
 *     - Return soft RAND_MAX constant (0x7fffffff).
 *   uint32_t __gj_rand_max_id_7971  (alias)
 *   __libcgj_batch7971_marker = "libcgj-batch7971"
 *
 * Exclusive continuum CREATE-ONLY (7971-7980: rand/srand seed stubs —
 * rand_max_id, srand_default_id, lcg_mult_id, lcg_incr_id, seed_is_zero,
 * seed_is_default, seed_ok, seed_lcg_step, seed_errorish, batch_id_7980).
 * Unique gj_rand_max_id_7971 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7971_marker[] = "libcgj-batch7971";

/* Soft RAND_MAX catalog value (common glibc/ISO C 31-bit max). */
#define B7971_RAND_MAX ((uint32_t)0x7fffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7971_rand_max_id(void)
{
	return B7971_RAND_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rand_max_id_7971 - soft RAND_MAX constant.
 *
 * Always returns 0x7fffffff. Catalog id only; does not call rand(3).
 * No parent wires.
 */
uint32_t
gj_rand_max_id_7971(void)
{
	(void)NULL;
	return b7971_rand_max_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rand_max_id_7971(void)
    __attribute__((alias("gj_rand_max_id_7971")));
