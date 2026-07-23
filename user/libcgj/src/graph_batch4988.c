/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4988: x86_64 PTE low-flag soft extract
 * (uint64, _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_pte_flags_u(uint64_t pte);
 *     - Soft-mask the low 12 flag bits out of a PTE word:
 *         pte & 0xfff
 *   uint64_t __gj_pte_flags_u  (alias)
 *   __libcgj_batch4988_marker = "libcgj-batch4988"
 *
 * Exclusive continuum CREATE-ONLY (4981-4990). Unique gj_pte_flags_u
 * surface only; no multi-def. Soft freestanding low-nibble/flag mask —
 * does not include the soft NX high bit. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4988_marker[] = "libcgj-batch4988";

/* Soft low 12 PTE flag bits. */
#define B4988_FLAG_LO  0xfffull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4988_flags(uint64_t u64Pte)
{
	return u64Pte & B4988_FLAG_LO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pte_flags_u - soft-extract low 12 flag bits from a PTE word.
 *
 * pte: 64-bit page-table entry word (by value)
 *
 * Returns pte & 0xfff. Self-contained soft mask; no parent wires.
 */
uint64_t
gj_pte_flags_u(uint64_t u64Pte)
{
	(void)NULL;
	return b4988_flags(u64Pte);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_pte_flags_u(uint64_t u64Pte)
    __attribute__((alias("gj_pte_flags_u")));
