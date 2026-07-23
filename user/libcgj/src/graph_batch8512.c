/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8512: PRNG LCG addend id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prng_lcg_add_id_8512(void);
 *     - Return soft ANSI C PRNG LCG addend / increment (12345).
 *   uint32_t __gj_prng_lcg_add_id_8512  (alias)
 *   __libcgj_batch8512_marker = "libcgj-batch8512"
 *
 * Exclusive continuum CREATE-ONLY (8511-8520: prng LCG stubs integer —
 * lcg_mul_id, lcg_add_id, lcg_mod_id, step_u, seed_ok_u, range_u,
 * bool_u, byte_u, ready_u, batch_id_8520). Unique
 * gj_prng_lcg_add_id_8512 surface only; no multi-def.
 * Distinct from gj_rand_lcg_incr_id_7974 (batch7974). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8512_marker[] = "libcgj-batch8512";

/* Soft ANSI C example LCG addend (ISO C catalog constant). */
#define B8512_LCG_ADD ((uint32_t)12345u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8512_prng_lcg_add_id(void)
{
	return B8512_LCG_ADD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prng_lcg_add_id_8512 - soft ANSI PRNG LCG addend constant.
 *
 * Always returns 12345. Catalog id only; does not call rand(3).
 * No parent wires.
 */
uint32_t
gj_prng_lcg_add_id_8512(void)
{
	(void)NULL;
	return b8512_prng_lcg_add_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prng_lcg_add_id_8512(void)
    __attribute__((alias("gj_prng_lcg_add_id_8512")));
