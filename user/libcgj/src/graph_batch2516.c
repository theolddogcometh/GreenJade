/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2516: skip one JSON string (bounded index).
 *
 * Surface (unique symbols):
 *   size_t gj_json_skip_string_n(const char *s, size_t n, size_t i);
 *     - s[i] must be '"'. Advance past the closing unescaped '"' and
 *       return the index immediately after it. Handles \\ escapes
 *       (including \\uXXXX as six bytes after the backslash). On error
 *       (NULL s, i >= n, not opening quote, truncated escape, unclosed
 *       string) returns n (caller treats as fail / end).
 *   size_t __gj_json_skip_string_n  (alias)
 *   __libcgj_batch2516_marker = "libcgj-batch2516"
 *
 * JSON exclusive lightweight wave (2511-2520). Distinct from
 * gj_json_skip_ws_n (whitespace only) — string token skip; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2516_marker[] = "libcgj-batch2516";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if ASCII hex digit. */
static int
b2516_is_hex(unsigned char u8C)
{
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	if (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'f') {
		return 1;
	}
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'F') {
		return 1;
	}
	return 0;
}

/*
 * Skip string starting at cI where sz[cI] == '"'.
 * Returns index after closing quote, or cN on error.
 */
static size_t
b2516_skip_string(const char *sz, size_t cN, size_t cI)
{
	if (sz == NULL || cI >= cN) {
		return cN;
	}
	if ((unsigned char)sz[cI] != (unsigned char)'"') {
		return cN;
	}
	cI++; /* past opening quote */

	while (cI < cN) {
		unsigned char u8C = (unsigned char)sz[cI];

		if (u8C == (unsigned char)'"') {
			return cI + 1u;
		}
		if (u8C == (unsigned char)'\\') {
			cI++;
			if (cI >= cN) {
				return cN;
			}
			u8C = (unsigned char)sz[cI];
			cI++;
			if (u8C == (unsigned char)'u') {
				size_t k;

				/* four hex digits */
				if (cI + 4u > cN) {
					return cN;
				}
				for (k = 0u; k < 4u; k++) {
					if (b2516_is_hex(
					    (unsigned char)sz[cI + k]) == 0) {
						return cN;
					}
				}
				cI += 4u;
			}
			/* other single-char escapes: just skip the byte */
			continue;
		}
		/* bare control in string body is illegal */
		if (u8C < 0x20u) {
			return cN;
		}
		cI++;
	}
	return cN; /* unclosed */
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_skip_string_n - return index after a JSON string at s[i].
 *
 * s: buffer of n bytes
 * n: length of s
 * i: index of opening '"'
 *
 * Pure index walk; does not decode content. Error / unclosed → n.
 */
size_t
gj_json_skip_string_n(const char *sz, size_t cN, size_t cI)
{
	(void)NULL;
	return b2516_skip_string(sz, cN, cI);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_json_skip_string_n(const char *sz, size_t cN, size_t cI)
    __attribute__((alias("gj_json_skip_string_n")));
