/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2158: non-atomic u32 compare-and-swap hint.
 *
 * Surface (unique symbols):
 *   int gj_atomic_u32_cas_hint(volatile uint32_t *p,
 *                              uint32_t *expected,
 *                              uint32_t desired);
 *     - If *p == *expected, store desired into *p and return 1.
 *       Else write the observed *p into *expected and return 0.
 *       NULL p or expected -> 0. Plain volatile load/compare/store;
 *       NOT an atomic CMPXCHG.
 *   int __gj_atomic_u32_cas_hint  (alias)
 *   __libcgj_batch2158_marker = "libcgj-batch2158"
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: the read-compare-write sequence is
 * not indivisible. Two callers can both observe a match and both store
 * desired. No fence. Cooperative / single-thread harness or external
 * serializer only. Not C11 atomic_compare_exchange_strong.
 *
 * Post-2150 atomics-hint exclusive wave (pure volatile). Unique surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2158_marker[] = "libcgj-batch2158";

/* ---- freestanding helpers ---------------------------------------------- */

/* Equality of two u32 words as a 0/1 int. */
static int
b2158_eq(uint32_t u32A, uint32_t u32B)
{
	if (u32A == u32B) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_u32_cas_hint - cooperative compare-and-swap on a volatile u32.
 *
 * p:        pointer to volatile word; NULL -> 0.
 * expected: in/out expected value; on failure updated to observed *p.
 *           NULL -> 0.
 * desired:  value stored when the compare matches.
 *
 * Returns 1 on successful swap, 0 on mismatch or bad args.
 */
int
gj_atomic_u32_cas_hint(volatile uint32_t *pWord, uint32_t *pExpected,
                       uint32_t u32Desired)
{
	uint32_t u32Cur;

	(void)NULL;

	if (pWord == NULL || pExpected == NULL) {
		return 0;
	}
	u32Cur = *pWord;
	if (b2158_eq(u32Cur, *pExpected) == 0) {
		*pExpected = u32Cur;
		return 0;
	}
	*pWord = u32Desired;
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_atomic_u32_cas_hint(volatile uint32_t *pWord, uint32_t *pExpected,
                             uint32_t u32Desired)
    __attribute__((alias("gj_atomic_u32_cas_hint")));
