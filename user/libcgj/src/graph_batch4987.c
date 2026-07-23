/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4987: x86_64 PTE physical-address soft extract
 * (uint64, _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_pte_addr_u(uint64_t pte);
 *     - Soft-mask the page-frame address out of a PTE word:
 *         pte & ~0xfff & ~(1ull << 63)
 *       Clears the low 12 flag bits and the soft NX high bit.
 *   uint64_t __gj_pte_addr_u  (alias)
 *   __libcgj_batch4987_marker = "libcgj-batch4987"
 *
 * Exclusive continuum CREATE-ONLY (4981-4990). Unique gj_pte_addr_u
 * surface only; no multi-def. Soft freestanding mask — not a full
 * SDM physical-address width probe. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4987_marker[] = "libcgj-batch4987";

/* Soft low 12 flag bits and soft NX high bit. */
#define B4987_FLAG_LO  0xfffull
#define B4987_NX       (1ull << 63)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4987_addr(uint64_t u64Pte)
{
	return u64Pte & ~B4987_FLAG_LO & ~B4987_NX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pte_addr_u - soft-extract page-frame address bits from a PTE word.
 *
 * pte: 64-bit page-table entry word (by value)
 *
 * Returns pte with low 12 flag bits and soft NX (bit 63) cleared.
 * Self-contained soft mask; no parent wires.
 */
uint64_t
gj_pte_addr_u(uint64_t u64Pte)
{
	(void)NULL;
	return b4987_addr(u64Pte);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_pte_addr_u(uint64_t u64Pte)
    __attribute__((alias("gj_pte_addr_u")));
