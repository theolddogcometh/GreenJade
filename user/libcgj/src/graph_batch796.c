/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch796: IPv4 dotted-quad string predicate.
 *
 * Surface (unique symbols):
 *   int gj_host_is_ipv4_str(const char *s);
 *     — Return 1 if s is exactly "d.d.d.d" with four decimal octets
 *       each in 0..255 (no leading junk, no trailing junk, no empty
 *       fields, no leading '+'/'-'). Leading zeros allowed (e.g. "01").
 *       NULL → 0.
 *   int __gj_host_is_ipv4_str  (alias)
 *   __libcgj_batch796_marker = "libcgj-batch796"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch796_marker[] = "libcgj-batch796";

/* ---- helpers ----------------------------------------------------------- */

/*
 * Parse one decimal octet at *pp. Digits only, value 0..255, at least one
 * digit. On success advance *pp past the digits and return 1; else 0.
 */
static int
b796_parse_octet(const char **pp)
{
	const char *p;
	unsigned uVal;
	unsigned uDigits;

	p = *pp;
	if (p == NULL || *p < '0' || *p > '9') {
		return 0;
	}

	uVal = 0u;
	uDigits = 0u;
	while (*p >= '0' && *p <= '9') {
		if (uDigits >= 3u) {
			return 0; /* more than 3 digits cannot fit 0..255 */
		}
		uVal = uVal * 10u + (unsigned)(*p - '0');
		if (uVal > 255u) {
			return 0;
		}
		uDigits++;
		p++;
	}

	if (uDigits == 0u) {
		return 0;
	}

	*pp = p;
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_host_is_ipv4_str — 1 if s looks like d.d.d.d with 0..255 octets.
 */
int
gj_host_is_ipv4_str(const char *sz)
{
	const char *p;
	unsigned uOctet;

	if (sz == NULL || sz[0] == '\0') {
		return 0;
	}

	p = sz;
	for (uOctet = 0u; uOctet < 4u; uOctet++) {
		if (uOctet > 0u) {
			if (*p != '.') {
				return 0;
			}
			p++;
		}
		if (!b796_parse_octet(&p)) {
			return 0;
		}
	}

	/* Must consume entire string. */
	if (*p != '\0') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_host_is_ipv4_str(const char *sz)
    __attribute__((alias("gj_host_is_ipv4_str")));
