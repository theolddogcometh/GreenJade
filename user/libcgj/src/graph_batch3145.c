/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3145: count of 2 MiB pages covering a span.
 *
 * Surface (unique symbols):
 *   uint64_t gj_page_count_2m_u(uint64_t bytes);
 *     - Ceiling of bytes / 2 MiB (0x200000).
 *       bytes == 0 → 0.
 *       Overflow of the intermediate (bytes + 2MiB - 1) → 0.
 *   uint64_t __gj_page_count_2m_u  (alias)
 *   __libcgj_batch3145_marker = "libcgj-batch3145"
 *
 * Milestone 3150 exclusive continuum CREATE-ONLY (3141-3150). Distinct
 * from gj_page_count_4k_u (batch3144) — unique gj_page_count_2m_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3145_marker[] = "libcgj-batch3145";

/* Standard x86_64 2 MiB large page size. */
#define B3145_PAGE_2M  0x200000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* ceil(cb / 2MiB); 0 when cb is 0 or intermediate overflows. */
static uint64_t
b3145_page_count_2m(uint64_t u64Cb)
{
	uint64_t u64Mask;
	uint64_t u64Adj;

	if (u64Cb == 0ull) {
		return 0ull;
	}

	u64Mask = B3145_PAGE_2M - 1ull;
	if (u64Cb > (UINT64_MAX - u64Mask)) {
		return 0ull;
	}
	u64Adj = (u64Cb + u64Mask) & ~u64Mask;
	return u64Adj / B3145_PAGE_2M;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_page_count_2m_u - number of 2 MiB pages needed to cover bytes.
 *
 * bytes: span length in bytes (0 → 0).
 *
 * Returns ceil(bytes / 2097152), or 0 if the intermediate round-up would
 * overflow. No parent wires.
 */
uint64_t
gj_page_count_2m_u(uint64_t u64Cb)
{
	(void)NULL;
	return b3145_page_count_2m(u64Cb);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_page_count_2m_u(uint64_t u64Cb)
    __attribute__((alias("gj_page_count_2m_u")));
