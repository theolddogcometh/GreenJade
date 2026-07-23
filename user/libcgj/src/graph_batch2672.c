/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2672: product bar3 Steam ready predicate.
 *
 * Surface (unique symbols):
 *   int gj_bar3_steam_ready_p(uint32_t mask);
 *     - Return 1 if product bar3 mask has the Steam readiness bit set
 *       (bit0), else 0. Soft pure-data gate; not a runtime Steam probe.
 *   int __gj_bar3_steam_ready_p  (alias)
 *   __libcgj_batch2672_marker = "libcgj-batch2672"
 *
 * Milestone 2680 exclusive product bar3 bitmask helpers (2671-2680).
 * Unique gj_bar3_steam_ready_p surface only; no multi-def. Distinct from
 * gj_bar3_steam_bit (batch2498 uint32 extract), gj_product_bar_steam_hint
 * (batch1991 always-1), and gj_bar3_ready_hint (batch2299 full three).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2672_marker[] = "libcgj-batch2672";

/* Steam readiness bit within product bar3 mask (bit0). */
#define B2672_STEAM_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if Steam readiness bit is set in mask. */
static int
b2672_steam_ready_p(uint32_t uMask)
{
	if ((uMask & B2672_STEAM_BIT) != 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_steam_ready_p - 1 iff bar3 Steam readiness bit is set.
 *
 * mask: product bar3 readiness bitmask (bit0 = Steam slot)
 *
 * Returns 1 when bit0 is set, else 0. Does not call libc or probe
 * Steam host installs.
 */
int
gj_bar3_steam_ready_p(uint32_t uMask)
{
	(void)NULL;
	return b2672_steam_ready_p(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bar3_steam_ready_p(uint32_t uMask)
    __attribute__((alias("gj_bar3_steam_ready_p")));
