/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8522: soft atomic-store op-kind id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atomic_store_id_8522(void);
 *     - Return soft atomic-op catalog id for store (2).
 *   uint32_t __gj_atomic_store_id_8522  (alias)
 *   __libcgj_batch8522_marker = "libcgj-batch8522"
 *
 * Exclusive continuum CREATE-ONLY (8521-8530: atomic op id stubs —
 * atomic_load_id, atomic_store_id, atomic_add_u, atomic_sub_u,
 * atomic_or_u, atomic_and_u, atomic_xor_u, atomic_cas_ok_u,
 * atomic_fence_id, batch_id_8530). Unique gj_atomic_store_id_8522
 * surface only; no multi-def. No real atomics (no __atomic / C11
 * atomic). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8522_marker[] = "libcgj-batch8522";

/* Soft atomic-op catalog: store kind (local constant only). */
#define B8522_ATOMIC_OP_STORE  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8522_store_id(void)
{
	return B8522_ATOMIC_OP_STORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_store_id_8522 - soft atomic-store op-kind catalog id.
 *
 * Always returns 2 (store). Pure data tag; does not store memory or
 * issue a fence. No parent wires.
 */
uint32_t
gj_atomic_store_id_8522(void)
{
	(void)NULL;
	return b8522_store_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atomic_store_id_8522(void)
    __attribute__((alias("gj_atomic_store_id_8522")));
