/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1125: URI scheme validation (RFC 3986).
 *
 * Surface (unique symbols):
 *   int gj_uri_scheme_ok(const char *scheme);
 *     — Return 1 if scheme matches:
 *       ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
 *       Length 1..64. NULL / empty / non-ALPHA first → 0.
 *   int __gj_uri_scheme_ok  (alias)
 *   __libcgj_batch1125_marker = "libcgj-batch1125"
 *
 * Distinct from URI parse (batch103) — unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1125_marker[] = "libcgj-batch1125";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uri_scheme_ok — 1 if szScheme is a valid URI scheme name.
 */
int
gj_uri_scheme_ok(const char *szScheme)
{
	size_t cb;
	size_t i;
	unsigned char uCh;

	if (szScheme == NULL) {
		return 0;
	}

	cb = 0u;
	while (szScheme[cb] != '\0') {
		cb++;
		if (cb > 64u) {
			return 0;
		}
	}
	if (cb == 0u || cb > 64u) {
		return 0;
	}

	uCh = (unsigned char)szScheme[0];
	if (!((uCh >= 'A' && uCh <= 'Z') || (uCh >= 'a' && uCh <= 'z'))) {
		return 0;
	}

	for (i = 1u; i < cb; i++) {
		uCh = (unsigned char)szScheme[i];
		if ((uCh >= 'A' && uCh <= 'Z') ||
		    (uCh >= 'a' && uCh <= 'z') ||
		    (uCh >= '0' && uCh <= '9') ||
		    uCh == '+' || uCh == '-' || uCh == '.') {
			continue;
		}
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uri_scheme_ok(const char *szScheme)
    __attribute__((alias("gj_uri_scheme_ok")));
