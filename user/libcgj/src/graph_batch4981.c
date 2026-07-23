/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4981: x86_64 PTE Present flag mask (uint64, _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_pte_present_u(void);
 *     - Returns the soft Present bit mask for a page-table entry (bit 0),
 *       as the constant 1. Value API (no pointer); distinct from the
 *       pointer probe gj_pte_present_bit (batch1776).
 *   uint64_t __gj_pte_present_u  (alias)
 *   __libcgj_batch4981_marker = "libcgj-batch4981"
 *
 * Exclusive continuum CREATE-ONLY (4981-4990: pte_present_u,
 * pte_writable_u, pte_user_u, pte_nx_bit_u, pte_is_present_u,
 * pte_is_writable_u, pte_addr_u, pte_flags_u, pte_make_u,
 * batch_id_4990). Distinct from gj_pte_present_bit (batch1776) —
 * unique gj_pte_present_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4981_marker[] = "libcgj-batch4981";

/* Soft Present flag (PTE bit 0). */
#define B4981_PTE_P  1ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4981_present_mask(void)
{
	return B4981_PTE_P;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pte_present_u - return the soft PTE Present flag mask (bit 0).
 *
 * Always returns 1 (1ull << 0). Useful for OR/AND construction of PTE
 * words without hard-coding flag constants at call sites.
 * Self-contained; no parent wires.
 */
uint64_t
gj_pte_present_u(void)
{
	(void)NULL;
	return b4981_present_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_pte_present_u(void)
    __attribute__((alias("gj_pte_present_u")));
