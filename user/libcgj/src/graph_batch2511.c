/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2511: JSON string-body escape-needed predicate.
 *
 * Surface (unique symbols):
 *   int gj_json_escape_needed_p(const char *s, size_t n);
 *     - Return 1 if any octet in s[0..n) must be escaped inside a JSON
 *       string body: '"' / '\\' / controls 0x00..0x1f. Else 0.
 *       NULL s with n > 0 → 0. n == 0 → 0 (empty needs no escape).
 *   int __gj_json_escape_needed_p  (alias)
 *   __libcgj_batch2511_marker = "libcgj-batch2511"
 *
 * JSON exclusive lightweight wave (2511-2520). Distinct from
 * gj_json_needs_escape (batch1136 per-char) — string scan form; avoid
 * multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2511_marker[] = "libcgj-batch2511";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if single octet requires JSON string-body escaping. */
static int
b2511_byte_needs(unsigned char u8C)
{
	if (u8C == (unsigned char)'"' || u8C == (unsigned char)'\\') {
		return 1;
	}
	if (u8C < 0x20u) {
		return 1;
	}
	return 0;
}

/* Scan s[0..n) for any escape-required octet. */
static int
b2511_needed_p(const char *sz, size_t cN)
{
	size_t i;

	if (sz == NULL || cN == 0u) {
		return 0;
	}
	for (i = 0u; i < cN; i++) {
		if (b2511_byte_needs((unsigned char)sz[i]) != 0) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_escape_needed_p - 1 iff any byte in s[0..n) needs JSON escape.
 *
 * s: input bytes (may be NULL when n == 0)
 * n: byte count to scan
 *
 * Does not emit escapes; pure predicate for pre-checks. Does not call libc.
 */
int
gj_json_escape_needed_p(const char *sz, size_t cN)
{
	(void)NULL;
	return b2511_needed_p(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_escape_needed_p(const char *sz, size_t cN)
    __attribute__((alias("gj_json_escape_needed_p")));
