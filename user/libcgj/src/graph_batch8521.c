/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8521: soft atomic-load op-kind id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atomic_load_id_8521(void);
 *     - Return soft atomic-op catalog id for load (1).
 *   uint32_t __gj_atomic_load_id_8521  (alias)
 *   __libcgj_batch8521_marker = "libcgj-batch8521"
 *
 * Exclusive continuum CREATE-ONLY (8521-8530: atomic op id stubs —
 * atomic_load_id, atomic_store_id, atomic_add_u, atomic_sub_u,
 * atomic_or_u, atomic_and_u, atomic_xor_u, atomic_cas_ok_u,
 * atomic_fence_id, batch_id_8530). Unique gj_atomic_load_id_8521
 * surface only; no multi-def. No real atomics (no __atomic / C11
 * atomic). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8521_marker[] = "libcgj-batch8521";

/* Soft atomic-op catalog: load kind (local constant only). */
#define B8521_ATOMIC_OP_LOAD  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8521_load_id(void)
{
	return B8521_ATOMIC_OP_LOAD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_load_id_8521 - soft atomic-load op-kind catalog id.
 *
 * Always returns 1 (load). Pure data tag; does not load memory or issue
 * a fence. No parent wires.
 */
uint32_t
gj_atomic_load_id_8521(void)
{
	(void)NULL;
	return b8521_load_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atomic_load_id_8521(void)
    __attribute__((alias("gj_atomic_load_id_8521")));
