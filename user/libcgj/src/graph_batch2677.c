/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2677: product bar3 set Steam bit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_set_steam(uint32_t mask);
 *     - Return mask with the Steam readiness bit set (bit0 | 0x1).
 *       Soft pure-data update; not a runtime Steam install.
 *   uint32_t __gj_bar3_set_steam  (alias)
 *   __libcgj_batch2677_marker = "libcgj-batch2677"
 *
 * Milestone 2680 exclusive product bar3 bitmask helpers (2671-2680).
 * Unique gj_bar3_set_steam surface only; no multi-def. Pair with
 * gj_bar3_steam_ready_p (batch2672) and gj_bar3_steam_bit (batch2498).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2677_marker[] = "libcgj-batch2677";

/* Steam readiness bit within product bar3 mask (bit0). */
#define B2677_STEAM_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* Set Steam readiness bit in mask. */
static uint32_t
b2677_set_steam(uint32_t uMask)
{
	return uMask | B2677_STEAM_BIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_set_steam - set Steam readiness bit on bar3 mask.
 *
 * mask: product bar3 readiness bitmask
 *
 * Returns mask | 0x1. Idempotent if bit0 already set. Does not call
 * libc.
 */
uint32_t
gj_bar3_set_steam(uint32_t uMask)
{
	(void)NULL;
	return b2677_set_steam(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_set_steam(uint32_t uMask)
    __attribute__((alias("gj_bar3_set_steam")));
