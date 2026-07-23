/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1774: align address/size down to a 2 MiB page.
 *
 * Surface (unique symbols):
 *   int gj_page_align_down_2m(uint64_t x, uint64_t *out);
 *     — Round x down to the previous multiple of 2 MiB (0x200000).
 *       Writes *out. x already aligned → *out = x. x < 2MiB → *out = 0.
 *       out == NULL → -1. Success → 0.
 *   int __gj_page_align_down_2m  (alias)
 *   __libcgj_batch1774_marker = "libcgj-batch1774"
 *
 * Page/table exclusive wave (1771–1780). Unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1774_marker[] = "libcgj-batch1774";

/* Standard x86_64 2 MiB large page size. */
#define B1774_PAGE_2M 0x200000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* x & ~(2MiB - 1); returns 0 on success, -1 if pOut is NULL. */
static int
b1774_align_down_2m(uint64_t u64X, uint64_t *pOut)
{
	uint64_t u64Mask;

	if (pOut == NULL) {
		return -1;
	}

	u64Mask = B1774_PAGE_2M - 1ull;
	*pOut = u64X & ~u64Mask;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_page_align_down_2m — round x down to a 2 MiB boundary into *out.
 *
 * u64X: byte address or size to round down.
 * pOut: result sink (must be non-NULL).
 *
 * Returns 0 on success, -1 if pOut is NULL.
 */
int
gj_page_align_down_2m(uint64_t u64X, uint64_t *pOut)
{
	return b1774_align_down_2m(u64X, pOut);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_page_align_down_2m(uint64_t u64X, uint64_t *pOut)
    __attribute__((alias("gj_page_align_down_2m")));
