/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2633: Steam client binary basename soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_steam_bin_ok(const char *name);
 *     - Return 1 if name is a non-NULL non-empty basename of length
 *       1..64 with no '/', using only [A-Za-z0-9._-], suitable as a
 *       Steam client binary name (e.g. "steam", "steam.sh"). Soft
 *       shape check; does not resolve or exec.
 *   int __gj_steam_bin_ok  (alias)
 *   __libcgj_batch2633_marker = "libcgj-batch2633"
 *
 * Product path exclusive wave (2631-2640). Distinct from
 * gj_steam_root_ok (batch2365 install root path) and
 * gj_steam_appid_ok_u (batch2271) — unique gj_steam_bin_ok surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2633_marker[] = "libcgj-batch2633";

/* Max basename length for a Steam client binary product name. */
#define B2633_NAME_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if ch is a permitted Steam binary basename character. */
static int
b2633_ch_ok(char ch)
{
	if (ch >= 'a' && ch <= 'z') {
		return 1;
	}
	if (ch >= 'A' && ch <= 'Z') {
		return 1;
	}
	if (ch >= '0' && ch <= '9') {
		return 1;
	}
	if (ch == '.' || ch == '-' || ch == '_') {
		return 1;
	}
	return 0;
}

/* Soft basename shape check for Steam client binary names. */
static int
b2633_bin_ok(const char *szName)
{
	size_t nLen;

	if (szName == NULL) {
		return 0;
	}
	nLen = 0u;
	while (szName[nLen] != '\0') {
		if (nLen >= (size_t)B2633_NAME_MAX) {
			return 0;
		}
		if (szName[nLen] == '/') {
			return 0;
		}
		if (!b2633_ch_ok(szName[nLen])) {
			return 0;
		}
		nLen++;
	}
	if (nLen == 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_bin_ok - soft check for a Steam client binary basename.
 *
 * name: candidate basename (e.g. "steam", "steam.sh"); not a path
 *
 * Returns 1 when name is non-empty, length <= 64, has no '/', and uses
 * only [A-Za-z0-9._-], else 0. Does not call libc.
 */
int
gj_steam_bin_ok(const char *szName)
{
	(void)NULL;
	return b2633_bin_ok(szName);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_steam_bin_ok(const char *szName)
    __attribute__((alias("gj_steam_bin_ok")));
