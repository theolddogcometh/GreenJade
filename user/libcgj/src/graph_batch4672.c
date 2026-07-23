/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4672: C-string prefix predicate (string-
 * measure wave, collision rename).
 *
 * Surface (unique symbols):
 *   int gj_str_has_prefix_u2(const char *s, const char *p);
 *     - Return 1 if s begins with every octet of p (including when p
 *       is empty and s is non-NULL), else 0. NULL s or NULL p → 0.
 *       Renamed from gj_str_has_prefix_u (collision with batch3283).
 *   int __gj_str_has_prefix_u2  (alias)
 *   __libcgj_batch4672_marker = "libcgj-batch4672"
 *
 * Exclusive continuum CREATE-ONLY (4671-4680). Distinct from
 * gj_str_has_prefix_u (batch3283), gj_str_has_prefix (batch707),
 * gj_starts_with (batch356) — unique gj_str_has_prefix_u2 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4672_marker[] = "libcgj-batch4672";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4672_has_prefix(const char *sz, const char *szPfx)
{
	size_t i;

	if (sz == NULL || szPfx == NULL) {
		return 0;
	}

	i = 0u;
	while (szPfx[i] != '\0') {
		if (sz[i] == '\0' || sz[i] != szPfx[i]) {
			return 0;
		}
		i++;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_has_prefix_u2 - true if s has p as a leading substring (exact).
 *
 * s: haystack C string (NULL → 0)
 * p: required prefix (NULL → 0; empty string → 1 when s is non-NULL)
 *
 * Comparison is unsigned-byte exact (no locale). No parent wires.
 */
int
gj_str_has_prefix_u2(const char *s, const char *p)
{
	(void)NULL;
	return b4672_has_prefix(s, p);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_has_prefix_u2(const char *s, const char *p)
    __attribute__((alias("gj_str_has_prefix_u2")));
