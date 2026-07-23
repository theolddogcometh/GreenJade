/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2159: non-atomic u64 load hint.
 *
 * Surface (unique symbols):
 *   uint64_t gj_atomic_u64_load_hint(const volatile uint64_t *p);
 *     - Return *p via a plain volatile load. NULL -> 0.
 *       NOT an acquire atomic load; on 32-bit targets a 64-bit volatile
 *       load may tear - this TU is freestanding x86_64 (-msse2) oriented.
 *   uint64_t __gj_atomic_u64_load_hint  (alias)
 *   __libcgj_batch2159_marker = "libcgj-batch2159"
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: pure volatile read, no fence, no
 * LOCK. Cooperative / single-thread or external sync only. Not C11
 * atomic_load on atomic_ullong / uint64_t atomic.
 *
 * Post-2150 atomics-hint exclusive wave (pure volatile). Unique surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2159_marker[] = "libcgj-batch2159";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2159_zero(void)
{
	return 0ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_u64_load_hint - cooperative volatile u64 load.
 *
 * p: pointer to volatile 64-bit word; NULL -> 0.
 *
 * Returns the sampled value (or 0 on NULL).
 */
uint64_t
gj_atomic_u64_load_hint(const volatile uint64_t *pWord)
{
	(void)NULL;

	if (pWord == NULL) {
		return b2159_zero();
	}
	return *pWord;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_atomic_u64_load_hint(const volatile uint64_t *pWord)
    __attribute__((alias("gj_atomic_u64_load_hint")));
