/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2005: ends-with known-length literal.
 *
 * Surface (unique symbols):
 *   int gj_str_ends_with_lit(const char *s, const char *lit, size_t lit_len);
 *     — Return 1 if s ends with lit[0..lit_len) (raw window). Requires
 *       strlen(s) >= lit_len and equality on the trailing window.
 *       lit_len == 0 → 1 when s != NULL. NULL s or (NULL lit with
 *       lit_len > 0) → 0.
 *   int __gj_str_ends_with_lit  (alias)
 *   __libcgj_batch2005_marker = "libcgj-batch2005"
 *
 * Distinct from gj_str_ends_with_n (batch1002) and gj_str_ends
 * (batch602) — avoid multi-def. Post-2000 continuum str helpers
 * exclusive wave (2001–2010). Clean-room freestanding pure C
 * (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>

const char __libcgj_batch2005_marker[] = "libcgj-batch2005";

/* ---- freestanding helpers ---------------------------------------------- */

/* Byte length of a NUL-terminated string (NULL → 0). */
static size_t
b2005_strlen(const char *sz)
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

static int
b2005_ends_lit(const char *sz, const char *szLit, size_t nLit)
{
	size_t cbS;
	size_t i;
	size_t iBase;

	if (sz == NULL) {
		return 0;
	}
	if (nLit == 0u) {
		return 1;
	}
	if (szLit == NULL) {
		return 0;
	}

	cbS = b2005_strlen(sz);
	if (nLit > cbS) {
		return 0;
	}

	iBase = cbS - nLit;
	for (i = 0u; i < nLit; i++) {
		if (sz[iBase + i] != szLit[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_ends_with_lit — true if s has lit[0..lit_len) as a suffix.
 *
 * s:       haystack C string (NULL → 0; lit_len == 0 and s non-NULL → 1)
 * lit:     literal buffer of at least lit_len octets when lit_len > 0
 * lit_len: known length of the literal suffix
 *
 * Comparison is unsigned-byte exact (no locale).
 */
int
gj_str_ends_with_lit(const char *s, const char *lit, size_t lit_len)
{
	(void)NULL;
	return b2005_ends_lit(s, lit, lit_len);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_ends_with_lit(const char *s, const char *lit, size_t lit_len)
    __attribute__((alias("gj_str_ends_with_lit")));
