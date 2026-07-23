/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5703: bar3 checklist Deck bit extract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_checklist_deck_bit_5703(uint32_t mask);
 *     - Return the Deck checklist bit of product bar3 mask
 *       ((mask & 0x2) != 0) as 0 or 1. Soft pure-data extract.
 *   uint32_t __gj_bar3_checklist_deck_bit_5703  (alias)
 *   __libcgj_batch5703_marker = "libcgj-batch5703"
 *
 * Exclusive continuum CREATE-ONLY (5701-5710: bar3 install checklist
 * bits). Unique gj_bar3_checklist_deck_bit_5703 surface only; no
 * multi-def. Distinct from gj_bar3_deck_bit (batch2499) and
 * gj_bar3_checklist_steam_bit_5702. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5703_marker[] = "libcgj-batch5703";

/* Deck readiness bit within product bar3 checklist (bit1). */
#define B5703_DECK_BIT  0x2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5703_deck_bit(uint32_t uMask)
{
	if ((uMask & B5703_DECK_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_checklist_deck_bit_5703 - extract Deck checklist bit.
 *
 * mask: product bar3 install-checklist bitmask (bit1 = Deck slot)
 *
 * Returns 1 when bit1 is set, else 0. Soft pure-data extract; not a
 * hardware Deck probe. No parent wires.
 */
uint32_t
gj_bar3_checklist_deck_bit_5703(uint32_t uMask)
{
	(void)NULL;
	return b5703_deck_bit(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_checklist_deck_bit_5703(uint32_t uMask)
    __attribute__((alias("gj_bar3_checklist_deck_bit_5703")));
