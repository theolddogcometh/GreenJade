/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1002: length-limited C-string suffix predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_ends_with_n(const char *s, const char *sfx, size_t n);
 *     — Return 1 if s ends with the first n octets of sfx (raw n-byte
 *       window). Requires strlen(s) >= n and equality on the trailing
 *       window. n == 0 → 1 when s != NULL. NULL s or NULL sfx (n > 0) → 0.
 *   int __gj_str_ends_with_n  (alias)
 *   __libcgj_batch1002_marker = "libcgj-batch1002"
 *
 * Distinct from gj_ends_with (batch356) and gj_ends_with_ci (batch508) —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1002_marker[] = "libcgj-batch1002";

/* ---- freestanding helpers ---------------------------------------------- */

/* Byte length of a NUL-terminated string (NULL → 0). */
static size_t
b1002_strlen(const char *sz)
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
 * gj_str_ends_with_n — true if s has sfx[0..n) as a trailing window.
 *
 * s:   haystack C string (NULL → 0; n == 0 and s non-NULL → 1)
 * sfx: suffix buffer of at least n octets when n > 0 (NULL with n > 0 → 0)
 * n:   number of leading octets of sfx that must match the end of s
 *
 * Comparison is unsigned-byte exact (no locale).
 */
int
gj_str_ends_with_n(const char *s, const char *sfx, size_t n)
{
	size_t cbS;
	size_t i;
	size_t iBase;

	if (s == NULL) {
		return 0;
	}
	if (n == 0u) {
		return 1;
	}
	if (sfx == NULL) {
		return 0;
	}

	cbS = b1002_strlen(s);
	if (n > cbS) {
		return 0;
	}

	iBase = cbS - n;
	for (i = 0u; i < n; i++) {
		if (s[iBase + i] != sfx[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_ends_with_n(const char *s, const char *sfx, size_t n)
    __attribute__((alias("gj_str_ends_with_n")));
