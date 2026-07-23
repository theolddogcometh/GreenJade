/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5037: freestanding MM page round-up (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_mm_round_up_u(uint64_t addr);
 *     - Round addr up to the next 4 KiB page boundary:
 *       (addr + 0xfff) & ~0xfff. Already aligned → unchanged.
 *       Overflow of (addr + 0xfff) → 0.
 *   uint64_t __gj_mm_round_up_u  (alias)
 *   __libcgj_batch5037_marker = "libcgj-batch5037"
 *
 * Exclusive continuum CREATE-ONLY (5031-5040: mm page helpers).
 * Distinct from gj_page_round_up (batch975) — unique gj_mm_round_up_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5037_marker[] = "libcgj-batch5037";

/* Page offset mask: 4096 - 1 == 0xfff. */
#define B5037_PAGE_MASK  0xfffull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5037_round_up(uint64_t u64Addr)
{
	if (u64Addr > (UINT64_MAX - B5037_PAGE_MASK)) {
		return 0ull;
	}
	return (u64Addr + B5037_PAGE_MASK) & ~B5037_PAGE_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mm_round_up_u - round addr up to a 4 KiB page boundary.
 *
 * addr: linear address or size-as-address
 *
 * Already page-aligned values return unchanged. Overflow → 0.
 * Self-contained; no parent wires.
 */
uint64_t
gj_mm_round_up_u(uint64_t u64Addr)
{
	(void)NULL;
	return b5037_round_up(u64Addr);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mm_round_up_u(uint64_t u64Addr)
    __attribute__((alias("gj_mm_round_up_u")));
