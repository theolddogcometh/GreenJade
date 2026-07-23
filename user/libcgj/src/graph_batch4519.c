/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4519: atomic-less plain soft compare-and-swap.
 *
 * Surface (unique symbols):
 *   int gj_counter_cas_u2(uint32_t *p, uint32_t exp, uint32_t neu);
 *     - If *p == exp, store neu into *p and return 1. Else leave *p
 *       unchanged and return 0. NULL p -> 0. Plain load/compare/store;
 *       NOT an atomic CMPXCHG (soft CAS).
 *   int __gj_counter_cas_u2  (alias)
 *   __libcgj_batch4519_marker = "libcgj-batch4519"
 *
 * Exclusive continuum CREATE-ONLY (4511-4520: atomic-less counters
 * unique). Distinct from gj_counter_cas_u (batch2548: volatile CAS with
 * expected in/out pointer) — rename on collision; unique
 * gj_counter_cas_u2 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4519_marker[] = "libcgj-batch4519";

/* ---- freestanding helpers ---------------------------------------------- */

/* Equality of two u32 words as a 0/1 int. */
static int
b4519_eq(uint32_t u32A, uint32_t u32B)
{
	if (u32A == u32B) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_counter_cas_u2 - atomic-less soft compare-and-swap on a plain u32.
 *
 * p:   pointer to counter word; NULL -> 0.
 * exp: expected current value (by value).
 * neu: value stored when the compare matches.
 *
 * Returns 1 on successful swap, 0 on mismatch or NULL p.
 * Self-contained; no parent wires.
 */
int
gj_counter_cas_u2(uint32_t *pWord, uint32_t u32Exp, uint32_t u32Neu)
{
	(void)NULL;

	if (pWord == NULL) {
		return 0;
	}
	if (b4519_eq(*pWord, u32Exp) == 0) {
		return 0;
	}
	*pWord = u32Neu;
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_counter_cas_u2(uint32_t *pWord, uint32_t u32Exp, uint32_t u32Neu)
    __attribute__((alias("gj_counter_cas_u2")));
