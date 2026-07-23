/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5032: freestanding MM page-size constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mm_page_size_u(void);
 *     - Returns the base page size in bytes (always 4096).
 *   uint32_t __gj_mm_page_size_u  (alias)
 *   __libcgj_batch5032_marker = "libcgj-batch5032"
 *
 * Exclusive continuum CREATE-ONLY (5031-5040: mm page helpers).
 * Distinct from gj_page_size (batch988) — unique gj_mm_page_size_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5032_marker[] = "libcgj-batch5032";

/* Standard x86_64 base page size (4 KiB). */
#define B5032_PAGE_SIZE  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5032_page_size(void)
{
	return B5032_PAGE_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mm_page_size_u - report the freestanding 4 KiB page size.
 *
 * Always returns 4096. Self-contained; no parent wires.
 */
uint32_t
gj_mm_page_size_u(void)
{
	(void)NULL;
	return b5032_page_size();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mm_page_size_u(void)
    __attribute__((alias("gj_mm_page_size_u")));
