/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5035: freestanding MM page base from PFN (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_mm_page_u(uint64_t pfn);
 *     - Convert a 4 KiB page-frame number to the page base address:
 *       pfn << 12. On shift overflow of the high bits the low 64-bit
 *       result is the natural modular shift (unsigned C semantics).
 *   uint64_t __gj_mm_page_u  (alias)
 *   __libcgj_batch5035_marker = "libcgj-batch5035"
 *
 * Exclusive continuum CREATE-ONLY (5031-5040: mm page helpers).
 * Distinct from gj_mm_pfn_u (batch5034) — unique gj_mm_page_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5035_marker[] = "libcgj-batch5035";

/* Base page shift: log2(4096) == 12. */
#define B5035_PAGE_SHIFT  12u

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5035_page(uint64_t u64Pfn)
{
	return u64Pfn << B5035_PAGE_SHIFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mm_page_u - convert a 4 KiB page-frame number to a page base address.
 *
 * pfn: page-frame number
 *
 * Returns pfn << 12. Inverse of gj_mm_pfn_u for in-range PFNs.
 * Self-contained; no parent wires.
 */
uint64_t
gj_mm_page_u(uint64_t u64Pfn)
{
	(void)NULL;
	return b5035_page(u64Pfn);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mm_page_u(uint64_t u64Pfn)
    __attribute__((alias("gj_mm_page_u")));
