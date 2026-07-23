/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4324: freestanding C-string starts-with
 * predicate (_u surface).
 *
 * Surface (unique symbols):
 *   int gj_str_startswith_u(const char *s, const char *prefix);
 *     - Return 1 if s begins with every octet of prefix (including when
 *       prefix is empty), else 0. NULL s or NULL prefix returns 0.
 *   int __gj_str_startswith_u  (alias)
 *   __libcgj_batch4324_marker = "libcgj-batch4324"
 *
 * Exclusive continuum CREATE-ONLY (4321-4330 string utils). Distinct
 * from gj_str_starts (batch601) and gj_starts_with (batch356) — unique
 * gj_str_startswith_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4324_marker[] = "libcgj-batch4324";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4324_startswith(const char *sz, const char *szPfx)
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
 * gj_str_startswith_u - 1 if s has prefix as a leading substring.
 *
 * s:      haystack C string (NULL → 0)
 * prefix: required prefix (NULL → 0; empty string matches any non-NULL s)
 *
 * Comparison is unsigned-byte exact (no locale). Does not call libc.
 * Self-contained; no parent wires.
 */
int
gj_str_startswith_u(const char *s, const char *prefix)
{
	(void)NULL;
	return b4324_startswith(s, prefix);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_startswith_u(const char *s, const char *prefix)
    __attribute__((alias("gj_str_startswith_u")));
