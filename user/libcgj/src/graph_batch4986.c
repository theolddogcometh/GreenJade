/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4986: x86_64 PTE Writable predicate (value, _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pte_is_writable_u(uint64_t pte);
 *     - Returns 1 if the Writable / R/W bit (bit 1) is set in the PTE
 *       word, else 0. Value API (by-value uint64); distinct from the
 *       pointer probe gj_pte_writable_bit (batch1777).
 *   uint32_t __gj_pte_is_writable_u  (alias)
 *   __libcgj_batch4986_marker = "libcgj-batch4986"
 *
 * Exclusive continuum CREATE-ONLY (4981-4990). Distinct from
 * gj_pte_writable_bit (batch1777) and gj_pte_writable_u (batch4982) —
 * unique gj_pte_is_writable_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4986_marker[] = "libcgj-batch4986";

/* Soft Writable / R/W flag (PTE bit 1). */
#define B4986_PTE_W  2ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4986_is_writable(uint64_t u64Pte)
{
	return ((u64Pte & B4986_PTE_W) != 0ull) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pte_is_writable_u - 1 if soft Writable (bit 1) is set in pte.
 *
 * pte: 64-bit page-table entry word (by value)
 *
 * Returns 1 when bit 1 is set, else 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_pte_is_writable_u(uint64_t u64Pte)
{
	(void)NULL;
	return b4986_is_writable(u64Pte);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pte_is_writable_u(uint64_t u64Pte)
    __attribute__((alias("gj_pte_is_writable_u")));
