/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6019: Steam AppID path-component soft-ok (deepen).
 *
 * Surface (unique symbols):
 *   int gj_steam_path_comp_is_appid_6019(const char *comp);
 *     — Return 1 if comp is a non-empty all-decimal-digit path component
 *       (plausible Steam AppID / account id directory name), else 0.
 *       NULL / empty / non-digit → 0. Does not parse bounds beyond digits.
 *   int __gj_steam_path_comp_is_appid_6019  (alias)
 *   __libcgj_batch6019_marker = "libcgj-batch6019"
 *
 * Post-6000 steam path deepen exclusive CREATE-ONLY (6011-6020).
 * Unique gj_steam_path_comp_is_appid_6019 surface only; no multi-def.
 * Distinct from gj_proton_prefix_ok_u (batch2272) and path-has component
 * predicates. Soft pure-data only; no parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6019_marker[] = "libcgj-batch6019";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if ch is ASCII decimal digit. */
static int
b6019_is_digit(char ch)
{
	return (ch >= '0' && ch <= '9') ? 1 : 0;
}

/*
 * True if sz is non-empty and every character up to the first '/' or NUL
 * is a decimal digit (single path component, not a multi-component path).
 */
static int
b6019_comp_is_appid(const char *sz)
{
	size_t i;

	if (sz[0] == '\0' || sz[0] == '/') {
		return 0;
	}
	i = 0;
	while (sz[i] != '\0' && sz[i] != '/') {
		if (!b6019_is_digit(sz[i])) {
			return 0;
		}
		i++;
	}
	/* Reject multi-component inputs that continue after '/'. */
	if (sz[i] == '/') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_path_comp_is_appid_6019 — 1 iff comp is all-digit AppID-like.
 *
 * Examples:
 *   "570"     → 1
 *   "123456"  → 1
 *   "0"       → 1
 *   "Game"    → 0
 *   "57a0"    → 0
 *   "570/pfx" → 0  (multi-component)
 *   ""        → 0
 *   NULL      → 0
 */
int
gj_steam_path_comp_is_appid_6019(const char *szComp)
{
	(void)sizeof(NULL);
	if (szComp == NULL) {
		return 0;
	}
	return b6019_comp_is_appid(szComp);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_steam_path_comp_is_appid_6019(const char *szComp)
    __attribute__((alias("gj_steam_path_comp_is_appid_6019")));
