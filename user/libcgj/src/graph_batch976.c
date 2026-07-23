/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch976: round size down to a 4 KiB page boundary.
 *
 * Surface (unique symbols):
 *   size_t gj_page_round_down(size_t x);
 *     — Round x down to the previous multiple of 4096. x already
 *       aligned → x. x < 4096 → 0.
 *   size_t __gj_page_round_down  (alias)
 *   __libcgj_batch976_marker = "libcgj-batch976"
 *
 * Distinct from gj_align_down (batch527) and gj_page_round_up (batch975).
 * Unique gj_page_round_down surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch976_marker[] = "libcgj-batch976";

/* Standard x86_64 base page size used by this freestanding helper. */
#define B976_PAGE 4096u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_page_round_down — x & ~4095.
 *
 * x: byte count or address-as-size to round down to a page boundary.
 */
size_t
gj_page_round_down(size_t x)
{
	size_t uMask = (size_t)B976_PAGE - 1u;

	return x & ~uMask;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_page_round_down(size_t x)
    __attribute__((alias("gj_page_round_down")));
