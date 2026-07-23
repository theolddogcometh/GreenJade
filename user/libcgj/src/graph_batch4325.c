/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4325: freestanding C-string ends-with
 * predicate (_u surface).
 *
 * Surface (unique symbols):
 *   int gj_str_endswith_u(const char *s, const char *suffix);
 *     - Return 1 if s ends with every octet of suffix (including when
 *       suffix is empty), else 0. NULL s or NULL suffix returns 0.
 *   int __gj_str_endswith_u  (alias)
 *   __libcgj_batch4325_marker = "libcgj-batch4325"
 *
 * Exclusive continuum CREATE-ONLY (4321-4330 string utils). Distinct
 * from gj_str_ends (batch602) and gj_ends_with (batch356) — unique
 * gj_str_endswith_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4325_marker[] = "libcgj-batch4325";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4325_len(const char *sz)
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
b4325_endswith(const char *sz, const char *szSfx)
{
	size_t cbS;
	size_t cbSfx;
	size_t i;
	size_t iBase;

	if (sz == NULL || szSfx == NULL) {
		return 0;
	}

	cbS = b4325_len(sz);
	cbSfx = b4325_len(szSfx);
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
 * gj_str_endswith_u - 1 if s has suffix as a trailing substring.
 *
 * s:      haystack C string (NULL → 0)
 * suffix: required suffix (NULL → 0; empty string matches any non-NULL s)
 *
 * Comparison is unsigned-byte exact (no locale). Does not call libc.
 * Self-contained; no parent wires.
 */
int
gj_str_endswith_u(const char *s, const char *suffix)
{
	(void)NULL;
	return b4325_endswith(s, suffix);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_endswith_u(const char *s, const char *suffix)
    __attribute__((alias("gj_str_endswith_u")));
