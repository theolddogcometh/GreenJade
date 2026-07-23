/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2499: product bar3 Deck readiness bit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_deck_bit(uint32_t mask);
 *     - Return the Deck readiness bit of product bar3 mask
 *       ((mask & 0x2) != 0) as 0 or 1. Soft pure-data extract for the
 *       Steam Deck / handheld product-bar slot; not a hardware probe.
 *   uint32_t __gj_bar3_deck_bit  (alias)
 *   __libcgj_batch2499_marker = "libcgj-batch2499"
 *
 * Milestone 2500 exclusive product helpers (2491-2500). Unique
 * gj_bar3_deck_bit surface only; no multi-def. Distinct from
 * gj_bar3_steam_bit (batch2498 bit0), gj_product_bar_deck_hint
 * (batch1992 always-1), gj_product_bar3_bits (batch2398), and
 * gj_bar3_ready_hint (batch2299).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2499_marker[] = "libcgj-batch2499";

/* Deck readiness bit within product bar3 mask (bit1). */
#define B2499_DECK_BIT  0x2u

/* ---- freestanding helpers ---------------------------------------------- */

/* Extract Deck readiness bit from mask as 0 or 1. */
static uint32_t
b2499_deck_bit(uint32_t uMask)
{
	if ((uMask & B2499_DECK_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_deck_bit - extract Deck readiness bit from bar3 mask.
 *
 * mask: product bar3 readiness bitmask (bit1 = Deck slot)
 *
 * Returns 1 when bit1 is set, else 0. Pair with gj_bar3_steam_bit and
 * gj_bar3_ready_hint for full bar3 completeness. Does not call libc.
 */
uint32_t
gj_bar3_deck_bit(uint32_t uMask)
{
	(void)NULL;
	return b2499_deck_bit(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_deck_bit(uint32_t uMask)
    __attribute__((alias("gj_bar3_deck_bit")));
