/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3364: JSON number-start predicate (_u buffer).
 *
 * Surface (unique symbols):
 *   int gj_json_is_number_start_u(const char *s, size_t n);
 *     - Return 1 if the first non-JSON-whitespace octet in s[0..n) may
 *       begin an RFC 8259 number: ASCII '-' or decimal digit '0'..'9'.
 *       JSON ws is space / tab / CR / LF. Else 0. NULL / empty /
 *       all-ws → 0. Caller validates the remainder of the token.
 *   int __gj_json_is_number_start_u  (alias)
 *   __libcgj_batch3364_marker = "libcgj-batch3364"
 *
 * Exclusive continuum CREATE-ONLY (3361-3370). Distinct from
 * gj_json_is_number_start (batch2514 char form) and gj_json_is_digit /
 * gj_json_is_digit_u — buffer/ws form with unique _u name; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3364_marker[] = "libcgj-batch3364";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if uCh is JSON insignificant whitespace (RFC 8259 §2). */
static int
b3364_is_ws(unsigned char uCh)
{
	return (uCh == 0x20u || uCh == 0x09u || uCh == 0x0au ||
	    uCh == 0x0du) ? 1 : 0;
}

/* 1 if uCh is '-' or ASCII decimal digit. */
static int
b3364_is_num_start_ch(unsigned char uCh)
{
	if (uCh == (unsigned char)'-') {
		return 1;
	}
	if (uCh >= (unsigned char)'0' && uCh <= (unsigned char)'9') {
		return 1;
	}
	return 0;
}

/* 1 if first non-ws octet may start a JSON number. */
static int
b3364_is_number_start(const char *sz, size_t cN)
{
	size_t i;

	for (i = 0u; i < cN; i++) {
		unsigned char uCh = (unsigned char)sz[i];

		if (b3364_is_ws(uCh) != 0) {
			continue;
		}
		return b3364_is_num_start_ch(uCh);
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_number_start_u - 1 if s begins a JSON number token.
 *
 * Leading JSON whitespace skipped; first significant char is '-' or digit.
 */
int
gj_json_is_number_start_u(const char *sz, size_t cN)
{
	(void)NULL;
	if (sz == NULL || cN == 0u) {
		return 0;
	}
	return b3364_is_number_start(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_number_start_u(const char *sz, size_t cN)
    __attribute__((alias("gj_json_is_number_start_u")));
