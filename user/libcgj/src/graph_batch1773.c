/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1773: align address/size up to a 2 MiB page.
 *
 * Surface (unique symbols):
 *   int gj_page_align_up_2m(uint64_t x, uint64_t *out);
 *     — Round x up to the next multiple of 2 MiB (0x200000). Writes *out.
 *       x already aligned → *out = x. out == NULL → -1.
 *       Overflow of (x + 2MiB - 1) → -1. Success → 0.
 *   int __gj_page_align_up_2m  (alias)
 *   __libcgj_batch1773_marker = "libcgj-batch1773"
 *
 * Page/table exclusive wave (1771–1780). Matches x86_64 large-page (PS)
 * size used with PDE.PS / PTE_PS. Unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1773_marker[] = "libcgj-batch1773";

/* Standard x86_64 2 MiB large page size. */
#define B1773_PAGE_2M 0x200000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* (x + 2MiB - 1) & ~(2MiB - 1); 0 on success, -1 on NULL/overflow. */
static int
b1773_align_up_2m(uint64_t u64X, uint64_t *pOut)
{
	uint64_t u64Mask;
	uint64_t u64Adj;

	if (pOut == NULL) {
		return -1;
	}

	u64Mask = B1773_PAGE_2M - 1ull;
	if (u64X > (UINT64_MAX - u64Mask)) {
		return -1;
	}
	u64Adj = (u64X + u64Mask) & ~u64Mask;
	*pOut = u64Adj;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_page_align_up_2m — round x up to a 2 MiB boundary into *out.
 *
 * u64X: byte address or size to round up.
 * pOut: result sink (must be non-NULL).
 *
 * Returns 0 on success, -1 if pOut is NULL or the intermediate overflows.
 */
int
gj_page_align_up_2m(uint64_t u64X, uint64_t *pOut)
{
	return b1773_align_up_2m(u64X, pOut);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_page_align_up_2m(uint64_t u64X, uint64_t *pOut)
    __attribute__((alias("gj_page_align_up_2m")));
