/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2413: case-insensitive n-octet prefix
 * predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_starts_ci_n(const char *s, const char *pfx, size_t n);
 *     - Return 1 if s begins with the first n octets of pfx under
 *       ASCII case fold (A-Z -> a-z). pfx need not be NUL-terminated for
 *       those n bytes. s is scanned as a C string and must provide at
 *       least n matching octets without hitting NUL early. n == 0 -> 1
 *       when s != NULL. NULL s or NULL pfx (with n > 0) -> 0.
 *   int __gj_str_starts_ci_n  (alias)
 *   __libcgj_batch2413_marker = "libcgj-batch2413"
 *
 * Distinct from gj_str_starts (batch601), gj_starts_with_ci (batch507),
 * and gj_str_starts_with_n (batch1001) — unique surface; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2413_marker[] = "libcgj-batch2413";

/* ---- freestanding helpers ---------------------------------------------- */

/* ASCII A-Z -> a-z; all other octets unchanged. */
static unsigned char
b2413_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_starts_ci_n - true if s has pfx[0..n) as a CI leading window.
 *
 * s:   haystack C string (NULL -> 0; n == 0 and s non-NULL -> 1)
 * pfx: prefix buffer of at least n octets when n > 0 (NULL with n > 0 -> 0)
 * n:   number of leading octets of pfx that must match
 *
 * Comparison folds ASCII letters only; other octets compare exactly.
 */
int
gj_str_starts_ci_n(const char *s, const char *pfx, size_t n)
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
		if (s[i] == '\0') {
			return 0;
		}
		if (b2413_fold((unsigned char)s[i]) !=
		    b2413_fold((unsigned char)pfx[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_starts_ci_n(const char *s, const char *pfx, size_t n)
    __attribute__((alias("gj_str_starts_ci_n")));
