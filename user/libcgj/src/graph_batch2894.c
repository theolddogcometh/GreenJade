/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2894: product bar3 Top50 soft-ok (unsigned).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_top50_ok_u(uint32_t mask);
 *     - Return 1 if product bar3 mask has the Deck Top 50 readiness
 *       bit set (bit2 / 0x4), else 0. Soft pure-data gate; does not
 *       claim matrix title PASS. Unsigned 0/1 form.
 *   uint32_t __gj_bar3_top50_ok_u  (alias)
 *   __libcgj_batch2894_marker = "libcgj-batch2894"
 *
 * Milestone 2900 exclusive product helpers (2891-2900). Unique
 * gj_bar3_top50_ok_u surface only; no multi-def. Distinct from
 * gj_bar3_top50_ready_p (batch2674 int), gj_bar3_set_top50
 * (batch2679), and gj_product_bar_top50_hint (batch1993).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2894_marker[] = "libcgj-batch2894";

/* Top50 readiness bit within product bar3 mask (bit2). */
#define B2894_TOP50_BIT  0x4u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if Top50 readiness bit is set in mask. */
static uint32_t
b2894_top50_ok(uint32_t uMask)
{
	if ((uMask & B2894_TOP50_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_top50_ok_u - 1 iff bar3 Top50 readiness bit is set.
 *
 * mask: product bar3 readiness bitmask (bit2 = Top50 slot)
 *
 * Returns 1 when bit2 is set, else 0. Soft pure-data only; does not
 * claim matrix title PASS. Does not call libc.
 */
uint32_t
gj_bar3_top50_ok_u(uint32_t uMask)
{
	(void)NULL;
	return b2894_top50_ok(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_top50_ok_u(uint32_t uMask)
    __attribute__((alias("gj_bar3_top50_ok_u")));
