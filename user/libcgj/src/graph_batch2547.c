/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2547: non-atomic cooperative counter add.
 *
 * Surface (unique symbols):
 *   uint32_t gj_counter_add_u(volatile uint32_t *p, uint32_t v);
 *     - Fetch-add style: read *p, write *p + v (mod 2^32), return the
 *       previous value. NULL -> 0 without store.
 *       Plain volatile RMW as separate load+store; NOT atomic XADD.
 *   uint32_t __gj_counter_add_u  (alias)
 *   __libcgj_batch2547_marker = "libcgj-batch2547"
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: load and store are not indivisible.
 * Concurrent cooperative callers can lose updates. No fence. For
 * single-thread counters or externally serialized RMW only. Not C11
 * atomic_fetch_add.
 *
 * Sync exclusive pure-volatile hints wave (2541-2550). Unique surface.
 * Distinct from gj_atomic_u32_add_hint (batch2156).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2547_marker[] = "libcgj-batch2547";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular u32 sum (wraps naturally in unsigned arithmetic). */
static uint32_t
b2547_sum(uint32_t u32A, uint32_t u32B)
{
	return u32A + u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_counter_add_u - cooperative fetch-add on a volatile u32.
 *
 * p: pointer to volatile word; NULL -> 0.
 * v: addend applied to *p after sampling.
 *
 * Returns the value observed before the add (0 if p is NULL).
 */
uint32_t
gj_counter_add_u(volatile uint32_t *pWord, uint32_t u32V)
{
	uint32_t u32Prev;

	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	u32Prev = *pWord;
	*pWord = b2547_sum(u32Prev, u32V);
	return u32Prev;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_counter_add_u(volatile uint32_t *pWord, uint32_t u32V)
    __attribute__((alias("gj_counter_add_u")));
