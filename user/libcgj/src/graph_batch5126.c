/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5126: soft non-hardware u32 atomic AND.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atomic_and_u32_u(uint32_t *p, uint32_t v);
 *     - Soft RMW: *p = *p & v; returns the NEW value. NULL -> 0 without
 *       store. Plain load+store; NOT hardware atomic AND and NOT C11
 *       atomic_fetch_and.
 *   uint32_t __gj_atomic_and_u32_u  (alias)
 *   __libcgj_batch5126_marker = "libcgj-batch5126"
 *
 * Soft non-hardware atomics. Cooperative / single-thread or externally
 * serialized use only. No fence, no lock, no __atomic builtins.
 *
 * Exclusive continuum CREATE-ONLY (5121-5130: atomic_load_u32_u,
 * atomic_store_u32_u, atomic_add_u32_u, atomic_sub_u32_u,
 * atomic_or_u32_u, atomic_and_u32_u, atomic_xor_u32_u, atomic_cas_u32_u,
 * atomic_xchg_u32_u, batch_id_5130). Unique _u soft surface; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5126_marker[] = "libcgj-batch5126";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5126_and(uint32_t u32A, uint32_t u32B)
{
	return u32A & u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_and_u32_u - soft non-hardware u32 bitwise AND (returns new).
 *
 * p: pointer to word; NULL -> 0.
 * v: mask AND'd into *p.
 *
 * Returns the value written after the AND (0 if p is NULL).
 * Self-contained; no parent wires.
 */
uint32_t
gj_atomic_and_u32_u(uint32_t *pWord, uint32_t u32V)
{
	uint32_t u32New;

	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	u32New = b5126_and(*pWord, u32V);
	*pWord = u32New;
	return u32New;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atomic_and_u32_u(uint32_t *pWord, uint32_t u32V)
    __attribute__((alias("gj_atomic_and_u32_u")));
