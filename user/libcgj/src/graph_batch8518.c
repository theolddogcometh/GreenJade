/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8518: PRNG byte extract stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prng_byte_u_8518(uint32_t state);
 *     - Return soft byte from PRNG state (low 8 bits, 0..255).
 *   uint32_t __gj_prng_byte_u_8518  (alias)
 *   __libcgj_batch8518_marker = "libcgj-batch8518"
 *
 * Exclusive continuum CREATE-ONLY (8511-8520: prng LCG stubs integer —
 * lcg_mul_id, lcg_add_id, lcg_mod_id, step_u, seed_ok_u, range_u,
 * bool_u, byte_u, ready_u, batch_id_8520). Unique
 * gj_prng_byte_u_8518 surface only; no multi-def.
 * Soft catalog byte extract only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8518_marker[] = "libcgj-batch8518";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8518_prng_byte(uint32_t u32State)
{
	return u32State & 0xffu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prng_byte_u_8518 - soft byte extract from PRNG state word.
 *
 * state: current PRNG state / raw word
 *
 * Returns state & 0xff (0..255). Pure integer catalog extract; does not
 * call rand(3). No parent wires.
 */
uint32_t
gj_prng_byte_u_8518(uint32_t u32State)
{
	(void)NULL;
	return b8518_prng_byte(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prng_byte_u_8518(uint32_t u32State)
    __attribute__((alias("gj_prng_byte_u_8518")));
