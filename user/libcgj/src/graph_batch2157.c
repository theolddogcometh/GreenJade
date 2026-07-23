/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2157: non-atomic u32 fetch-sub hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atomic_u32_sub_hint(volatile uint32_t *p, uint32_t v);
 *     - Fetch-sub style: read *p, write *p - v (mod 2^32), return the
 *       previous value. NULL -> 0 without store.
 *       Plain volatile RMW as separate load+store; NOT atomic XADD.
 *   uint32_t __gj_atomic_u32_sub_hint  (alias)
 *   __libcgj_batch2157_marker = "libcgj-batch2157"
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: load and store are not indivisible.
 * Underflow wraps in uint32_t modular arithmetic by design (matches
 * unsigned C semantics; not a saturating counter). Cooperative /
 * single-thread or external sync only. Not C11 atomic_fetch_sub.
 *
 * Post-2150 atomics-hint exclusive wave (pure volatile). Unique surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2157_marker[] = "libcgj-batch2157";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular u32 difference (wraps naturally in unsigned arithmetic). */
static uint32_t
b2157_diff(uint32_t u32A, uint32_t u32B)
{
	return u32A - u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_u32_sub_hint - cooperative fetch-sub on a volatile u32.
 *
 * p: pointer to volatile word; NULL -> 0.
 * v: subtrahend applied to *p after sampling.
 *
 * Returns the value observed before the sub (0 if p is NULL).
 */
uint32_t
gj_atomic_u32_sub_hint(volatile uint32_t *pWord, uint32_t u32V)
{
	uint32_t u32Prev;

	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	u32Prev = *pWord;
	*pWord = b2157_diff(u32Prev, u32V);
	return u32Prev;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atomic_u32_sub_hint(volatile uint32_t *pWord, uint32_t u32V)
    __attribute__((alias("gj_atomic_u32_sub_hint")));
