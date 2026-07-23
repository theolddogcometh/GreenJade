/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1772: align address/size down to a 4 KiB page.
 *
 * Surface (unique symbols):
 *   int gj_page_align_down_4k(uint64_t x, uint64_t *out);
 *     — Round x down to the previous multiple of 4096. Writes *out.
 *       x already aligned → *out = x. x < 4096 → *out = 0.
 *       out == NULL → -1. Success → 0.
 *   int __gj_page_align_down_4k  (alias)
 *   __libcgj_batch1772_marker = "libcgj-batch1772"
 *
 * Page/table exclusive wave (1771–1780). Distinct from gj_page_round_down
 * (batch976) and gj_align_down (batch527) — unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1772_marker[] = "libcgj-batch1772";

/* Standard x86_64 base page size (4 KiB). */
#define B1772_PAGE_4K 4096ull

/* ---- freestanding helpers ---------------------------------------------- */

/* x & ~4095; returns 0 on success, -1 if pOut is NULL. */
static int
b1772_align_down_4k(uint64_t u64X, uint64_t *pOut)
{
	uint64_t u64Mask;

	if (pOut == NULL) {
		return -1;
	}

	u64Mask = B1772_PAGE_4K - 1ull;
	*pOut = u64X & ~u64Mask;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_page_align_down_4k — round x down to a 4 KiB boundary into *out.
 *
 * u64X: byte address or size to round down.
 * pOut: result sink (must be non-NULL).
 *
 * Returns 0 on success, -1 if pOut is NULL.
 */
int
gj_page_align_down_4k(uint64_t u64X, uint64_t *pOut)
{
	return b1772_align_down_4k(u64X, pOut);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_page_align_down_4k(uint64_t u64X, uint64_t *pOut)
    __attribute__((alias("gj_page_align_down_4k")));
