/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2954: case-insensitive NUL-terminated prefix
 * predicate under the gj_str_starts_ci_u surface name.
 *
 * Surface (unique symbols):
 *   int gj_str_starts_ci_u(const char *s, const char *pfx);
 *     - Return 1 if s begins with every octet of pfx under ASCII
 *       case-insensitive compare (including when pfx is empty), else 0.
 *       NULL s or NULL pfx returns 0.
 *   int __gj_str_starts_ci_u  (alias)
 *   __libcgj_batch2954_marker = "libcgj-batch2954"
 *
 * Post-2950 exclusive string helpers wave (2951-2960). Distinct from
 * gj_starts_with_ci (batch507), gj_str_starts (batch601), and
 * gj_str_starts_ci_n (batch2413) — unique surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2954_marker[] = "libcgj-batch2954";

/* ---- freestanding helpers ---------------------------------------------- */

/* ASCII A-Z → a-z; all other octets unchanged. */
static unsigned char
b2954_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

static int
b2954_starts_ci(const char *s, const char *pfx)
{
	size_t i;

	if (s == NULL || pfx == NULL) {
		return 0;
	}

	i = 0u;
	while (pfx[i] != '\0') {
		if (s[i] == '\0') {
			return 0;
		}
		if (b2954_fold((unsigned char)s[i]) !=
		    b2954_fold((unsigned char)pfx[i])) {
			return 0;
		}
		i++;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_starts_ci_u - true if s has pfx as a leading substring (ASCII CI).
 *
 * s:   haystack C string (NULL → 0)
 * pfx: required prefix (NULL → 0; empty string matches any non-NULL s)
 *
 * Comparison folds ASCII letters only (A–Z / a–z); other octets compare
 * exactly. Stops at the first mismatch or when pfx is exhausted.
 */
int
gj_str_starts_ci_u(const char *s, const char *pfx)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b2954_starts_ci(s, pfx);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_starts_ci_u(const char *s, const char *pfx)
    __attribute__((alias("gj_str_starts_ci_u")));
