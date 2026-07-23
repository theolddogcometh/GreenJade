/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5039: freestanding MM pages-for-length (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_mm_pages_for_u(uint64_t len);
 *     - Number of 4 KiB pages needed to cover len bytes:
 *       (len + 4095) / 4096. len == 0 → 0.
 *       Overflow of (len + 4095) → 0.
 *   uint64_t __gj_mm_pages_for_u  (alias)
 *   __libcgj_batch5039_marker = "libcgj-batch5039"
 *
 * Exclusive continuum CREATE-ONLY (5031-5040: mm page helpers).
 * Distinct from gj_u64_pages_spanned_u (batch4266) and
 * gj_page_count_4k (batch1775) — unique gj_mm_pages_for_u surface
 * only (length-only ceil, no address offset); no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5039_marker[] = "libcgj-batch5039";

/* Standard x86_64 base page size (4 KiB). */
#define B5039_PAGE_SIZE  4096ull
#define B5039_PAGE_MASK  (B5039_PAGE_SIZE - 1ull)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Ceil(len / 4096). len == 0 → 0. Overflow of (len + 4095) → 0.
 */
static uint64_t
b5039_pages_for(uint64_t u64Len)
{
	if (u64Len == 0ull) {
		return 0ull;
	}
	if (u64Len > (UINT64_MAX - B5039_PAGE_MASK)) {
		return 0ull;
	}
	return (u64Len + B5039_PAGE_MASK) / B5039_PAGE_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mm_pages_for_u - count 4 KiB pages needed to cover len bytes.
 *
 * len: byte length (0 → 0 pages)
 *
 * Formula: (len + 4095) / 4096. Overflow of intermediate → 0.
 * Self-contained; no parent wires.
 */
uint64_t
gj_mm_pages_for_u(uint64_t u64Len)
{
	(void)NULL;
	return b5039_pages_for(u64Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mm_pages_for_u(uint64_t u64Len)
    __attribute__((alias("gj_mm_pages_for_u")));
