/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3984: page-alignment predicate (size_t, 4 KiB).
 *
 * Surface (unique symbols):
 *   int gj_is_page_aligned_u(size_t x);
 *     - Return 1 if x is a multiple of 4096 (x & 4095 == 0), else 0.
 *   int __gj_is_page_aligned_u  (alias)
 *   __libcgj_batch3984_marker = "libcgj-batch3984"
 *
 * Exclusive continuum CREATE-ONLY (3981-3990). Distinct from
 * gj_u64_is_aligned (batch1069) / gj_page_align_u (batch3982) — unique
 * gj_is_page_aligned_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3984_marker[] = "libcgj-batch3984";

/* Standard x86_64 base page size used by this freestanding helper. */
#define B3984_PAGE  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3984_is_page_aligned(size_t uX)
{
	size_t uMask = (size_t)B3984_PAGE - 1u;

	return ((uX & uMask) == 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_page_aligned_u - true when uX is aligned to a 4 KiB page.
 *
 * Returns 1 if (uX % 4096) == 0, else 0. No parent wires.
 */
int
gj_is_page_aligned_u(size_t uX)
{
	(void)NULL;
	return b3984_is_page_aligned(uX);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_page_aligned_u(size_t uX)
    __attribute__((alias("gj_is_page_aligned_u")));
