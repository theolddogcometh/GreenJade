/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5033: freestanding MM page-offset mask (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mm_page_mask_u(void);
 *     - Returns the in-page offset mask for a 4 KiB page (always 0xfff).
 *   uint32_t __gj_mm_page_mask_u  (alias)
 *   __libcgj_batch5033_marker = "libcgj-batch5033"
 *
 * Exclusive continuum CREATE-ONLY (5031-5040: mm page helpers).
 * Distinct from gj_page_size (batch988) and gj_mm_page_size_u
 * (batch5032) — unique gj_mm_page_mask_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5033_marker[] = "libcgj-batch5033";

/* Page offset mask: 4096 - 1 == 0xfff. */
#define B5033_PAGE_MASK  0xfffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5033_page_mask(void)
{
	return B5033_PAGE_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mm_page_mask_u - report the freestanding 4 KiB page offset mask.
 *
 * Always returns 0xfff (low 12 bits set). Self-contained; no parent wires.
 */
uint32_t
gj_mm_page_mask_u(void)
{
	(void)NULL;
	return b5033_page_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mm_page_mask_u(void)
    __attribute__((alias("gj_mm_page_mask_u")));
