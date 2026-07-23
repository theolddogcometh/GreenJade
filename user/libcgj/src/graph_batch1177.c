/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1177: Git object SHA-1 hex identity check.
 *
 * Surface (unique symbols):
 *   int gj_git_is_sha1_hex(const char *s);
 *     — Return 1 if s is exactly 40 ASCII hex digits [0-9a-fA-F];
 *       else 0. NULL / wrong length / non-hex → 0.
 *   int __gj_git_is_sha1_hex  (alias)
 *   __libcgj_batch1177_marker = "libcgj-batch1177"
 *
 * Distinct from gj_is_hex_str (batch450) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1177_marker[] = "libcgj-batch1177";

/* ---- helpers ------------------------------------------------------------ */

static int
b1177_is_hex(char ch)
{
	if (ch >= '0' && ch <= '9') {
		return 1;
	}
	if (ch >= 'a' && ch <= 'f') {
		return 1;
	}
	if (ch >= 'A' && ch <= 'F') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_git_is_sha1_hex — 1 iff s is exactly 40 hex digits.
 */
int
gj_git_is_sha1_hex(const char *sz)
{
	const char *p;
	unsigned c;

	if (sz == NULL) {
		return 0;
	}
	c = 0u;
	for (p = sz; *p != '\0'; p++) {
		if (!b1177_is_hex(*p)) {
			return 0;
		}
		c++;
		if (c > 40u) {
			return 0;
		}
	}
	if (c != 40u) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_git_is_sha1_hex(const char *sz)
    __attribute__((alias("gj_git_is_sha1_hex")));
