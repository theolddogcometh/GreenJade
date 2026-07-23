/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5036: freestanding MM in-page offset (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_mm_offset_u(uint64_t addr);
 *     - Byte offset of addr within its 4 KiB page: addr & 0xfff.
 *   uint64_t __gj_mm_offset_u  (alias)
 *   __libcgj_batch5036_marker = "libcgj-batch5036"
 *
 * Exclusive continuum CREATE-ONLY (5031-5040: mm page helpers).
 * Distinct from gj_ptr_page_offset_u (batch4269) — unique gj_mm_offset_u
 * surface only (address form, not pointer form); no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5036_marker[] = "libcgj-batch5036";

/* Page offset mask: 4096 - 1 == 0xfff. */
#define B5036_PAGE_MASK  0xfffull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5036_offset(uint64_t u64Addr)
{
	return u64Addr & B5036_PAGE_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mm_offset_u - byte offset of addr within its 4 KiB page.
 *
 * addr: linear / virtual address as uint64_t
 *
 * Returns addr & 0xfff. Self-contained; no parent wires.
 */
uint64_t
gj_mm_offset_u(uint64_t u64Addr)
{
	(void)NULL;
	return b5036_offset(u64Addr);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mm_offset_u(uint64_t u64Addr)
    __attribute__((alias("gj_mm_offset_u")));
