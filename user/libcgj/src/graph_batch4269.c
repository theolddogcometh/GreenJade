/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4269: freestanding pointer page offset (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_ptr_page_offset_u(const void *p);
 *     - Byte offset of p within its 4096-byte page:
 *       ((uint64_t)(uintptr_t)p) & 4095.
 *   uint64_t __gj_ptr_page_offset_u  (alias)
 *   __libcgj_batch4269_marker = "libcgj-batch4269"
 *
 * Exclusive continuum CREATE-ONLY (4261-4270): align/page wave.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4269_marker[] = "libcgj-batch4269";

#define B4269_PAGE_MASK  4095ull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Page offset: low 12 bits of the pointer as uint64_t.
 */
static uint64_t
b4269_page_offset(const void *p)
{
	return ((uint64_t)(uintptr_t)p) & B4269_PAGE_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptr_page_offset_u - offset of p within its 4 KiB page.
 *
 * p: pointer (may be NULL → 0)
 * Returns (uintptr as u64) & 4095. No parent wires.
 */
uint64_t
gj_ptr_page_offset_u(const void *p)
{
	(void)NULL;
	return b4269_page_offset(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ptr_page_offset_u(const void *p)
    __attribute__((alias("gj_ptr_page_offset_u")));
