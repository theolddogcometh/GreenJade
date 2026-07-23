/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6012: Steam workshop path component (deepen).
 *
 * Surface (unique symbols):
 *   int gj_path_has_workshop_6012(const char *path);
 *     — Return 1 if path contains a full path component "workshop"
 *       (steamapps/workshop content layout), else 0.
 *       NULL / no match → 0. Case-sensitive; '/' is the only separator.
 *   int __gj_path_has_workshop_6012  (alias)
 *   __libcgj_batch6012_marker = "libcgj-batch6012"
 *
 * Post-6000 steam path deepen exclusive CREATE-ONLY (6011-6020).
 * Unique gj_path_has_workshop_6012 surface only; no multi-def.
 * Distinct from gj_path_has_steamapps_5464 and gj_path_has_userdata_6011.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6012_marker[] = "libcgj-batch6012";

/* Needle: full path component "workshop" (Steam Workshop content). */
static const char b6012_needle[] = "workshop";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b6012_len(const char *sz)
{
	size_t n;

	n = 0;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b6012_comp_eq(const char *p)
{
	size_t i;
	size_t nNeedle;

	nNeedle = b6012_len(b6012_needle);
	for (i = 0; i < nNeedle; i++) {
		if (p[i] != b6012_needle[i]) {
			return 0;
		}
	}
	if (p[nNeedle] != '\0' && p[nNeedle] != '/') {
		return 0;
	}
	return 1;
}

static int
b6012_has_comp(const char *szPath)
{
	const char *p;

	p = szPath;
	if (b6012_comp_eq(p)) {
		return 1;
	}
	while (*p != '\0') {
		if (*p == '/') {
			p++;
			if (*p == '\0') {
				break;
			}
			if (b6012_comp_eq(p)) {
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
 * gj_path_has_workshop_6012 — 1 iff path has a "workshop" component.
 *
 * Examples:
 *   ".../steamapps/workshop/content/570" → 1
 *   "workshop"                           → 1
 *   ".../workshopX/..."                  → 0
 *   NULL                                 → 0
 */
int
gj_path_has_workshop_6012(const char *szPath)
{
	(void)sizeof(NULL);
	if (szPath == NULL) {
		return 0;
	}
	return b6012_has_comp(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_has_workshop_6012(const char *szPath)
    __attribute__((alias("gj_path_has_workshop_6012")));
