/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1865: Steam compatdata path component hint.
 *
 * Surface (unique symbols):
 *   int gj_steam_path_is_compatdata(const char *path);
 *     — Return 1 if path contains a full path component "compatdata"
 *       (between '/' separators or string bounds), else 0.
 *       NULL / no match → 0. Case-sensitive; '/' is the only separator.
 *   int __gj_steam_path_is_compatdata  (alias)
 *   __libcgj_batch1865_marker = "libcgj-batch1865"
 *
 * Steam/desktop product exclusive pure-data wave (1861–1870).
 * Distinct from prior path predicates. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1865_marker[] = "libcgj-batch1865";

/* Needle: full path component "compatdata" */
static const char b1865_needle[] = "compatdata";

/* ---- freestanding helpers ---------------------------------------------- */

/* Length of a NUL-terminated string (no libc). */
static size_t
b1865_len(const char *sz)
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
b1865_comp_eq(const char *p)
{
	size_t i;
	size_t nNeedle;

	nNeedle = b1865_len(b1865_needle);
	for (i = 0; i < nNeedle; i++) {
		if (p[i] != b1865_needle[i]) {
			return 0;
		}
	}
	if (p[nNeedle] != '\0' && p[nNeedle] != '/') {
		return 0;
	}
	return 1;
}

/* Scan path for a full component equal to needle. */
static int
b1865_has_comp(const char *szPath)
{
	const char *p;

	p = szPath;
	/* Leading component (no leading slash required). */
	if (b1865_comp_eq(p)) {
		return 1;
	}
	while (*p != '\0') {
		if (*p == '/') {
			p++;
			if (*p == '\0') {
				break;
			}
			if (b1865_comp_eq(p)) {
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
 * gj_steam_path_is_compatdata — 1 iff path has a "compatdata" component.
 *
 * Examples:
 *   ".../steamapps/compatdata/570" → 1
 *   "compatdata"                   → 1
 *   ".../compatdataX/..."          → 0  (not a full component)
 *   NULL                           → 0
 */
int
gj_steam_path_is_compatdata(const char *szPath)
{
	(void)sizeof(NULL);
	if (szPath == NULL) {
		return 0;
	}
	return b1865_has_comp(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_steam_path_is_compatdata(const char *szPath)
    __attribute__((alias("gj_steam_path_is_compatdata")));
