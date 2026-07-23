/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3885: TOML bare-key character predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_toml_is_key_char_u(int c);
 *     - Return 1 if c is valid in a TOML bare key: A-Z, a-z, 0-9,
 *       underscore, or hyphen. Else 0.
 *   int __gj_toml_is_key_char_u  (alias)
 *   __libcgj_batch3885_marker = "libcgj-batch3885"
 *
 * Exclusive continuum CREATE-ONLY (3881-3890). Distinct from
 * gj_toml_is_key_char (batch1597) and gj_toml_is_bare_key_char
 * (batch913) — unique _u surface; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3885_marker[] = "libcgj-batch3885";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3885_is_key_char(int nC)
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
 * gj_toml_is_key_char_u — true for TOML bare-key octets [A-Za-z0-9_-].
 *
 * Quoted keys accept a wider set; this predicate is only for unquoted
 * bare keys (TOML v1 bare-key production). No parent wires.
 */
int
gj_toml_is_key_char_u(int nC)
{
	(void)NULL;
	return b3885_is_key_char(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_toml_is_key_char_u(int nC)
    __attribute__((alias("gj_toml_is_key_char_u")));
