/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1779: physical page-frame number from phys addr.
 *
 * Surface (unique symbols):
 *   int gj_pfn_from_phys(uint64_t phys, uint64_t *out);
 *     — Extract the 4 KiB page-frame number: phys >> 12, written to *out.
 *       out == NULL → -1. Success → 0.
 *       Low 12 bits of phys are discarded (page offset).
 *   int __gj_pfn_from_phys  (alias)
 *   __libcgj_batch1779_marker = "libcgj-batch1779"
 *
 * Page/table exclusive wave (1771–1780). Matches x86_64 4 KiB frame index
 * used with PTE address fields (bits 12..). Unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1779_marker[] = "libcgj-batch1779";

/* Log2 of the 4 KiB base page size (PFN shift). */
#define B1779_PAGE_SHIFT 12u

/* ---- freestanding helpers ---------------------------------------------- */

/* phys >> 12 into *pOut; 0 on success, -1 if pOut is NULL. */
static int
b1779_pfn_from_phys(uint64_t u64Phys, uint64_t *pOut)
{
	if (pOut == NULL) {
		return -1;
	}
	*pOut = u64Phys >> B1779_PAGE_SHIFT;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pfn_from_phys — 4 KiB page-frame number of a physical address.
 *
 * u64Phys: physical address in bytes.
 * pOut:    result sink (must be non-NULL).
 *
 * Returns 0 on success, -1 if pOut is NULL.
 */
int
gj_pfn_from_phys(uint64_t u64Phys, uint64_t *pOut)
{
	return b1779_pfn_from_phys(u64Phys, pOut);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_pfn_from_phys(uint64_t u64Phys, uint64_t *pOut)
    __attribute__((alias("gj_pfn_from_phys")));
