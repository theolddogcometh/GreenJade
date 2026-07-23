/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8513: PRNG LCG modulus id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prng_lcg_mod_id_8513(void);
 *     - Return soft ANSI C PRNG LCG modulus (2^31 = 0x80000000).
 *   uint32_t __gj_prng_lcg_mod_id_8513  (alias)
 *   __libcgj_batch8513_marker = "libcgj-batch8513"
 *
 * Exclusive continuum CREATE-ONLY (8511-8520: prng LCG stubs integer —
 * lcg_mul_id, lcg_add_id, lcg_mod_id, step_u, seed_ok_u, range_u,
 * bool_u, byte_u, ready_u, batch_id_8520). Unique
 * gj_prng_lcg_mod_id_8513 surface only; no multi-def.
 * Domain is 2^31 (RAND_MAX+1 catalog). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8513_marker[] = "libcgj-batch8513";

/* Soft ANSI C example LCG modulus (2^31 catalog domain). */
#define B8513_LCG_MOD ((uint32_t)0x80000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8513_prng_lcg_mod_id(void)
{
	return B8513_LCG_MOD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prng_lcg_mod_id_8513 - soft ANSI PRNG LCG modulus constant.
 *
 * Always returns 0x80000000 (2^31). Catalog id only; does not call
 * rand(3). No parent wires.
 */
uint32_t
gj_prng_lcg_mod_id_8513(void)
{
	(void)NULL;
	return b8513_prng_lcg_mod_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prng_lcg_mod_id_8513(void)
    __attribute__((alias("gj_prng_lcg_mod_id_8513")));
