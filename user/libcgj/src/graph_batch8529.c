/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8529: soft atomic-fence op-kind id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atomic_fence_id_8529(void);
 *     - Return soft atomic-op catalog id for fence (3).
 *   uint32_t __gj_atomic_fence_id_8529  (alias)
 *   __libcgj_batch8529_marker = "libcgj-batch8529"
 *
 * Exclusive continuum CREATE-ONLY (8521-8530: atomic op id stubs —
 * atomic_load_id, atomic_store_id, atomic_add_u, atomic_sub_u,
 * atomic_or_u, atomic_and_u, atomic_xor_u, atomic_cas_ok_u,
 * atomic_fence_id, batch_id_8530). Unique gj_atomic_fence_id_8529
 * surface only; no multi-def. No real atomics (no mfence / C11
 * atomic_thread_fence). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8529_marker[] = "libcgj-batch8529";

/* Soft atomic-op catalog: fence kind (local constant only). */
#define B8529_ATOMIC_OP_FENCE  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8529_fence_id(void)
{
	return B8529_ATOMIC_OP_FENCE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_fence_id_8529 - soft atomic-fence op-kind catalog id.
 *
 * Always returns 3 (fence). Pure data tag; does not emit a memory
 * barrier or compiler fence. No parent wires.
 */
uint32_t
gj_atomic_fence_id_8529(void)
{
	(void)NULL;
	return b8529_fence_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atomic_fence_id_8529(void)
    __attribute__((alias("gj_atomic_fence_id_8529")));
