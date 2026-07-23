/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch708: freestanding NUL-terminated string suffix
 * predicate under the gj_str_has_suffix surface name.
 *
 * Surface (unique symbols):
 *   int gj_str_has_suffix(const char *s, const char *sfx);
 *     — Return 1 (OK) if s ends with every octet of sfx (including when
 *       sfx is empty), else 0 (FAIL). NULL s or NULL sfx returns 0 (FAIL).
 *   int __gj_str_has_suffix  (alias)
 *   __libcgj_batch708_marker = "libcgj-batch708"
 *
 * Does NOT define gj_ends_with / __gj_ends_with (batch356), gj_str_ends /
 * __gj_str_ends (batch602), or gj_ends_with_ci / __gj_ends_with_ci
 * (batch508) — avoid multi-def. Same exact-suffix predicate idea,
 * distinct gj_str_has_suffix surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch708_marker[] = "libcgj-batch708";

/* ---- freestanding helpers ---------------------------------------------- */

/* Byte length of a NUL-terminated string (NULL → 0). */
static size_t
b708_strlen(const char *sz)
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
 * gj_str_has_suffix — true (OK) if s has sfx as a trailing substring.
 *
 * s:   haystack C string (NULL → 0 / FAIL)
 * sfx: required suffix (NULL → 0 / FAIL; empty string matches any
 *      non-NULL s)
 *
 * Comparison is unsigned-byte exact (no locale). Requires
 * strlen(s) >= strlen(sfx) and equality on the trailing window.
 * Returns 1 on match (OK), 0 otherwise (FAIL).
 */
int
gj_str_has_suffix(const char *s, const char *sfx)
{
	size_t cbS;
	size_t cbSfx;
	size_t i;
	size_t iBase;

	if (s == NULL || sfx == NULL) {
		return 0;
	}

	cbS = b708_strlen(s);
	cbSfx = b708_strlen(sfx);
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

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_has_suffix(const char *s, const char *sfx)
    __attribute__((alias("gj_str_has_suffix")));
