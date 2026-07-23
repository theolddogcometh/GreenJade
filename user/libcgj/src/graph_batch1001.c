/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1001: length-limited C-string prefix predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_starts_with_n(const char *s, const char *pfx, size_t n);
 *     — Return 1 if s begins with the first n octets of pfx (raw n-byte
 *       window; pfx need not be NUL-terminated for those n bytes). s is
 *       scanned as a C string and must provide at least n matching octets
 *       without hitting NUL early. n == 0 → 1 when s != NULL. NULL s or
 *       NULL pfx (with n > 0) → 0.
 *   int __gj_str_starts_with_n  (alias)
 *   __libcgj_batch1001_marker = "libcgj-batch1001"
 *
 * Distinct from gj_starts_with (batch356) and gj_starts_with_ci
 * (batch507) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1001_marker[] = "libcgj-batch1001";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_starts_with_n — true if s has pfx[0..n) as a leading window.
 *
 * s:   haystack C string (NULL → 0; n == 0 and s non-NULL → 1)
 * pfx: prefix buffer of at least n octets when n > 0 (NULL with n > 0 → 0)
 * n:   number of leading octets of pfx that must match
 *
 * Comparison is unsigned-byte exact (no locale). Stops at the first
 * mismatch or when s ends before n octets have matched.
 */
int
gj_str_starts_with_n(const char *s, const char *pfx, size_t n)
{
	size_t i;

	if (s == NULL) {
		return 0;
	}
	if (n == 0u) {
		return 1;
	}
	if (pfx == NULL) {
		return 0;
	}

	for (i = 0u; i < n; i++) {
		if (s[i] == '\0' || s[i] != pfx[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_starts_with_n(const char *s, const char *pfx, size_t n)
    __attribute__((alias("gj_str_starts_with_n")));
