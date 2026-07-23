/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8519: PRNG ready tag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prng_ready_u_8519(void);
 *     - Return 1 (soft PRNG LCG catalog readiness complete).
 *   uint32_t __gj_prng_ready_u_8519  (alias)
 *   __libcgj_batch8519_marker = "libcgj-batch8519"
 *
 * Exclusive continuum CREATE-ONLY (8511-8520: prng LCG stubs integer —
 * lcg_mul_id, lcg_add_id, lcg_mod_id, step_u, seed_ok_u, range_u,
 * bool_u, byte_u, ready_u, batch_id_8520). Unique
 * gj_prng_ready_u_8519 surface only; no multi-def.
 * Soft compile-time readiness tag only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8519_marker[] = "libcgj-batch8519";

/* Soft PRNG LCG catalog readiness lamp. */
#define B8519_PRNG_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8519_prng_ready(void)
{
	return B8519_PRNG_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prng_ready_u_8519 - report soft PRNG LCG catalog readiness.
 *
 * Always returns 1 (ready). Link-time presence of this symbol tags the
 * prng LCG stub wave. Does not call libc. No parent wires.
 */
uint32_t
gj_prng_ready_u_8519(void)
{
	(void)NULL;
	return b8519_prng_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prng_ready_u_8519(void)
    __attribute__((alias("gj_prng_ready_u_8519")));
