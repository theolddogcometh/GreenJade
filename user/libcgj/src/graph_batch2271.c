/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2271: Steam AppID soft-ok predicate (unsigned).
 *
 * Surface (unique symbols):
 *   int gj_steam_appid_ok_u(uint32_t appid);
 *     - Return 1 if appid is a plausible Steam AppID: nonzero and not
 *       the all-bits-one sentinel (0xffffffff). Else 0. Soft catalog-free
 *       hint only; does not consult a store API.
 *   int __gj_steam_appid_ok_u  (alias)
 *   __libcgj_batch2271_marker = "libcgj-batch2271"
 *
 * Post-2270 product exclusive wave (2271-2280). Distinct from
 * gj_appid_is_valid_hint (batch1861) - unique gj_steam_appid_ok_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2271_marker[] = "libcgj-batch2271";

/* All-bits-one is treated as an unset or error AppID sentinel. */
#define B2271_APPID_BAD  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if appid is nonzero and not the bad sentinel. */
static int
b2271_appid_ok(uint32_t uAppid)
{
	if (uAppid == 0u) {
		return 0;
	}
	if (uAppid == B2271_APPID_BAD) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_appid_ok_u - soft check that appid is a plausible Steam AppID.
 *
 * appid: candidate Steam application identifier (unsigned)
 *
 * Returns 1 when 0 < appid and appid != 0xffffffff, else 0.
 * Does not call libc.
 */
int
gj_steam_appid_ok_u(uint32_t uAppid)
{
	(void)NULL;
	return b2271_appid_ok(uAppid);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_steam_appid_ok_u(uint32_t uAppid)
    __attribute__((alias("gj_steam_appid_ok_u")));
