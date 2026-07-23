/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6011: Steam userdata path component (deepen).
 *
 * Surface (unique symbols):
 *   int gj_path_has_userdata_6011(const char *path);
 *     — Return 1 if path contains a full path component "userdata"
 *       (Steam per-user cloud/local save root), else 0.
 *       NULL / no match → 0. Case-sensitive; '/' is the only separator.
 *   int __gj_path_has_userdata_6011  (alias)
 *   __libcgj_batch6011_marker = "libcgj-batch6011"
 *
 * Post-6000 steam path deepen exclusive CREATE-ONLY (6011-6020).
 * Unique gj_path_has_userdata_6011 surface only; no multi-def.
 * Distinct from gj_path_has_steamapps_5464 and prior path predicates.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6011_marker[] = "libcgj-batch6011";

/* Needle: full path component "userdata" (Steam user data root). */
static const char b6011_needle[] = "userdata";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b6011_len(const char *sz)
{
	size_t n;

	n = 0;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b6011_comp_eq(const char *p)
{
	size_t i;
	size_t nNeedle;

	nNeedle = b6011_len(b6011_needle);
	for (i = 0; i < nNeedle; i++) {
		if (p[i] != b6011_needle[i]) {
			return 0;
		}
	}
	if (p[nNeedle] != '\0' && p[nNeedle] != '/') {
		return 0;
	}
	return 1;
}

static int
b6011_has_comp(const char *szPath)
{
	const char *p;

	p = szPath;
	if (b6011_comp_eq(p)) {
		return 1;
	}
	while (*p != '\0') {
		if (*p == '/') {
			p++;
			if (*p == '\0') {
				break;
			}
			if (b6011_comp_eq(p)) {
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
 * gj_path_has_userdata_6011 — 1 iff path has a "userdata" component.
 *
 * Examples:
 *   ".../Steam/userdata/12345/570" → 1
 *   "userdata"                     → 1
 *   ".../userdataX/..."            → 0
 *   NULL                           → 0
 */
int
gj_path_has_userdata_6011(const char *szPath)
{
	(void)sizeof(NULL);
	if (szPath == NULL) {
		return 0;
	}
	return b6011_has_comp(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_has_userdata_6011(const char *szPath)
    __attribute__((alias("gj_path_has_userdata_6011")));
