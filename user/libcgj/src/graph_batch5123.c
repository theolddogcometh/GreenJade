/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5123: soft non-hardware u32 atomic add.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atomic_add_u32_u(uint32_t *p, uint32_t v);
 *     - Soft RMW: *p = *p + v (mod 2^32); returns the NEW value.
 *       NULL -> 0 without store. Plain load+store; NOT hardware XADD
 *       and NOT C11 atomic_fetch_add / atomic_add.
 *   uint32_t __gj_atomic_add_u32_u  (alias)
 *   __libcgj_batch5123_marker = "libcgj-batch5123"
 *
 * Soft non-hardware atomics. Cooperative / single-thread or externally
 * serialized use only. No fence, no lock, no __atomic builtins.
 *
 * Exclusive continuum CREATE-ONLY (5121-5130: atomic_load_u32_u,
 * atomic_store_u32_u, atomic_add_u32_u, atomic_sub_u32_u,
 * atomic_or_u32_u, atomic_and_u32_u, atomic_xor_u32_u, atomic_cas_u32_u,
 * atomic_xchg_u32_u, batch_id_5130). Distinct from gj_atomic_u32_add_hint
 * (batch2156, returns previous) — unique _u soft surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5123_marker[] = "libcgj-batch5123";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5123_sum(uint32_t u32A, uint32_t u32B)
{
	return u32A + u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_add_u32_u - soft non-hardware u32 add (returns new).
 *
 * p: pointer to word; NULL -> 0.
 * v: addend applied to *p.
 *
 * Returns the value written after the add (0 if p is NULL).
 * Self-contained; no parent wires.
 */
uint32_t
gj_atomic_add_u32_u(uint32_t *pWord, uint32_t u32V)
{
	uint32_t u32New;

	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	u32New = b5123_sum(*pWord, u32V);
	*pWord = u32New;
	return u32New;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atomic_add_u32_u(uint32_t *pWord, uint32_t u32V)
    __attribute__((alias("gj_atomic_add_u32_u")));
