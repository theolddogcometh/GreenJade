/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1866: Steam shadercache path component hint.
 *
 * Surface (unique symbols):
 *   int gj_steam_path_is_shadercache(const char *path);
 *     — Return 1 if path contains a full path component "shadercache"
 *       (between '/' separators or string bounds), else 0.
 *       NULL / no match → 0. Case-sensitive; '/' is the only separator.
 *   int __gj_steam_path_is_shadercache  (alias)
 *   __libcgj_batch1866_marker = "libcgj-batch1866"
 *
 * Steam/desktop product exclusive pure-data wave (1861–1870).
 * Distinct from gj_steam_path_is_compatdata. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1866_marker[] = "libcgj-batch1866";

/* Needle: full path component "shadercache" */
static const char b1866_needle[] = "shadercache";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1866_len(const char *sz)
{
	size_t n;

	n = 0;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/* True if path component starting at p equals needle and ends at '/' or NUL. */
static int
b1866_comp_eq(const char *p)
{
	size_t i;
	size_t nNeedle;

	nNeedle = b1866_len(b1866_needle);
	for (i = 0; i < nNeedle; i++) {
		if (p[i] != b1866_needle[i]) {
			return 0;
		}
	}
	if (p[nNeedle] != '\0' && p[nNeedle] != '/') {
		return 0;
	}
	return 1;
}

static int
b1866_has_comp(const char *szPath)
{
	const char *p;

	p = szPath;
	if (b1866_comp_eq(p)) {
		return 1;
	}
	while (*p != '\0') {
		if (*p == '/') {
			p++;
			if (*p == '\0') {
				break;
			}
			if (b1866_comp_eq(p)) {
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
 * gj_steam_path_is_shadercache — 1 iff path has a "shadercache" component.
 *
 * Examples:
 *   ".../steamapps/shadercache/570" → 1
 *   "shadercache"                   → 1
 *   ".../shadercacheX/..."          → 0
 *   NULL                            → 0
 */
int
gj_steam_path_is_shadercache(const char *szPath)
{
	(void)sizeof(NULL);
	if (szPath == NULL) {
		return 0;
	}
	return b1866_has_comp(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_steam_path_is_shadercache(const char *szPath)
    __attribute__((alias("gj_steam_path_is_shadercache")));
