/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2155: non-atomic u32 store hint.
 *
 * Surface (unique symbols):
 *   void gj_atomic_u32_store_hint(volatile uint32_t *p, uint32_t v);
 *     - Store v into *p via a plain volatile write. NULL is a no-op.
 *       NOT a release/relaxed atomic store; no fence.
 *   void __gj_atomic_u32_store_hint  (alias)
 *   __libcgj_batch2155_marker = "libcgj-batch2155"
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: pure volatile write. No release
 * ordering, no multi-writer arbitration. Cooperative / single-thread
 * or external sync only. Not C11 atomic_store.
 *
 * Post-2150 atomics-hint exclusive wave (pure volatile). Unique surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2155_marker[] = "libcgj-batch2155";

/* ---- freestanding helpers ---------------------------------------------- */

/* Identity passthrough keeps a static helper in this TU. */
static uint32_t
b2155_id(uint32_t u32V)
{
	return u32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_u32_store_hint - cooperative volatile u32 store.
 *
 * p: pointer to volatile word; NULL -> no-op.
 * v: value written to *p.
 */
void
gj_atomic_u32_store_hint(volatile uint32_t *pWord, uint32_t u32V)
{
	(void)NULL;

	if (pWord == NULL) {
		return;
	}
	*pWord = b2155_id(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_atomic_u32_store_hint(volatile uint32_t *pWord, uint32_t u32V)
    __attribute__((alias("gj_atomic_u32_store_hint")));
