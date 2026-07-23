/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch913: TOML bare-key character predicate.
 *
 * Surface (unique symbols):
 *   int gj_toml_is_bare_key_char(int c);
 *     — Return 1 if c is valid in a TOML bare key: A-Z, a-z, 0-9,
 *       underscore, or hyphen. Else 0.
 *   int __gj_toml_is_bare_key_char  (alias)
 *   __libcgj_batch913_marker = "libcgj-batch913"
 *
 * Does NOT redefine gj_toml_get_string / gj_toml_get_int (batch81) —
 * avoid multi-def. Matches TOML bare-key production.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch913_marker[] = "libcgj-batch913";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_toml_is_bare_key_char — true for TOML bare-key octets.
 *
 * Allowed: [A-Za-z0-9_-]. Quoted keys accept a wider set; this
 * predicate is only for unquoted bare keys.
 */
int
gj_toml_is_bare_key_char(int c)
{
	if (c >= 'A' && c <= 'Z') {
		return 1;
	}
	if (c >= 'a' && c <= 'z') {
		return 1;
	}
	if (c >= '0' && c <= '9') {
		return 1;
	}
	if (c == '_' || c == '-') {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_toml_is_bare_key_char(int c)
    __attribute__((alias("gj_toml_is_bare_key_char")));
