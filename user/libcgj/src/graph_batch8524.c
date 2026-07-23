/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8524: pure unsigned sub atomic-op stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atomic_sub_u_8524(uint32_t a, uint32_t b);
 *     - Return a - b as uint32_t (wrap on underflow). Pure arithmetic;
 *       no memory RMW and not a hardware XADD / C11 atomic_fetch_sub.
 *   uint32_t __gj_atomic_sub_u_8524  (alias)
 *   __libcgj_batch8524_marker = "libcgj-batch8524"
 *
 * Exclusive continuum CREATE-ONLY (8521-8530: atomic op id stubs —
 * atomic_load_id, atomic_store_id, atomic_add_u, atomic_sub_u,
 * atomic_or_u, atomic_and_u, atomic_xor_u, atomic_cas_ok_u,
 * atomic_fence_id, batch_id_8530). Unique gj_atomic_sub_u_8524 surface
 * only; no multi-def. Distinct from gj_atomic_sub_u32_u (batch5124).
 * No real atomics. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8524_marker[] = "libcgj-batch8524";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8524_sub(uint32_t u32A, uint32_t u32B)
{
	return u32A - u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_sub_u_8524 - pure unsigned sub (mod 2^32).
 *
 * a: left operand / minuend
 * b: right operand / subtrahend
 *
 * Returns a - b with wrap. Pure function; no pointers, no fences.
 * No parent wires.
 */
uint32_t
gj_atomic_sub_u_8524(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8524_sub(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atomic_sub_u_8524(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_atomic_sub_u_8524")));
