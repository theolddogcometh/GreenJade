/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6015: Steam sourcemods path component (deepen).
 *
 * Surface (unique symbols):
 *   int gj_path_has_sourcemods_6015(const char *path);
 *     — Return 1 if path contains a full path component "sourcemods"
 *       (steamapps/sourcemods Source engine mod layout), else 0.
 *       NULL / no match → 0. Case-sensitive; '/' is the only separator.
 *   int __gj_path_has_sourcemods_6015  (alias)
 *   __libcgj_batch6015_marker = "libcgj-batch6015"
 *
 * Post-6000 steam path deepen exclusive CREATE-ONLY (6011-6020).
 * Unique gj_path_has_sourcemods_6015 surface only; no multi-def.
 * Distinct from gj_path_has_steam_common_5465 and workshop paths.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6015_marker[] = "libcgj-batch6015";

/* Needle: full path component "sourcemods" (Source engine mods). */
static const char b6015_needle[] = "sourcemods";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b6015_len(const char *sz)
{
	size_t n;

	n = 0;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b6015_comp_eq(const char *p)
{
	size_t i;
	size_t nNeedle;

	nNeedle = b6015_len(b6015_needle);
	for (i = 0; i < nNeedle; i++) {
		if (p[i] != b6015_needle[i]) {
			return 0;
		}
	}
	if (p[nNeedle] != '\0' && p[nNeedle] != '/') {
		return 0;
	}
	return 1;
}

static int
b6015_has_comp(const char *szPath)
{
	const char *p;

	p = szPath;
	if (b6015_comp_eq(p)) {
		return 1;
	}
	while (*p != '\0') {
		if (*p == '/') {
			p++;
			if (*p == '\0') {
				break;
			}
			if (b6015_comp_eq(p)) {
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
 * gj_path_has_sourcemods_6015 — 1 iff path has a "sourcemods" component.
 *
 * Examples:
 *   ".../steamapps/sourcemods/MyMod" → 1
 *   "sourcemods"                     → 1
 *   ".../sourcemodsX/..."            → 0
 *   NULL                             → 0
 */
int
gj_path_has_sourcemods_6015(const char *szPath)
{
	(void)sizeof(NULL);
	if (szPath == NULL) {
		return 0;
	}
	return b6015_has_comp(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_has_sourcemods_6015(const char *szPath)
    __attribute__((alias("gj_path_has_sourcemods_6015")));
