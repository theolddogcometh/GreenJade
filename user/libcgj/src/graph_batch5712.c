/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5712: Steam client ready predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_client_ready_p_5712(uint32_t mask);
 *     - Return 1 if product bar3 mask has the Steam-client readiness
 *       bit set (bit0), else 0. Soft pure-data gate; not a runtime
 *       Steam client install check.
 *   uint32_t __gj_steam_client_ready_p_5712  (alias)
 *   __libcgj_batch5712_marker = "libcgj-batch5712"
 *
 * Exclusive CREATE-ONLY (5711-5720: bar3 steam client ready bits).
 * Unique gj_steam_client_ready_p_5712 surface only; no multi-def.
 * Distinct from gj_bar3_steam_ready_p (batch2672 int), 
 * gj_bar3_steam_client_bit_5711 (batch5711 extract), and
 * gj_product_bar_steam_hint (batch1991 always-1). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5712_marker[] = "libcgj-batch5712";

/* Steam-client readiness bit within product bar3 mask (bit0). */
#define B5712_STEAM_CLIENT_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if Steam-client readiness bit is set in mask. */
static uint32_t
b5712_steam_client_ready_p(uint32_t uMask)
{
	if ((uMask & B5712_STEAM_CLIENT_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_client_ready_p_5712 - 1 iff Steam-client ready bit is set.
 *
 * mask: product bar3 readiness bitmask (bit0 = Steam client slot)
 *
 * Returns 1 when bit0 is set, else 0. Pair with
 * gj_bar3_steam_client_bit_5711 for extract vs predicate forms.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_steam_client_ready_p_5712(uint32_t uMask)
{
	(void)NULL;
	return b5712_steam_client_ready_p(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_client_ready_p_5712(uint32_t uMask)
    __attribute__((alias("gj_steam_client_ready_p_5712")));
