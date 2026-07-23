/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2003: equality to a known-length literal.
 *
 * Surface (unique symbols):
 *   int gj_str_eq_literal_p(const char *s, const char *lit, size_t lit_len);
 *     — Return 1 if s is non-NULL and equals the raw literal window
 *       lit[0..lit_len) and is terminated at s[lit_len] (exact whole-
 *       string match against a compile-time-known-length literal).
 *       lit_len == 0 → 1 when s is non-NULL and empty. NULL s → 0.
 *       NULL lit with lit_len > 0 → 0.
 *   int __gj_str_eq_literal_p  (alias)
 *   __libcgj_batch2003_marker = "libcgj-batch2003"
 *
 * Distinct from gj_str_eq (batch413) and gj_str_eq_n (batch414) —
 * avoid multi-def. Post-2000 continuum str helpers exclusive wave
 * (2001–2010). Clean-room freestanding pure C (integer/pointer only).
 * Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc,
 * no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2003_marker[] = "libcgj-batch2003";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Exact whole-string match of sz against lit[0..nLit).
 * Requires sz[nLit] == '\0' so lengths agree.
 */
static int
b2003_eq_lit(const char *sz, const char *szLit, size_t nLit)
{
	size_t i;

	if (sz == NULL) {
		return 0;
	}
	if (nLit == 0u) {
		return (sz[0] == '\0') ? 1 : 0;
	}
	if (szLit == NULL) {
		return 0;
	}

	for (i = 0u; i < nLit; i++) {
		if (sz[i] == '\0' || sz[i] != szLit[i]) {
			return 0;
		}
	}
	/* Require exact end: s must not be longer than the literal. */
	if (sz[nLit] != '\0') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_eq_literal_p — 1 if s equals lit[0..lit_len) exactly.
 *
 * s:       NUL-terminated haystack (NULL → 0)
 * lit:     literal buffer of at least lit_len octets when lit_len > 0
 * lit_len: known length of the literal (sizeof("foo")-1 style)
 *
 * Comparison is unsigned-byte exact (no locale). Does not call libc.
 */
int
gj_str_eq_literal_p(const char *s, const char *lit, size_t lit_len)
{
	(void)NULL;
	return b2003_eq_lit(s, lit, lit_len);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_eq_literal_p(const char *s, const char *lit, size_t lit_len)
    __attribute__((alias("gj_str_eq_literal_p")));
