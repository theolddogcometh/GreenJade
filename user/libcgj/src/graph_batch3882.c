/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3882: skip one JSON string (NUL-terminated, _u).
 *
 * Surface (unique symbols):
 *   const char *gj_json_skip_string_u(const char *s);
 *     - s must point at opening '"'. Advance past the closing unescaped
 *       '"' and return the pointer immediately after it. Handles \\
 *       escapes (including \\uXXXX as four hex digits after 'u'). On
 *       error (NULL s, not opening quote, truncated escape, unclosed
 *       string, bare control) returns NULL.
 *   const char *__gj_json_skip_string_u  (alias)
 *   __libcgj_batch3882_marker = "libcgj-batch3882"
 *
 * Exclusive continuum CREATE-ONLY (3881-3890). Distinct from
 * gj_json_skip_string_n (batch2516 bounded index form) — NUL-terminated
 * pointer form; unique name; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3882_marker[] = "libcgj-batch3882";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if ASCII hex digit. */
static int
b3882_is_hex(unsigned char u8C)
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
 * Skip string at sz where *sz == '"'.
 * Returns pointer after closing quote, or NULL on error.
 */
static const char *
b3882_skip_string(const char *sz)
{
	unsigned char u8C;

	if (sz == NULL) {
		return NULL;
	}
	if ((unsigned char)*sz != (unsigned char)'"') {
		return NULL;
	}
	sz++; /* past opening quote */

	while (*sz != '\0') {
		u8C = (unsigned char)*sz;

		if (u8C == (unsigned char)'"') {
			return sz + 1;
		}
		if (u8C == (unsigned char)'\\') {
			sz++;
			if (*sz == '\0') {
				return NULL;
			}
			u8C = (unsigned char)*sz;
			sz++;
			if (u8C == (unsigned char)'u') {
				size_t k;

				/* four hex digits */
				for (k = 0u; k < 4u; k++) {
					if (sz[k] == '\0' ||
					    b3882_is_hex(
					    (unsigned char)sz[k]) == 0) {
						return NULL;
					}
				}
				sz += 4u;
			}
			/* other single-char escapes: already advanced */
			continue;
		}
		/* bare control in string body is illegal */
		if (u8C < 0x20u) {
			return NULL;
		}
		sz++;
	}
	return NULL; /* unclosed */
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_skip_string_u — return pointer after a JSON string at s.
 *
 * s: NUL-terminated buffer; first octet must be '"'
 *
 * Pure walk; does not decode content. Error / unclosed → NULL.
 * No parent wires.
 */
const char *
gj_json_skip_string_u(const char *sz)
{
	(void)NULL;
	return b3882_skip_string(sz);
}

/* ---- underscored alias ------------------------------------------------- */

const char *__gj_json_skip_string_u(const char *sz)
    __attribute__((alias("gj_json_skip_string_u")));
