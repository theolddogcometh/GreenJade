/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2414: case-insensitive n-octet suffix
 * predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_ends_ci_n(const char *s, const char *sfx, size_t n);
 *     - Return 1 if s ends with the first n octets of sfx under ASCII
 *       case fold (A-Z -> a-z). Requires strlen(s) >= n and CI equality
 *       on the trailing window. n == 0 -> 1 when s != NULL. NULL s or
 *       NULL sfx (n > 0) -> 0.
 *   int __gj_str_ends_ci_n  (alias)
 *   __libcgj_batch2414_marker = "libcgj-batch2414"
 *
 * Distinct from gj_str_ends (batch602), gj_ends_with_ci (batch508), and
 * gj_str_ends_with_n (batch1002) — unique surface; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2414_marker[] = "libcgj-batch2414";

/* ---- freestanding helpers ---------------------------------------------- */

/* ASCII A-Z -> a-z; all other octets unchanged. */
static unsigned char
b2414_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* Byte length of a NUL-terminated string (NULL -> 0). */
static size_t
b2414_strlen(const char *sz)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_ends_ci_n - true if s has sfx[0..n) as a CI trailing window.
 *
 * s:   haystack C string (NULL -> 0; n == 0 and s non-NULL -> 1)
 * sfx: suffix buffer of at least n octets when n > 0 (NULL with n > 0 -> 0)
 * n:   number of leading octets of sfx that must match the end of s
 *
 * Comparison folds ASCII letters only; other octets compare exactly.
 */
int
gj_str_ends_ci_n(const char *s, const char *sfx, size_t n)
{
	size_t cbS;
	size_t i;
	size_t iBase;

	if (s == NULL) {
		return 0;
	}
	if (n == 0u) {
		return 1;
	}
	if (sfx == NULL) {
		return 0;
	}

	cbS = b2414_strlen(s);
	if (n > cbS) {
		return 0;
	}

	iBase = cbS - n;
	for (i = 0u; i < n; i++) {
		if (b2414_fold((unsigned char)s[iBase + i]) !=
		    b2414_fold((unsigned char)sfx[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_ends_ci_n(const char *s, const char *sfx, size_t n)
    __attribute__((alias("gj_str_ends_ci_n")));
