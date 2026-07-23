/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2892: product bar3 Steam soft-ok (unsigned).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_steam_ok_u(uint32_t mask);
 *     - Return 1 if product bar3 mask has the Steam readiness bit set
 *       (bit0 / 0x1), else 0. Soft pure-data gate; not a runtime Steam
 *       host probe. Unsigned 0/1 form.
 *   uint32_t __gj_bar3_steam_ok_u  (alias)
 *   __libcgj_batch2892_marker = "libcgj-batch2892"
 *
 * Milestone 2900 exclusive product helpers (2891-2900). Unique
 * gj_bar3_steam_ok_u surface only; no multi-def. Distinct from
 * gj_bar3_steam_bit (batch2498), gj_bar3_steam_ready_p (batch2672 int),
 * and gj_bar3_set_steam (batch2677).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2892_marker[] = "libcgj-batch2892";

/* Steam readiness bit within product bar3 mask (bit0). */
#define B2892_STEAM_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if Steam readiness bit is set in mask. */
static uint32_t
b2892_steam_ok(uint32_t uMask)
{
	if ((uMask & B2892_STEAM_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_steam_ok_u - 1 iff bar3 Steam readiness bit is set.
 *
 * mask: product bar3 readiness bitmask (bit0 = Steam slot)
 *
 * Returns 1 when bit0 is set, else 0. Soft pure-data only; does not
 * probe Steam. Does not call libc.
 */
uint32_t
gj_bar3_steam_ok_u(uint32_t uMask)
{
	(void)NULL;
	return b2892_steam_ok(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_steam_ok_u(uint32_t uMask)
    __attribute__((alias("gj_bar3_steam_ok_u")));
