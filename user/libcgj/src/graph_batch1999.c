/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1999: dual-license tag soft substring check.
 *
 * Surface (unique symbols):
 *   int gj_dual_license_tag_ok(const char *tag);
 *     — Soft check: return 1 if tag contains the substring "MIT" or
 *       the substring "Apache" (case-sensitive), else 0. NULL → 0.
 *       Matches dual MIT OR Apache-2.0 product tags without parsing
 *       SPDX grammar.
 *   int __gj_dual_license_tag_ok  (alias)
 *   __libcgj_batch1999_marker = "libcgj-batch1999"
 *
 * Product bar helpers exclusive pure-data wave (1991–2000). Clean-room
 * freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1999_marker[] = "libcgj-batch1999";

static const char b1999_mit[] = "MIT";
static const char b1999_apache[] = "Apache";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1999_len(const char *sz)
{
	size_t n;

	n = 0;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/*
 * Return 1 if hay contains needle as a contiguous substring.
 * Empty needle → 1. Neither may be NULL (caller checks).
 */
static int
b1999_has_sub(const char *szHay, const char *szNeedle)
{
	size_t nHay;
	size_t nNeedle;
	size_t i;
	size_t j;

	nHay = b1999_len(szHay);
	nNeedle = b1999_len(szNeedle);
	if (nNeedle == 0u) {
		return 1;
	}
	if (nNeedle > nHay) {
		return 0;
	}
	for (i = 0; i + nNeedle <= nHay; i++) {
		for (j = 0; j < nNeedle; j++) {
			if (szHay[i + j] != szNeedle[j]) {
				break;
			}
		}
		if (j == nNeedle) {
			return 1;
		}
	}
	return 0;
}

/* 1 if tag has "MIT" or "Apache" as a substring. */
static int
b1999_tag_ok(const char *szTag)
{
	if (b1999_has_sub(szTag, b1999_mit)) {
		return 1;
	}
	if (b1999_has_sub(szTag, b1999_apache)) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dual_license_tag_ok — soft MIT/Apache dual-license tag check.
 *
 * tag: license identifier string (SPDX-ish or free text), or NULL.
 * Examples:
 *   "MIT OR Apache-2.0" → 1  (has MIT)
 *   "Apache-2.0"        → 1  (has Apache)
 *   "GPL-2.0-only"      → 0
 *   NULL                 → 0
 */
int
gj_dual_license_tag_ok(const char *szTag)
{
	(void)NULL;
	if (szTag == NULL) {
		return 0;
	}
	return b1999_tag_ok(szTag);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dual_license_tag_ok(const char *szTag)
    __attribute__((alias("gj_dual_license_tag_ok")));
