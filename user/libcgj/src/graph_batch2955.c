/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2955: case-insensitive NUL-terminated suffix
 * predicate under the gj_str_ends_ci_u surface name.
 *
 * Surface (unique symbols):
 *   int gj_str_ends_ci_u(const char *s, const char *sfx);
 *     - Return 1 if s ends with every octet of sfx under ASCII
 *       case-insensitive compare (including when sfx is empty), else 0.
 *       NULL s or NULL sfx returns 0.
 *   int __gj_str_ends_ci_u  (alias)
 *   __libcgj_batch2955_marker = "libcgj-batch2955"
 *
 * Post-2950 exclusive string helpers wave (2951-2960). Distinct from
 * gj_ends_with_ci (batch508), gj_str_ends (batch602), and
 * gj_str_ends_ci_n (batch2414) — unique surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2955_marker[] = "libcgj-batch2955";

/* ---- freestanding helpers ---------------------------------------------- */

/* ASCII A-Z → a-z; all other octets unchanged. */
static unsigned char
b2955_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* Byte length of a NUL-terminated string (NULL → 0). */
static size_t
b2955_len(const char *sz)
{
	size_t cb;

	cb = 0u;
	if (sz == NULL) {
		return 0u;
	}
	while (sz[cb] != '\0') {
		cb++;
	}
	return cb;
}

static int
b2955_ends_ci(const char *s, const char *sfx)
{
	size_t cbS;
	size_t cbSfx;
	size_t i;
	size_t iBase;

	if (s == NULL || sfx == NULL) {
		return 0;
	}

	cbS = b2955_len(s);
	cbSfx = b2955_len(sfx);
	if (cbSfx > cbS) {
		return 0;
	}

	iBase = cbS - cbSfx;
	for (i = 0u; i < cbSfx; i++) {
		if (b2955_fold((unsigned char)s[iBase + i]) !=
		    b2955_fold((unsigned char)sfx[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_ends_ci_u - true if s has sfx as a trailing substring (ASCII CI).
 *
 * s:   haystack C string (NULL → 0)
 * sfx: required suffix (NULL → 0; empty string matches any non-NULL s)
 *
 * Comparison folds ASCII letters only; other octets compare exactly.
 * Requires strlen(s) >= strlen(sfx) and CI equality on the trailing window.
 */
int
gj_str_ends_ci_u(const char *s, const char *sfx)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b2955_ends_ci(s, sfx);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_ends_ci_u(const char *s, const char *sfx)
    __attribute__((alias("gj_str_ends_ci_u")));
