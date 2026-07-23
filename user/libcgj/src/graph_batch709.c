/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch709: bounded memory prefix predicate.
 *
 * Surface (unique symbols):
 *   int gj_mem_starts_with(const void *hay, size_t n,
 *                          const void *pfx, size_t np);
 *     — Return 1 (OK) if hay[0..n) begins with the np octets of pfx,
 *       else 0 (FAIL). np == 0 is a vacuous match (returns 1) even if
 *       hay or pfx is NULL. If np > 0 and either pointer is NULL, or
 *       if np > n, returns 0.
 *   int __gj_mem_starts_with  (alias)
 *   __libcgj_batch709_marker = "libcgj-batch709"
 *
 * Distinct from gj_starts_with (batch356, NUL-terminated C strings) and
 * from gj_mem_eq / gj_mem_cmp — unique gj_ names only; no multi-def with
 * the existing string graph. Returns a boolean-style 0/1 rather than a
 * signed lexicographic difference.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch709_marker[] = "libcgj-batch709";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_starts_with — true if hay[0..n) has pfx[0..np) as a leading
 * window (unsigned octets).
 *
 * hay: haystack region of at least n bytes when n > 0 (and np > 0)
 * n:   length of hay in octets
 * pfx: required prefix of at least np bytes when np > 0
 * np:  length of prefix in octets (0 → 1, vacuous match)
 *
 * Does not call libc memcmp / strncmp. Early-exits on the first mismatch.
 */
int
gj_mem_starts_with(const void *hay, size_t n, const void *pfx, size_t np)
{
	const unsigned char *ph;
	const unsigned char *pp;
	size_t i;

	if (np == 0u) {
		return 1;
	}
	if (np > n) {
		return 0;
	}
	if (hay == NULL || pfx == NULL) {
		return 0;
	}
	if (hay == pfx) {
		return 1;
	}

	ph = (const unsigned char *)hay;
	pp = (const unsigned char *)pfx;
	for (i = 0u; i < np; i++) {
		if (ph[i] != pp[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_starts_with(const void *hay, size_t n, const void *pfx, size_t np)
    __attribute__((alias("gj_mem_starts_with")));
