/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3099: product bar3 partial readiness (uint).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_partial_u(uint32_t mask);
 *     - Return 1 if mask has at least one but not all of the low three
 *       product bar3 slots set (popcount of mask & 0x7 is in 1..2),
 *       else 0. Soft pure-data partial-progress gate for Steam/Deck/
 *       Top50 completeness. Unsigned form.
 *   uint32_t __gj_bar3_partial_u  (alias)
 *   __libcgj_batch3099_marker = "libcgj-batch3099"
 *
 * Milestone 3100 exclusive continuum CREATE-ONLY (3091-3100). Unique
 * gj_bar3_partial_u surface only; no multi-def. Distinct from
 * gj_bar3_progress_u (batch2997 popcount), gj_bar3_score_u
 * (batch2699), gj_bar3_mask_all_u (batch2895 four-slot), and
 * gj_bar3_ready_hint (batch2299). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3099_marker[] = "libcgj-batch3099";

/* Three product bar3 readiness bits (bits 0..2). */
#define B3099_BAR3_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

/* Popcount of low three bar3 bits (0..3). */
static uint32_t
b3099_pop3(uint32_t uMask)
{
	uint32_t uBits;
	uint32_t uScore;

	uBits = uMask & B3099_BAR3_MASK;
	uScore = 0u;
	if ((uBits & 0x1u) != 0u) {
		uScore = uScore + 1u;
	}
	if ((uBits & 0x2u) != 0u) {
		uScore = uScore + 1u;
	}
	if ((uBits & 0x4u) != 0u) {
		uScore = uScore + 1u;
	}
	return uScore;
}

/* 1u if bar3 is partial (score 1 or 2). */
static uint32_t
b3099_partial_u(uint32_t uMask)
{
	uint32_t uScore;

	uScore = b3099_pop3(uMask);
	if (uScore == 0u) {
		return 0u;
	}
	if (uScore >= 3u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_partial_u - 1u iff bar3 mask has partial (not empty/full) set.
 *
 * mask: product bar3 readiness bitmask (bits 0..2 = Steam/Deck/Top50)
 *
 * Returns 1 when popcount(mask & 0x7) is in {1, 2}, else 0. Soft
 * pure-data only; does not probe Steam/Deck. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_bar3_partial_u(uint32_t uMask)
{
	(void)NULL;
	return b3099_partial_u(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_partial_u(uint32_t uMask)
    __attribute__((alias("gj_bar3_partial_u")));
