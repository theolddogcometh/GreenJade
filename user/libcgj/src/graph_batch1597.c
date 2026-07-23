/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1597: TOML bare-key character predicate.
 *
 * Surface (unique symbols):
 *   int gj_toml_is_key_char(int c);
 *     — Return 1 if c is valid in a TOML bare key: A-Z, a-z, 0-9,
 *       underscore, or hyphen. Else 0.
 *   int __gj_toml_is_key_char  (alias)
 *   __libcgj_batch1597_marker = "libcgj-batch1597"
 *
 * Distinct from gj_toml_is_bare_key_char (batch913) — unique public name
 * for the exclusive token wave (same bare-key set).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1597_marker[] = "libcgj-batch1597";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1597_is_key_char(int nC)
{
	if (nC >= 'A' && nC <= 'Z') {
		return 1;
	}
	if (nC >= 'a' && nC <= 'z') {
		return 1;
	}
	if (nC >= '0' && nC <= '9') {
		return 1;
	}
	if (nC == '_' || nC == '-') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_toml_is_key_char — true for TOML bare-key octets [A-Za-z0-9_-].
 *
 * Quoted keys accept a wider set; this predicate is only for unquoted
 * bare keys (TOML v1 bare-key production).
 */
int
gj_toml_is_key_char(int nC)
{
	return b1597_is_key_char(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_toml_is_key_char(int nC)
    __attribute__((alias("gj_toml_is_key_char")));
