/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5038: freestanding MM page round-down (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_mm_round_down_u(uint64_t addr);
 *     - Round addr down to the previous 4 KiB page boundary:
 *       addr & ~0xfff. Already aligned → unchanged. addr < 4096 → 0.
 *   uint64_t __gj_mm_round_down_u  (alias)
 *   __libcgj_batch5038_marker = "libcgj-batch5038"
 *
 * Exclusive continuum CREATE-ONLY (5031-5040: mm page helpers).
 * Distinct from gj_page_round_down (batch976) and gj_mm_round_up_u
 * (batch5037) — unique gj_mm_round_down_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5038_marker[] = "libcgj-batch5038";

/* Page offset mask: 4096 - 1 == 0xfff. */
#define B5038_PAGE_MASK  0xfffull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5038_round_down(uint64_t u64Addr)
{
	return u64Addr & ~B5038_PAGE_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mm_round_down_u - round addr down to a 4 KiB page boundary.
 *
 * addr: linear address or size-as-address
 *
 * Returns addr & ~0xfff. Self-contained; no parent wires.
 */
uint64_t
gj_mm_round_down_u(uint64_t u64Addr)
{
	(void)NULL;
	return b5038_round_down(u64Addr);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mm_round_down_u(uint64_t u64Addr)
    __attribute__((alias("gj_mm_round_down_u")));
