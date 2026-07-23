/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1775: count of 4 KiB pages covering a span.
 *
 * Surface (unique symbols):
 *   int gj_page_count_4k(uint64_t bytes, uint64_t *out);
 *     — Ceiling of bytes / 4096 written to *out.
 *       bytes == 0 → *out = 0. out == NULL → -1.
 *       Overflow of the intermediate (bytes + 4095) → -1.
 *       Success → 0.
 *   int __gj_page_count_4k  (alias)
 *   __libcgj_batch1775_marker = "libcgj-batch1775"
 *
 * Page/table exclusive wave (1771–1780). Unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1775_marker[] = "libcgj-batch1775";

/* Standard x86_64 base page size (4 KiB). */
#define B1775_PAGE_4K 4096ull

/* ---- freestanding helpers ---------------------------------------------- */

/* ceil(cb / 4096); 0 on success, -1 on NULL/overflow. */
static int
b1775_page_count_4k(uint64_t u64Cb, uint64_t *pOut)
{
	uint64_t u64Mask;
	uint64_t u64Adj;

	if (pOut == NULL) {
		return -1;
	}

	if (u64Cb == 0ull) {
		*pOut = 0ull;
		return 0;
	}

	u64Mask = B1775_PAGE_4K - 1ull;
	if (u64Cb > (UINT64_MAX - u64Mask)) {
		return -1;
	}
	u64Adj = (u64Cb + u64Mask) & ~u64Mask;
	*pOut = u64Adj / B1775_PAGE_4K;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_page_count_4k — number of 4 KiB pages needed to cover bytes.
 *
 * u64Cb: span length in bytes.
 * pOut:  result sink (must be non-NULL).
 *
 * Returns 0 on success, -1 if pOut is NULL or the intermediate overflows.
 */
int
gj_page_count_4k(uint64_t u64Cb, uint64_t *pOut)
{
	return b1775_page_count_4k(u64Cb, pOut);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_page_count_4k(uint64_t u64Cb, uint64_t *pOut)
    __attribute__((alias("gj_page_count_4k")));
