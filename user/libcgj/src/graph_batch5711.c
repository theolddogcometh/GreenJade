/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5711: bar3 Steam client readiness bit extract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_steam_client_bit_5711(uint32_t mask);
 *     - Return the Steam-client readiness bit of product bar3 mask
 *       ((mask & 0x1) != 0) as 0 or 1. Soft pure-data extract for the
 *       Steam client product-bar slot; not a runtime Steam probe.
 *   uint32_t __gj_bar3_steam_client_bit_5711  (alias)
 *   __libcgj_batch5711_marker = "libcgj-batch5711"
 *
 * Exclusive CREATE-ONLY (5711-5720: bar3 steam client ready bits).
 * Unique gj_bar3_steam_client_bit_5711 surface only; no multi-def.
 * Distinct from gj_bar3_steam_bit (batch2498), gj_bar3_steam_ready_p
 * (batch2672), and gj_bar3_steam_ok_u (batch2892). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5711_marker[] = "libcgj-batch5711";

/* Steam-client readiness bit within product bar3 mask (bit0). */
#define B5711_STEAM_CLIENT_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* Extract Steam-client readiness bit from mask as 0 or 1. */
static uint32_t
b5711_steam_client_bit(uint32_t uMask)
{
	if ((uMask & B5711_STEAM_CLIENT_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_steam_client_bit_5711 - extract Steam-client ready bit.
 *
 * mask: product bar3 readiness bitmask (bit0 = Steam client slot)
 *
 * Returns 1 when bit0 is set, else 0. Soft pure-data only; does not
 * call libc or probe Steam host installs. No parent wires.
 */
uint32_t
gj_bar3_steam_client_bit_5711(uint32_t uMask)
{
	(void)NULL;
	return b5711_steam_client_bit(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_steam_client_bit_5711(uint32_t uMask)
    __attribute__((alias("gj_bar3_steam_client_bit_5711")));
