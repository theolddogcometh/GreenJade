/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4985: x86_64 PTE Present predicate (value, _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pte_is_present_u(uint64_t pte);
 *     - Returns 1 if the Present bit (bit 0) is set in the PTE word,
 *       else 0. Value API (by-value uint64); distinct from the pointer
 *       probe gj_pte_present_bit (batch1776).
 *   uint32_t __gj_pte_is_present_u  (alias)
 *   __libcgj_batch4985_marker = "libcgj-batch4985"
 *
 * Exclusive continuum CREATE-ONLY (4981-4990). Distinct from
 * gj_pte_present_bit (batch1776) and gj_pte_present_u (batch4981) —
 * unique gj_pte_is_present_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4985_marker[] = "libcgj-batch4985";

/* Soft Present flag (PTE bit 0). */
#define B4985_PTE_P  1ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4985_is_present(uint64_t u64Pte)
{
	return ((u64Pte & B4985_PTE_P) != 0ull) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pte_is_present_u - 1 if soft Present (bit 0) is set in pte.
 *
 * pte: 64-bit page-table entry word (by value)
 *
 * Returns 1 when bit 0 is set, else 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_pte_is_present_u(uint64_t u64Pte)
{
	(void)NULL;
	return b4985_is_present(u64Pte);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pte_is_present_u(uint64_t u64Pte)
    __attribute__((alias("gj_pte_is_present_u")));
