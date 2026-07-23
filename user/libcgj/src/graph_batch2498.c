/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2498: product bar3 Steam readiness bit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_steam_bit(uint32_t mask);
 *     - Return the Steam readiness bit of product bar3 mask
 *       ((mask & 0x1) != 0) as 0 or 1. Soft pure-data extract for the
 *       Steam host-side product-bar slot; not a runtime Steam probe.
 *   uint32_t __gj_bar3_steam_bit  (alias)
 *   __libcgj_batch2498_marker = "libcgj-batch2498"
 *
 * Milestone 2500 exclusive product helpers (2491-2500). Unique
 * gj_bar3_steam_bit surface only; no multi-def. Distinct from
 * gj_product_bar_steam_hint (batch1991 always-1), gj_product_bar3_bits
 * (batch2398 low-three extract), and gj_bar3_ready_hint (batch2299).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2498_marker[] = "libcgj-batch2498";

/* Steam readiness bit within product bar3 mask (bit0). */
#define B2498_STEAM_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* Extract Steam readiness bit from mask as 0 or 1. */
static uint32_t
b2498_steam_bit(uint32_t uMask)
{
	if ((uMask & B2498_STEAM_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_steam_bit - extract Steam readiness bit from bar3 mask.
 *
 * mask: product bar3 readiness bitmask (bit0 = Steam slot)
 *
 * Returns 1 when bit0 is set, else 0. Pair with gj_bar3_deck_bit and
 * gj_bar3_ready_hint for full bar3 completeness. Does not call libc.
 */
uint32_t
gj_bar3_steam_bit(uint32_t uMask)
{
	(void)NULL;
	return b2498_steam_bit(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_steam_bit(uint32_t uMask)
    __attribute__((alias("gj_bar3_steam_bit")));
