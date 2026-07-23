/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5713: bar3 Steam product-slot ok gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_steam_slot_ok_5713(uint32_t mask);
 *     - Return 1 if bar3 Steam product-slot bit (bit0) is set, else 0.
 *       Soft slot-level gate for Steam client continuum wiring.
 *   uint32_t __gj_bar3_steam_slot_ok_5713  (alias)
 *   __libcgj_batch5713_marker = "libcgj-batch5713"
 *
 * Exclusive CREATE-ONLY (5711-5720: bar3 steam client ready bits).
 * Unique gj_bar3_steam_slot_ok_5713 surface only; no multi-def.
 * Distinct from gj_bar3_steam_ok_u (batch2892), gj_bar3_steam_bit
 * (batch2498), and gj_steam_client_ready_p_5712 (batch5712).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5713_marker[] = "libcgj-batch5713";

/* Bar3 Steam product-slot bit (bit0). */
#define B5713_STEAM_SLOT_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5713_steam_slot_ok(uint32_t uMask)
{
	if ((uMask & B5713_STEAM_SLOT_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_steam_slot_ok_5713 - bar3 Steam product-slot ok gate.
 *
 * mask: product bar3 readiness bitmask (bit0 = Steam slot)
 *
 * Returns 1 when the Steam slot bit is set, else 0. Soft pure-data
 * only; does not probe Steam installs. No parent wires.
 */
uint32_t
gj_bar3_steam_slot_ok_5713(uint32_t uMask)
{
	(void)NULL;
	return b5713_steam_slot_ok(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_steam_slot_ok_5713(uint32_t uMask)
    __attribute__((alias("gj_bar3_steam_slot_ok_5713")));
