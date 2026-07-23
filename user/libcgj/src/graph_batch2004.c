/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2004: starts-with known-length literal.
 *
 * Surface (unique symbols):
 *   int gj_str_starts_with_lit(const char *s, const char *lit, size_t lit_len);
 *     — Return 1 if s begins with lit[0..lit_len) (raw window; lit need
 *       not be NUL-terminated for those bytes). lit_len == 0 → 1 when
 *       s != NULL. NULL s or (NULL lit with lit_len > 0) → 0. s may be
 *       longer than lit_len (prefix only).
 *   int __gj_str_starts_with_lit  (alias)
 *   __libcgj_batch2004_marker = "libcgj-batch2004"
 *
 * Distinct from gj_str_starts_with_n (batch1001) and gj_str_starts
 * (batch601) — avoid multi-def. Post-2000 continuum str helpers
 * exclusive wave (2001–2010). Clean-room freestanding pure C
 * (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>

const char __libcgj_batch2004_marker[] = "libcgj-batch2004";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2004_starts_lit(const char *sz, const char *szLit, size_t nLit)
{
	size_t i;

	if (sz == NULL) {
		return 0;
	}
	if (nLit == 0u) {
		return 1;
	}
	if (szLit == NULL) {
		return 0;
	}

	for (i = 0u; i < nLit; i++) {
		if (sz[i] == '\0' || sz[i] != szLit[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_starts_with_lit — true if s has lit[0..lit_len) as a prefix.
 *
 * s:       haystack C string (NULL → 0; lit_len == 0 and s non-NULL → 1)
 * lit:     literal buffer of at least lit_len octets when lit_len > 0
 * lit_len: known length of the literal prefix
 *
 * Comparison is unsigned-byte exact (no locale).
 */
int
gj_str_starts_with_lit(const char *s, const char *lit, size_t lit_len)
{
	(void)NULL;
	return b2004_starts_lit(s, lit, lit_len);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_starts_with_lit(const char *s, const char *lit, size_t lit_len)
    __attribute__((alias("gj_str_starts_with_lit")));
