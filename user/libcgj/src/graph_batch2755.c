/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2755: freestanding C-string prefix predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_starts_u(const char *s, const char *pfx);
 *     - Return 1 if s begins with every octet of pfx (including when
 *       pfx is empty), else 0. NULL s or NULL pfx returns 0.
 *   int __gj_str_starts_u  (alias)
 *   __libcgj_batch2755_marker = "libcgj-batch2755"
 *
 * Post-2750 classic str predicate exclusive wave (2751-2760). Distinct
 * from gj_str_starts (batch601) and gj_starts_with (batch356) — unique
 * starts_u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2755_marker[] = "libcgj-batch2755";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2755_starts(const char *sz, const char *szPfx)
{
	size_t i;

	if (sz == NULL || szPfx == NULL) {
		return 0;
	}

	i = 0u;
	while (szPfx[i] != '\0') {
		if (sz[i] == '\0') {
			return 0;
		}
		if ((unsigned char)sz[i] != (unsigned char)szPfx[i]) {
			return 0;
		}
		i++;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_starts_u - true if s has pfx as a leading substring (exact).
 *
 * s:   haystack C string (NULL → 0)
 * pfx: required prefix (NULL → 0; empty string matches any non-NULL s)
 *
 * Comparison is unsigned-byte exact (no locale). Stops at the first
 * mismatch or when pfx is exhausted; s need not be fully scanned.
 */
int
gj_str_starts_u(const char *s, const char *pfx)
{
	(void)NULL;
	return b2755_starts(s, pfx);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_starts_u(const char *s, const char *pfx)
    __attribute__((alias("gj_str_starts_u")));
