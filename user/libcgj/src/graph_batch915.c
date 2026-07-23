/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch915: CSV field quote-needed predicate.
 *
 * Surface (unique symbols):
 *   int gj_csv_needs_quote(const char *s);
 *     — Return 1 if s must be quoted per RFC 4180-style rules when
 *       written as a CSV field: contains comma, double-quote, CR, LF,
 *       or is empty (empty fields are commonly quoted for clarity).
 *       NULL s → 1 (treat as empty / unsafe). Else 0 if bare is safe.
 *   int __gj_csv_needs_quote  (alias)
 *   __libcgj_batch915_marker = "libcgj-batch915"
 *
 * Does NOT redefine gj_csv_quote / gj_csv_unquote / gj_csv_next_field —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch915_marker[] = "libcgj-batch915";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_csv_needs_quote — true if field text requires CSV quoting.
 *
 * Triggers on ',', '"', '\r', '\n', or empty / NULL input. Leading or
 * trailing spaces alone do not force quotes (RFC 4180 leaves that to
 * the writer); only the special structural characters listed above.
 */
int
gj_csv_needs_quote(const char *s)
{
	size_t i;

	if (s == NULL || s[0] == '\0') {
		return 1;
	}

	for (i = 0u; s[i] != '\0'; i++) {
		char ch = s[i];

		if (ch == ',' || ch == '"' || ch == '\r' || ch == '\n') {
			return 1;
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_csv_needs_quote(const char *s)
    __attribute__((alias("gj_csv_needs_quote")));
