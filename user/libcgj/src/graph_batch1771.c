/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1771: align address/size up to a 4 KiB page.
 *
 * Surface (unique symbols):
 *   int gj_page_align_up_4k(uint64_t x, uint64_t *out);
 *     — Round x up to the next multiple of 4096. Writes result to *out.
 *       x already aligned → *out = x. out == NULL → -1.
 *       Overflow of (x + 4095) → -1 (out left unchanged).
 *       Success → 0.
 *   int __gj_page_align_up_4k  (alias)
 *   __libcgj_batch1771_marker = "libcgj-batch1771"
 *
 * Page/table exclusive wave (1771–1780). Distinct from gj_page_round_up
 * (batch975) and gj_align_up (batch526) — unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1771_marker[] = "libcgj-batch1771";

/* Standard x86_64 base page size (4 KiB). */
#define B1771_PAGE_4K 4096ull

/* ---- freestanding helpers ---------------------------------------------- */

/* (x + 4095) & ~4095; returns 0 on success, -1 on overflow. */
static int
b1771_align_up_4k(uint64_t u64X, uint64_t *pOut)
{
	uint64_t u64Mask;
	uint64_t u64Adj;

	if (pOut == NULL) {
		return -1;
	}

	u64Mask = B1771_PAGE_4K - 1ull;
	if (u64X > (UINT64_MAX - u64Mask)) {
		return -1;
	}
	u64Adj = (u64X + u64Mask) & ~u64Mask;
	*pOut = u64Adj;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_page_align_up_4k — round x up to a 4 KiB boundary into *out.
 *
 * u64X: byte address or size to round up.
 * pOut: result sink (must be non-NULL).
 *
 * Returns 0 on success, -1 if pOut is NULL or the intermediate overflows.
 */
int
gj_page_align_up_4k(uint64_t u64X, uint64_t *pOut)
{
	return b1771_align_up_4k(u64X, pOut);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_page_align_up_4k(uint64_t u64X, uint64_t *pOut)
    __attribute__((alias("gj_page_align_up_4k")));
