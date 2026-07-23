/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6525: Top50 title Steam AppID soft-ok gate.
 *
 * Surface (unique symbols):
 *   int gj_top50_title_appid_ok_6525(uint32_t appid);
 *     - Return 1 if appid is a non-zero 31-bit-or-less AppID suitable
 *       for a Deck Top 50 matrix title row (1..0x7fffffff), else 0.
 *       Soft pure-data gate only; does not query Steam.
 *   int __gj_top50_title_appid_ok_6525  (alias)
 *   __libcgj_batch6525_marker = "libcgj-batch6525"
 *
 * Exclusive continuum CREATE-ONLY (6521-6530: post-6500 top50 title
 * deepen). Unique gj_top50_title_appid_ok_6525 surface only; no
 * multi-def. Distinct from gj_game_id_ok_u (batch2636-class). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6525_marker[] = "libcgj-batch6525";

/* Non-zero AppID upper bound (signed-positive 32-bit range). */
#define B6525_APPID_MIN  1u
#define B6525_APPID_MAX  0x7fffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6525_appid_ok(uint32_t uAppid)
{
	if (uAppid < B6525_APPID_MIN) {
		return 0;
	}
	if (uAppid > B6525_APPID_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_top50_title_appid_ok_6525 - soft check Top50 title Steam AppID.
 *
 * appid: Steam application id for a matrix title row.
 * Returns 1 when appid is in [1, 0x7fffffff], else 0.
 * Does not query a store. Does not call libc. No parent wires.
 */
int
gj_top50_title_appid_ok_6525(uint32_t uAppid)
{
	(void)NULL;
	return b6525_appid_ok(uAppid);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_top50_title_appid_ok_6525(uint32_t uAppid)
    __attribute__((alias("gj_top50_title_appid_ok_6525")));
