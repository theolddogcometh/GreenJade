/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6014: Steam appcache path component (deepen).
 *
 * Surface (unique symbols):
 *   int gj_path_has_appcache_6014(const char *path);
 *     — Return 1 if path contains a full path component "appcache"
 *       (Steam client application/library cache root), else 0.
 *       NULL / no match → 0. Case-sensitive; '/' is the only separator.
 *   int __gj_path_has_appcache_6014  (alias)
 *   __libcgj_batch6014_marker = "libcgj-batch6014"
 *
 * Post-6000 steam path deepen exclusive CREATE-ONLY (6011-6020).
 * Unique gj_path_has_appcache_6014 surface only; no multi-def.
 * Distinct from gj_path_has_depotcache_5469 and shadercache (1866).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6014_marker[] = "libcgj-batch6014";

/* Needle: full path component "appcache" (Steam client app cache). */
static const char b6014_needle[] = "appcache";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b6014_len(const char *sz)
{
	size_t n;

	n = 0;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b6014_comp_eq(const char *p)
{
	size_t i;
	size_t nNeedle;

	nNeedle = b6014_len(b6014_needle);
	for (i = 0; i < nNeedle; i++) {
		if (p[i] != b6014_needle[i]) {
			return 0;
		}
	}
	if (p[nNeedle] != '\0' && p[nNeedle] != '/') {
		return 0;
	}
	return 1;
}

static int
b6014_has_comp(const char *szPath)
{
	const char *p;

	p = szPath;
	if (b6014_comp_eq(p)) {
		return 1;
	}
	while (*p != '\0') {
		if (*p == '/') {
			p++;
			if (*p == '\0') {
				break;
			}
			if (b6014_comp_eq(p)) {
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
 * gj_path_has_appcache_6014 — 1 iff path has an "appcache" component.
 *
 * Examples:
 *   ".../Steam/appcache/appinfo.vdf" → 1
 *   "appcache"                       → 1
 *   ".../appcacheX/..."              → 0
 *   NULL                             → 0
 */
int
gj_path_has_appcache_6014(const char *szPath)
{
	(void)sizeof(NULL);
	if (szPath == NULL) {
		return 0;
	}
	return b6014_has_comp(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_has_appcache_6014(const char *szPath)
    __attribute__((alias("gj_path_has_appcache_6014")));
