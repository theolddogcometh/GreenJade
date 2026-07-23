/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4982: x86_64 PTE Writable / R/W flag mask
 * (uint64, _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_pte_writable_u(void);
 *     - Returns the soft Writable (R/W) bit mask for a page-table entry
 *       (bit 1), as the constant 2. Value API (no pointer); distinct
 *       from the pointer probe gj_pte_writable_bit (batch1777).
 *   uint64_t __gj_pte_writable_u  (alias)
 *   __libcgj_batch4982_marker = "libcgj-batch4982"
 *
 * Exclusive continuum CREATE-ONLY (4981-4990). Distinct from
 * gj_pte_writable_bit (batch1777) — unique gj_pte_writable_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4982_marker[] = "libcgj-batch4982";

/* Soft Writable / R/W flag (PTE bit 1). */
#define B4982_PTE_W  2ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4982_writable_mask(void)
{
	return B4982_PTE_W;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pte_writable_u - return the soft PTE Writable flag mask (bit 1).
 *
 * Always returns 2 (1ull << 1). Useful for OR/AND construction of PTE
 * words without hard-coding flag constants at call sites.
 * Self-contained; no parent wires.
 */
uint64_t
gj_pte_writable_u(void)
{
	(void)NULL;
	return b4982_writable_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_pte_writable_u(void)
    __attribute__((alias("gj_pte_writable_u")));
