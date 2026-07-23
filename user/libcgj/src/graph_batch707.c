/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch707: NUL-terminated string prefix predicate
 * (has-prefix, exact bytes) under the gj_str_has_prefix surface name.
 *
 * Surface (unique symbols):
 *   int gj_str_has_prefix(const char *s, const char *pfx);
 *     — Return 1 if s begins with every octet of pfx (including when
 *       pfx is empty and s is non-NULL), else 0. NULL s or NULL pfx
 *       returns 0.
 *   int __gj_str_has_prefix  (alias)
 *   __libcgj_batch707_marker = "libcgj-batch707"
 *
 * Does NOT define gj_starts_with / __gj_starts_with (batch356),
 * gj_str_starts / __gj_str_starts (batch601), or gj_starts_with_ci
 * (batch507) — avoid multi-def. Same exact-prefix predicate, unique
 * gj_str_has_prefix symbol only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch707_marker[] = "libcgj-batch707";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_has_prefix — true if s has pfx as a leading substring (exact).
 *
 * s:   haystack C string (NULL → 0)
 * pfx: required prefix (NULL → 0; empty string → 1 when s is non-NULL)
 *
 * Comparison is unsigned-byte exact (no locale). Stops at the first
 * mismatch or when pfx is exhausted; s need not be fully scanned.
 */
int
gj_str_has_prefix(const char *s, const char *pfx)
{
	size_t i;

	if (s == NULL || pfx == NULL) {
		return 0;
	}

	i = 0u;
	while (pfx[i] != '\0') {
		if (s[i] == '\0' || s[i] != pfx[i]) {
			return 0;
		}
		i++;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_has_prefix(const char *s, const char *pfx)
    __attribute__((alias("gj_str_has_prefix")));
