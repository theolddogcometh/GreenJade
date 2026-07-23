/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5034: freestanding MM page-frame number (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_mm_pfn_u(uint64_t addr);
 *     - Convert a linear address to a 4 KiB page-frame number:
 *       addr >> 12.
 *   uint64_t __gj_mm_pfn_u  (alias)
 *   __libcgj_batch5034_marker = "libcgj-batch5034"
 *
 * Exclusive continuum CREATE-ONLY (5031-5040: mm page helpers).
 * Distinct from gj_ptr_page_offset_u (batch4269) — unique gj_mm_pfn_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5034_marker[] = "libcgj-batch5034";

/* Base page shift: log2(4096) == 12. */
#define B5034_PAGE_SHIFT  12u

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5034_pfn(uint64_t u64Addr)
{
	return u64Addr >> B5034_PAGE_SHIFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mm_pfn_u - convert a linear address to a 4 KiB page-frame number.
 *
 * addr: linear / virtual address as uint64_t
 *
 * Returns addr >> 12. Self-contained; no parent wires.
 */
uint64_t
gj_mm_pfn_u(uint64_t u64Addr)
{
	(void)NULL;
	return b5034_pfn(u64Addr);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mm_pfn_u(uint64_t u64Addr)
    __attribute__((alias("gj_mm_pfn_u")));
