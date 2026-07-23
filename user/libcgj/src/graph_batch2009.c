/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2009: bounded ASCII alnum class span.
 *
 * Surface (unique symbols):
 *   size_t gj_str_span_class_n(const char *s, size_t n);
 *     — Count leading octets of s[0..n) that are ASCII alphanumeric
 *       (A-Z / a-z / 0-9) using a 256-entry class table. Stops at the
 *       first non-class byte or at n. Binary-safe: n is the sole bound
 *       (embedded NULs are non-class and stop the span). NULL s with
 *       n > 0 → 0; n == 0 → 0.
 *   size_t __gj_str_span_class_n  (alias)
 *   __libcgj_batch2009_marker = "libcgj-batch2009"
 *
 * Distinct from gj_str_span (batch632 accept-set form) — avoid multi-def.
 * Post-2000 continuum str helpers exclusive wave (2001–2010). Clean-room
 * freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2009_marker[] = "libcgj-batch2009";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 256-byte class table: 1 = ASCII alnum, 0 = other.
 * Built once into a static const so span is a table lookup.
 */
static const uint8_t b2009_alnum[256] = {
	/* 0x00..0x2f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 0x30..0x3f  '0'..'9' */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
	/* 0x40..0x4f  'A'.. */
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 0x50..0x5f  ..'Z' */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
	/* 0x60..0x6f  'a'.. */
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 0x70..0x7f  ..'z' */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
	/* 0x80..0xff non-ASCII → 0 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static size_t
b2009_span(const char *sz, size_t n)
{
	size_t i;
	const unsigned char *p;

	if (n == 0u || sz == NULL) {
		return 0u;
	}

	p = (const unsigned char *)sz;
	for (i = 0u; i < n; i++) {
		if (b2009_alnum[p[i]] == 0u) {
			break;
		}
	}
	return i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_span_class_n — count leading ASCII alnum in s[0..n).
 *
 * s: base (NULL with n > 0 → 0)
 * n: maximum bytes to inspect
 *
 * Class is fixed ASCII isalnum via table (no locale, no libc ctype).
 */
size_t
gj_str_span_class_n(const char *s, size_t n)
{
	(void)NULL;
	return b2009_span(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_span_class_n(const char *s, size_t n)
    __attribute__((alias("gj_str_span_class_n")));
