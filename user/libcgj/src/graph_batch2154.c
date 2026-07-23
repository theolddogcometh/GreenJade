/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2154: non-atomic u32 load hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atomic_u32_load_hint(const volatile uint32_t *p);
 *     - Return *p via a plain volatile load. NULL -> 0.
 *       NOT an acquire/relaxed atomic load; no fence.
 *   uint32_t __gj_atomic_u32_load_hint  (alias)
 *   __libcgj_batch2154_marker = "libcgj-batch2154"
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: pure volatile read. Compiler will
 * not cache across volatile, but there is no hardware acquire, no
 * LOCK, and no multi-core coherence guarantee beyond the platform
 * volatile model. Cooperative / single-thread or external sync only.
 * Not C11 atomic_load on atomic_uint.
 *
 * Post-2150 atomics-hint exclusive wave (pure volatile). Unique surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2154_marker[] = "libcgj-batch2154";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2154_zero(void)
{
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_u32_load_hint - cooperative volatile u32 load.
 *
 * p: pointer to volatile word; NULL -> 0.
 *
 * Returns the sampled value (or 0 on NULL).
 */
uint32_t
gj_atomic_u32_load_hint(const volatile uint32_t *pWord)
{
	(void)NULL;

	if (pWord == NULL) {
		return b2154_zero();
	}
	return *pWord;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atomic_u32_load_hint(const volatile uint32_t *pWord)
    __attribute__((alias("gj_atomic_u32_load_hint")));
