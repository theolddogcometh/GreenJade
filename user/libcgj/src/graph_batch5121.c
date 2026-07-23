/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5121: soft non-hardware u32 atomic load.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atomic_load_u32_u(const uint32_t *p);
 *     - Soft load of *p. NULL -> 0. Plain read; NOT a hardware atomic
 *       load and NOT a C11 atomic_load.
 *   uint32_t __gj_atomic_load_u32_u  (alias)
 *   __libcgj_batch5121_marker = "libcgj-batch5121"
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

const char __libcgj_batch5121_marker[] = "libcgj-batch5121";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5121_load(const uint32_t *pWord)
{
	if (pWord == NULL) {
		return 0u;
	}
	return *pWord;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atomic_load_u32_u - soft non-hardware u32 load.
 *
 * p: pointer to word; NULL -> 0.
 *
 * Returns *p, or 0 when p is NULL. Self-contained; no parent wires.
 */
uint32_t
gj_atomic_load_u32_u(const uint32_t *pWord)
{
	(void)NULL;
	return b5121_load(pWord);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atomic_load_u32_u(const uint32_t *pWord)
    __attribute__((alias("gj_atomic_load_u32_u")));
