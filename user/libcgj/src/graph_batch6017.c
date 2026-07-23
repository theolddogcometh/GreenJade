/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6017: Steam steamapps/temp path component (deepen).
 *
 * Surface (unique symbols):
 *   int gj_path_has_steam_temp_6017(const char *path);
 *     — Return 1 if path contains a full path component "temp"
 *       (steamapps/temp transient download/staging layout), else 0.
 *       NULL / no match → 0. Case-sensitive; '/' is the only separator.
 *   int __gj_path_has_steam_temp_6017  (alias)
 *   __libcgj_batch6017_marker = "libcgj-batch6017"
 *
 * Post-6000 steam path deepen exclusive CREATE-ONLY (6011-6020).
 * Unique gj_path_has_steam_temp_6017 surface only; no multi-def.
 * Distinct from gj_path_has_downloading_6013 and depotcache (5469).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6017_marker[] = "libcgj-batch6017";

/* Needle: full path component "temp" (Steam staging/temp under steamapps). */
static const char b6017_needle[] = "temp";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b6017_len(const char *sz)
{
	size_t n;

	n = 0;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b6017_comp_eq(const char *p)
{
	size_t i;
	size_t nNeedle;

	nNeedle = b6017_len(b6017_needle);
	for (i = 0; i < nNeedle; i++) {
		if (p[i] != b6017_needle[i]) {
			return 0;
		}
	}
	if (p[nNeedle] != '\0' && p[nNeedle] != '/') {
		return 0;
	}
	return 1;
}

static int
b6017_has_comp(const char *szPath)
{
	const char *p;

	p = szPath;
	if (b6017_comp_eq(p)) {
		return 1;
	}
	while (*p != '\0') {
		if (*p == '/') {
			p++;
			if (*p == '\0') {
				break;
			}
			if (b6017_comp_eq(p)) {
				return 1;
			}
		} else {
			p++;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_has_steam_temp_6017 — 1 iff path has a "temp" component.
 *
 * Examples:
 *   ".../steamapps/temp/570" → 1
 *   "temp"                   → 1
 *   ".../tempX/..."          → 0
 *   NULL                     → 0
 */
int
gj_path_has_steam_temp_6017(const char *szPath)
{
	(void)sizeof(NULL);
	if (szPath == NULL) {
		return 0;
	}
	return b6017_has_comp(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_has_steam_temp_6017(const char *szPath)
    __attribute__((alias("gj_path_has_steam_temp_6017")));
