/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2997: product bar3 progress score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_progress_u(uint32_t mask);
 *     - Return the number of set bits among the low three product bar3
 *       slots of mask (popcount of mask & 0x7), range 0..3. Soft
 *       pure-data progress for Steam/Deck/Top50 completeness.
 *   uint32_t __gj_bar3_progress_u  (alias)
 *   __libcgj_batch2997_marker = "libcgj-batch2997"
 *
 * Milestone 3000 exclusive continuum CREATE-ONLY (2991-3000). Unique
 * gj_bar3_progress_u surface only; no multi-def. Distinct from
 * gj_bar3_score_u (batch2699), gj_product_bar3_mask (batch2597),
 * gj_product_bar3_bits (batch2398), and gj_bar3_ready_hint (batch2299).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2997_marker[] = "libcgj-batch2997";

/* Three product bar3 readiness bits (bits 0..2). */
#define B2997_BAR3_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

/* Popcount of low three bar3 bits (0..3). */
static uint32_t
b2997_progress(uint32_t uMask)
{
	uint32_t uBits;
	uint32_t uProgress;

	uBits = uMask & B2997_BAR3_MASK;
	uProgress = 0u;
	if ((uBits & 0x1u) != 0u) {
		uProgress = uProgress + 1u;
	}
	if ((uBits & 0x2u) != 0u) {
		uProgress = uProgress + 1u;
	}
	if ((uBits & 0x4u) != 0u) {
		uProgress = uProgress + 1u;
	}
	return uProgress;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_progress_u - count set bar3 readiness slots in mask.
 *
 * mask: product bar3 readiness bitmask (bits 0..2 = Steam/Deck/Top50)
 *
 * Returns popcount of (mask & 0x7), always in 0..3. Soft pure-data
 * only; does not probe Steam/Deck. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_progress_u(uint32_t uMask)
{
	(void)NULL;
	return b2997_progress(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_progress_u(uint32_t uMask)
    __attribute__((alias("gj_bar3_progress_u")));
