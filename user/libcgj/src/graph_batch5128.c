/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5128: soft non-hardware u32 compare-and-swap.
 *
 * Surface (unique symbols):
 *   int gj_atomic_cas_u32_u(uint32_t *p, uint32_t exp, uint32_t neu);
 *     - If *p == exp, store neu into *p and return 1. Else return 0
 *       without updating *p. NULL p -> 0. Plain load/compare/store;
 *       NOT hardware CMPXCHG and NOT C11 atomic_compare_exchange.
 *   int __gj_atomic_cas_u32_u  (alias)
 *   __libcgj_batch5128_marker = "libcgj-batch5128"
 *
 * Soft non-hardware atomics. Cooperative / single-thread or externally
 * serialized use only. No fence, no lock, no __atomic builtins.
 *
 * Exclusive continuum CREATE-ONLY (5121-5130: atomic_load_u32_u,
 * atomic_store_u32_u, atomic_add_u32_u, atomic_sub_u32_u,
 * atomic_or_u32_u, atomic_and_u32_u, atomic_xor_u32_u, atomic_cas_u32_u,
 * atomic_xchg_u32_u, batch_id_5130). Distinct from gj_atomic_u32_cas_hint
 * (batch2158, in/out expected pointer) — unique _u soft surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5128_marker[] = "libcgj-batch5128";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5128_eq(uint32_t u32A, uint32_t u32B)
{
	if (u32A == u32B) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_cas_u32_u - soft non-hardware u32 compare-and-swap.
 *
 * p:   pointer to word; NULL -> 0.
 * exp: expected current value.
 * neu: value stored when the compare matches.
 *
 * Returns 1 if swapped, 0 on mismatch or NULL p.
 * Self-contained; no parent wires.
 */
int
gj_atomic_cas_u32_u(uint32_t *pWord, uint32_t u32Exp, uint32_t u32Neu)
{
	(void)NULL;

	if (pWord == NULL) {
		return 0;
	}
	if (b5128_eq(*pWord, u32Exp) == 0) {
		return 0;
	}
	*pWord = u32Neu;
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_atomic_cas_u32_u(uint32_t *pWord, uint32_t u32Exp, uint32_t u32Neu)
    __attribute__((alias("gj_atomic_cas_u32_u")));
