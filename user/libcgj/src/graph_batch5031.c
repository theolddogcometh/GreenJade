/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5031: freestanding MM page-shift constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mm_page_shift_u(void);
 *     - Returns the base page shift for a 4 KiB page (always 12).
 *   uint32_t __gj_mm_page_shift_u  (alias)
 *   __libcgj_batch5031_marker = "libcgj-batch5031"
 *
 * Exclusive continuum CREATE-ONLY (5031-5040: mm page helpers —
 * page_shift_u, page_size_u, page_mask_u, pfn_u, page_u, offset_u,
 * round_up_u, round_down_u, pages_for_u, batch_id_5040).
 * Distinct from gj_page_size (batch988) — unique gj_mm_page_shift_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5031_marker[] = "libcgj-batch5031";

/* Base page shift: log2(4096) == 12. */
#define B5031_PAGE_SHIFT  12u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5031_page_shift(void)
{
	return B5031_PAGE_SHIFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mm_page_shift_u - report the freestanding 4 KiB page shift.
 *
 * Always returns 12 (log2 of 4096). Self-contained; no parent wires.
 */
uint32_t
gj_mm_page_shift_u(void)
{
	(void)NULL;
	return b5031_page_shift();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mm_page_shift_u(void)
    __attribute__((alias("gj_mm_page_shift_u")));
