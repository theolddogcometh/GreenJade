/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2756: freestanding C-string suffix predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_ends_u(const char *s, const char *sfx);
 *     - Return 1 if s ends with every octet of sfx (including when
 *       sfx is empty), else 0. NULL s or NULL sfx returns 0.
 *   int __gj_str_ends_u  (alias)
 *   __libcgj_batch2756_marker = "libcgj-batch2756"
 *
 * Post-2750 classic str predicate exclusive wave (2751-2760). Distinct
 * from gj_str_ends (batch602) and gj_ends_with (batch356) — unique
 * ends_u surface; no multi-def. Companion to gj_str_starts_u (2755).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2756_marker[] = "libcgj-batch2756";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b2756_len(const char *sz)
{
	size_t n;

	if (sz == NULL) {
		return 0u;
	}
	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b2756_ends(const char *sz, const char *szSfx)
{
	size_t cbS;
	size_t cbSfx;
	size_t i;
	size_t iBase;

	if (sz == NULL || szSfx == NULL) {
		return 0;
	}

	cbS = b2756_len(sz);
	cbSfx = b2756_len(szSfx);
	if (cbSfx > cbS) {
		return 0;
	}

	iBase = cbS - cbSfx;
	for (i = 0u; i < cbSfx; i++) {
		if ((unsigned char)sz[iBase + i] !=
		    (unsigned char)szSfx[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_ends_u - true if s has sfx as a trailing substring (exact).
 *
 * s:   haystack C string (NULL → 0)
 * sfx: required suffix (NULL → 0; empty string matches any non-NULL s)
 *
 * Comparison is unsigned-byte exact (no locale). Requires
 * length(s) >= length(sfx) and equality on the trailing window.
 */
int
gj_str_ends_u(const char *s, const char *sfx)
{
	(void)NULL;
	return b2756_ends(s, sfx);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_ends_u(const char *s, const char *sfx)
    __attribute__((alias("gj_str_ends_u")));
