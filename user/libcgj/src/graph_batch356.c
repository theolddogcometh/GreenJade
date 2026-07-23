/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch356: NUL-terminated string prefix / suffix
 * predicates (starts-with / ends-with).
 *
 * Surface (unique symbols):
 *   int gj_starts_with(const char *s, const char *pfx);
 *     — Return 1 if s begins with every octet of pfx (including when
 *       pfx is empty), else 0. NULL s or NULL pfx returns 0.
 *   int gj_ends_with(const char *s, const char *sfx);
 *     — Return 1 if s ends with every octet of sfx (including when
 *       sfx is empty), else 0. NULL s or NULL sfx returns 0.
 *   __gj_starts_with / __gj_ends_with  (aliases)
 *   __libcgj_batch356_marker = "libcgj-batch356"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch356_marker[] = "libcgj-batch356";

/* ---- freestanding helpers ---------------------------------------------- */

/* Byte length of a NUL-terminated string (NULL → 0). */
static size_t
b356_strlen(const char *sz)
{
	size_t cb = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[cb] != '\0') {
		cb++;
	}
	return cb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_starts_with — true if s has pfx as a leading substring.
 *
 * s:   haystack C string (NULL → 0)
 * pfx: required prefix (NULL → 0; empty string matches any non-NULL s)
 *
 * Comparison is unsigned-byte exact (no locale). Stops at the first
 * mismatch or when pfx is exhausted; s need not be fully scanned.
 */
int
gj_starts_with(const char *s, const char *pfx)
{
	size_t i;

	if (s == NULL || pfx == NULL) {
		return 0;
	}

	i = 0u;
	while (pfx[i] != '\0') {
		if (s[i] == '\0' || s[i] != pfx[i]) {
			return 0;
		}
		i++;
	}
	return 1;
}

/*
 * gj_ends_with — true if s has sfx as a trailing substring.
 *
 * s:   haystack C string (NULL → 0)
 * sfx: required suffix (NULL → 0; empty string matches any non-NULL s)
 *
 * Comparison is unsigned-byte exact (no locale). Requires
 * strlen(s) >= strlen(sfx) and equality on the trailing window.
 */
int
gj_ends_with(const char *s, const char *sfx)
{
	size_t cbS;
	size_t cbSfx;
	size_t i;
	size_t iBase;

	if (s == NULL || sfx == NULL) {
		return 0;
	}

	cbS = b356_strlen(s);
	cbSfx = b356_strlen(sfx);
	if (cbSfx > cbS) {
		return 0;
	}

	iBase = cbS - cbSfx;
	for (i = 0u; i < cbSfx; i++) {
		if (s[iBase + i] != sfx[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_starts_with(const char *s, const char *pfx)
    __attribute__((alias("gj_starts_with")));

int __gj_ends_with(const char *s, const char *sfx)
    __attribute__((alias("gj_ends_with")));
