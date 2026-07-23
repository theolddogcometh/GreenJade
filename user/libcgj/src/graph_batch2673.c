/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2673: product bar3 Deck ready predicate.
 *
 * Surface (unique symbols):
 *   int gj_bar3_deck_ready_p(uint32_t mask);
 *     - Return 1 if product bar3 mask has the Deck readiness bit set
 *       (bit1), else 0. Soft pure-data gate; not a hardware Deck probe.
 *   int __gj_bar3_deck_ready_p  (alias)
 *   __libcgj_batch2673_marker = "libcgj-batch2673"
 *
 * Milestone 2680 exclusive product bar3 bitmask helpers (2671-2680).
 * Unique gj_bar3_deck_ready_p surface only; no multi-def. Distinct from
 * gj_bar3_deck_bit (batch2499 uint32 extract), gj_product_bar_deck_hint
 * (batch1992 always-1), and gj_bar3_steam_ready_p (batch2672 bit0).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2673_marker[] = "libcgj-batch2673";

/* Deck readiness bit within product bar3 mask (bit1). */
#define B2673_DECK_BIT  0x2u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if Deck readiness bit is set in mask. */
static int
b2673_deck_ready_p(uint32_t uMask)
{
	if ((uMask & B2673_DECK_BIT) != 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_deck_ready_p - 1 iff bar3 Deck readiness bit is set.
 *
 * mask: product bar3 readiness bitmask (bit1 = Deck slot)
 *
 * Returns 1 when bit1 is set, else 0. Does not call libc or probe
 * Steam Deck hardware.
 */
int
gj_bar3_deck_ready_p(uint32_t uMask)
{
	(void)NULL;
	return b2673_deck_ready_p(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bar3_deck_ready_p(uint32_t uMask)
    __attribute__((alias("gj_bar3_deck_ready_p")));
