/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8511: PRNG LCG multiplier id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prng_lcg_mul_id_8511(void);
 *     - Return soft ANSI C PRNG LCG multiplier (1103515245).
 *   uint32_t __gj_prng_lcg_mul_id_8511  (alias)
 *   __libcgj_batch8511_marker = "libcgj-batch8511"
 *
 * Exclusive continuum CREATE-ONLY (8511-8520: prng LCG stubs integer —
 * lcg_mul_id, lcg_add_id, lcg_mod_id, step_u, seed_ok_u, range_u,
 * bool_u, byte_u, ready_u, batch_id_8520). Unique
 * gj_prng_lcg_mul_id_8511 surface only; no multi-def.
 * Distinct from gj_rand_lcg_mult_id_7973 (batch7973). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8511_marker[] = "libcgj-batch8511";

/* Soft ANSI C example LCG multiplier (ISO C catalog constant). */
#define B8511_LCG_MUL ((uint32_t)1103515245u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8511_prng_lcg_mul_id(void)
{
	return B8511_LCG_MUL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prng_lcg_mul_id_8511 - soft ANSI PRNG LCG multiplier constant.
 *
 * Always returns 1103515245. Catalog id only; does not call rand(3).
 * No parent wires.
 */
uint32_t
gj_prng_lcg_mul_id_8511(void)
{
	(void)NULL;
	return b8511_prng_lcg_mul_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prng_lcg_mul_id_8511(void)
    __attribute__((alias("gj_prng_lcg_mul_id_8511")));
