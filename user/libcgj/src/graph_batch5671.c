/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5671: game launch appid validate stub.
 *
 * Surface (unique symbols):
 *   int gj_appid_validate_5671(uint32_t appid);
 *     - Return 1 if appid is a plausible Steam AppID: nonzero and not
 *       the reserved soft-sentinel 0xFFFFFFFFu, else 0. Pure-data gate.
 *   int __gj_appid_validate_5671  (alias)
 *   __libcgj_batch5671_marker = "libcgj-batch5671"
 *
 * Game launch pipeline (appid validate / depot mount / process spawn
 * stub scores) exclusive CREATE-ONLY wave (5671-5680). Unique
 * gj_appid_validate_5671 surface only; no multi-def. Distinct from
 * gj_appid_is_valid_hint (batch1861) and gj_steam_appid_ok_u
 * (batch2271). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5671_marker[] = "libcgj-batch5671";

/* Soft reserved sentinel; never a real catalog AppID in this stub. */
#define B5671_APPID_BAD  0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5671_appid_ok(uint32_t uAppid)
{
	if (uAppid == 0u) {
		return 0;
	}
	if (uAppid == B5671_APPID_BAD) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_appid_validate_5671 - soft check that appid is launch-plausible.
 *
 * appid: Steam AppID as an unsigned 32-bit value.
 * Returns 1 when appid is nonzero and not 0xFFFFFFFF, else 0.
 * Does not consult a catalog. Does not call libc. No parent wires.
 */
int
gj_appid_validate_5671(uint32_t uAppid)
{
	(void)NULL;
	return b5671_appid_ok(uAppid);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_appid_validate_5671(uint32_t uAppid)
    __attribute__((alias("gj_appid_validate_5671")));
