/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5702: bar3 checklist Steam bit extract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_checklist_steam_bit_5702(uint32_t mask);
 *     - Return the Steam checklist bit of product bar3 mask
 *       ((mask & 0x1) != 0) as 0 or 1. Soft pure-data extract.
 *   uint32_t __gj_bar3_checklist_steam_bit_5702  (alias)
 *   __libcgj_batch5702_marker = "libcgj-batch5702"
 *
 * Exclusive continuum CREATE-ONLY (5701-5710: bar3 install checklist
 * bits). Unique gj_bar3_checklist_steam_bit_5702 surface only; no
 * multi-def. Distinct from gj_bar3_steam_bit (batch2498) and
 * gj_bar3_checklist_full_mask_5701. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5702_marker[] = "libcgj-batch5702";

/* Steam readiness bit within product bar3 checklist (bit0). */
#define B5702_STEAM_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5702_steam_bit(uint32_t uMask)
{
	if ((uMask & B5702_STEAM_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_checklist_steam_bit_5702 - extract Steam checklist bit.
 *
 * mask: product bar3 install-checklist bitmask (bit0 = Steam slot)
 *
 * Returns 1 when bit0 is set, else 0. Soft pure-data extract; not a
 * runtime Steam probe. No parent wires.
 */
uint32_t
gj_bar3_checklist_steam_bit_5702(uint32_t uMask)
{
	(void)NULL;
	return b5702_steam_bit(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_checklist_steam_bit_5702(uint32_t uMask)
    __attribute__((alias("gj_bar3_checklist_steam_bit_5702")));
