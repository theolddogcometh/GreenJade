/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3362: JSON array-start predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_json_is_array_start_u(const char *s, size_t n);
 *     - Return 1 if the first non-JSON-whitespace octet in s[0..n) is
 *       '[' (RFC 8259 array begin). JSON ws is space / tab / CR / LF.
 *       Else 0. NULL / empty / all-ws → 0.
 *   int __gj_json_is_array_start_u  (alias)
 *   __libcgj_batch3362_marker = "libcgj-batch3362"
 *
 * Exclusive continuum CREATE-ONLY (3361-3370). Distinct from
 * gj_json_is_structural (batch2519) / gj_json_is_object_start_u
 * (batch3361) — array-open buffer form; unique name; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3362_marker[] = "libcgj-batch3362";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if uCh is JSON insignificant whitespace (RFC 8259 §2). */
static int
b3362_is_ws(unsigned char uCh)
{
	return (uCh == 0x20u || uCh == 0x09u || uCh == 0x0au ||
	    uCh == 0x0du) ? 1 : 0;
}

/* 1 if first non-ws octet is '['. */
static int
b3362_is_array_start(const char *sz, size_t cN)
{
	size_t i;

	for (i = 0u; i < cN; i++) {
		unsigned char uCh = (unsigned char)sz[i];

		if (b3362_is_ws(uCh) != 0) {
			continue;
		}
		return (uCh == (unsigned char)'[') ? 1 : 0;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_array_start_u - 1 if s begins a JSON array ('[').
 *
 * Leading JSON whitespace is skipped; first significant char must be '['.
 */
int
gj_json_is_array_start_u(const char *sz, size_t cN)
{
	(void)NULL;
	if (sz == NULL || cN == 0u) {
		return 0;
	}
	return b3362_is_array_start(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_array_start_u(const char *sz, size_t cN)
    __attribute__((alias("gj_json_is_array_start_u")));
