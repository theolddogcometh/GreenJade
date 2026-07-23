/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5122: soft non-hardware u32 atomic store.
 *
 * Surface (unique symbols):
 *   void gj_atomic_store_u32_u(uint32_t *p, uint32_t v);
 *     - Soft store of v into *p. NULL is a no-op. Plain write; NOT a
 *       hardware atomic store and NOT a C11 atomic_store.
 *   void __gj_atomic_store_u32_u  (alias)
 *   __libcgj_batch5122_marker = "libcgj-batch5122"
 *
 * Soft non-hardware atomics. Cooperative / single-thread or externally
 * serialized use only. No fence, no lock, no __atomic builtins.
 *
 * Exclusive continuum CREATE-ONLY (5121-5130: atomic_load_u32_u,
 * atomic_store_u32_u, atomic_add_u32_u, atomic_sub_u32_u,
 * atomic_or_u32_u, atomic_and_u32_u, atomic_xor_u32_u, atomic_cas_u32_u,
 * atomic_xchg_u32_u, batch_id_5130). Distinct from gj_atomic_*_hint
 * (batch215x) — unique _u soft surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5122_marker[] = "libcgj-batch5122";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5122_id(uint32_t u32V)
{
	return u32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_store_u32_u - soft non-hardware u32 store.
 *
 * p: pointer to word; NULL -> no-op.
 * v: value written to *p.
 *
 * Self-contained; no parent wires.
 */
void
gj_atomic_store_u32_u(uint32_t *pWord, uint32_t u32V)
{
	(void)NULL;

	if (pWord == NULL) {
		return;
	}
	*pWord = b5122_id(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_atomic_store_u32_u(uint32_t *pWord, uint32_t u32V)
    __attribute__((alias("gj_atomic_store_u32_u")));
