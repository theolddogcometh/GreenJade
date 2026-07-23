/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5467: Steam compatdata path component (suffixed).
 *
 * Surface (unique symbols):
 *   int gj_path_has_compatdata_5467(const char *path);
 *     — Return 1 if path contains a full path component "compatdata"
 *       (steamapps/compatdata/<appid> Proton prefix layout), else 0.
 *       NULL / no match → 0. Case-sensitive; '/' is the only separator.
 *   int __gj_path_has_compatdata_5467  (alias)
 *   __libcgj_batch5467_marker = "libcgj-batch5467"
 *
 * Steam network/filesystem exclusive pure-data wave (5461–5470).
 * Distinct from gj_steam_path_is_compatdata (batch1865). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5467_marker[] = "libcgj-batch5467";

/* Needle: full path component "compatdata". */
static const char b5467_needle[] = "compatdata";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b5467_len(const char *sz)
{
	size_t n;

	n = 0;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b5467_comp_eq(const char *p)
{
	size_t i;
	size_t nNeedle;

	nNeedle = b5467_len(b5467_needle);
	for (i = 0; i < nNeedle; i++) {
		if (p[i] != b5467_needle[i]) {
			return 0;
		}
	}
	if (p[nNeedle] != '\0' && p[nNeedle] != '/') {
		return 0;
	}
	return 1;
}

static int
b5467_has_comp(const char *szPath)
{
	const char *p;

	p = szPath;
	if (b5467_comp_eq(p)) {
		return 1;
	}
	while (*p != '\0') {
		if (*p == '/') {
			p++;
			if (*p == '\0') {
				break;
			}
			if (b5467_comp_eq(p)) {
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
 * gj_path_has_compatdata_5467 — 1 iff path has a "compatdata" component.
 *
 * Examples:
 *   ".../steamapps/compatdata/570" → 1
 *   "compatdata"                   → 1
 *   ".../compatdataX/..."          → 0
 *   NULL                           → 0
 */
int
gj_path_has_compatdata_5467(const char *szPath)
{
	(void)sizeof(NULL);
	if (szPath == NULL) {
		return 0;
	}
	return b5467_has_comp(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_has_compatdata_5467(const char *szPath)
    __attribute__((alias("gj_path_has_compatdata_5467")));
