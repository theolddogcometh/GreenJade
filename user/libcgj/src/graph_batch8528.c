/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8528: pure CAS-ok compare atomic-op stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atomic_cas_ok_u_8528(uint32_t cur, uint32_t exp);
 *     - Return 1 if cur == exp (soft CAS would succeed), else 0.
 *       Pure equality predicate; no memory RMW and not CMPXCHG / C11
 *       atomic_compare_exchange.
 *   uint32_t __gj_atomic_cas_ok_u_8528  (alias)
 *   __libcgj_batch8528_marker = "libcgj-batch8528"
 *
 * Exclusive continuum CREATE-ONLY (8521-8530: atomic op id stubs —
 * atomic_load_id, atomic_store_id, atomic_add_u, atomic_sub_u,
 * atomic_or_u, atomic_and_u, atomic_xor_u, atomic_cas_ok_u,
 * atomic_fence_id, batch_id_8530). Unique gj_atomic_cas_ok_u_8528
 * surface only; no multi-def. Distinct from gj_atomic_cas_u32_u
 * (batch5128) and gj_atomic_u32_cas_hint (batch2158). No real atomics.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8528_marker[] = "libcgj-batch8528";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8528_cas_ok(uint32_t u32Cur, uint32_t u32Exp)
{
	if (u32Cur == u32Exp) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_cas_ok_u_8528 - pure soft CAS success predicate.
 *
 * cur: observed current value
 * exp: expected value for a successful compare-and-swap
 *
 * Returns 1 when cur == exp, else 0. Pure function; no store, no fence.
 * No parent wires.
 */
uint32_t
gj_atomic_cas_ok_u_8528(uint32_t u32Cur, uint32_t u32Exp)
{
	(void)NULL;
	return b8528_cas_ok(u32Cur, u32Exp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atomic_cas_ok_u_8528(uint32_t u32Cur, uint32_t u32Exp)
    __attribute__((alias("gj_atomic_cas_ok_u_8528")));
