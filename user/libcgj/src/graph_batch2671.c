/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2671: product bar3 install readiness bit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_bit(uint32_t mask);
 *     - Return the install readiness bit of product bar3 mask
 *       ((mask & 0x8) != 0) as 0 or 1. Soft pure-data extract for the
 *       install-image product-bar slot (bit3); not a runtime ESP probe.
 *   uint32_t __gj_bar3_install_bit  (alias)
 *   __libcgj_batch2671_marker = "libcgj-batch2671"
 *
 * Milestone 2680 exclusive product bar3 bitmask helpers (2671-2680).
 * Unique gj_bar3_install_bit surface only; no multi-def. Distinct from
 * gj_bar3_steam_bit (batch2498 bit0), gj_bar3_deck_bit (batch2499 bit1),
 * and gj_install_ready_bits (batch2598 low-four extract).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2671_marker[] = "libcgj-batch2671";

/* Install readiness bit within product bar3 mask (bit3). */
#define B2671_INSTALL_BIT  0x8u

/* ---- freestanding helpers ---------------------------------------------- */

/* Extract install readiness bit from mask as 0 or 1. */
static uint32_t
b2671_install_bit(uint32_t uMask)
{
	if ((uMask & B2671_INSTALL_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_bit - extract install readiness bit from bar3 mask.
 *
 * mask: product bar3 readiness bitmask (bit3 = install slot)
 *
 * Returns 1 when bit3 is set, else 0. Pair with steam/deck/top50
 * ready_p helpers and gj_bar3_all_ready_p. Does not call libc.
 */
uint32_t
gj_bar3_install_bit(uint32_t uMask)
{
	(void)NULL;
	return b2671_install_bit(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_bit(uint32_t uMask)
    __attribute__((alias("gj_bar3_install_bit")));
