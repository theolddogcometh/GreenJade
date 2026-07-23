/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8516: PRNG range map stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prng_range_u_8516(uint32_t state, uint32_t bound);
 *     - Map state into [0, bound) via soft remainder; 0 if bound is 0.
 *   uint32_t __gj_prng_range_u_8516  (alias)
 *   __libcgj_batch8516_marker = "libcgj-batch8516"
 *
 * Exclusive continuum CREATE-ONLY (8511-8520: prng LCG stubs integer —
 * lcg_mul_id, lcg_add_id, lcg_mod_id, step_u, seed_ok_u, range_u,
 * bool_u, byte_u, ready_u, batch_id_8520). Unique
 * gj_prng_range_u_8516 surface only; no multi-def.
 * Soft catalog remainder only (not CSPRNG / not Lemire). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8516_marker[] = "libcgj-batch8516";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8516_prng_range(uint32_t u32State, uint32_t u32Bound)
{
	if (u32Bound == 0u)
		return 0u;
	return u32State % u32Bound;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prng_range_u_8516 - soft map PRNG state into [0, bound).
 *
 * state: current PRNG state / raw word
 * bound: exclusive upper bound (0 → return 0)
 *
 * Returns state % bound when bound != 0, else 0. Pure integer catalog
 * map; does not call rand(3). No parent wires.
 */
uint32_t
gj_prng_range_u_8516(uint32_t u32State, uint32_t u32Bound)
{
	(void)NULL;
	return b8516_prng_range(u32State, u32Bound);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prng_range_u_8516(uint32_t u32State, uint32_t u32Bound)
    __attribute__((alias("gj_prng_range_u_8516")));
