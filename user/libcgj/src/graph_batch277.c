/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch277: TOML bare-key validation.
 *
 * Surface (unique symbols):
 *   int gj_toml_bare_key_ok(const char *s);
 *     — Validate NUL-terminated s as a TOML bare key: [A-Za-z0-9_-]+
 *       Returns 1 if ok, 0 if bad (empty or illegal char), -1 if s is NULL.
 *   __gj_toml_bare_key_ok  (alias)
 *   __libcgj_batch277_marker = "libcgj-batch277"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied. Distinct from batch81
 * TOML get helpers (which parse flat key=value lines).
 */

#include <stddef.h>

const char __libcgj_batch277_marker[] = "libcgj-batch277";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b277_is_bare_key_ch — one character allowed in a TOML bare key.
 * Spec: unquoted keys are A-Za-z0-9_- only (no dots; dotted keys are
 * composed of bare/quoted segments, not a single bare key).
 */
static int
b277_is_bare_key_ch(unsigned char ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
	       (ch >= '0' && ch <= '9') || ch == '_' || ch == '-';
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_toml_bare_key_ok — validate a NUL-terminated TOML bare key string.
 *
 * Returns:
 *   1  — non-empty, every byte in [A-Za-z0-9_-]
 *   0  — empty string or any illegal character
 *  -1  — s is NULL
 */
int
gj_toml_bare_key_ok(const char *s)
{
	const char *p;

	if (s == NULL) {
		return -1;
	}
	if (*s == '\0') {
		return 0;
	}
	for (p = s; *p != '\0'; p++) {
		if (!b277_is_bare_key_ch((unsigned char)*p)) {
			return 0;
		}
	}
	return 1;
}

int __gj_toml_bare_key_ok(const char *s)
    __attribute__((alias("gj_toml_bare_key_ok")));
