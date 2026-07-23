/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1776: x86_64 PTE Present bit probe.
 *
 * Surface (unique symbols):
 *   int gj_pte_present_bit(const uint64_t *pte);
 *     — Return 1 if *pte has the Present bit (bit 0) set; else 0.
 *       pte == NULL → 0.
 *   int __gj_pte_present_bit  (alias)
 *   __libcgj_batch1776_marker = "libcgj-batch1776"
 *
 * Page/table exclusive wave (1771–1780). Matches Intel SDM PTE.P / kernel
 * PTE_P (1ull << 0). Unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1776_marker[] = "libcgj-batch1776";

/* x86_64 page-table Present flag (bit 0). */
#define B1776_PTE_P (1ull << 0)

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if Present is set in the PTE word. */
static int
b1776_present(const uint64_t *pPte)
{
	if (pPte == NULL) {
		return 0;
	}
	return ((*pPte & B1776_PTE_P) != 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pte_present_bit — 1 if PTE Present (bit 0) is set.
 *
 * pPte: pointer to a 64-bit page-table entry word (may be NULL).
 *
 * Returns 1 when Present is set, 0 when clear or pPte is NULL.
 */
int
gj_pte_present_bit(const uint64_t *pPte)
{
	return b1776_present(pPte);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_pte_present_bit(const uint64_t *pPte)
    __attribute__((alias("gj_pte_present_bit")));
