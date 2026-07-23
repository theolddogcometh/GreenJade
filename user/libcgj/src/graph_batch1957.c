/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1957: MIME type has-slash soft predicate.
 *
 * Surface (unique symbols):
 *   int gj_mime_type_has_slash(const char *type);
 *     — Soft shape check: return 1 if type is non-NULL, contains at
 *       least one '/', and both the type and subtype sides are
 *       non-empty (first and last octets are not '/'; first '/' is
 *       not at either end). Does not validate restricted-name tokens
 *       (see gj_mime_type_ok in batch1127).
 *   int __gj_mime_type_has_slash  (alias)
 *   __libcgj_batch1957_marker = "libcgj-batch1957"
 *
 * Distinct from gj_mime_type_ok (batch1127) — avoid multi-def.
 *
 * Package/install exclusive pure-data wave (1951–1960). Clean-room
 * freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1957_marker[] = "libcgj-batch1957";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if sz has a mid-string '/' with non-empty type and subtype. */
static int
b1957_has_slash(const char *szType)
{
	size_t i;
	size_t iSlash;
	int fFound;

	if (szType == NULL) {
		return 0;
	}
	if (szType[0] == '\0' || szType[0] == '/') {
		return 0;
	}

	fFound = 0;
	iSlash = 0u;
	i = 0u;
	while (szType[i] != '\0') {
		if (szType[i] == '/') {
			if (fFound != 0) {
				/* more than one slash — soft reject */
				return 0;
			}
			fFound = 1;
			iSlash = i;
		}
		i++;
	}
	if (fFound == 0) {
		return 0;
	}
	/* subtype must be non-empty: slash not last octet */
	if (szType[iSlash + 1u] == '\0') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mime_type_has_slash — 1 if type looks like type/subtype shape.
 *
 * type: NUL-terminated media type (NULL / no slash / empty sides → 0)
 * Exactly one '/' with non-empty type and subtype tokens required.
 */
int
gj_mime_type_has_slash(const char *szType)
{
	(void)NULL;
	return b1957_has_slash(szType);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mime_type_has_slash(const char *szType)
    __attribute__((alias("gj_mime_type_has_slash")));
