/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3363: JSON string-start predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_json_is_string_start_u(const char *s, size_t n);
 *     - Return 1 if the first non-JSON-whitespace octet in s[0..n) is
 *       '"' (RFC 8259 string begin). JSON ws is space / tab / CR / LF.
 *       Else 0. NULL / empty / all-ws → 0. Does not validate close.
 *   int __gj_json_is_string_start_u  (alias)
 *   __libcgj_batch3363_marker = "libcgj-batch3363"
 *
 * Exclusive continuum CREATE-ONLY (3361-3370). Distinct from
 * gj_json_skip_string_n (batch2516 skip walk) — boolean start only;
 * unique name; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3363_marker[] = "libcgj-batch3363";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if uCh is JSON insignificant whitespace (RFC 8259 §2). */
static int
b3363_is_ws(unsigned char uCh)
{
	return (uCh == 0x20u || uCh == 0x09u || uCh == 0x0au ||
	    uCh == 0x0du) ? 1 : 0;
}

/* 1 if first non-ws octet is '"'. */
static int
b3363_is_string_start(const char *sz, size_t cN)
{
	size_t i;

	for (i = 0u; i < cN; i++) {
		unsigned char uCh = (unsigned char)sz[i];

		if (b3363_is_ws(uCh) != 0) {
			continue;
		}
		return (uCh == (unsigned char)'"') ? 1 : 0;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_string_start_u - 1 if s begins a JSON string ('"').
 *
 * Leading JSON whitespace is skipped; first significant char must be '"'.
 */
int
gj_json_is_string_start_u(const char *sz, size_t cN)
{
	(void)NULL;
	if (sz == NULL || cN == 0u) {
		return 0;
	}
	return b3363_is_string_start(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_string_start_u(const char *sz, size_t cN)
    __attribute__((alias("gj_json_is_string_start_u")));
