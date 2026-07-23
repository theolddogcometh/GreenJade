/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1777: x86_64 PTE Writable / R/W bit probe.
 *
 * Surface (unique symbols):
 *   int gj_pte_writable_bit(const uint64_t *pte);
 *     — Return 1 if *pte has the R/W (Writable) bit (bit 1) set; else 0.
 *       pte == NULL → 0.
 *   int __gj_pte_writable_bit  (alias)
 *   __libcgj_batch1777_marker = "libcgj-batch1777"
 *
 * Page/table exclusive wave (1771–1780). Matches Intel SDM PTE.R/W / kernel
 * PTE_W (1ull << 1). Unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1777_marker[] = "libcgj-batch1777";

/* x86_64 page-table R/W (Writable) flag (bit 1). */
#define B1777_PTE_W (1ull << 1)

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if Writable is set in the PTE word. */
static int
b1777_writable(const uint64_t *pPte)
{
	if (pPte == NULL) {
		return 0;
	}
	return ((*pPte & B1777_PTE_W) != 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pte_writable_bit — 1 if PTE Writable / R/W (bit 1) is set.
 *
 * pPte: pointer to a 64-bit page-table entry word (may be NULL).
 *
 * Returns 1 when Writable is set, 0 when clear or pPte is NULL.
 */
int
gj_pte_writable_bit(const uint64_t *pPte)
{
	return b1777_writable(pPte);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_pte_writable_bit(const uint64_t *pPte)
    __attribute__((alias("gj_pte_writable_bit")));
