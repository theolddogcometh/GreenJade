/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8525: pure unsigned OR atomic-op stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atomic_or_u_8525(uint32_t a, uint32_t b);
 *     - Return a | b as uint32_t. Pure bitwise OR; no memory RMW and
 *       not a hardware LOCK OR / C11 atomic_fetch_or.
 *   uint32_t __gj_atomic_or_u_8525  (alias)
 *   __libcgj_batch8525_marker = "libcgj-batch8525"
 *
 * Exclusive continuum CREATE-ONLY (8521-8530: atomic op id stubs —
 * atomic_load_id, atomic_store_id, atomic_add_u, atomic_sub_u,
 * atomic_or_u, atomic_and_u, atomic_xor_u, atomic_cas_ok_u,
 * atomic_fence_id, batch_id_8530). Unique gj_atomic_or_u_8525 surface
 * only; no multi-def. Distinct from gj_atomic_or_u32_u (batch5125).
 * No real atomics. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8525_marker[] = "libcgj-batch8525";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8525_or(uint32_t u32A, uint32_t u32B)
{
	return u32A | u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_or_u_8525 - pure unsigned bitwise OR.
 *
 * a: left operand
 * b: right operand / mask
 *
 * Returns a | b. Pure function; no pointers, no fences.
 * No parent wires.
 */
uint32_t
gj_atomic_or_u_8525(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8525_or(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atomic_or_u_8525(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_atomic_or_u_8525")));
