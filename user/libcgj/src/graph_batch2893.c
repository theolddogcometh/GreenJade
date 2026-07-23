/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2893: product bar3 Deck soft-ok (unsigned).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_deck_ok_u(uint32_t mask);
 *     - Return 1 if product bar3 mask has the Deck readiness bit set
 *       (bit1 / 0x2), else 0. Soft pure-data gate; not a hardware Deck
 *       probe. Unsigned 0/1 form.
 *   uint32_t __gj_bar3_deck_ok_u  (alias)
 *   __libcgj_batch2893_marker = "libcgj-batch2893"
 *
 * Milestone 2900 exclusive product helpers (2891-2900). Unique
 * gj_bar3_deck_ok_u surface only; no multi-def. Distinct from
 * gj_bar3_deck_bit (batch2499), gj_bar3_deck_ready_p (batch2673 int),
 * and gj_bar3_set_deck (batch2678).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2893_marker[] = "libcgj-batch2893";

/* Deck readiness bit within product bar3 mask (bit1). */
#define B2893_DECK_BIT  0x2u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if Deck readiness bit is set in mask. */
static uint32_t
b2893_deck_ok(uint32_t uMask)
{
	if ((uMask & B2893_DECK_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_deck_ok_u - 1 iff bar3 Deck readiness bit is set.
 *
 * mask: product bar3 readiness bitmask (bit1 = Deck slot)
 *
 * Returns 1 when bit1 is set, else 0. Soft pure-data only; does not
 * probe Deck hardware. Does not call libc.
 */
uint32_t
gj_bar3_deck_ok_u(uint32_t uMask)
{
	(void)NULL;
	return b2893_deck_ok(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_deck_ok_u(uint32_t uMask)
    __attribute__((alias("gj_bar3_deck_ok_u")));
