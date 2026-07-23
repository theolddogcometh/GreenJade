/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4673: C-string suffix predicate (string-
 * measure wave, collision rename).
 *
 * Surface (unique symbols):
 *   int gj_str_has_suffix_u2(const char *s, const char *suf);
 *     - Return 1 if s ends with every octet of suf (including when
 *       suf is empty and s is non-NULL), else 0. NULL s or NULL suf
 *       → 0. Renamed from gj_str_has_suffix_u (collision batch3284).
 *   int __gj_str_has_suffix_u2  (alias)
 *   __libcgj_batch4673_marker = "libcgj-batch4673"
 *
 * Exclusive continuum CREATE-ONLY (4671-4680). Distinct from
 * gj_str_has_suffix_u (batch3284), gj_str_has_suffix (batch708),
 * gj_ends_with (batch356) — unique gj_str_has_suffix_u2 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4673_marker[] = "libcgj-batch4673";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4673_strlen(const char *sz)
{
	size_t cb;

	if (sz == NULL) {
		return 0u;
	}
	cb = 0u;
	while (sz[cb] != '\0') {
		cb++;
	}
	return cb;
}

static int
b4673_has_suffix(const char *sz, const char *szSuf)
{
	size_t cbS;
	size_t cbSuf;
	size_t i;
	size_t iBase;

	if (sz == NULL || szSuf == NULL) {
		return 0;
	}

	cbS = b4673_strlen(sz);
	cbSuf = b4673_strlen(szSuf);
	if (cbSuf > cbS) {
		return 0;
	}

	iBase = cbS - cbSuf;
	for (i = 0u; i < cbSuf; i++) {
		if (sz[iBase + i] != szSuf[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_has_suffix_u2 - true if s has suf as a trailing substring.
 *
 * s:   haystack C string (NULL → 0)
 * suf: required suffix (NULL → 0; empty matches any non-NULL s)
 *
 * Comparison is unsigned-byte exact (no locale). No parent wires.
 */
int
gj_str_has_suffix_u2(const char *s, const char *suf)
{
	(void)NULL;
	return b4673_has_suffix(s, suf);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_has_suffix_u2(const char *s, const char *suf)
    __attribute__((alias("gj_str_has_suffix_u2")));
