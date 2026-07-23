/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8379: page-align size_t up (4 KiB) stub.
 *
 * Surface (unique symbols):
 *   size_t gj_page_align_u_8379(size_t x);
 *     - Round x up to the next multiple of 4096. x already aligned → x.
 *       Overflow of (x + 4095) → 0.
 *   size_t __gj_page_align_u_8379  (alias)
 *   __libcgj_batch8379_marker = "libcgj-batch8379"
 *
 * Exclusive continuum CREATE-ONLY (8371-8380: mmap/prot flag stubs
 * integer). Distinct from gj_page_align_u (batch3982) — unique
 * batch-suffixed surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8379_marker[] = "libcgj-batch8379";

/* Standard x86_64 base page size used by this freestanding helper. */
#define B8379_PAGE  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b8379_page_align(size_t uX)
{
	size_t uMask = (size_t)B8379_PAGE - 1u;
	size_t uAdj;

	if (uX > (SIZE_MAX - uMask)) {
		return 0u;
	}
	uAdj = (uX + uMask) & ~uMask;
	return uAdj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_page_align_u_8379 - (x + 4095) & ~4095 with overflow → 0.
 *
 * x: byte count or address-as-size to round up to a page boundary.
 * Already page-aligned values return unchanged. No parent wires.
 */
size_t
gj_page_align_u_8379(size_t uX)
{
	(void)NULL;
	return b8379_page_align(uX);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_page_align_u_8379(size_t uX)
    __attribute__((alias("gj_page_align_u_8379")));
