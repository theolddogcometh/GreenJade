/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5717: bar3 Steam client combined ready gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_steam_client_ready_5717(uint32_t mask);
 *     - Return 1 if bar3 Steam-client readiness bit (bit0) is set,
 *       else 0. Combined ready gate for Steam client continuum.
 *   uint32_t __gj_bar3_steam_client_ready_5717  (alias)
 *   __libcgj_batch5717_marker = "libcgj-batch5717"
 *
 * Exclusive CREATE-ONLY (5711-5720: bar3 steam client ready bits).
 * Unique gj_bar3_steam_client_ready_5717 surface only; no multi-def.
 * Distinct from gj_bar3_ready_5700 (batch5697 always-0 continuum),
 * gj_bar3_steam_ready_p (batch2672), and gj_steam_client_ready_p_5712
 * (batch5712). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5717_marker[] = "libcgj-batch5717";

/* Bar3 Steam-client readiness bit (bit0). */
#define B5717_STEAM_CLIENT_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5717_steam_client_ready(uint32_t uMask)
{
	if ((uMask & B5717_STEAM_CLIENT_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_steam_client_ready_5717 - bar3 Steam client combined ready.
 *
 * mask: product bar3 readiness bitmask (bit0 = Steam client slot)
 *
 * Returns 1 when the Steam-client ready bit is set, else 0. Soft
 * pure-data only; does not probe Steam. No parent wires.
 */
uint32_t
gj_bar3_steam_client_ready_5717(uint32_t uMask)
{
	(void)NULL;
	return b5717_steam_client_ready(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_steam_client_ready_5717(uint32_t uMask)
    __attribute__((alias("gj_bar3_steam_client_ready_5717")));
