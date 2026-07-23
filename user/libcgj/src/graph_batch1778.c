/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1778: x86_64 PTE User/Supervisor bit probe.
 *
 * Surface (unique symbols):
 *   int gj_pte_user_bit(const uint64_t *pte);
 *     — Return 1 if *pte has the User/Supervisor bit (bit 2) set; else 0.
 *       pte == NULL → 0. Set means user-mode access allowed.
 *   int __gj_pte_user_bit  (alias)
 *   __libcgj_batch1778_marker = "libcgj-batch1778"
 *
 * Page/table exclusive wave (1771–1780). Matches Intel SDM PTE.U/S / kernel
 * PTE_U (1ull << 2). Unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1778_marker[] = "libcgj-batch1778";

/* x86_64 page-table User/Supervisor flag (bit 2). */
#define B1778_PTE_U (1ull << 2)

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if User is set in the PTE word. */
static int
b1778_user(const uint64_t *pPte)
{
	if (pPte == NULL) {
		return 0;
	}
	return ((*pPte & B1778_PTE_U) != 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pte_user_bit — 1 if PTE User/Supervisor (bit 2) is set.
 *
 * pPte: pointer to a 64-bit page-table entry word (may be NULL).
 *
 * Returns 1 when User is set, 0 when clear or pPte is NULL.
 */
int
gj_pte_user_bit(const uint64_t *pPte)
{
	return b1778_user(pPte);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_pte_user_bit(const uint64_t *pPte)
    __attribute__((alias("gj_pte_user_bit")));
