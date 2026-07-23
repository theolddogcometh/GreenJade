/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch466: freestanding BSON cstring (NUL-terminated
 * within a bounded buffer).
 *
 * Surface (unique symbols):
 *   int gj_bson_cstring(const uint8_t *p, size_t n, const char **out,
 *                       size_t *len);
 *     — Locate a BSON cstring at p[0..n): zero or more non-NUL octets
 *       followed by a required 0x00 that must fall inside the buffer.
 *       Zero-copy: *out points at p as a C string; *len is the byte
 *       count excluding the terminator. On success return 0. On failure
 *       return -1 (no write to *out / *len).
 *   int __gj_bson_cstring  (alias)
 *   __libcgj_batch466_marker = "libcgj-batch466"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch466_marker[] = "libcgj-batch466";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bson_cstring — BSON cstring at p within n bytes (NUL must be in range).
 *
 * BSON wire form (subset of the BSON "cstring" production):
 *   cstring ::= (byte*) "\x00"
 * where no payload byte may be 0x00; the terminator is mandatory and
 * must appear at some index i with 0 <= i < n.
 *
 * Edge cases:
 *   p / out / len NULL → -1
 *   n == 0             → -1  (no room for terminator)
 *   empty string "\0"  → *out = p, *len = 0, return 0
 *   no 0x00 in p[0..n) → -1
 *
 * Does not validate modified UTF-8; octets other than 0x00 are accepted
 * as payload (BSON cstring is a length-bounded byte sequence + NUL).
 */
int
gj_bson_cstring(const uint8_t *p, size_t n, const char **out, size_t *len)
{
	size_t i;

	if (p == NULL || out == NULL || len == NULL || n == 0u) {
		return -1;
	}

	for (i = 0u; i < n; i++) {
		if (p[i] == (uint8_t)0u) {
			*out = (const char *)(const void *)p;
			*len = i;
			return 0;
		}
	}

	/* Terminator not present within the supplied window. */
	return -1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bson_cstring(const uint8_t *p, size_t n, const char **out,
                      size_t *len)
    __attribute__((alias("gj_bson_cstring")));
