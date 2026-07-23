/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4983: x86_64 PTE User/Supervisor flag mask
 * (uint64, _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_pte_user_u(void);
 *     - Returns the soft User/Supervisor bit mask for a page-table entry
 *       (bit 2), as the constant 4. Value API (no pointer); distinct
 *       from the pointer probe gj_pte_user_bit (batch1778).
 *   uint64_t __gj_pte_user_u  (alias)
 *   __libcgj_batch4983_marker = "libcgj-batch4983"
 *
 * Exclusive continuum CREATE-ONLY (4981-4990). Distinct from
 * gj_pte_user_bit (batch1778) — unique gj_pte_user_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4983_marker[] = "libcgj-batch4983";

/* Soft User/Supervisor flag (PTE bit 2). */
#define B4983_PTE_U  4ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4983_user_mask(void)
{
	return B4983_PTE_U;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pte_user_u - return the soft PTE User/Supervisor flag mask (bit 2).
 *
 * Always returns 4 (1ull << 2). Useful for OR/AND construction of PTE
 * words without hard-coding flag constants at call sites.
 * Self-contained; no parent wires.
 */
uint64_t
gj_pte_user_u(void)
{
	(void)NULL;
	return b4983_user_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_pte_user_u(void)
    __attribute__((alias("gj_pte_user_u")));
