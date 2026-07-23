/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1127: MIME media-type type/subtype check.
 *
 * Surface (unique symbols):
 *   int gj_mime_type_ok(const char *type);
 *     — Return 1 if type is "type/subtype" where each token is 1+
 *       restricted-name chars: ALPHA / DIGIT / "!" / "#" / "$" / "&" /
 *       "^" / "_" / "." / "+" / "-" (RFC 6838 restricted-name).
 *       No parameters, no OWS. Length 3..128. NULL / empty → 0.
 *   int __gj_mime_type_ok  (alias)
 *   __libcgj_batch1127_marker = "libcgj-batch1127"
 *
 * Distinct from Content-Type parsers — unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1127_marker[] = "libcgj-batch1127";

/* ---- helpers ------------------------------------------------------------ */

static int
b1127_is_token(unsigned char uCh)
{
	if ((uCh >= 'A' && uCh <= 'Z') ||
	    (uCh >= 'a' && uCh <= 'z') ||
	    (uCh >= '0' && uCh <= '9')) {
		return 1;
	}
	switch (uCh) {
	case '!':
	case '#':
	case '$':
	case '&':
	case '^':
	case '_':
	case '.':
	case '+':
	case '-':
		return 1;
	default:
		return 0;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mime_type_ok — 1 if szType is type/subtype with valid tokens.
 */
int
gj_mime_type_ok(const char *szType)
{
	size_t i;
	size_t cbType;
	size_t cbSub;
	size_t cbTotal;

	if (szType == NULL) {
		return 0;
	}

	/* type token */
	cbType = 0u;
	while (szType[cbType] != '\0' && szType[cbType] != '/') {
		if (!b1127_is_token((unsigned char)szType[cbType])) {
			return 0;
		}
		cbType++;
		if (cbType > 127u) {
			return 0;
		}
	}
	if (cbType == 0u || szType[cbType] != '/') {
		return 0;
	}

	/* subtype token */
	i = cbType + 1u;
	cbSub = 0u;
	while (szType[i] != '\0') {
		if (!b1127_is_token((unsigned char)szType[i])) {
			return 0;
		}
		cbSub++;
		i++;
		if (cbSub > 127u) {
			return 0;
		}
	}
	if (cbSub == 0u) {
		return 0;
	}

	cbTotal = cbType + 1u + cbSub;
	if (cbTotal < 3u || cbTotal > 128u) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mime_type_ok(const char *szType)
    __attribute__((alias("gj_mime_type_ok")));
