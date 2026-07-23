/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2675: product bar3 all-ready predicate.
 *
 * Surface (unique symbols):
 *   int gj_bar3_all_ready_p(uint32_t mask);
 *     - Return 1 if mask has all four product bar3 readiness bits set
 *       (steam/deck/top50/install: bits 0..3), i.e.
 *       (mask & 0xf) == 0xf. Else 0. Soft pure-data completeness gate.
 *   int __gj_bar3_all_ready_p  (alias)
 *   __libcgj_batch2675_marker = "libcgj-batch2675"
 *
 * Milestone 2680 exclusive product bar3 bitmask helpers (2671-2680).
 * Unique gj_bar3_all_ready_p surface only; no multi-def. Distinct from
 * gj_bar3_ready_hint (batch2299 three-bit 0x7) and the per-slot
 * steam/deck/top50 ready_p helpers (batch2672-2674).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2675_marker[] = "libcgj-batch2675";

/* Four product bar3 readiness bits: steam/deck/top50/install (0..3). */
#define B2675_ALL_MASK  0xfu

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if all four bar3 readiness bits are present in mask. */
static int
b2675_all_ready_p(uint32_t uMask)
{
	if ((uMask & B2675_ALL_MASK) == B2675_ALL_MASK) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_all_ready_p - soft check that bar3 readiness mask is complete.
 *
 * mask: product bar3 readiness bitmask (low four bits required)
 *
 * Returns 1 when (mask & 0xf) == 0xf, else 0. Extra high bits are
 * ignored. Covers install + steam + deck + top50. Does not call libc.
 */
int
gj_bar3_all_ready_p(uint32_t uMask)
{
	(void)NULL;
	return b2675_all_ready_p(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bar3_all_ready_p(uint32_t uMask)
    __attribute__((alias("gj_bar3_all_ready_p")));
