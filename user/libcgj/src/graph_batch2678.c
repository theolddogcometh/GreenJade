/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2678: product bar3 set Deck bit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_set_deck(uint32_t mask);
 *     - Return mask with the Deck readiness bit set (bit1 | 0x2).
 *       Soft pure-data update; not a hardware Deck detection.
 *   uint32_t __gj_bar3_set_deck  (alias)
 *   __libcgj_batch2678_marker = "libcgj-batch2678"
 *
 * Milestone 2680 exclusive product bar3 bitmask helpers (2671-2680).
 * Unique gj_bar3_set_deck surface only; no multi-def. Pair with
 * gj_bar3_deck_ready_p (batch2673) and gj_bar3_deck_bit (batch2499).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2678_marker[] = "libcgj-batch2678";

/* Deck readiness bit within product bar3 mask (bit1). */
#define B2678_DECK_BIT  0x2u

/* ---- freestanding helpers ---------------------------------------------- */

/* Set Deck readiness bit in mask. */
static uint32_t
b2678_set_deck(uint32_t uMask)
{
	return uMask | B2678_DECK_BIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_set_deck - set Deck readiness bit on bar3 mask.
 *
 * mask: product bar3 readiness bitmask
 *
 * Returns mask | 0x2. Idempotent if bit1 already set. Does not call
 * libc.
 */
uint32_t
gj_bar3_set_deck(uint32_t uMask)
{
	(void)NULL;
	return b2678_set_deck(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_set_deck(uint32_t uMask)
    __attribute__((alias("gj_bar3_set_deck")));
